/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "DrawingEngineFactory.hpp"

#include <SDL.h>
#include <algorithm>
#include <openrct2/Game.h>
#include <openrct2/common.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/Guard.hpp>
#include <openrct2/drawing/IDrawingEngine.h>
#include <openrct2/drawing/X8DrawingEngine.h>
#include <openrct2/ui/UiContext.h>
#include <vpx/vp8cx.h>
#include <vpx/vpx_encoder.h>

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Ui;

struct VpxRational
{
    int numerator;
    int denominator;
};

typedef struct
{
    uint32_t codec_fourcc;
    int frame_width;
    int frame_height;
    struct VpxRational time_base;
} VpxVideoInfo;

typedef struct VpxInterface
{
    const char* name;
    uint32_t fourcc;
    vpx_codec_iface_t* (*codec_interface)(void);
} VpxInterface;

#define VP9_FOURCC 0x30395056

static const VpxInterface vpx_encoders[] = {
    { "vp9", VP9_FOURCC, &vpx_codec_vp9_cx },
};

struct VpxVideoWriterStruct
{
    VpxVideoInfo info;
    FILE* file;
    int frame_count;
};

typedef struct VpxVideoWriterStruct VpxVideoWriter;

typedef enum
{
    kContainerIVF
} VpxContainer;

static void mem_put_le16(void* dst, uint16_t data)
{
    memcpy(dst, &data, 2);
}
static void mem_put_le32(void* dst, uint32_t data)
{
    memcpy(dst, &data, 4);
}
static void ivf_write_file_header_with_video_info(
    FILE* outfile, unsigned int fourcc, int frame_cnt, int frame_width, int frame_height, vpx_rational_t timebase)
{
    char header[32];

    header[0] = 'D';
    header[1] = 'K';
    header[2] = 'I';
    header[3] = 'F';
    mem_put_le16(header + 4, 0);             // version
    mem_put_le16(header + 6, 32);            // header size
    mem_put_le32(header + 8, fourcc);        // fourcc
    mem_put_le16(header + 12, frame_width);  // width
    mem_put_le16(header + 14, frame_height); // height
    mem_put_le32(header + 16, timebase.den); // rate
    mem_put_le32(header + 20, timebase.num); // scale
    mem_put_le32(header + 24, frame_cnt);    // length
    mem_put_le32(header + 28, 0);            // unused

    fwrite(header, 1, 32, outfile);
}

static void ivf_write_file_header(FILE* outfile, const struct vpx_codec_enc_cfg* cfg, unsigned int fourcc, int frame_cnt)
{
    ivf_write_file_header_with_video_info(outfile, fourcc, frame_cnt, cfg->g_w, cfg->g_h, cfg->g_timebase);
}
/*
static void ivf_write_frame_header(FILE* outfile, int64_t pts, size_t frame_size)
{
    char header[12];

    mem_put_le32(header, static_cast<int>(frame_size));
    mem_put_le32(header + 4, static_cast<int>(pts & 0xFFFFFFFF));
    mem_put_le32(header + 8, static_cast<int>(pts >> 32));
    fwrite(header, 1, 12, outfile);
}
*/

static void write_header(FILE* file, const VpxVideoInfo* info, int frame_count)
{
    struct vpx_codec_enc_cfg cfg;
    cfg.g_w = info->frame_width;
    cfg.g_h = info->frame_height;
    cfg.g_timebase.num = info->time_base.numerator;
    cfg.g_timebase.den = info->time_base.denominator;

    ivf_write_file_header(file, &cfg, info->codec_fourcc, frame_count);
}

static VpxVideoWriter* vpx_video_writer_open(const char* filename, VpxContainer container, const VpxVideoInfo* info)
{
    if (container == kContainerIVF)
    {
        VpxVideoWriter* writer = NULL;
        FILE* const file = fopen(filename, "wb");
        if (!file)
        {
            fprintf(stderr, "%s can't be written to.\n", filename);
            return NULL;
        }
        writer = static_cast<VpxVideoWriter*>(malloc(sizeof(*writer)));
        if (!writer)
        {
            fprintf(stderr, "Can't allocate VpxVideoWriter.\n");
            return NULL;
        }
        writer->frame_count = 0;
        writer->info = *info;
        writer->file = file;

        write_header(writer->file, info, 0);

        return writer;
    }
    fprintf(stderr, "VpxVideoWriter supports only IVF.\n");
    return NULL;
}

static void vpx_video_writer_close(VpxVideoWriter* writer)
{
    if (writer)
    {
        // Rewriting frame header with real frame count
        rewind(writer->file);
        write_header(writer->file, &writer->info, writer->frame_count);

        fclose(writer->file);
        free(writer);
    }
}
/*
static int vpx_video_writer_write_frame(VpxVideoWriter* writer, const uint8_t* buffer, size_t size, int64_t pts)
{
    ivf_write_frame_header(writer->file, pts, size);
    if (fwrite(buffer, 1, size, writer->file) != size)
        return 0;

    ++writer->frame_count;

    return 1;
}
*/
static void die_codec(vpx_codec_ctx_t* ctx, const char* s)
{
    const char* detail = vpx_codec_error_detail(ctx);

    LOG_ERROR("%s: %s\n", s, vpx_codec_error(ctx));
    if (detail)
        LOG_ERROR("    %s\n", detail);
    exit(EXIT_FAILURE);
}

class SoftwareDrawingEngine final : public X8DrawingEngine
{
private:
    std::shared_ptr<IUiContext> const _uiContext;
    SDL_Window* _window = nullptr;
    SDL_Surface* _surface = nullptr;
    SDL_Surface* _RGBASurface = nullptr;
    SDL_Palette* _palette = nullptr;
    VpxVideoInfo info{};
    const VpxInterface* encoder = NULL;
    vpx_image_t raw{};
    VpxVideoWriter* writer = NULL;
    vpx_codec_ctx_t codec{};

public:
    explicit SoftwareDrawingEngine(const std::shared_ptr<IUiContext>& uiContext)
        : X8DrawingEngine(uiContext)
        , _uiContext(uiContext)
    {
        _window = static_cast<SDL_Window*>(_uiContext->GetWindow());
    }

    ~SoftwareDrawingEngine() override
    {
        SDL_FreeSurface(_surface);
        SDL_FreeSurface(_RGBASurface);
        SDL_FreePalette(_palette);

        vpx_img_free(&raw);
        if (vpx_codec_destroy(&codec))
        {
            die_codec(&codec, "Failed to destroy codec.");
        }
        vpx_video_writer_close(writer);
    }

    void Initialise() override
    {
        encoder = &vpx_encoders[0];
    }

    void Resize(uint32_t width, uint32_t height) override
    {
        static int i;
        printf("resize %d, %ux%u\n\n", i++, width, height);
        fflush(stdout);
        SDL_FreeSurface(_surface);
        SDL_FreeSurface(_RGBASurface);
        SDL_FreePalette(_palette);

        _surface = SDL_CreateRGBSurface(0, width, height, 8, 0, 0, 0, 0);
        _RGBASurface = SDL_CreateRGBSurfaceWithFormat(0, width, height, 32, SDL_PIXELFORMAT_RGBA8888);
        SDL_SetSurfaceBlendMode(_RGBASurface, SDL_BLENDMODE_NONE);
        _palette = SDL_AllocPalette(256);

        if (_surface == nullptr || _palette == nullptr || _RGBASurface == nullptr)
        {
            LOG_FATAL("%p || %p || %p == nullptr %s", _surface, _palette, _RGBASurface, SDL_GetError());
            exit(-1);
        }

        if (SDL_SetSurfacePalette(_surface, _palette))
        {
            LOG_FATAL("SDL_SetSurfacePalette failed %s", SDL_GetError());
            exit(-1);
        }

        info.codec_fourcc = encoder->fourcc;
        info.frame_width = width;
        info.frame_height = height;
        info.time_base.numerator = 1;
        info.time_base.denominator = 60;
        if (info.frame_width <= 0 || info.frame_height <= 0 || (info.frame_width % 2) != 0 || (info.frame_height % 2) != 0)
        {
            LOG_FATAL("Invalid frame size: %dx%d", info.frame_width, info.frame_height);
        }

        if (!vpx_img_alloc(&raw, VPX_IMG_FMT_I444, info.frame_width, info.frame_height, 1))
        {
            LOG_FATAL("Failed to allocate image.");
        }
        LOG_INFO("Using %s\n", vpx_codec_iface_name(encoder->codec_interface()));
        vpx_codec_enc_cfg_t cfg{};
        vpx_codec_err_t res = vpx_codec_enc_config_default(encoder->codec_interface(), &cfg, 0);
        if (res)
        {
            die_codec(&codec, "Failed to get default codec config.");
        }
        cfg.g_w = info.frame_width;
        cfg.g_h = info.frame_height;
        cfg.g_timebase.num = info.time_base.numerator;
        cfg.g_timebase.den = info.time_base.denominator;
        cfg.g_threads = 8;

        writer = vpx_video_writer_open("/tmp/out.webm", kContainerIVF, &info);
        if (!writer)
        {
            LOG_FATAL("Failed to open %s for writing.", "/tmp/out.webm");
        }
        if (vpx_codec_enc_init(&codec, encoder->codec_interface(), &cfg, 0))
        {
            LOG_FATAL("Failed to initialize encoder");
        }
        if (vpx_codec_control_(&codec, VP9E_SET_LOSSLESS, 1))
        {
            die_codec(&codec, "Failed to use lossless mode");
        }

        ConfigureBits(width, height, _surface->pitch);
    }

    void SetPalette(const GamePalette& palette) override
    {
        SDL_Surface* windowSurface = SDL_GetWindowSurface(_window);
        if (windowSurface != nullptr && _palette != nullptr)
        {
            SDL_Colour colours[256];
            for (int32_t i = 0; i < 256; i++)
            {
                colours[i].r = palette[i].Red;
                colours[i].g = palette[i].Green;
                colours[i].b = palette[i].Blue;
                colours[i].a = palette[i].Alpha;
            }
            SDL_SetPaletteColors(_palette, colours, 0, 256);
        }
    }

    void EndDraw() override
    {
        Display();
    }

private:
    void Display()
    {
        // Lock the surface before setting its pixels
        if (SDL_MUSTLOCK(_surface))
        {
            if (SDL_LockSurface(_surface) < 0)
            {
                LOG_ERROR("locking failed %s", SDL_GetError());
                return;
            }
        }

        // Copy pixels from the virtual screen buffer to the surface
        std::copy_n(_bits, _surface->pitch * _surface->h, static_cast<uint8_t*>(_surface->pixels));

        // Unlock the surface
        if (SDL_MUSTLOCK(_surface))
        {
            SDL_UnlockSurface(_surface);
        }
        if (SDL_BlitSurface(_surface, nullptr, _RGBASurface, nullptr))
        {
            LOG_FATAL("SDL_BlitSurface %s", SDL_GetError());
            exit(1);
        }
        std::unique_ptr<uint8_t[]> dst = std::make_unique<uint8_t[]>(_RGBASurface->pitch * _RGBASurface->h * 4);
        if (SDL_ConvertPixels(
                _RGBASurface->w, _RGBASurface->h, _RGBASurface->format->format, _RGBASurface->pixels, _RGBASurface->pitch,
                SDL_PIXELFORMAT_IYUV, dst.get(), _RGBASurface->pitch)
            != 0)
        {
            LOG_ERROR("SDL reported error: %s\n", SDL_GetError());
        }
        static int frame_number;
        if (frame_number++ == 0)
        {
            {
                auto file = fopen("/tmp/frameyuv", "wb");
                fwrite(dst.get(), _RGBASurface->pitch * _RGBASurface->h * 4, 1, file);
                fclose(file);
            }
            {
                auto file = fopen("/tmp/framergba", "wb");
                fwrite(_RGBASurface->pixels, _RGBASurface->pitch * _RGBASurface->h * 4, 1, file);
                fclose(file);
            }
        }

        // Copy the surface to the window
        if (gConfigGeneral.WindowScale == 1 || gConfigGeneral.WindowScale <= 0)
        {
            SDL_Surface* windowSurface = SDL_GetWindowSurface(_window);
            if (SDL_BlitSurface(_surface, nullptr, windowSurface, nullptr))
            {
                LOG_FATAL("SDL_BlitSurface %s", SDL_GetError());
                exit(1);
            }
        }
        else
        {
            // first blit to rgba surface to change the pixel format
            if (SDL_BlitSurface(_surface, nullptr, _RGBASurface, nullptr))
            {
                LOG_FATAL("SDL_BlitSurface %s", SDL_GetError());
                exit(1);
            }

            // then scale to window size. Without changing to RGBA first, SDL complains
            // about blit configurations being incompatible.
            if (SDL_BlitScaled(_RGBASurface, nullptr, SDL_GetWindowSurface(_window), nullptr))
            {
                LOG_FATAL("SDL_BlitScaled %s", SDL_GetError());
                exit(1);
            }
        }
        if (SDL_UpdateWindowSurface(_window))
        {
            LOG_FATAL("SDL_UpdateWindowSurface %s", SDL_GetError());
            exit(1);
        }
    }
};

std::unique_ptr<IDrawingEngine> OpenRCT2::Ui::CreateSoftwareDrawingEngine(const std::shared_ptr<IUiContext>& uiContext)
{
    return std::make_unique<SoftwareDrawingEngine>(uiContext);
}
