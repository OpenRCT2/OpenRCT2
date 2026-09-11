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
        X8DrawingEngine::BeginDraw();
    }

    void EndDraw() override
    {
        X8DrawingEngine::EndDraw();
        AMIGA_TRACE_ONCE("gfx: first EndDraw");
        // Present the whole framebuffer each frame. Per-dirty-block blitting left stale regions on
        // the RTG screen (invalidation gaps show as black rectangles); a full 640x480 chunky blit is
        // ~1 ms on P96/uaegfx, so just push the complete X8 buffer every frame.
        auto* window = static_cast<SDL_Window*>(_uiContext.GetWindow());
        if (_bits != nullptr && window != nullptr)
        {
            int32_t w = std::min<int32_t>(static_cast<int32_t>(_width), window->width);
            int32_t h = std::min<int32_t>(static_cast<int32_t>(_height), window->height);
            amiga_ui_blit(reinterpret_cast<const uint8_t*>(_bits), static_cast<int>(_pitch), 0, 0, w, h);
        }
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
        // Presented in EndDraw as a full-screen blit; nothing to do per block.
        (void)x0; (void)y0; (void)x1; (void)y1;
    }
};

std::unique_ptr<IDrawingEngine> OpenRCT2::Ui::CreateHardwareDisplayDrawingEngine(IUiContext& uiContext)
{
    return std::make_unique<AmigaDrawingEngine>(uiContext);
}

#endif // __amigaos__
