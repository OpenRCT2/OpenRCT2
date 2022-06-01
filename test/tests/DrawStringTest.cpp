/*****************************************************************************
 * Copyright (c) 2014-2022 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <openrct2/Context.h>
#include <openrct2/OpenRCT2.h>
#include <openrct2/PlatformEnvironment.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/IDrawingContext.h>
#include <openrct2/drawing/IDrawingEngine.h>
#include <openrct2/localisation/LocalisationService.h>
#include <openrct2/platform/Platform.h>

TEST(DrawStringTests, basic)
{
    gOpenRCT2Headless = true;
    gOpenRCT2NoGraphics = true;

    Platform::CoreInit();

    auto context = OpenRCT2::CreateContext();
    bool initialised = context->Initialise();
    ASSERT_TRUE(initialised);

    int32_t x{};
    int32_t y{};
    int32_t width{};
    int32_t height{};
    ZoomLevel zoom_level{};

    rct_drawpixelinfo dpi{};
    dpi.x = x;
    dpi.y = y;
    dpi.width = width;
    dpi.height = height;
    dpi.pitch = (dpi.width + 31) % 32;
    dpi.zoom_level = zoom_level;

    std::unique_ptr<uint8_t[]> bits = std::make_unique<uint8_t[]>(dpi.height * (dpi.width + dpi.pitch));
    dpi.bits = bits.get();
    int32_t colour{};
    const ScreenCoordsXY coords{};
    bool noFormatting{};
    FontSpriteBase fontSpriteBase{};
    const_utf8string text = "Hello world!";
    ttf_draw_string(&dpi, text, colour, coords, noFormatting, fontSpriteBase, context.get());
}

namespace OpenRCT2
{
    struct MockPlatformEnvironment : public IPlatformEnvironment
    {
        u8string GetDirectoryPath(DIRBASE base) const override
        {
            return "";
        }
        u8string GetDirectoryPath(DIRBASE base, DIRID did) const override
        {
            return "";
        }
        u8string GetFilePath(PATHID pathid) const override
        {
            return "";
        }
        u8string FindFile(DIRBASE base, DIRID did, u8string_view fileName) const override
        {
            return "";
        }
        void SetBasePath(DIRBASE base, u8string_view path) override
        {
        }
    };
    struct MockContext final : public IContext
    {
        [[nodiscard]] std::shared_ptr<Audio::IAudioContext> GetAudioContext() override
        {
            return nullptr;
        }
        [[nodiscard]] std::shared_ptr<Ui::IUiContext> GetUiContext() override
        {
            return nullptr;
        }
        GameState* GetGameState() override
        {
            return nullptr;
        }
        [[nodiscard]] std::shared_ptr<IPlatformEnvironment> GetPlatformEnvironment() override
        {
            return nullptr;
        }
        Localisation::LocalisationService* GetLocalisationService() override
        {
            return &_localisationService;
        }
        IObjectManager* GetObjectManager() const override
        {
            return nullptr;
        }
        IObjectRepository* GetObjectRepository() const override
        {
            return nullptr;
        }
#ifdef ENABLE_SCRIPTING
        Scripting::ScriptEngine* GetScriptEngine() override
        {
            return nullptr;
        }
#endif
        ITrackDesignRepository* GetTrackDesignRepository() override
        {
            return nullptr;
        }
        IScenarioRepository* GetScenarioRepository() override
        {
            return nullptr;
        }
        IReplayManager* GetReplayManager() override
        {
            return nullptr;
        }
        IGameStateSnapshots* GetGameStateSnapshots() override
        {
            return nullptr;
        }
        DrawingEngine GetDrawingEngineType() override
        {
            return {};
        }
        Drawing::IDrawingEngine* GetDrawingEngine() override
        {
            return nullptr;
        }
        Paint::Painter* GetPainter() override
        {
            return nullptr;
        }
#ifndef DISABLE_NETWORK
        NetworkBase* GetNetwork() override
        {
            return nullptr;
        }
#endif
        int32_t RunOpenRCT2(int argc, const char** argv) override
        {
            return {};
        }

        bool Initialise() override
        {
            return true;
        }
        void InitialiseDrawingEngine() override
        {
        }
        void DisposeDrawingEngine() override
        {
        }
        bool LoadParkFromFile(const std::string& path, bool loadTitleScreenOnFail = false, bool asScenario = false) override
        {
            return true;
        }
        bool LoadParkFromStream(
            IStream* stream, const std::string& path, bool loadTitleScreenFirstOnFail = false, bool asScenario = false) override
        {
            return true;
        }
        void WriteLine(const std::string& s) override
        {
        }
        void WriteErrorLine(const std::string& s) override
        {
        }
        void Finish() override
        {
        }
        void Quit() override
        {
        }

        bool HasNewVersionInfo() const override
        {
            return false;
        }
        const NewVersionInfo* GetNewVersionInfo() const override
        {
            return nullptr;
        }

        void SetTimeScale(float newScale) override
        {
        }
        float GetTimeScale() const override
        {
            return {};
        }
        Localisation::LocalisationService _localisationService{ nullptr };
    };
} // namespace OpenRCT2

namespace OpenRCT2::Drawing
{
    struct MockDrawingEngine : public IDrawingEngine
    {
        MOCK_METHOD(void, Initialise, (), (override));
        MOCK_METHOD(void, Resize, (uint32_t width, uint32_t height), (override));
        MOCK_METHOD(void, SetPalette, (const GamePalette& colours), (override));

        MOCK_METHOD(void, SetVSync, (bool vsync), (override));

        MOCK_METHOD(void, Invalidate, (int32_t left, int32_t top, int32_t right, int32_t bottom), (override));
        MOCK_METHOD(void, BeginDraw, (), (override));
        MOCK_METHOD(void, EndDraw, (), (override));
        MOCK_METHOD(void, PaintWindows, (), (override));
        MOCK_METHOD(void, PaintWeather, (), (override));
        MOCK_METHOD(void, CopyRect, (int32_t x, int32_t y, int32_t width, int32_t height, int32_t dx, int32_t dy), (override));
        MOCK_METHOD(std::string, Screenshot, (), (override));

        MOCK_METHOD(IDrawingContext*, GetDrawingContext, (), (override));
        MOCK_METHOD(rct_drawpixelinfo*, GetDrawingPixelInfo, (), (override));

        MOCK_METHOD(DRAWING_ENGINE_FLAGS, GetFlags, (), (override));

        MOCK_METHOD(void, InvalidateImage, (uint32_t image), (override));
    };

    struct MockDrawingContext : public IDrawingContext
    {
        MOCK_METHOD(void, Clear, (rct_drawpixelinfo * dpi, uint8_t paletteIndex), (override));
        MOCK_METHOD(
            void, FillRect,
            (rct_drawpixelinfo * dpi, uint32_t colour, int32_t left, int32_t top, int32_t right, int32_t bottom), (override));
        MOCK_METHOD(
            void, FilterRect,
            (rct_drawpixelinfo * dpi, FilterPaletteID palette, int32_t left, int32_t top, int32_t right, int32_t bottom),
            (override));
        MOCK_METHOD(void, DrawLine, (rct_drawpixelinfo * dpi, uint32_t colour, const ScreenLine& line), (override));
        MOCK_METHOD(void, DrawSprite, (rct_drawpixelinfo * dpi, ImageId image, int32_t x, int32_t y), (override));
        MOCK_METHOD(
            void, DrawSpriteRawMasked, (rct_drawpixelinfo * dpi, int32_t x, int32_t y, ImageId maskImage, ImageId colourImage),
            (override));
        MOCK_METHOD(
            void, DrawSpriteSolid, (rct_drawpixelinfo * dpi, ImageId image, int32_t x, int32_t y, uint8_t colour), (override));
        MOCK_METHOD(
            void, DrawGlyph, (rct_drawpixelinfo * dpi, uint32_t image, int32_t x, int32_t y, const PaletteMap& palette),
            (override));
        MOCK_METHOD(
            void, DrawBitmap,
            (rct_drawpixelinfo * dpi, uint32_t image, const void* pixels, int32_t width, int32_t height, int32_t x, int32_t y),
            (override));
    };
} // namespace OpenRCT2::Drawing

TEST(DrawStringTests, noContext)
{
    const_utf8string text = "ABCDEFghijklm";
    size_t text_len = strlen(text);

    OpenRCT2::MockContext ctx;
    OpenRCT2::Drawing::MockDrawingEngine mde;
    OpenRCT2::Drawing::MockDrawingContext mdc;

    int32_t x{};
    int32_t y{};
    int32_t width{ 256 };
    int32_t height{ 32 };
    ZoomLevel zoom_level{};

    rct_drawpixelinfo dpi{};
    dpi.x = x;
    dpi.y = y;
    dpi.width = width;
    dpi.height = height;
    dpi.pitch = (dpi.width + 31) % 32;
    dpi.zoom_level = zoom_level;
    dpi.DrawingEngine = &mde;

    EXPECT_CALL(mde, GetDrawingContext()).Times(testing::Exactly(text_len)).WillRepeatedly(testing::Return(&mdc));
    for (size_t i = 0; i < text_len; i++)
    {
        EXPECT_CALL(mdc, DrawGlyph(&dpi, 0x20000EF5+text[i], 0, 0, testing::_));
    }

    const size_t buf_size = dpi.height * (dpi.width + dpi.pitch);
    std::unique_ptr<uint8_t[]> bits = std::make_unique<uint8_t[]>(buf_size);
    dpi.bits = bits.get();
    int32_t colour{};
    const ScreenCoordsXY coords{};
    bool noFormatting{};
    FontSpriteBase fontSpriteBase{};
    ttf_draw_string(&dpi, text, colour, coords, noFormatting, fontSpriteBase, &ctx);
}
