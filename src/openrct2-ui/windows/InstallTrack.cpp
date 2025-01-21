/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "../UiStringIds.h"

#include <memory>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Diagnostic.h>
#include <openrct2/PlatformEnvironment.h>
#include <openrct2/audio/audio.h>
#include <openrct2/core/File.h>
#include <openrct2/core/Path.hpp>
#include <openrct2/core/UnitConversion.h>
#include <openrct2/localisation/Formatter.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/ride/RideConstruction.h>
#include <openrct2/ride/RideData.h>
#include <openrct2/ride/TrackDesign.h>
#include <openrct2/ride/TrackDesignRepository.h>
#include <openrct2/sprites.h>
#include <openrct2/ui/WindowManager.h>
#include <string>
#include <vector>

namespace OpenRCT2::Ui::Windows
{
    enum
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_TRACK_PREVIEW,
        WIDX_ROTATE,
        WIDX_TOGGLE_SCENERY,
        WIDX_INSTALL,
        WIDX_CANCEL
    };

    static constexpr StringId WINDOW_TITLE = STR_TRACK_DESIGN_INSTALL_WINDOW_TITLE;
    static constexpr int32_t WW = 380;
    static constexpr int32_t WH = 460;
    constexpr int32_t PREVIEW_BUTTONS_LEFT = WW - 25;
    constexpr int32_t ACTION_BUTTONS_LEFT = WW - 100;

    // clang-format off
    static constexpr Widget window_install_track_widgets[] = {
        WINDOW_SHIM(WINDOW_TITLE, WW, WH),
        MakeWidget({                   4,  18}, {372, 219}, WindowWidgetType::FlatBtn, WindowColour::Primary                                                              ),
        MakeWidget({PREVIEW_BUTTONS_LEFT, 422}, { 22,  24}, WindowWidgetType::FlatBtn, WindowColour::Primary, ImageId(SPR_ROTATE_ARROW),                     STR_ROTATE_90_TIP     ),
        MakeWidget({PREVIEW_BUTTONS_LEFT, 398}, { 22,  24}, WindowWidgetType::FlatBtn, WindowColour::Primary, ImageId(SPR_SCENERY),                          STR_TOGGLE_SCENERY_TIP),
        MakeWidget({ ACTION_BUTTONS_LEFT, 241}, { 97,  15}, WindowWidgetType::Button,  WindowColour::Primary, STR_INSTALL_NEW_TRACK_DESIGN_INSTALL                        ),
        MakeWidget({ ACTION_BUTTONS_LEFT, 259}, { 97,  15}, WindowWidgetType::Button,  WindowColour::Primary, STR_INSTALL_NEW_TRACK_DESIGN_CANCEL                         ),
    };
    // clang-format on

    class InstallTrackWindow final : public Window
    {
    private:
        std::unique_ptr<TrackDesign> _trackDesign;
        std::string _trackPath;
        std::string _trackName;
        std::vector<uint8_t> _trackDesignPreviewPixels;

    public:
        void SetupTrack(const utf8* path, std::unique_ptr<TrackDesign> trackDesign)
        {
            _trackDesign = std::move(trackDesign);
            _trackPath = path;
            _trackName = GetNameFromTrackPath(path);
            _trackDesignPreviewPixels.resize(4 * kTrackPreviewImageSize);

            UpdatePreview();
            Invalidate();
        }

        void OnOpen() override
        {
            SetWidgets(window_install_track_widgets);

            WindowInitScrollWidgets(*this);
            WindowPushOthersRight(*this);
        }

        void OnClose() override
        {
            _trackPath.clear();
            _trackName.clear();
            _trackDesignPreviewPixels.clear();
            _trackDesignPreviewPixels.shrink_to_fit();
            _trackDesign = nullptr;
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CLOSE:
                case WIDX_CANCEL:
                    Close();
                    break;
                case WIDX_ROTATE:
                    _currentTrackPieceDirection++;
                    _currentTrackPieceDirection %= 4;
                    Invalidate();
                    break;
                case WIDX_TOGGLE_SCENERY:
                    gTrackDesignSceneryToggle = !gTrackDesignSceneryToggle;
                    UpdatePreview();
                    Invalidate();
                    break;
                case WIDX_INSTALL:
                    InstallTrackDesign();
                    break;
            }
        }

        void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            if (widgetIndex != WIDX_INSTALL || text.empty())
            {
                return;
            }

            _trackName = std::string(text);

            OnMouseUp(WIDX_INSTALL);
        }

        void OnPrepareDraw() override
        {
            pressed_widgets |= 1uLL << WIDX_TRACK_PREVIEW;
            if (!gTrackDesignSceneryToggle)
            {
                pressed_widgets |= (1uLL << WIDX_TOGGLE_SCENERY);
            }
            else
            {
                pressed_widgets &= ~(1uLL << WIDX_TOGGLE_SCENERY);
            }
        }

        void OnDraw(DrawPixelInfo& dpi) override
        {
            DrawWidgets(dpi);

            // Track preview
            Widget* widget = &widgets[WIDX_TRACK_PREVIEW];
            auto screenPos = windowPos + ScreenCoordsXY{ widget->left + 1, widget->top + 1 };
            int32_t colour = ColourMapA[colours[0].colour].darkest;
            GfxFillRect(dpi, { screenPos, screenPos + ScreenCoordsXY{ 369, 216 } }, colour);

            G1Element g1temp = {};
            g1temp.offset = _trackDesignPreviewPixels.data() + (_currentTrackPieceDirection * kTrackPreviewImageSize);
            g1temp.width = 370;
            g1temp.height = 217;
            g1temp.flags = G1_FLAG_HAS_TRANSPARENCY;
            GfxSetG1Element(SPR_TEMP, &g1temp);
            DrawingEngineInvalidateImage(SPR_TEMP);
            GfxDrawSprite(dpi, ImageId(SPR_TEMP), screenPos);

            screenPos = windowPos + ScreenCoordsXY{ widget->midX(), widget->bottom - 12 };

            // Warnings
            const TrackDesign& td = *_trackDesign;
            if (td.gameStateData.hasFlag(TrackDesignGameStateFlag::SceneryUnavailable))
            {
                if (!gTrackDesignSceneryToggle)
                {
                    // Scenery not available
                    DrawTextEllipsised(
                        dpi, screenPos, 308, STR_DESIGN_INCLUDES_SCENERY_WHICH_IS_UNAVAILABLE, {}, { TextAlignment::CENTRE });
                    screenPos.y -= kListRowHeight;
                }
            }

            // Information
            screenPos = windowPos + ScreenCoordsXY{ widget->left + 1, widget->bottom + 4 };
            // 0x006D3CF1 -- 0x006d3d71 missing

            // Track design name & type
            {
                auto trackName = _trackName.c_str();
                auto ft = Formatter();
                ft.Add<const char*>(trackName);
                DrawTextBasic(dpi, screenPos - ScreenCoordsXY{ 1, 0 }, STR_TRACK_DESIGN_NAME, ft);
                screenPos.y += kListRowHeight;
            }

            // Friendly Track name
            {
                auto ft = Formatter();

                const auto* objectEntry = ObjectManagerLoadObject(&td.trackAndVehicle.vehicleObject.Entry);
                if (objectEntry != nullptr)
                {
                    auto groupIndex = ObjectManagerGetLoadedObjectEntryIndex(objectEntry);
                    auto rideName = GetRideNaming(td.trackAndVehicle.rtdIndex, *GetRideEntryByIndex(groupIndex));
                    ft.Add<StringId>(rideName.Name);
                }
                else
                {
                    // Fall back on the technical track name if the vehicle object cannot be loaded
                    ft.Add<StringId>(GetRideTypeDescriptor(td.trackAndVehicle.rtdIndex).Naming.Name);
                }

                DrawTextBasic(dpi, screenPos, STR_TRACK_DESIGN_TYPE, ft);
                screenPos.y += kListRowHeight + 4;
            }

            // Stats
            {
                fixed32_2dp rating = td.statistics.ratings.excitement;
                auto ft = Formatter();
                ft.Add<int32_t>(rating);
                DrawTextBasic(dpi, screenPos, STR_TRACK_LIST_EXCITEMENT_RATING, ft);
                screenPos.y += kListRowHeight;
            }
            {
                fixed32_2dp rating = td.statistics.ratings.intensity;
                auto ft = Formatter();
                ft.Add<int32_t>(rating);
                DrawTextBasic(dpi, screenPos, STR_TRACK_LIST_INTENSITY_RATING, ft);
                screenPos.y += kListRowHeight;
            }
            {
                fixed32_2dp rating = td.statistics.ratings.nausea;
                auto ft = Formatter();
                ft.Add<int32_t>(rating);
                DrawTextBasic(dpi, screenPos, STR_TRACK_LIST_NAUSEA_RATING, ft);
                screenPos.y += kListRowHeight + 4;
            }

            const auto& rtd = GetRideTypeDescriptor(td.trackAndVehicle.rtdIndex);
            if (rtd.specialType != RtdSpecialType::maze)
            {
                if (rtd.specialType == RtdSpecialType::miniGolf)
                {
                    // Holes
                    auto ft = Formatter();
                    ft.Add<uint16_t>(td.statistics.holes);
                    DrawTextBasic(dpi, screenPos, STR_HOLES, ft);
                    screenPos.y += kListRowHeight;
                }
                else
                {
                    // Maximum speed
                    {
                        uint16_t speed = ToHumanReadableSpeed(td.statistics.maxSpeed << 16);
                        auto ft = Formatter();
                        ft.Add<uint16_t>(speed);
                        DrawTextBasic(dpi, screenPos, STR_MAX_SPEED, ft);
                        screenPos.y += kListRowHeight;
                    }
                    // Average speed
                    {
                        uint16_t speed = ToHumanReadableSpeed(td.statistics.averageSpeed << 16);
                        auto ft = Formatter();
                        ft.Add<uint16_t>(speed);
                        DrawTextBasic(dpi, screenPos, STR_AVERAGE_SPEED, ft);
                        screenPos.y += kListRowHeight;
                    }
                }

                // Ride length
                auto ft = Formatter();
                ft.Add<StringId>(STR_RIDE_LENGTH_ENTRY);
                ft.Add<uint16_t>(td.statistics.rideLength);
                DrawTextEllipsised(dpi, screenPos, 214, STR_TRACK_LIST_RIDE_LENGTH, ft);
                screenPos.y += kListRowHeight;
            }

            if (GetRideTypeDescriptor(td.trackAndVehicle.rtdIndex).HasFlag(RtdFlag::hasGForces))
            {
                // Maximum positive vertical Gs
                {
                    int32_t gForces = td.statistics.maxPositiveVerticalG;
                    auto ft = Formatter();
                    ft.Add<int32_t>(gForces);
                    DrawTextBasic(dpi, screenPos, STR_MAX_POSITIVE_VERTICAL_G, ft);
                    screenPos.y += kListRowHeight;
                }
                // Maximum negative vertical Gs
                {
                    int32_t gForces = td.statistics.maxNegativeVerticalG;
                    auto ft = Formatter();
                    ft.Add<int32_t>(gForces);
                    DrawTextBasic(dpi, screenPos, STR_MAX_NEGATIVE_VERTICAL_G, ft);
                    screenPos.y += kListRowHeight;
                }
                // Maximum lateral Gs
                {
                    int32_t gForces = td.statistics.maxLateralG;
                    auto ft = Formatter();
                    ft.Add<int32_t>(gForces);
                    DrawTextBasic(dpi, screenPos, STR_MAX_LATERAL_G, ft);
                    screenPos.y += kListRowHeight;
                }
                if (td.statistics.totalAirTime != 0)
                {
                    int32_t airTime = ToHumanReadableAirTime(td.statistics.totalAirTime);
                    auto ft = Formatter();
                    ft.Add<int32_t>(airTime);
                    DrawTextBasic(dpi, screenPos, STR_TOTAL_AIR_TIME, ft);
                    screenPos.y += kListRowHeight;
                }
            }

            if (GetRideTypeDescriptor(td.trackAndVehicle.rtdIndex).HasFlag(RtdFlag::hasDrops))
            {
                auto ft = Formatter();
                ft.Add<uint16_t>(td.statistics.drops);
                DrawTextBasic(dpi, screenPos, STR_DROPS, ft);
                screenPos.y += kListRowHeight;

                // Drop height is multiplied by 0.75
                DrawTextBasic(dpi, screenPos, STR_HIGHEST_DROP_HEIGHT, ft);
                screenPos.y += kListRowHeight;
            }

            if (td.statistics.inversions != 0)
            {
                // Inversions
                auto ft = Formatter();
                ft.Add<uint16_t>(td.statistics.inversions);
                DrawTextBasic(dpi, screenPos, STR_INVERSIONS, ft);
                screenPos.y += kListRowHeight;
            }

            screenPos.y += 4;

            if (!td.statistics.spaceRequired.IsNull())
            {
                // Space required
                auto ft = Formatter();
                ft.Add<uint16_t>(td.statistics.spaceRequired.x);
                ft.Add<uint16_t>(td.statistics.spaceRequired.y);
                DrawTextBasic(dpi, screenPos, STR_TRACK_LIST_SPACE_REQUIRED, ft);
                screenPos.y += kListRowHeight;
            }

            if (td.gameStateData.cost != 0)
            {
                auto ft = Formatter();
                ft.Add<money64>(td.gameStateData.cost);
                DrawTextBasic(dpi, screenPos, STR_TRACK_LIST_COST_AROUND, ft);
            }
        }

        void OnResize() override
        {
            ResizeFrame();
        }

    private:
        void UpdatePreview()
        {
            TrackDesignDrawPreview(*_trackDesign, _trackDesignPreviewPixels.data());
        }

        void InstallTrackDesign()
        {
            auto env = OpenRCT2::GetContext()->GetPlatformEnvironment();
            auto destPath = env->GetDirectoryPath(OpenRCT2::DIRBASE::USER, OpenRCT2::DIRID::TRACK);
            if (!Path::CreateDirectory(destPath))
            {
                LOG_ERROR("Unable to create directory '%s'", destPath.c_str());
                ContextShowError(STR_CANT_SAVE_TRACK_DESIGN, kStringIdNone, {});
                return;
            }

            destPath = Path::Combine(destPath, _trackName + u8".td6");

            if (File::Exists(destPath))
            {
                LOG_INFO("%s already exists, prompting user for a different track design name", destPath.c_str());
                ContextShowError(STR_UNABLE_TO_INSTALL_THIS_TRACK_DESIGN, kStringIdNone, {});
                WindowTextInputRawOpen(
                    this, WIDX_INSTALL, STR_SELECT_NEW_NAME_FOR_TRACK_DESIGN,
                    STR_AN_EXISTING_TRACK_DESIGN_ALREADY_HAS_THIS_NAME, {}, _trackName.c_str(), 255);
            }
            else
            {
                if (TrackRepositoryInstall(_trackPath.c_str(), _trackName.c_str()))
                {
                    Close();
                }
                else
                {
                    ContextShowError(STR_CANT_SAVE_TRACK_DESIGN, kStringIdNone, {});
                }
            }
        }
    };

    WindowBase* InstallTrackOpen(const utf8* path)
    {
        auto trackDesign = TrackDesignImport(path);
        if (trackDesign == nullptr)
        {
            ContextShowError(STR_UNABLE_TO_LOAD_FILE, kStringIdNone, {});
            return nullptr;
        }

        ObjectManagerUnloadAllObjects();
        if (trackDesign->trackAndVehicle.rtdIndex == kRideTypeNull)
        {
            LOG_ERROR("Failed to load track (ride type null): %s", path);
            return nullptr;
        }
        if (ObjectManagerLoadObject(&trackDesign->trackAndVehicle.vehicleObject.Entry) == nullptr)
        {
            LOG_ERROR("Failed to load track (vehicle load fail): %s", path);
            return nullptr;
        }

        auto* windowMgr = Ui::GetWindowManager();
        windowMgr->CloseByClass(WindowClass::EditorObjectSelection);
        windowMgr->CloseConstructionWindows();

        gTrackDesignSceneryToggle = false;
        _currentTrackPieceDirection = 2;

        int32_t screenWidth = ContextGetWidth();
        int32_t screenHeight = ContextGetHeight();
        auto screenPos = ScreenCoordsXY{ screenWidth / 2 - 201, std::max(kTopToolbarHeight + 1, screenHeight / 2 - 200) };

        auto* window = windowMgr->FocusOrCreate<InstallTrackWindow>(WindowClass::InstallTrack, screenPos, WW, WH, 0);
        window->SetupTrack(path, std::move(trackDesign));

        return window;
    }
} // namespace OpenRCT2::Ui::Windows
