/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "DrawingEngineFactory.hpp"

#include <SDL.h>
#include <atomic>
#include <cmath>
#include <condition_variable>
#include <cstring>
#include <libyuv.h>
#include <memory>
#include <mutex>
#include <openrct2/Diagnostic.h>
#include <openrct2/Game.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/Guard.hpp>
#include <openrct2/drawing/IDrawingEngine.h>
#include <openrct2/drawing/LightFX.h>
#include <openrct2/drawing/X8DrawingEngine.h>
#include <openrct2/paint/Paint.h>
#include <openrct2/scenes/title/TitleSequenceRender.h>
#include <openrct2/ui/UiContext.h>
#include <thread>
#include <vector>
#include <vpx/vp8cx.h>
#include <vpx/vpx_encoder.h>

using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Ui;

bool gShouldRender = true;
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

static void ivf_write_frame_header(FILE* outfile, int64_t pts, size_t frame_size)
{
    char header[12];

    mem_put_le32(header, static_cast<int>(frame_size));
    mem_put_le32(header + 4, static_cast<int>(pts & 0xFFFFFFFF));
    mem_put_le32(header + 8, static_cast<int>(pts >> 32));
    fwrite(header, 1, 12, outfile);
}

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

static int vpx_video_writer_write_frame(VpxVideoWriter* writer, const uint8_t* buffer, size_t size, int64_t pts)
{
    ivf_write_frame_header(writer->file, pts, size);
    if (fwrite(buffer, 1, size, writer->file) != size)
        return 0;

    ++writer->frame_count;

    return 1;
}

static void die_codec(vpx_codec_ctx_t* ctx, const char* s)
{
    const char* detail = vpx_codec_error_detail(ctx);

    LOG_ERROR("%s: %s\n", s, vpx_codec_error(ctx));
    if (detail)
        LOG_ERROR("    %s\n", detail);
    exit(EXIT_FAILURE);
}

static int encode_frame(vpx_codec_ctx_t* codec, vpx_image_t* img, int frame_index, int flags, VpxVideoWriter* writer)
{
    int got_pkts = 0;
    vpx_codec_iter_t iter = NULL;
    const vpx_codec_cx_pkt_t* pkt = NULL;
    const vpx_codec_err_t res = vpx_codec_encode(codec, img, frame_index, 1, flags, VPX_DL_GOOD_QUALITY);
    if (res != VPX_CODEC_OK)
        die_codec(codec, "Failed to encode frame");
    while ((pkt = vpx_codec_get_cx_data(codec, &iter)) != NULL)
    {
        got_pkts = 1;
        if (pkt->kind == VPX_CODEC_CX_FRAME_PKT)
        {
            const int keyframe = (pkt->data.frame.flags & VPX_FRAME_IS_KEY) != 0;
            if (!vpx_video_writer_write_frame(
                    writer, static_cast<uint8_t*>(pkt->data.frame.buf), pkt->data.frame.sz, pkt->data.frame.pts))
            {
                die_codec(codec, "Failed to write compressed frame");
            }
            printf(keyframe ? "K" : ".");
            if (frame_index % 100 == 0)
            {
                printf("%d", frame_index);
            }
            fflush(stdout);
        }
    }
    return got_pkts;
}

struct EncodeThreadData
{
    std::mutex* SurfaceMutex;
    std::condition_variable* NotifyCV;
    int* Ready;
    VpxVideoWriter** writer;
    vpx_codec_ctx_t* codec{};
    vpx_image_t* raw{};
    uint32_t width{};
    uint32_t height{};
    uint32_t scale{};

    // Double buffering: A/B pixel buffers
    std::unique_ptr<uint8_t[]> pixelBufferA{};
    std::unique_ptr<uint8_t[]> pixelBufferB{};
    size_t bufferSize{};

    // Which buffer is currently being used by the encoding thread
    std::atomic<int> activeBuffer{ 0 }; // 0 = A, 1 = B
};

static void EncodeThreadFunc(EncodeThreadData& etd)
{
    while (true)
    {
        std::unique_lock lock(*etd.SurfaceMutex);
        (*etd.NotifyCV).wait(lock, [&etd] { return *etd.Ready != 0; });
        if (*etd.Ready == 2)
        {
            printf("closing down thread");
            fflush(stdout);
            break;
        }
        *etd.Ready = 0;

        // Get the current buffer to encode
        uint8_t* currentPixelBuffer = (etd.activeBuffer == 0) ? etd.pixelBufferA.get() : etd.pixelBufferB.get();

        if (currentPixelBuffer && etd.bufferSize > 0)
        {
            auto scaledWidth = etd.width * etd.scale;
            auto scaledHeight = etd.height * etd.scale;

            // Create source surface from the pixel buffer
            SDL_Surface* tempSurface = SDL_CreateRGBSurfaceWithFormatFrom(
                currentPixelBuffer, etd.width, etd.height, 32, etd.width * 4, SDL_PIXELFORMAT_ARGB8888);

            if (tempSurface != nullptr)
            {
                // Create scaled surface buffer
                SDL_Surface* scaledSurface = SDL_CreateRGBSurfaceWithFormat(
                    0, scaledWidth, scaledHeight, 32, SDL_PIXELFORMAT_ARGB8888);

                if (scaledSurface != nullptr)
                {
                    // Scale the surface
                    if (SDL_BlitScaled(tempSurface, nullptr, scaledSurface, nullptr) == 0)
                    {
                        // Convert to YUV for encoding
                        libyuv::ARGBToI420(
                            static_cast<uint8_t*>(scaledSurface->pixels), scaledSurface->pitch, etd.raw->planes[0],
                            etd.raw->stride[0], etd.raw->planes[1], etd.raw->stride[1], etd.raw->planes[2], etd.raw->stride[1],
                            scaledWidth, scaledHeight);

                        static int frame_count;
                        encode_frame(etd.codec, etd.raw, frame_count++, 0, *etd.writer);
                    }
                    else
                    {
                        LOG_ERROR("SDL_BlitScaled failed: %s", SDL_GetError());
                    }
                    SDL_FreeSurface(scaledSurface);
                }
                SDL_FreeSurface(tempSurface);
            }
        }
    }
}

class HardwareDisplayDrawingEngine final : public X8DrawingEngine
{
private:
    constexpr static uint32_t kDirtyVisualTime = 40;
    constexpr static uint32_t kDirtyRegionAlpha = 100;

    IUiContext& _uiContext;
    SDL_Window* _window = nullptr;
    SDL_Renderer* _sdlRenderer = nullptr;
    SDL_Texture* _screenTexture = nullptr;
    SDL_Texture* _scaledScreenTexture = nullptr;
    SDL_PixelFormat* _screenTextureFormat = nullptr;
    uint32_t _paletteHWMapped[256] = { 0 };
    uint32_t _lightPaletteHWMapped[256] = { 0 };

    bool _useVsync = true;

    std::vector<uint32_t> _dirtyVisualsTime;

    bool smoothNN = false;

    VpxVideoInfo info{};
    const VpxInterface* encoder = NULL;
    vpx_image_t raw{};
    VpxVideoWriter* writer = NULL;
    vpx_codec_ctx_t codec{};
    std::thread EncodeThread{};
    std::mutex SurfaceMutex{};
    std::condition_variable NotifyCV{};
    int Ready{};
    EncodeThreadData etd{};

public:
    explicit HardwareDisplayDrawingEngine(IUiContext& uiContext)
        : X8DrawingEngine(uiContext)
        , _uiContext(uiContext)
    {
        _window = static_cast<SDL_Window*>(_uiContext.GetWindow());
    }

    ~HardwareDisplayDrawingEngine() override
    {
        {
            std::unique_lock lock(SurfaceMutex);
            Ready = 2;
            NotifyCV.notify_one();
        }
        EncodeThread.join();

        if (_screenTexture != nullptr)
        {
            SDL_DestroyTexture(_screenTexture);
        }
        if (_scaledScreenTexture != nullptr)
        {
            SDL_DestroyTexture(_scaledScreenTexture);
        }
        SDL_FreeFormat(_screenTextureFormat);
        SDL_DestroyRenderer(_sdlRenderer);

        while (encode_frame(&codec, NULL, -1, 0, writer))
        {
        }

        vpx_img_free(&raw);
        if (vpx_codec_destroy(&codec))
        {
            die_codec(&codec, "Failed to destroy codec.");
        }
        vpx_video_writer_close(writer);
    }

    void Initialise() override
    {
        _sdlRenderer = SDL_CreateRenderer(_window, -1, SDL_RENDERER_ACCELERATED | (_useVsync ? SDL_RENDERER_PRESENTVSYNC : 0));

        encoder = &vpx_encoders[0];
        etd.codec = &codec;
        etd.NotifyCV = &NotifyCV;
        etd.raw = &raw;
        etd.Ready = &Ready;
        etd.writer = &writer;
        etd.SurfaceMutex = &SurfaceMutex;
        EncodeThread = std::thread(EncodeThreadFunc, std::ref(etd));
    }

    void SetVSync(bool vsync) override
    {
        if (_useVsync != vsync)
        {
            _useVsync = vsync;
#if SDL_VERSION_ATLEAST(2, 0, 18)
            SDL_RenderSetVSync(_sdlRenderer, vsync ? 1 : 0);
#else
            SDL_DestroyRenderer(_sdlRenderer);
            _screenTexture = nullptr;
            _scaledScreenTexture = nullptr;
            Initialise();
            Resize(_uiContext->GetWidth(), _uiContext->GetHeight());
#endif
        }
    }

    void Resize(uint32_t width, uint32_t height) override
    {
        if (width == 0 || height == 0)
        {
            return;
        }

        if (_screenTexture != nullptr)
        {
            SDL_DestroyTexture(_screenTexture);
        }
        SDL_FreeFormat(_screenTextureFormat);

        SDL_RendererInfo rendererInfo = {};
        int32_t result = SDL_GetRendererInfo(_sdlRenderer, &rendererInfo);
        if (result < 0)
        {
            LOG_WARNING("HWDisplayDrawingEngine::Resize error: %s", SDL_GetError());
            return;
        }
        uint32_t pixelFormat = SDL_PIXELFORMAT_UNKNOWN;
        for (uint32_t i = 0; i < rendererInfo.num_texture_formats; i++)
        {
            uint32_t format = rendererInfo.texture_formats[i];
            if (!SDL_ISPIXELFORMAT_FOURCC(format) && !SDL_ISPIXELFORMAT_INDEXED(format)
                && (pixelFormat == SDL_PIXELFORMAT_UNKNOWN || SDL_BYTESPERPIXEL(format) < SDL_BYTESPERPIXEL(pixelFormat)))
            {
                pixelFormat = format;
            }
        }

        ScaleQuality scaleQuality = GetContext()->GetUiContext().GetScaleQuality();
        if (scaleQuality == ScaleQuality::SmoothNearestNeighbour)
        {
            scaleQuality = ScaleQuality::Linear;
            smoothNN = true;
        }
        else
        {
            smoothNN = false;
        }

        if (smoothNN)
        {
            if (_scaledScreenTexture != nullptr)
            {
                SDL_DestroyTexture(_scaledScreenTexture);
            }

            char scaleQualityBuffer[4];
            snprintf(scaleQualityBuffer, sizeof(scaleQualityBuffer), "%d", static_cast<int32_t>(scaleQuality));
            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0");

            _screenTexture = SDL_CreateTexture(_sdlRenderer, pixelFormat, SDL_TEXTUREACCESS_STREAMING, width, height);
            Guard::Assert(
                _screenTexture != nullptr, "Failed to create unscaled screen texture (%ux%u, pixelFormat = %u): %s", width,
                height, pixelFormat, SDL_GetError());

            SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, scaleQualityBuffer);

            uint32_t scale = std::ceil(Config::Get().general.WindowScale);
            _scaledScreenTexture = SDL_CreateTexture(
                _sdlRenderer, pixelFormat, SDL_TEXTUREACCESS_TARGET, width * scale, height * scale);

            Guard::Assert(
                _scaledScreenTexture != nullptr,
                "Failed to create scaled screen texture (%ux%u, scale = %u, pixelFormat = %u): %s", width, height, scale,
                pixelFormat, SDL_GetError());
        }
        else
        {
            _screenTexture = SDL_CreateTexture(_sdlRenderer, pixelFormat, SDL_TEXTUREACCESS_STREAMING, width, height);
            Guard::Assert(
                _screenTexture != nullptr, "Failed to create screen texture (%ux%u, pixelFormat = %u): %s", width, height,
                pixelFormat, SDL_GetError());
        }

        uint32_t format;
        SDL_QueryTexture(_screenTexture, &format, nullptr, nullptr, nullptr);
        _screenTextureFormat = SDL_AllocFormat(format);

        X8DrawingEngine::Resize(width, height);

        info.codec_fourcc = encoder->fourcc;
        info.frame_width = width * Config::Get().general.WindowScale;
        info.frame_height = height * Config::Get().general.WindowScale;
        info.time_base.numerator = 1;
        info.time_base.denominator = FPS;
        if (info.frame_width <= 0 || info.frame_height <= 0 || (info.frame_width % 2) != 0 || (info.frame_height % 2) != 0)
        {
            LOG_FATAL(
                "Invalid frame size: %dx%d (need to be larger than zero and even-sized)", info.frame_width, info.frame_height);
        }

        if (!vpx_img_alloc(&raw, VPX_IMG_FMT_I420, info.frame_width, info.frame_height, 1))
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
        cfg.g_profile = 0;

        using namespace std::string_literals;
        char* titleSeqName = getenv("TITLE_SEQUENCE_NAME");
        std::string titleSequenceNameStr;
        if (titleSeqName != nullptr)
        {
            titleSequenceNameStr = titleSeqName;
        }
        std::string filename = "out"s + titleSequenceNameStr + ".webm";
        printf("Rendering to file %s\n", filename.c_str());
        writer = vpx_video_writer_open(filename.c_str(), kContainerIVF, &info);
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

        // Update encode thread data with new dimensions
        etd.width = width;
        etd.height = height;
        etd.scale = Config::Get().general.WindowScale;

        // Allocate A/B pixel buffers for double buffering
        etd.bufferSize = width * height * 4; // RGBA
        etd.pixelBufferA = std::make_unique<uint8_t[]>(etd.bufferSize);
        etd.pixelBufferB = std::make_unique<uint8_t[]>(etd.bufferSize);
        etd.activeBuffer = 0; // Start with buffer A
    }

    void SetPalette(const GamePalette& palette) override
    {
        if (_screenTextureFormat != nullptr)
        {
            for (int32_t i = 0; i < 256; i++)
            {
                _paletteHWMapped[i] = SDL_MapRGB(_screenTextureFormat, palette[i].Red, palette[i].Green, palette[i].Blue);
            }

            if (Config::Get().general.EnableLightFx)
            {
                auto& lightPalette = LightFx::GetPalette();
                for (int32_t i = 0; i < 256; i++)
                {
                    const auto& src = lightPalette[i];
                    _lightPaletteHWMapped[i] = SDL_MapRGBA(_screenTextureFormat, src.Red, src.Green, src.Blue, src.Alpha);
                }
            }
        }
    }

    void BeginDraw() override
    {
        X8DrawingEngine::BeginDraw();
    }

    void EndDraw() override
    {
        X8DrawingEngine::EndDraw();

        Display();
    }

protected:
    void OnDrawDirtyBlock(int32_t left, int32_t top, int32_t right, int32_t bottom) override
    {
        if (gShowDirtyVisuals)
        {
            const auto columns = ((right - left) + (_invalidationGrid.getBlockWidth() - 1)) / _invalidationGrid.getBlockWidth();
            const auto rows = ((bottom - top) + (_invalidationGrid.getBlockHeight() - 1)) / _invalidationGrid.getBlockHeight();
            const auto firstRow = top / _invalidationGrid.getBlockHeight();
            const auto firstColumn = left / _invalidationGrid.getBlockWidth();

            for (uint32_t y = 0; y < rows; y++)
            {
                for (uint32_t x = 0; x < columns; x++)
                {
                    SetDirtyVisualTime(firstColumn + x, firstRow + y, gCurrentRealTimeTicks + kDirtyVisualTime);
                }
            }
        }
    }

private:
    void Display()
    {
        if (Config::Get().general.EnableLightFx)
        {
            void* pixels;
            int32_t pitch;
            if (SDL_LockTexture(_screenTexture, nullptr, &pixels, &pitch) == 0)
            {
                LightFx::RenderToTexture(pixels, pitch, _bits, _width, _height, _paletteHWMapped, _lightPaletteHWMapped);
                SDL_UnlockTexture(_screenTexture);
            }
        }
        else
        {
            CopyBitsToTexture(
                _screenTexture, _bits, static_cast<int32_t>(_width), static_cast<int32_t>(_height), _paletteHWMapped);
        }
        if (smoothNN)
        {
            SDL_SetRenderTarget(_sdlRenderer, _scaledScreenTexture);
            SDL_RenderCopy(_sdlRenderer, _screenTexture, nullptr, nullptr);

            SDL_SetRenderTarget(_sdlRenderer, nullptr);
            SDL_RenderCopy(_sdlRenderer, _scaledScreenTexture, nullptr, nullptr);
        }
        else
        {
            SDL_RenderCopy(_sdlRenderer, _screenTexture, nullptr, nullptr);
        }

        if (gShowDirtyVisuals)
        {
            RenderDirtyVisuals();
        }

        if (gShouldRender)
        {
            // Determine which buffer to write to (opposite of the one being encoded)
            int writeBuffer = (etd.activeBuffer == 0) ? 1 : 0;
            uint8_t* writePixelBuffer = (writeBuffer == 0) ? etd.pixelBufferA.get() : etd.pixelBufferB.get();

            if (writePixelBuffer && etd.bufferSize > 0)
            {
                int pitch;
                void* pixels;
                if (SDL_LockTexture(_screenTexture, nullptr, &pixels, &pitch) == 0)
                {
                    // Calculate how much data to copy based on actual pitch
                    const size_t rowSize = std::min(static_cast<size_t>(etd.width * 4), static_cast<size_t>(pitch));
                    const uint8_t* srcPixels = static_cast<const uint8_t*>(pixels);

                    // Copy row by row to handle pitch correctly
                    for (uint32_t y = 0; y < etd.height; ++y)
                    {
                        std::memcpy(writePixelBuffer + y * etd.width * 4, srcPixels + y * pitch, rowSize);
                    }

                    SDL_UnlockTexture(_screenTexture);

                    // Swap buffers atomically
                    etd.activeBuffer = writeBuffer;

                    // Notify encoding thread
                    std::unique_lock lock(SurfaceMutex);
                    Ready = 1;
                    NotifyCV.notify_one();
                }
                else
                {
                    LOG_WARNING("Failed to lock texture for encoding: %s", SDL_GetError());
                }
            }
        }
        SDL_RenderPresent(_sdlRenderer);
    }

    void CopyBitsToTexture(SDL_Texture* texture, uint8_t* src, int32_t width, int32_t height, const uint32_t* palette)
    {
        void* pixels;
        int32_t pitch;
        if (SDL_LockTexture(texture, nullptr, &pixels, &pitch) == 0)
        {
            int32_t padding = pitch - (width * 4);
            if (pitch == width * 4)
            {
                uint32_t* dst = static_cast<uint32_t*>(pixels);
                for (int32_t i = width * height; i > 0; i--)
                {
                    *dst++ = palette[*src++];
                }
            }
            else
            {
                if (pitch == (width * 2) + padding)
                {
                    uint16_t* dst = static_cast<uint16_t*>(pixels);
                    for (int32_t y = height; y > 0; y--)
                    {
                        for (int32_t x = width; x > 0; x--)
                        {
                            const uint8_t lower = *reinterpret_cast<const uint8_t*>(&palette[*src++]);
                            const uint8_t upper = *reinterpret_cast<const uint8_t*>(&palette[*src++]);
                            *dst++ = (lower << 8) | upper;
                        }
                        dst = reinterpret_cast<uint16_t*>(reinterpret_cast<uint8_t*>(dst) + padding);
                    }
                }
                else if (pitch == width + padding)
                {
                    uint8_t* dst = static_cast<uint8_t*>(pixels);
                    for (int32_t y = height; y > 0; y--)
                    {
                        for (int32_t x = width; x > 0; x--)
                        {
                            *dst++ = *reinterpret_cast<const uint8_t*>(&palette[*src++]);
                        }
                        dst += padding;
                    }
                }
            }
            SDL_UnlockTexture(texture);
        }
    }

    uint32_t GetDirtyVisualTime(uint32_t x, uint32_t y)
    {
        uint32_t result = 0;
        uint32_t i = y * _invalidationGrid.getColumnCount() + x;
        if (_dirtyVisualsTime.size() > i)
        {
            result = _dirtyVisualsTime[i];
        }
        return result;
    }

    void SetDirtyVisualTime(uint32_t x, uint32_t y, uint32_t value)
    {
        const auto rows = _invalidationGrid.getRowCount();
        const auto columns = _invalidationGrid.getColumnCount();

        _dirtyVisualsTime.resize(rows * columns);

        uint32_t i = y * _invalidationGrid.getColumnCount() + x;
        if (_dirtyVisualsTime.size() > i)
        {
            _dirtyVisualsTime[i] = value;
        }
    }

    void RenderDirtyVisuals()
    {
        int windowX, windowY, renderX, renderY;
        SDL_GetWindowSize(_window, &windowX, &windowY);
        SDL_GetRendererOutputSize(_sdlRenderer, &renderX, &renderY);

        float scaleX = Config::Get().general.WindowScale * renderX / static_cast<float>(windowX);
        float scaleY = Config::Get().general.WindowScale * renderY / static_cast<float>(windowY);

        SDL_SetRenderDrawBlendMode(_sdlRenderer, SDL_BLENDMODE_BLEND);
        for (uint32_t y = 0; y < _invalidationGrid.getRowCount(); y++)
        {
            for (uint32_t x = 0; x < _invalidationGrid.getColumnCount(); x++)
            {
                const auto timeEnd = GetDirtyVisualTime(x, y);
                const auto timeLeft = gCurrentRealTimeTicks < timeEnd ? timeEnd - gCurrentRealTimeTicks : 0;
                if (timeLeft > 0)
                {
                    uint8_t alpha = timeLeft * kDirtyRegionAlpha / kDirtyVisualTime;
                    SDL_Rect ddRect;
                    ddRect.x = static_cast<int32_t>(x * _invalidationGrid.getBlockWidth() * scaleX);
                    ddRect.y = static_cast<int32_t>(y * _invalidationGrid.getBlockHeight() * scaleY);
                    ddRect.w = static_cast<int32_t>(_invalidationGrid.getBlockWidth() * scaleX);
                    ddRect.h = static_cast<int32_t>(_invalidationGrid.getBlockHeight() * scaleY);

                    SDL_SetRenderDrawColor(_sdlRenderer, 255, 255, 255, alpha);
                    SDL_RenderFillRect(_sdlRenderer, &ddRect);
                }
            }
        }
    }
};

std::unique_ptr<IDrawingEngine> OpenRCT2::Ui::CreateHardwareDisplayDrawingEngine(IUiContext& uiContext)
{
    return std::make_unique<HardwareDisplayDrawingEngine>(uiContext);
}
