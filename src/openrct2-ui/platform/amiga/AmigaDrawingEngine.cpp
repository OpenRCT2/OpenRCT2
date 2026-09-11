/*****************************************************************************
 * AmigaOS drawing engine: the X8 software rasteriser presented on an 8-bit
 * RTG screen. Dirty blocks are blitted straight from the paletted framebuffer
 * with WriteChunkyPixels; the palette goes to the screen's ViewPort.
 * Replaces HardwareDisplayDrawingEngine.cpp (SDL_Renderer) on AmigaOS.
 *****************************************************************************/
#ifdef __amigaos__

    #include "../../drawing/engines/DrawingEngineFactory.hpp"
    #include "AmigaWindow.h"
    #include "amiga_ui.h"

    #include <memory>
    #include <openrct2/config/Config.h>
    #include <openrct2/drawing/IDrawingEngine.h>
    #include <openrct2/drawing/X8DrawingEngine.h>
    #include <openrct2/ui/UiContext.h>
    #include <openrct2/platform/AmigaTrace.h>
    #include <openrct2/core/String.hpp>

extern "C" unsigned amiga_ticks_ms(void);
using namespace OpenRCT2;
using namespace OpenRCT2::Drawing;
using namespace OpenRCT2::Ui;

class AmigaDrawingEngine final : public X8DrawingEngine
{
private:
    IUiContext& _uiContext;
    // Union of the dirty blocks drawn this frame; only this region is pushed to the RTG screen.
    int32_t _dbX0 = 0, _dbY0 = 0, _dbX1 = 0, _dbY1 = 0;
    void resetDirtyBox()
    {
        _dbX0 = _dbY0 = 0x7FFFFFFF;
        _dbX1 = _dbY1 = -0x7FFFFFFF;
    }

public:
    explicit AmigaDrawingEngine(IUiContext& uiContext)
        : X8DrawingEngine(uiContext)
        , _uiContext(uiContext)
    {
        AMIGA_TRACE("gfx: AmigaDrawingEngine created");
    }

    void Initialise() override
    {
    }

    void Resize(uint32_t width, uint32_t height) override
    {
        // LightFX writes 32-bit pixels; an 8-bit screen cannot show them.
        Config::Get().general.enableLightFx = false;
        AMIGA_TRACE(String::stdFormat("gfx: Resize %u x %u", width, height).c_str());
        X8DrawingEngine::Resize(width, height);
        AMIGA_TRACE("gfx: Resize done");
    }

    void SetPalette(const GamePalette& palette) override
    {
        uint8_t rgb[256 * 3];
        for (size_t i = 0; i < 256; i++)
        {
            rgb[i * 3 + 0] = palette[i].red;
            rgb[i * 3 + 1] = palette[i].green;
            rgb[i * 3 + 2] = palette[i].blue;
        }
        AMIGA_TRACE_ONCE("gfx: first SetPalette");
        amiga_ui_set_palette(rgb);
    }

    void BeginDraw() override
    {
        AMIGA_TRACE_ONCE("gfx: first BeginDraw");
        resetDirtyBox();
        X8DrawingEngine::BeginDraw();
    }

    void EndDraw() override
    {
        X8DrawingEngine::EndDraw();
        AMIGA_TRACE_ONCE("gfx: first EndDraw");
        // Present the union of this frame's dirty blocks as one contiguous chunky blit. This keeps the
        // picture whole (unlike per-block blitting, which left invalidation gaps) while only converting the
        // region that actually changed -- cheap on P96/uaegfx and much cheaper than a full-frame blit on
        // real Picasso96 hardware, where the chunky->native conversion is the cost.
        auto* window = static_cast<SDL_Window*>(_uiContext.GetWindow());
        if (_bits != nullptr && window != nullptr && _dbX1 > _dbX0 && _dbY1 > _dbY0)
        {
            int32_t x0 = std::max<int32_t>(0, _dbX0);
            int32_t y0 = std::max<int32_t>(0, _dbY0);
            int32_t x1 = std::min<int32_t>(std::min<int32_t>(_dbX1, static_cast<int32_t>(_width)), window->width);
            int32_t y1 = std::min<int32_t>(std::min<int32_t>(_dbY1, static_cast<int32_t>(_height)), window->height);
            if (x1 > x0 && y1 > y0)
            {
                const uint8_t* src = reinterpret_cast<const uint8_t*>(_bits) + static_cast<size_t>(y0) * _pitch + x0;
                amiga_ui_blit(src, static_cast<int>(_pitch), x0, y0, x1 - x0, y1 - y0);
            }
        }
        resetDirtyBox();
        // Frame-rate probe: report frames and average blit ms every ~100 frames.
        static unsigned frames = 0, t0 = 0;
        if (t0 == 0)
            t0 = amiga_ticks_ms();
        if (++frames >= 100)
        {
            unsigned now = amiga_ticks_ms();
            unsigned dt = now - t0;
            AMIGA_TRACE(String::stdFormat("gfx: %u frames in %u ms = %u.%02u fps", frames, dt,
                dt ? frames * 1000u / dt : 0u, dt ? (frames * 100000u / dt) % 100u : 0u).c_str());
            frames = 0;
            t0 = now;
        }
    }

protected:
    void OnDrawDirtyBlock(int32_t left, int32_t top, int32_t right, int32_t bottom) override
    {
        if (_bits == nullptr)
            return;
        auto* window = static_cast<SDL_Window*>(_uiContext.GetWindow());
        if (window == nullptr)
            return;
        // right/bottom are exclusive; clip to both the framebuffer and the screen
        int32_t x0 = std::max<int32_t>(0, left);
        int32_t y0 = std::max<int32_t>(0, top);
        int32_t x1 = std::min<int32_t>(std::min<int32_t>(right, static_cast<int32_t>(_width)), window->width);
        int32_t y1 = std::min<int32_t>(std::min<int32_t>(bottom, static_cast<int32_t>(_height)), window->height);
        if (x1 <= x0 || y1 <= y0)
            return;
        // Accumulate into the frame's dirty bounding box; the union is blitted once in EndDraw.
        _dbX0 = std::min(_dbX0, x0);
        _dbY0 = std::min(_dbY0, y0);
        _dbX1 = std::max(_dbX1, x1);
        _dbY1 = std::max(_dbY1, y1);
    }
};

std::unique_ptr<IDrawingEngine> OpenRCT2::Ui::CreateHardwareDisplayDrawingEngine(IUiContext& uiContext)
{
    return std::make_unique<AmigaDrawingEngine>(uiContext);
}

#endif // __amigaos__
