/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../interface/Theme.h"

#include <openrct2-ui/interface/Objective.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Diagnostic.h>
#include <openrct2/FileClassifier.h>
#include <openrct2/ParkImporter.h>
#include <openrct2/SpriteIds.h>
#include <openrct2/audio/Audio.h>
#include <openrct2/config/Config.h>
#include <openrct2/core/FileStream.h>
#include <openrct2/drawing/ColourMap.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/interface/ColourWithFlags.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/localisation/LocalisationService.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/object/ScenarioMetaObject.h>
#include <openrct2/park/ParkPreview.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/scenario/Scenario.h>
#include <openrct2/scenario/ScenarioCategory.h>
#include <openrct2/scenario/ScenarioRepository.h>
#include <openrct2/scenario/ScenarioSources.h>
#include <openrct2/ui/WindowManager.h>
#include <vector>

using namespace OpenRCT2::Drawing;

namespace OpenRCT2::Ui::Windows
{
    static constexpr int32_t kInitialNumUnlockedScenarios = 5;
    static constexpr uint8_t kNumTabs = 10;
    static constexpr uint8_t kPadding = 5;
    static constexpr int32_t kPreviewPaneWidthRegular = 180;
    static constexpr int32_t kPreviewPaneWidthScreenshots = 254;
    static constexpr int32_t kTabHeight = 34;
    static constexpr int32_t kWidgetsStart = 17;
    static constexpr int32_t kTabsStart = kWidgetsStart;
    static constexpr int32_t kTabWidth = 92;
    static constexpr int32_t kTrueFontSize = 24;
    static constexpr ScreenSize kWindowSize = { 734, 384 };
    static constexpr StringId kWindowTitle = STR_SELECT_SCENARIO;

    enum class ListItemType : uint8_t
    {
        Heading,
        Scenario,
    };

    struct ScenarioListItem
    {
        ListItemType type;
        union
        {
            struct
            {
                StringId string_id;
            } heading;
            struct
            {
                const ScenarioIndexEntry* scenario;
                bool is_locked;
            } scenario;
        };
    };

    enum
    {
        WIDX_BACKGROUND,
        WIDX_TITLEBAR,
        WIDX_CLOSE,
        WIDX_TABCONTENT,
        WIDX_TAB1,
        WIDX_TAB2,
        WIDX_TAB3,
        WIDX_TAB4,
        WIDX_TAB5,
        WIDX_TAB6,
        WIDX_TAB7,
        WIDX_TAB8,
        WIDX_TAB9,
        WIDX_TAB10,
        WIDX_SCENARIOLIST
    };

    static constexpr StringId kScenarioOriginStringIds[] = {
        STR_SCENARIO_CATEGORY_RCT1,        STR_SCENARIO_CATEGORY_RCT1_AA,    STR_SCENARIO_CATEGORY_RCT1_LL,
        STR_SCENARIO_CATEGORY_RCT2,        STR_SCENARIO_CATEGORY_RCT2_WW,    STR_SCENARIO_CATEGORY_RCT2_TT,
        STR_SCENARIO_CATEGORY_UCES,        STR_SCENARIO_CATEGORY_REAL_PARKS, STR_SCENARIO_CATEGORY_EXTRAS_PARKS,
        STR_SCENARIO_CATEGORY_OTHER_PARKS,
    };

    // clang-format off
    static constexpr auto _scenarioSelectWidgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget({ kTabWidth + 1, kWidgetsStart }, { kWindowSize.width, 284 },         WidgetType::resize, WindowColour::secondary                     ), // tab content panel
        makeRemapWidget({ 3, kTabsStart + (kTabHeight * 0) }, { kTabWidth, kTabHeight }, WidgetType::tab,    WindowColour::secondary, SPR_G2_SIDEWAYS_TAB), // tab 01
        makeRemapWidget({ 3, kTabsStart + (kTabHeight * 1) }, { kTabWidth, kTabHeight }, WidgetType::tab,    WindowColour::secondary, SPR_G2_SIDEWAYS_TAB), // tab 02
        makeRemapWidget({ 3, kTabsStart + (kTabHeight * 2) }, { kTabWidth, kTabHeight }, WidgetType::tab,    WindowColour::secondary, SPR_G2_SIDEWAYS_TAB), // tab 03
        makeRemapWidget({ 3, kTabsStart + (kTabHeight * 3) }, { kTabWidth, kTabHeight }, WidgetType::tab,    WindowColour::secondary, SPR_G2_SIDEWAYS_TAB), // tab 04
        makeRemapWidget({ 3, kTabsStart + (kTabHeight * 4) }, { kTabWidth, kTabHeight }, WidgetType::tab,    WindowColour::secondary, SPR_G2_SIDEWAYS_TAB), // tab 05
        makeRemapWidget({ 3, kTabsStart + (kTabHeight * 5) }, { kTabWidth, kTabHeight }, WidgetType::tab,    WindowColour::secondary, SPR_G2_SIDEWAYS_TAB), // tab 06
        makeRemapWidget({ 3, kTabsStart + (kTabHeight * 6) }, { kTabWidth, kTabHeight }, WidgetType::tab,    WindowColour::secondary, SPR_G2_SIDEWAYS_TAB), // tab 07
        makeRemapWidget({ 3, kTabsStart + (kTabHeight * 7) }, { kTabWidth, kTabHeight }, WidgetType::tab,    WindowColour::secondary, SPR_G2_SIDEWAYS_TAB), // tab 08
        makeRemapWidget({ 3, kTabsStart + (kTabHeight * 8) }, { kTabWidth, kTabHeight }, WidgetType::tab,    WindowColour::secondary, SPR_G2_SIDEWAYS_TAB), // tab 09
        makeRemapWidget({ 3, kTabsStart + (kTabHeight * 8) }, { kTabWidth, kTabHeight }, WidgetType::tab,    WindowColour::secondary, SPR_G2_SIDEWAYS_TAB), // tab 10
        makeWidget({ kTabWidth + 3, kWidgetsStart + 1 }, { kWindowSize.width - kPreviewPaneWidthRegular, 362 }, WidgetType::scroll, WindowColour::secondary, SCROLL_VERTICAL    ) // level list
    );
    // clang-format on

    class ScenarioSelectWindow final : public Window
    {
    private:
        bool _showLockedInformation = false;
        std::function<void(std::string_view)> _callback;
        std::vector<ScenarioListItem> _listItems;
        const ScenarioIndexEntry* _highlightedScenario = nullptr;
        ParkPreview _preview;
        BackgroundWorker::Job _previewLoadJob;

    public:
        ScenarioSelectWindow(std::function<void(std::string_view)> callback)
            : _callback(callback)
        {
        }

        void onOpen() override
        {
            setWidgets(_scenarioSelectWidgets);

            // Load scenario list
            ScenarioRepositoryScan();

            _highlightedScenario = nullptr;
            initTabs();
            initialiseListItems();
            initScrollWidgets();
        }

        void onLanguageChange() override
        {
            _listItems.clear();
            ScenarioRepositoryScan();
            initialiseListItems();
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            if (widgetIndex == WIDX_CLOSE)
            {
                close();
            }
        }

        void onMouseDown(WidgetIndex widgetIndex) override
        {
            if (widgetIndex >= WIDX_TAB1 && widgetIndex <= WIDX_TAB10)
            {
                selectedTab = widgetIndex - 4;
                Config::Get().interface.scenarioSelectLastTab = selectedTab;
                Config::Save();

                _highlightedScenario = nullptr;
                _preview = {};

                initialiseListItems();
                invalidate();
                onResize();
                onPrepareDraw();
                initScrollWidgets();
                invalidate();
            }
        }

        int32_t GetPreviewPaneWidth() const
        {
            if (Config::Get().interface.scenarioPreviewScreenshots)
                return kPreviewPaneWidthScreenshots;
            else
                return kPreviewPaneWidthRegular;
        }

        void LoadPreview()
        {
            _preview = {};

            if (_highlightedScenario == nullptr)
                return;

            if (_previewLoadJob.isValid())
                _previewLoadJob.cancel();

            auto& bgWorker = GetContext()->GetBackgroundWorker();
            auto& path = _highlightedScenario->Path;
            auto& name = _highlightedScenario->InternalName;

            ClassifiedFileInfo info;
            bool isClassified = TryClassifyFile(path, &info);

            if (isClassified && info.Type == FileType::park)
            {
                _previewLoadJob = bgWorker.addJob(
                    [path, name]() {
                        try
                        {
                            auto fs = FileStream(path, FileMode::open);
                            auto& objectRepository = GetContext()->GetObjectRepository();
                            auto parkImporter = ParkImporter::CreateParkFile(objectRepository);
                            parkImporter->LoadFromStream(&fs, false, true, path.c_str());
                            return parkImporter->GetParkPreview();
                        }
                        catch (const std::exception& e)
                        {
                            LOG_ERROR("Could not get preview for \"%s\" due to %s", path.c_str(), e.what());
                            return ParkPreview{};
                        }
                    },
                    [](const ParkPreview preview) {
                        auto* windowMgr = GetWindowManager();
                        auto* wnd = windowMgr->FindByClass(WindowClass::scenarioSelect);
                        if (wnd == nullptr)
                        {
                            return;
                        }
                        auto* scenarioSelectWnd = static_cast<ScenarioSelectWindow*>(wnd);
                        scenarioSelectWnd->UpdateParkPreview(preview);
                    });
            }
            else
            {
                SourceDescriptor source{};
                if (!ScenarioSources::TryGetByName(name, &source))
                    return;

                auto& objManager = GetContext()->GetObjectManager();
                if (auto obj = objManager.LoadTempObject(source.textObjectId, true); obj != nullptr)
                {
                    auto& scenarioMetaObj = reinterpret_cast<ScenarioMetaObject&>(*obj);
                    scenarioMetaObj.Load();

                    ParkPreview preview{};
                    preview.images.push_back(scenarioMetaObj.GetMiniMapImage());
                    preview.images.push_back(scenarioMetaObj.GetPreviewImage());
                    _preview = preview;

                    scenarioMetaObj.Unload();
                }
            }
        }

        void UpdateParkPreview(const ParkPreview& preview)
        {
            _preview = preview;
            invalidate();
        }

        ScreenCoordsXY DrawPreview(RenderTarget& rt, ScreenCoordsXY screenPos)
        {
            auto targetImageType = PreviewImageType::miniMap;
            if (Config::Get().interface.scenarioPreviewScreenshots)
                targetImageType = PreviewImageType::screenshot;

            // Find minimap image to draw, if available
            PreviewImage* image = nullptr;
            for (auto& candidate : _preview.images)
            {
                if (candidate.type == targetImageType)
                {
                    image = &candidate;
                    break;
                }
            }

            if (image == nullptr)
                return screenPos;

            // Draw frame
            auto startFrameX = width - (GetPreviewPaneWidth() / 2) - (image->width / 2) - kPadding;
            auto frameStartPos = ScreenCoordsXY(windowPos.x + startFrameX, screenPos.y + 15);
            auto frameEndPos = frameStartPos + ScreenCoordsXY(image->width + 1, image->height + 1);
            Rectangle::fillInset(
                rt, { frameStartPos, frameEndPos }, colours[1], Rectangle::BorderStyle::inset, Rectangle::FillBrightness::dark,
                Rectangle::FillMode::dontLightenWhenInset);

            // Draw image, if available
            auto imagePos = frameStartPos + ScreenCoordsXY(1, 1);
            drawPreviewImage(*image, rt, imagePos);

            return frameEndPos;
        }

        void onDraw(RenderTarget& rt) override
        {
            drawWidgets(rt);

            StringId format = STR_WINDOW_COLOUR_2_STRINGID;
            FontStyle fontStyle = FontStyle::medium;

            if (ScenarioSelectUseSmallFont())
            {
                format = STR_SMALL_WINDOW_COLOUR_2_STRINGID;
                fontStyle = FontStyle::small;
            }

            // Text for each tab
            for (uint32_t i = 0; i < std::size(kScenarioOriginStringIds); i++)
            {
                const Widget& widget = widgets[WIDX_TAB1 + i];
                if (widget.type == WidgetType::empty)
                    continue;

                auto ft = Formatter();
                ft.Add<StringId>(kScenarioOriginStringIds[i]);

                auto stringCoords = windowPos + ScreenCoordsXY{ widget.midX(), widget.midY() - 3 };
                DrawTextWrapped(
                    rt, stringCoords, 87, format, ft, { Drawing::Colour::lightWater, fontStyle, TextAlignment::centre });
            }

            auto previewPaneWidth = GetPreviewPaneWidth();

            // Return if no scenario highlighted
            auto* scenario = _highlightedScenario;
            if (scenario == nullptr)
            {
                if (_showLockedInformation)
                {
                    // Show locked information
                    auto screenPos = windowPos
                        + ScreenCoordsXY{ widgets[WIDX_SCENARIOLIST].right + 4, widgets[WIDX_TABCONTENT].top + 5 };
                    DrawTextEllipsised(
                        rt, screenPos + ScreenCoordsXY{ previewPaneWidth / 2, 0 }, previewPaneWidth, STR_SCENARIO_LOCKED, {},
                        { TextAlignment::centre });

                    DrawTextWrapped(rt, screenPos + ScreenCoordsXY{ 0, 15 }, previewPaneWidth, STR_SCENARIO_LOCKED_DESC);
                }
                else
                {
                    // Show general information about how to start.
                    auto screenPos = windowPos
                        + ScreenCoordsXY{ widgets[WIDX_SCENARIOLIST].right + 4, widgets[WIDX_TABCONTENT].top + 5 };

                    DrawTextWrapped(rt, screenPos + ScreenCoordsXY{ 0, 15 }, previewPaneWidth, STR_SCENARIO_HOVER_HINT);
                }
                return;
            }

            // Scenario path
            if (Config::Get().general.debuggingTools)
            {
                const auto shortPath = ShortenPath(scenario->Path, width - 6 - kTabWidth, FontStyle::medium);

                auto ft = Formatter();
                ft.Add<utf8*>(shortPath.c_str());
                DrawTextBasic(rt, windowPos + ScreenCoordsXY{ kTabWidth + 3, height - 3 - 11 }, STR_STRING, ft, { colours[1] });
            }

            // Scenario name
            auto screenPos = windowPos
                + ScreenCoordsXY{ widgets[WIDX_SCENARIOLIST].right + 4, widgets[WIDX_TABCONTENT].top + 5 };
            auto ft = Formatter();
            ft.Add<StringId>(STR_STRING);
            ft.Add<const char*>(scenario->Name.c_str());
            DrawTextEllipsised(
                rt, screenPos + ScreenCoordsXY{ previewPaneWidth / 2, 0 }, previewPaneWidth, STR_WINDOW_COLOUR_2_STRINGID, ft,
                { TextAlignment::centre });

            // Still loading the preview?
            if (_previewLoadJob.isValid())
            {
                ft = Formatter();
                ft.Add<StringId>(STR_LOADING_GENERIC);
                DrawTextBasic(
                    rt, screenPos + ScreenCoordsXY{ previewPaneWidth / 2, 15 }, STR_BLACK_STRING, ft,
                    { TextAlignment::centre });
                return;
            }

            // Draw preview
            auto previewEnd = DrawPreview(rt, screenPos);
            screenPos.y = previewEnd.y + 15;

            // Scenario details
            ft = Formatter();
            ft.Add<StringId>(STR_STRING);
            ft.Add<const char*>(scenario->Details.c_str());
            screenPos.y += DrawTextWrapped(rt, screenPos, previewPaneWidth, STR_BLACK_STRING, ft) + 5;

            // Scenario objective
            Scenario::Objective objective = { .Type = scenario->ObjectiveType,
                                              .Year = scenario->ObjectiveArg1,
                                              .NumGuests = static_cast<uint16_t>(scenario->ObjectiveArg3),
                                              .Currency = scenario->ObjectiveArg2 };

            ft = Formatter();
            ft.Add<StringId>(kObjectiveNames[EnumValue(scenario->ObjectiveType)]);
            formatObjective(ft, objective);
            screenPos.y += DrawTextWrapped(rt, screenPos, previewPaneWidth, STR_OBJECTIVE, ft) + 5;

            // Scenario score
            if (scenario->Highscore != nullptr)
            {
                // TODO: Should probably be translatable
                u8string completedByName = "???";
                if (!scenario->Highscore->name.empty())
                {
                    completedByName = scenario->Highscore->name;
                }
                ft = Formatter();
                ft.Add<StringId>(STR_STRING);
                ft.Add<const char*>(completedByName.c_str());
                ft.Add<money64>(scenario->Highscore->company_value);
                screenPos.y += DrawTextWrapped(rt, screenPos, previewPaneWidth, STR_COMPLETED_BY_WITH_COMPANY_VALUE, ft);
            }
        }

        void onPrepareDraw() override
        {
            pressedWidgets &= ~(
                (1uLL << WIDX_CLOSE) | (1uLL << WIDX_TAB1) | (1uLL << WIDX_TAB2) | (1uLL << WIDX_TAB3) | (1uLL << WIDX_TAB4)
                | (1uLL << WIDX_TAB5) | (1uLL << WIDX_TAB6) | (1uLL << WIDX_TAB7) | (1uLL << WIDX_TAB8) | (1uLL << WIDX_TAB9)
                | (1uLL << WIDX_TAB10));

            pressedWidgets |= 1LL << (selectedTab + WIDX_TAB1);

            const int32_t bottomMargin = Config::Get().general.debuggingTools ? 17 : 5;
            widgets[WIDX_SCENARIOLIST].right = width - GetPreviewPaneWidth() - 2 * kPadding;
            widgets[WIDX_SCENARIOLIST].bottom = height - bottomMargin;
        }

        ScreenSize onScrollGetSize(int32_t scrollIndex) override
        {
            const int32_t scenarioItemHeight = GetScenarioListItemSize();

            int32_t y = 0;
            for (const auto& listItem : _listItems)
            {
                switch (listItem.type)
                {
                    case ListItemType::Heading:
                        y += 18;
                        break;
                    case ListItemType::Scenario:
                        y += scenarioItemHeight;
                        break;
                }
            }

            return { kWindowSize.width, y };
        }

        void onScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            const int32_t scenarioItemHeight = GetScenarioListItemSize();

            bool originalShowLockedInformation = _showLockedInformation;
            _showLockedInformation = false;
            const ScenarioIndexEntry* selected = nullptr;
            auto mutableScreenCoords = screenCoords;
            for (const auto& listItem : _listItems)
            {
                switch (listItem.type)
                {
                    case ListItemType::Heading:
                        mutableScreenCoords.y -= 18;
                        break;
                    case ListItemType::Scenario:
                        mutableScreenCoords.y -= scenarioItemHeight;
                        if (mutableScreenCoords.y < 0)
                        {
                            if (listItem.scenario.is_locked)
                            {
                                _showLockedInformation = true;
                            }
                            else
                            {
                                selected = listItem.scenario.scenario;
                            }
                        }
                        break;
                }
                if (mutableScreenCoords.y < 0)
                {
                    break;
                }
            }

            if (_highlightedScenario != selected)
            {
                _highlightedScenario = selected;
                LoadPreview();
                invalidate();
            }
            else if (_showLockedInformation != originalShowLockedInformation)
            {
                invalidate();
            }
        }

        void onScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            const int32_t scenarioItemHeight = GetScenarioListItemSize();

            auto mutableScreenCoords = screenCoords;
            for (const auto& listItem : _listItems)
            {
                switch (listItem.type)
                {
                    case ListItemType::Heading:
                        mutableScreenCoords.y -= 18;
                        break;
                    case ListItemType::Scenario:
                        mutableScreenCoords.y -= scenarioItemHeight;
                        if (mutableScreenCoords.y < 0 && !listItem.scenario.is_locked)
                        {
                            OpenRCT2::Audio::Play(Audio::SoundId::click1, 0, windowPos.x + (width / 2));
                            gFirstTimeSaving = true;
                            // Callback will likely close this window! So should always return after it.
                            _callback(listItem.scenario.scenario->Path);
                            return;
                        }
                        break;
                }
                if (mutableScreenCoords.y < 0)
                {
                    break;
                }
            }
        }

        void onScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            auto paletteIndex = getColourMap(colours[1].colour).midLight;
            GfxClear(rt, paletteIndex);

            StringId highlighted_format = STR_WINDOW_COLOUR_2_STRINGID;
            StringId unhighlighted_format = STR_BLACK_STRING;
            if (ScenarioSelectUseSmallFont())
            {
                highlighted_format = STR_WHITE_STRING;
                unhighlighted_format = STR_WHITE_STRING;
            }

            const auto& listWidget = widgets[WIDX_SCENARIOLIST];
            int32_t listWidth = listWidget.width() - 13;

            const int32_t scenarioItemHeight = GetScenarioListItemSize();

            // Scenario title
            int32_t scenarioTitleHeight = FontGetLineHeight(FontStyle::medium);

            int32_t y = 0;
            for (const auto& listItem : _listItems)
            {
                if (y > rt.y + rt.height)
                {
                    continue;
                }

                switch (listItem.type)
                {
                    case ListItemType::Heading:
                    {
                        const int32_t horizontalRuleMargin = 4;
                        DrawCategoryHeading(
                            rt, horizontalRuleMargin, listWidth - horizontalRuleMargin, y + 2, listItem.heading.string_id);
                        y += 18;
                        break;
                    }
                    case ListItemType::Scenario:
                    {
                        // Draw hover highlight
                        const ScenarioIndexEntry* scenario = listItem.scenario.scenario;
                        bool isHighlighted = _highlightedScenario == scenario;
                        if (isHighlighted)
                        {
                            Rectangle::filter(rt, { 0, y, width, y + scenarioItemHeight - 1 }, FilterPaletteID::paletteDarken1);
                        }

                        bool isCompleted = scenario->Highscore != nullptr;
                        bool isDisabled = listItem.scenario.is_locked;

                        // Draw scenario name
                        StringId format = isDisabled ? static_cast<StringId>(STR_STRINGID)
                                                     : (isHighlighted ? highlighted_format : unhighlighted_format);
                        auto ft = Formatter();
                        ft.Add<StringId>(STR_STRING);
                        ft.Add<const char*>(scenario->Name.c_str());
                        auto colour = isDisabled ? colours[1].withFlag(ColourFlag::inset, true)
                                                 : ColourWithFlags{ Drawing::Colour::black };
                        auto darkness = isDisabled ? TextDarkness::dark : TextDarkness::regular;
                        const auto scrollCentre = widgets[WIDX_SCENARIOLIST].width() / 2;

                        DrawTextBasic(
                            rt, { scrollCentre, y + 1 }, format, ft,
                            { colour, FontStyle::medium, TextAlignment::centre, darkness });

                        // Check if scenario is completed
                        if (isCompleted)
                        {
                            // Draw completion tick
                            GfxDrawSprite(rt, ImageId(SPR_MENU_CHECKMARK), { widgets[WIDX_SCENARIOLIST].width() - 46, y + 1 });

                            // Draw completion score
                            u8string completedByName = "???";
                            if (!scenario->Highscore->name.empty())
                            {
                                completedByName = scenario->Highscore->name;
                            }
                            ft = Formatter();
                            ft.Add<StringId>(STR_COMPLETED_BY);
                            ft.Add<StringId>(STR_STRING);
                            ft.Add<const char*>(completedByName.c_str());
                            DrawTextBasic(
                                rt, { scrollCentre, y + scenarioTitleHeight + 1 }, format, ft,
                                { FontStyle::small, TextAlignment::centre });
                        }

                        y += scenarioItemHeight;
                        break;
                    }
                }
            }
        }

    private:
        void DrawCategoryHeading(RenderTarget& rt, int32_t left, int32_t right, int32_t y, StringId stringId) const
        {
            auto baseColour = colours[1];
            auto lightColour = getColourMap(baseColour.colour).lighter;
            auto darkColour = getColourMap(baseColour.colour).midDark;

            // Draw string
            int32_t centreX = (left + right) / 2;
            DrawTextBasic(rt, { centreX, y }, stringId, {}, { baseColour, TextAlignment::centre });

            // Get string dimensions
            utf8 buffer[512];
            auto bufferPtr = buffer;
            FormatStringLegacy(bufferPtr, sizeof(buffer), stringId, nullptr);
            int32_t categoryStringHalfWidth = (GfxGetStringWidth(bufferPtr, FontStyle::medium) / 2) + 4;
            int32_t strLeft = centreX - categoryStringHalfWidth;
            int32_t strRight = centreX + categoryStringHalfWidth;

            // Draw light horizontal rule
            int32_t lineY = y + 4;
            auto lightLineLeftTop1 = ScreenCoordsXY{ left, lineY };
            auto lightLineRightBottom1 = ScreenCoordsXY{ strLeft, lineY };
            GfxDrawLine(rt, { lightLineLeftTop1, lightLineRightBottom1 }, lightColour);

            auto lightLineLeftTop2 = ScreenCoordsXY{ strRight, lineY };
            auto lightLineRightBottom2 = ScreenCoordsXY{ right, lineY };
            GfxDrawLine(rt, { lightLineLeftTop2, lightLineRightBottom2 }, lightColour);

            // Draw dark horizontal rule
            lineY++;
            auto darkLineLeftTop1 = ScreenCoordsXY{ left, lineY };
            auto darkLineRightBottom1 = ScreenCoordsXY{ strLeft, lineY };
            GfxDrawLine(rt, { darkLineLeftTop1, darkLineRightBottom1 }, darkColour);

            auto darkLineLeftTop2 = ScreenCoordsXY{ strRight, lineY };
            auto darkLineRightBottom2 = ScreenCoordsXY{ right, lineY };
            GfxDrawLine(rt, { darkLineLeftTop2, darkLineRightBottom2 }, darkColour);
        }

        void initialiseListItems()
        {
            size_t numScenarios = ScenarioRepositoryGetCount();
            _listItems.clear();

            // Mega park unlock
            const uint32_t rct1RequiredCompletedScenarios = (1 << SC_MEGA_PARK) - 1;
            uint32_t rct1CompletedScenarios = 0;
            std::optional<size_t> megaParkListItemIndex = std::nullopt;

            int32_t numUnlocks = kInitialNumUnlockedScenarios;
            union
            {
                uint8_t raw = UINT8_MAX;
                Scenario::Category category;
                ScenarioSource source;
            } currentHeading{};
            for (size_t i = 0; i < numScenarios; i++)
            {
                const ScenarioIndexEntry* scenario = ScenarioRepositoryGetByIndex(i);

                if (!IsScenarioVisible(*scenario))
                    continue;

                // Category heading
                StringId headingStringId = kStringIdNone;
                if (selectedTab != EnumValue(ScenarioSource::Real) && currentHeading.category != scenario->Category)
                {
                    currentHeading.category = scenario->Category;
                    headingStringId = Scenario::kScenarioCategoryStringIds[currentHeading.raw];
                }

                if (headingStringId != kStringIdNone)
                {
                    ScenarioListItem headerItem;
                    headerItem.type = ListItemType::Heading;
                    headerItem.heading.string_id = headingStringId;
                    _listItems.push_back(std::move(headerItem));
                }

                // Scenario
                ScenarioListItem scenarioItem;
                scenarioItem.type = ListItemType::Scenario;
                scenarioItem.scenario.scenario = scenario;
                if (IsLockingEnabled())
                {
                    scenarioItem.scenario.is_locked = numUnlocks <= 0;
                    if (scenario->Highscore == nullptr)
                    {
                        numUnlocks--;
                    }
                    else
                    {
                        // Mark RCT1 scenario as completed
                        if (scenario->ScenarioId < SC_MEGA_PARK)
                        {
                            rct1CompletedScenarios |= 1 << scenario->ScenarioId;
                        }
                    }

                    // If scenario is Mega Park, keep a reference to it
                    if (scenario->ScenarioId == SC_MEGA_PARK)
                    {
                        megaParkListItemIndex = _listItems.size() - 1;
                    }
                }
                else
                {
                    scenarioItem.scenario.is_locked = false;
                }
                _listItems.push_back(std::move(scenarioItem));
            }

            // Mega park handling
            if (megaParkListItemIndex.has_value())
            {
                bool megaParkLocked = (rct1CompletedScenarios & rct1RequiredCompletedScenarios)
                    != rct1RequiredCompletedScenarios;
                _listItems[megaParkListItemIndex.value()].scenario.is_locked = megaParkLocked;
                if (megaParkLocked && Config::Get().general.scenarioHideMegaPark)
                {
                    // Remove mega park
                    _listItems.pop_back();

                    // Remove empty headings
                    for (auto it = _listItems.begin(); it != _listItems.end();)
                    {
                        const auto& listItem = *it;
                        if (listItem.type == ListItemType::Heading)
                        {
                            auto nextIt = std::next(it);
                            if (nextIt == _listItems.end() || nextIt->type == ListItemType::Heading)
                            {
                                it = _listItems.erase(it);
                                continue;
                            }
                        }
                        ++it;
                    }
                }
            }
        }

        bool IsScenarioVisible(const ScenarioIndexEntry& scenario) const
        {
            if (static_cast<uint8_t>(scenario.SourceGame) != selectedTab)
            {
                return false;
            }

            return true;
        }

        bool IsLockingEnabled() const
        {
            if (!Config::Get().general.scenarioUnlockingEnabled)
                return false;
            if (selectedTab >= 6)
                return false;

            return true;
        }

        void initTabs()
        {
            uint32_t showPages = 0;
            size_t numScenarios = ScenarioRepositoryGetCount();
            for (size_t i = 0; i < numScenarios; i++)
            {
                const ScenarioIndexEntry* scenario = ScenarioRepositoryGetByIndex(i);
                showPages |= 1 << static_cast<uint8_t>(scenario->SourceGame);
            }

            if (showPages & (1 << Config::Get().interface.scenarioSelectLastTab))
            {
                selectedTab = Config::Get().interface.scenarioSelectLastTab;
            }
            else
            {
                int32_t firstPage = Numerics::bitScanForward(showPages);
                if (firstPage != -1)
                {
                    selectedTab = firstPage;
                }
            }

            int32_t y = widgets[WIDX_TAB1].top;
            for (int32_t i = 0; i < kNumTabs; i++)
            {
                auto& widget = widgets[i + WIDX_TAB1];
                if (!(showPages & (1 << i)))
                {
                    widget.type = WidgetType::empty;
                    continue;
                }

                widget.type = WidgetType::tab;
                widget.top = y;
                widget.bottom = y + (kTabHeight - 1);
                y += kTabHeight;
            }
        }

        static bool ScenarioSelectUseSmallFont()
        {
            return ThemeGetFlags() & UITHEME_FLAG_USE_ALTERNATIVE_SCENARIO_SELECT_FONT;
        }

        static int32_t GetScenarioListItemSize()
        {
            if (!LocalisationService_UseTrueTypeFont())
                return kTrueFontSize;

            // Scenario title
            int32_t lineHeight = FontGetLineHeight(FontStyle::medium);

            // 'Completed by' line
            lineHeight += FontGetLineHeight(FontStyle::small);

            return lineHeight;
        }
    };

    WindowBase* ScenarioselectOpen(ScenarioSelectCallback callback)
    {
        return ScenarioselectOpen([callback](std::string_view scenario) { callback(std::string(scenario).c_str()); });
    }

    WindowBase* ScenarioselectOpen(std::function<void(std::string_view)> callback)
    {
        auto* windowMgr = GetWindowManager();
        auto* window = static_cast<ScenarioSelectWindow*>(windowMgr->BringToFrontByClass(WindowClass::scenarioSelect));
        if (window != nullptr)
        {
            return window;
        }

        window = windowMgr->Create<ScenarioSelectWindow>(
            WindowClass::scenarioSelect, {}, kWindowSize, { WindowFlag::autoPosition, WindowFlag::centreScreen }, callback);
        return window;
    }
} // namespace OpenRCT2::Ui::Windows
