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
#include <openrct2/drawing/Drawing.h>
#include <openrct2/drawing/Rectangle.h>
#include <openrct2/interface/ColourWithFlags.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/localisation/Formatting.h>
#include <openrct2/localisation/LocalisationService.h>
#include <openrct2/object/CampaignObject.h>
#include <openrct2/object/DefaultObjects.h>
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
    static constexpr uint8_t kPadding = 5;
    static constexpr int32_t kPreviewPaneWidthRegular = 180;
    static constexpr int32_t kPreviewPaneWidthScreenshots = 254;
    static constexpr int32_t kWidgetsStart = 17;
    static constexpr int32_t kTabWidth = 92;
    static constexpr int32_t kTabHeight = 34;
    static constexpr int32_t kTabOffsetX = 2;
    // Lower buffer for tabs
    static constexpr int32_t kTabBufferY = kTabHeight / 2;
    static constexpr int32_t kTabPanelWidth = kTabWidth + kTabOffsetX + 11;
    // Number of tabs the window can hold before a scroller is needed
    static constexpr uint8_t kNumTabs = 12;
    static constexpr int32_t kTrueFontSize = 24;
    static constexpr ScreenSize kWindowSize = { 750, 444 };
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
                const std::string* header_text;
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
        WIDX_SCENARIOLIST,
        WIDX_CAMPAIGNLIST,
    };

    // clang-format off
    static constexpr auto _scenarioSelectWidgets = makeWidgets(
        makeWindowShim(kWindowTitle, kWindowSize),
        makeWidget({ kTabPanelWidth, kWidgetsStart }, { kWindowSize.width, 284 },                                WidgetType::resize, WindowColour::secondary), // tab content panel
        makeWidget({ kTabPanelWidth + 2, kWidgetsStart + 1 }, { kWindowSize.width - kPreviewPaneWidthRegular, 362 }, WidgetType::scroll, WindowColour::secondary, SCROLL_VERTICAL), // level list
        makeWidget({ 0, kWidgetsStart}, { kTabPanelWidth, kWindowSize.height  - 17 },                                                     WidgetType::scroll, WindowColour::secondary, SCROLL_VERTICAL)
    );
    // clang-format on

    class ScenarioSelectWindow final : public Window
    {
    private:
        bool _showLockedInformation = false;
        bool _showCampaignScroller = false;
        std::function<void(std::string_view)> _callback;
        std::vector<ScenarioListItem> _listItems;
        const ScenarioIndexEntry* _highlightedScenario = nullptr;
        ParkPreview _preview;
        BackgroundWorker::Job _previewLoadJob;

        struct CampaignTabInfo
        {
            const CampaignObject* CampaignEntry;
            StringId TitleStringId;
            u8string TitleString;
        };
        std::vector<CampaignTabInfo> _tabEntries;

    public:
        ScenarioSelectWindow(std::function<void(std::string_view)> callback)
            : _callback(callback)
        {
        }

        void onOpen() override
        {
            setWidgets(_scenarioSelectWidgets);

            _highlightedScenario = nullptr;
            initTabs();
            initialiseListItems();
            initScrollWidgets();
        }

        void onLanguageChange() override
        {
            _listItems.clear();
            initTabs();
            initialiseListItems();
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            if (widgetIndex == WIDX_CLOSE)
            {
                close();
            }
        }

        void onClose() override
        {
            // Unload campaigns
            auto& objManager = GetContext()->GetObjectManager();
            objManager.UnloadAllType(ObjectType::campaign);
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

        ScreenCoordsXY DrawPreview(Drawing::RenderTarget& rt, ScreenCoordsXY screenPos)
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

        void onDraw(Drawing::RenderTarget& rt) override
        {
            drawWidgets(rt);

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
                const auto shortPath = ShortenPath(scenario->Path, width - 6 - kTabPanelWidth, FontStyle::medium);

                auto ft = Formatter();
                ft.Add<utf8*>(shortPath.c_str());
                DrawTextBasic(
                    rt, windowPos + ScreenCoordsXY{ kTabPanelWidth + 3, height - 3 - 11 }, STR_STRING, ft, { colours[1] });
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
            const int32_t bottomMargin = Config::Get().general.debuggingTools ? 17 : 5;
            widgets[WIDX_SCENARIOLIST].right = width - GetPreviewPaneWidth() - 2 * kPadding;
            widgets[WIDX_SCENARIOLIST].bottom = height - bottomMargin;
        }

        ScreenSize onScrollGetSize(int32_t scrollIndex) override
        {
            if (scrollIndex == 0)
            {
                // Scenario List
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
            else
            {
                // Campaign List
                return { kWindowSize.width, kTabBufferY + (static_cast<int32_t>(_tabEntries.size()) * kTabHeight) };
            }
        }

        void onScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            // Scenario list only, Campaign tabs have no hover behavior
            if (scrollIndex == 0)
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
        }

        void onScrollMouseDownScenarioList(const ScreenCoordsXY& screenCoords)
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
                            OpenRCT2::Audio::Play(OpenRCT2::Audio::SoundId::click1, 0, windowPos.x + (width / 2));
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

        void onScrollMouseDownCampaignList(const ScreenCoordsXY& screenCoords)
        {
            int32_t mouseTabY = screenCoords.y / kTabHeight;
            if (mouseTabY >= 0 && mouseTabY < static_cast<int32_t>(_tabEntries.size()))
            {
                selectedTab = mouseTabY;

                Config::Get().interface.scenarioSelectLastTab = selectedTab;
                Config::Save();

                _highlightedScenario = nullptr;
                _preview = {};

                initialiseListItems();
                invalidate();
                onResize();
                onPrepareDraw();

                WindowResetScrollWidget(*this, 0);
                invalidate();
            }
        }

        void onScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            if (scrollIndex == 0)
            {
                onScrollMouseDownScenarioList(screenCoords);
            }
            else
            {
                onScrollMouseDownCampaignList(screenCoords);
            }
        }

        void onScrollDrawScenarioList(Drawing::RenderTarget& rt)
        {
            auto paletteIndex = ColourMapA[colours[1].colour].mid_light;
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
                        DrawCategoryHeading(rt, horizontalRuleMargin, listWidth - horizontalRuleMargin, y + 2, listItem);
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
                                                 : ColourWithFlags{ COLOUR_BLACK };
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

        void onScrollDrawCampaignList(RenderTarget& rt)
        {
            StringId format = STR_WINDOW_COLOUR_2_STRINGID;
            FontStyle fontStyle = FontStyle::medium;

            if (ScenarioSelectUseSmallFont())
            {
                format = STR_SMALL_WINDOW_COLOUR_2_STRINGID;
                fontStyle = FontStyle::small;
            }

            ScreenCoordsXY tabCenter = { (kTabWidth / 2) - 1, (kTabHeight / 2) - 4 };
            int32_t tabScrollerOffset = (!_showCampaignScroller) ? 11 : 0;
            for (int32_t i = 0; i < static_cast<int32_t>(_tabEntries.size()); i++)
            {
                ScreenCoordsXY tabXY = { kTabOffsetX + tabScrollerOffset, i * kTabHeight };

                ImageIndex tabSprite = (i == selectedTab) ? SPR_G2_SIDEWAYS_TAB_ACTIVE : SPR_G2_SIDEWAYS_TAB;
                GfxDrawSprite(rt, ImageId(static_cast<uint32_t>(tabSprite), colours[1].colour), tabXY);

                CampaignTabInfo tabEntry = _tabEntries.at(i);
                auto ft = Formatter();

                ft.Add<StringId>(tabEntry.TitleStringId);
                if (tabEntry.TitleStringId != STR_OTHER_PARKS)
                {
                    ft.Add<utf8*>(tabEntry.TitleString.c_str());
                }

                DrawTextWrapped(rt, tabXY + tabCenter, 87, format, ft, { COLOUR_AQUAMARINE, fontStyle, TextAlignment::centre });
            }
        }

        void onScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            if (scrollIndex == 0)
            {
                onScrollDrawScenarioList(rt);
            }
            else
            {
                onScrollDrawCampaignList(rt);
            }
        }

    private:
        void DrawCategoryHeading(Drawing::RenderTarget& rt, int32_t left, int32_t right, int32_t y, const ScenarioListItem& headerItem) const
        {
            auto baseColour = colours[1];
            auto lightColour = ColourMapA[baseColour.colour].lighter;
            auto darkColour = ColourMapA[baseColour.colour].mid_dark;

            int32_t centreX = (left + right) / 2;

            StringId stringId;
            std::string stringBuffer;
            // Draw string
            auto ft = Formatter();

            if (headerItem.heading.string_id == STR_STRING && headerItem.heading.header_text != nullptr)
            {
                stringId = STR_STRING;
                stringBuffer = *headerItem.heading.header_text;
                ft.Add<const char*>(stringBuffer.c_str());
            }
            else
            {
                stringId = STR_STRINGID;
                stringBuffer = OpenRCT2::FormatStringID(headerItem.heading.string_id);
                ft.Add<StringId>(headerItem.heading.string_id);
            }
            DrawTextBasic(rt, { centreX, y }, stringId, ft, { baseColour, TextAlignment::centre });

            // Get string dimensions
            int32_t categoryStringHalfWidth = (GfxGetStringWidth(stringBuffer, FontStyle::medium) / 2) + 4;
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

        bool ScenarioUsedInCampaign(const ScenarioIndexEntry* scenario)
        {
            for (CampaignTabInfo tab : _tabEntries)
            {
                // Exclude misc tab
                if (tab.CampaignEntry != nullptr)
                {
                    for (const auto& scenarioRef : tab.CampaignEntry->GetScenarioList())
                    {
                        if (scenarioRef == scenario)
                        {
                            return true;
                        }
                    }
                }
            }
            return false;
        }

        void AddCustomCategoryHeaders(const CampaignObject* campaign, const CampaignGroup* group)
        {
            StringId headerStringID = STR_STRING;
            const std::string* headerString = nullptr;
            switch (group->headerType)
            {
                case HeaderType::headerTypePreset:
                    headerStringID = Scenario::kScenarioCategoryStringIds[EnumValue(group->headerPreset)];
                    break;
                case HeaderType::headerTypeLocalized:
                    headerString = new std::string(campaign->GetCustomString(&group->headerString));
                    break;
                case HeaderType::headerTypeDirect:
                    headerString = &(group->headerString);
                    break;
                default:
                    // Do nothing here
                    return;
            }
            auto* headerItem = new ScenarioListItem();
            headerItem->type = ListItemType::Heading;
            headerItem->heading.string_id = headerStringID;
            headerItem->heading.header_text = headerString;
            _listItems.push_back(std::move(*headerItem));
        }

        Scenario::Category AddDefaultCategoryHeaders(const ScenarioIndexEntry* scenario, Scenario::Category currentHeading)
        {
            StringId headingStringId = kStringIdNone;
            Scenario::Category nextHeading = currentHeading;
            if (currentHeading != scenario->Category)
            {
                nextHeading = scenario->Category;
                headingStringId = Scenario::kScenarioCategoryStringIds[EnumValue(nextHeading)];
            }

            if (headingStringId != kStringIdNone)
            {
                ScenarioListItem headerItem = {};
                headerItem.type = ListItemType::Heading;
                headerItem.heading.string_id = headingStringId;
                _listItems.push_back(std::move(headerItem));
            }

            return nextHeading;
        }

        void AddScenario(const ScenarioIndexEntry* scenario, bool locked)
        {
            ScenarioListItem scenarioItem = {};
            scenarioItem.type = ListItemType::Scenario;
            scenarioItem.scenario.scenario = scenario;

            scenarioItem.scenario.is_locked = (IsLockingEnabled()) ? locked : false;
            _listItems.push_back(std::move(scenarioItem));
        }

        void initialiseListItems()
        {
            _listItems.clear();

            CampaignTabInfo tabEntry = _tabEntries.at(selectedTab);

            // Used for auto-generated headers, if none are defined in campaign
            Scenario::Category currentHeading = Scenario::Category::none;
            if (tabEntry.TitleStringId == STR_OTHER_PARKS)
            {
                // Populate misc tab with all scenarios not linked to campaign objects
                int32_t numScenarios = static_cast<int32_t>(ScenarioRepositoryGetCount());

                for (int32_t i = 0; i < numScenarios; ++i)
                {
                    const auto& scenarioEntry = ScenarioRepositoryGetByIndex(i);
                    if (!ScenarioUsedInCampaign(scenarioEntry))
                    {
                        // Always use default headers for misc tab
                        currentHeading = AddDefaultCategoryHeaders(scenarioEntry, currentHeading);
                        AddScenario(scenarioEntry, false);
                    }
                }
            }
            else
            {
                const CampaignObject* campaign = tabEntry.CampaignEntry;

                bool useClassicProgression = campaign->GetNumUnlockedGroups() > 0;

                // Handle non-positive values as infinite
                const int32_t campaignUnlockedGroups = (campaign->GetNumUnlockedGroups() > 0)
                    ? campaign->GetNumUnlockedGroups()
                    : static_cast<int32_t>(campaign->GetGroupCount());
                const int32_t campaignUnlockedScenarios = (campaign->GetNumUnlockedScenarios() > 0)
                    ? campaign->GetNumUnlockedScenarios()
                    : static_cast<int32_t>(campaign->GetScenarioCount());

                int32_t groupsToUnlock = campaignUnlockedGroups;
                int32_t scenariosToUnlock = campaignUnlockedScenarios;

                // If any scenarios prior to bonus group are incomplete, eligibility is removed
                bool eligibleForBonusScenarios = true;

                int32_t numGroups = static_cast<int32_t>(campaign->GetGroupCount());
                for (int32_t i = 0; i < numGroups; i++)
                {
                    const CampaignGroup* group = campaign->GetGroupAt(i);

                    // Hide / skip bonus groups until all prior scenarios are completed
                    if (group->headerType == HeaderType::headerTypePreset && group->headerPreset == Scenario::Category::bonus
                        && !eligibleForBonusScenarios && Config::Get().general.scenarioHideBonusParks)
                    {
                        continue;
                    }

                    AddCustomCategoryHeaders(campaign, group);

                    // RCT Classic Progression Handling
                    bool groupUnlocked = true;
                    if (useClassicProgression)
                    {
                        // reset scenarios unlocked count per group
                        scenariosToUnlock = campaign->GetNumUnlockedScenarios();
                        groupUnlocked = (groupsToUnlock > 0);
                    }
                    bool groupCompleted = true;

                    int32_t numScenarios = static_cast<int32_t>(group->GetScenarioCount());
                    for (int32_t j = 0; j < numScenarios; j++)
                    {
                        const ScenarioIndexEntry* scenario = group->scenarioList.at(j);

                        bool scenarioUnlocked = groupUnlocked && (scenariosToUnlock > 0);

                        if (scenario != nullptr)
                        {
                            if (group->headerType == HeaderType::headerTypeDefault)
                            {
                                currentHeading = AddDefaultCategoryHeaders(scenario, currentHeading);
                            }
                            if (scenario->Highscore == nullptr)
                            {
                                eligibleForBonusScenarios = false;
                                groupCompleted = false;
                                scenariosToUnlock--;
                            }
                            AddScenario(scenario, !scenarioUnlocked);
                        }
                    }
                    if (!groupCompleted)
                    {
                        groupsToUnlock--;
                    }
                }
            }
        }

        bool IsLockingEnabled() const
        {
            return (
                Config::Get().general.scenarioUnlockingEnabled && _tabEntries.at(selectedTab).TitleStringId != STR_OTHER_PARKS);
        }

        static bool SortCampaigns(const CampaignObject* campaignA, const CampaignObject* campaignB)
        {
            if (campaignA->IsOfficial() ^ campaignB->IsOfficial())
            {
                // One official
                return campaignA->IsOfficial();
            }
            else if (campaignA->IsOfficial())
            {
                // Both official
                return std::find(kDefaultCampaignObjects.begin(), kDefaultCampaignObjects.end(), campaignA->GetIdentifier())
                    < std::find(kDefaultCampaignObjects.begin(), kDefaultCampaignObjects.end(), campaignB->GetIdentifier());
            }
            else
            {
                // Neither official
                // Compare author & title alphabetically
                if (campaignA->GetAuthors().at(0) != campaignB->GetAuthors().at(0))
                {
                    return campaignA->GetAuthors().at(0) < campaignB->GetAuthors().at(0);
                }
                return campaignA->GetName() < campaignB->GetName();
            }
        }

        void initTabs()
        {
            auto& objManager = GetContext()->GetObjectManager();
            objManager.LoadCampaignObjects();
            auto& loadedCampaigns = objManager.GetObjectList(ObjectType::campaign);
            std::sort(loadedCampaigns.begin(), loadedCampaigns.end(), [](const Object* campaignA, const Object* campaignB) {
                return SortCampaigns(
                    static_cast<const CampaignObject*>(campaignA), static_cast<const CampaignObject*>(campaignB));
            });
            for (size_t i = 0; i < loadedCampaigns.size(); i++)
            {
                auto campaignEntry = static_cast<CampaignObject*>(loadedCampaigns.at(i));
                if (campaignEntry != nullptr && campaignEntry->GetScenarioCount() > 0)
                {
                    CampaignTabInfo tab = {};
                    tab.CampaignEntry = campaignEntry;
                    tab.TitleStringId = STR_STRING;
                    tab.TitleString = campaignEntry->GetName();
                    _tabEntries.push_back(tab);
                }
            }
            // Add Misc Tab for all ungrouped scenarios
            CampaignTabInfo miscTab = {};
            miscTab.TitleStringId = STR_OTHER_PARKS;
            _tabEntries.emplace_back(miscTab);

            _showCampaignScroller = (_tabEntries.size() > kNumTabs);
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
