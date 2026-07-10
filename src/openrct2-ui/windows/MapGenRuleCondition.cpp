/*****************************************************************************
 * Copyright (c) 2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "openrct2/drawing/ColourMap.h"
#include "openrct2/drawing/Rectangle.h"
#include "openrct2/drawing/Text.h"

#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/interface/Window.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Diagnostic.h>
#include <openrct2/core/UnitConversion.h>
#include <openrct2/drawing/Drawing.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/object/ObjectLimits.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/map_generator/rule/Rule.h>

using namespace OpenRCT2::World::MapGenerator::Rule;
using namespace OpenRCT2::World::MapGenerator;

namespace OpenRCT2::Ui::Windows
{
    class MapGenWindow;
    enum WindowMapGenRuleConditionWidgetIdx
    {
        WIDX_BACKGROUND,
        WIDX_TITLE,
        WIDX_CLOSE,
        WIDX_CANCEL,
        WIDX_OKAY,

        WIDX_CONDITION_LABEL,
        WIDX_PREDICATE,
        WIDX_PREDICATE_DROPDOWN,
        WIDX_VALUE,
        WIDX_VALUE_UP,
        WIDX_VALUE_DOWN,

        WIDX_SEED_OFFSET_LABEL,
        WIDX_SEED_OFFSET,
        WIDX_SEED_OFFSET_UP,
        WIDX_SEED_OFFSET_DOWN,

        WIDX_BASE_FREQUENCY_LABEL,
        WIDX_BASE_FREQUENCY,
        WIDX_BASE_FREQUENCY_UP,
        WIDX_BASE_FREQUENCY_DOWN,

        WIDX_OCTAVES_LABEL,
        WIDX_OCTAVES,
        WIDX_OCTAVES_UP,
        WIDX_OCTAVES_DOWN,

        WIDX_EDGE_LOW_LABEL,
        WIDX_EDGE_LOW,
        WIDX_EDGE_LOW_UP,
        WIDX_EDGE_LOW_DOWN,

        WIDX_EDGE_HIGH_LABEL,
        WIDX_EDGE_HIGH,
        WIDX_EDGE_HIGH_UP,
        WIDX_EDGE_HIGH_DOWN,

        WIDX_LAND_STYLE_SCROLL,

        WIDX_FEATURE_LABEL,
        WIDX_FEATURE,
        WIDX_FEATURE_DROPDOWN,

        WIDX_HEIGHT_MODE_LABEL,
        WIDX_HEIGHT_MODE,
        WIDX_HEIGHT_MODE_DROPDOWN,

        WIDX_HEIGHT_SOURCE_FIRST_LABEL,
        WIDX_HEIGHT_SOURCE_FIRST,
        WIDX_HEIGHT_SOURCE_FIRST_DROPDOWN,

        WIDX_HEIGHT_SOURCE_SECOND_LABEL,
        WIDX_HEIGHT_SOURCE_SECOND,
        WIDX_HEIGHT_SOURCE_SECOND_DROPDOWN,
    };

    static constexpr ScreenSize kWindowSize = { 300, 156 };
    static constexpr ScreenSize kItemSize = { 47, 36 };
    static constexpr auto _widgets = makeWidgets(
        // clang-format off
        makeWindowShim(STR_MAPGEN_RULE_CONDITION_EDIT_WINDOW_TITLE, kWindowSize),
        makeWidget(         { 186, 134 }, { 109, 14 }, WidgetType::button, WindowColour::secondary, STR_CANCEL),
        makeWidget(         {   5, 134 }, { 109, 14 }, WidgetType::button, WindowColour::secondary, STR_OK),

        makeWidget(         {   5,  20 }, { 122, 14 }, WidgetType::label, WindowColour::secondary),
        makeDropdownWidgets({ 133,  20 }, {  48, 14 }, WidgetType::dropdownMenu, WindowColour::secondary, STR_MAPGEN_RULE_PREDICATE_GREATER_THAN),
        makeSpinnerWidgets( { 186,  20 }, { 109, 14 }, WidgetType::spinner, WindowColour::secondary),

        makeWidget(         {   5,  39 }, { 142, 14 }, WidgetType::label, WindowColour::secondary, STR_MAPGEN_SEED_OFFSET),
        makeSpinnerWidgets( { 186,  39 }, { 109, 14 }, WidgetType::spinner, WindowColour::secondary),

        makeWidget(         {   5,  58 }, { 150, 14 }, WidgetType::label, WindowColour::secondary, STR_MAPGEN_SIMPLEX_NOISE_BASE_FREQUENCY),
        makeSpinnerWidgets( { 186,  58 }, { 109, 14 }, WidgetType::spinner, WindowColour::secondary),

        makeWidget(         {   5,  77 }, { 150, 14 }, WidgetType::label, WindowColour::secondary, STR_MAPGEN_SIMPLEX_NOISE_OCTAVES),
        makeSpinnerWidgets( { 186,  77 }, { 109, 14 }, WidgetType::spinner, WindowColour::secondary),

        makeWidget(         {   5,  96 }, { 150, 14 }, WidgetType::label, WindowColour::secondary, STR_MAPGEN_RULE_EDGE_LOW),
        makeSpinnerWidgets( { 186,  96 }, { 109, 14 }, WidgetType::spinner, WindowColour::secondary),

        makeWidget(         {   5, 115 }, { 150, 14 }, WidgetType::label, WindowColour::secondary, STR_MAPGEN_RULE_EDGE_HIGH),
        makeSpinnerWidgets( { 186, 115 }, { 109, 14 }, WidgetType::spinner, WindowColour::secondary),
        
        makeWidget(         {   5,  39 }, { 290, 88 }, WidgetType::scroll, WindowColour::secondary, SCROLL_VERTICAL),

        makeWidget(         {   5,  58 }, { 150, 14 }, WidgetType::label, WindowColour::secondary, STR_MAPGEN_RULE_FEATURE),
        makeDropdownWidgets({ 186,  58 }, { 109, 14 }, WidgetType::dropdownMenu, WindowColour::secondary, STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_FEATURE_WATER),

        makeWidget(         {   5,  39 }, { 150, 14 }, WidgetType::label, WindowColour::secondary, STR_MAPGEN_RULE_CONDITION_HEIGHT_MODE),
        makeDropdownWidgets({ 186,  39 }, { 109, 14 }, WidgetType::dropdownMenu, WindowColour::secondary, STR_MAPGEN_RULE_CONDITION_HEIGHT_MODE_ABSOLUTE),

        makeWidget(         {   5,  58 }, { 150, 14 }, WidgetType::label, WindowColour::secondary, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE),
        makeDropdownWidgets({ 186,  58 }, { 109, 14 }, WidgetType::dropdownMenu, WindowColour::secondary, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_SELF_LAND),

        makeWidget(         {   5,  77 }, { 150, 14 }, WidgetType::label, WindowColour::secondary, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_SUBTRAHEND),
        makeDropdownWidgets({ 186,  77 }, { 109, 14 }, WidgetType::dropdownMenu, WindowColour::secondary, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_GLOBAL_WATER_LEVEL)
        // clang-format on
    );

    class MapGenRuleConditionWindow final : public Window
    {
    private:
        WidgetIdentifier callWidget = {};
        Condition condition = {};
        std::function<void(Condition&)> callback;
        std::vector<ObjectEntryIndex> landStyles;
        std::optional<ObjectEntryIndex> highlightedItem = std::nullopt;

        bool HasParentWindow() const
        {
            return callWidget.window.classification != WindowClass::null;
        }

        MapGenWindow* GetParentWindow() const
        {
            auto* windowMgr = GetWindowManager();
            if (HasParentWindow())
            {
                auto* parent = windowMgr->FindByNumber(callWidget.window.classification, callWidget.window.number);
                return reinterpret_cast<MapGenWindow*>(parent);
            }

            return nullptr;
        }

    public:
        void onOpen() override
        {
            setWidgets(_widgets);
            WindowInitScrollWidgets(*this);
            WindowSetResize(*this, kWindowSize, kWindowSize);

            auto& objectManager = GetContext()->GetObjectManager();
            for (ObjectEntryIndex surfaceIdx = 0; surfaceIdx < kMaxTerrainSurfaceObjects; surfaceIdx++)
            {
                TerrainSurfaceObject* surfaceObj = objectManager.GetLoadedObject<TerrainSurfaceObject>(surfaceIdx);
                if (surfaceObj != nullptr)
                {
                    landStyles.push_back(surfaceIdx);
                }
            }
        }

        void onUpdate() override
        {
            if (HasParentWindow())
            {
                // If the calling window is closed then close this window
                auto parentWindow = GetParentWindow();
                if (parentWindow == nullptr)
                {
                    close();
                }
            }

            if (!widgetIsHighlighted(*this, WIDX_LAND_STYLE_SCROLL))
            {
                highlightedItem = std::nullopt;
                invalidateWidget(WIDX_LAND_STYLE_SCROLL);
            }
        }

        void onPrepareDraw() override
        {
            bool valueVisible = true;
            bool seedVisible = false;
            bool freqVisible = false;
            bool octaVisible = false;
            bool edgeLowVisible = false;
            bool edgeHighVisible = false;
            bool landStyleVisible = false;
            bool featureVisible = false;
            bool heightDropdownsVisible = false;

            switch (condition.type)
            {
                case Type::Height:
                    widgets[WIDX_CONDITION_LABEL].text = STR_MAPGEN_RULE_CONDITION_HEIGHT;
                    heightDropdownsVisible = true;
                    break;
                case Type::Distance:
                    widgets[WIDX_CONDITION_LABEL].text = STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_LABEL;
                    featureVisible = true;
                    break;
                case Type::Noise:
                    widgets[WIDX_CONDITION_LABEL].text = STR_MAPGEN_RULE_CONDITION_NOISE;
                    seedVisible = true;
                    freqVisible = true;
                    octaVisible = true;
                    break;
                case Type::NormalAngle:
                    widgets[WIDX_CONDITION_LABEL].text = STR_MAPGEN_RULE_CONDITION_NORMAL_ANGLE;
                    break;
                case Type::Random:
                    widgets[WIDX_CONDITION_LABEL].text = STR_MAPGEN_RULE_CONDITION_PRNG;
                    seedVisible = true;
                    break;
                case Type::BlendHeight:
                    widgets[WIDX_CONDITION_LABEL].text = STR_MAPGEN_RULE_COND_BLEND_HEIGHT_VERBOSE;
                    valueVisible = false;
                    seedVisible = true;
                    edgeLowVisible = true;
                    edgeHighVisible = true;
                    break;
                case Type::BlendNoise:
                    widgets[WIDX_CONDITION_LABEL].text = STR_MAPGEN_RULE_COND_BLEND_NOISE_VERBOSE;
                    valueVisible = false;
                    seedVisible = true;
                    freqVisible = true;
                    octaVisible = true;
                    edgeLowVisible = true;
                    edgeHighVisible = true;
                    break;
                case Type::BlendDistance:
                    widgets[WIDX_CONDITION_LABEL].text = STR_MAPGEN_RULE_COND_BLEND_DISTANCE_VERBOSE;
                    valueVisible = false;
                    seedVisible = true;
                    edgeLowVisible = true;
                    edgeHighVisible = true;
                    featureVisible = true;
                    break;
                case Type::LandStyle:
                    valueVisible = false;
                    landStyleVisible = true;
                    break;
            }

            //  widgets[WIDX_CONDITION_LABEL] stays visible
            widgets[WIDX_VALUE].type = valueVisible ? WidgetType::spinner : WidgetType::empty;
            widgets[WIDX_VALUE_UP].type = valueVisible ? WidgetType::button : WidgetType::empty;
            widgets[WIDX_VALUE_DOWN].type = valueVisible ? WidgetType::button : WidgetType::empty;

            widgets[WIDX_SEED_OFFSET_LABEL].type = seedVisible ? WidgetType::label : WidgetType::empty;
            widgets[WIDX_SEED_OFFSET].type = seedVisible ? WidgetType::spinner : WidgetType::empty;
            widgets[WIDX_SEED_OFFSET_UP].type = seedVisible ? WidgetType::button : WidgetType::empty;
            widgets[WIDX_SEED_OFFSET_DOWN].type = seedVisible ? WidgetType::button : WidgetType::empty;

            widgets[WIDX_BASE_FREQUENCY_LABEL].type = freqVisible ? WidgetType::label : WidgetType::empty;
            widgets[WIDX_BASE_FREQUENCY].type = freqVisible ? WidgetType::spinner : WidgetType::empty;
            widgets[WIDX_BASE_FREQUENCY_UP].type = freqVisible ? WidgetType::button : WidgetType::empty;
            widgets[WIDX_BASE_FREQUENCY_DOWN].type = freqVisible ? WidgetType::button : WidgetType::empty;

            widgets[WIDX_OCTAVES_LABEL].type = octaVisible ? WidgetType::label : WidgetType::empty;
            widgets[WIDX_OCTAVES].type = octaVisible ? WidgetType::spinner : WidgetType::empty;
            widgets[WIDX_OCTAVES_UP].type = octaVisible ? WidgetType::button : WidgetType::empty;
            widgets[WIDX_OCTAVES_DOWN].type = octaVisible ? WidgetType::button : WidgetType::empty;

            widgets[WIDX_EDGE_LOW_LABEL].type = edgeLowVisible ? WidgetType::label : WidgetType::empty;
            widgets[WIDX_EDGE_LOW].type = edgeLowVisible ? WidgetType::spinner : WidgetType::empty;
            widgets[WIDX_EDGE_LOW_UP].type = edgeLowVisible ? WidgetType::button : WidgetType::empty;
            widgets[WIDX_EDGE_LOW_DOWN].type = edgeLowVisible ? WidgetType::button : WidgetType::empty;

            widgets[WIDX_EDGE_HIGH_LABEL].type = edgeHighVisible ? WidgetType::label : WidgetType::empty;
            widgets[WIDX_EDGE_HIGH].type = edgeHighVisible ? WidgetType::spinner : WidgetType::empty;
            widgets[WIDX_EDGE_HIGH_UP].type = edgeHighVisible ? WidgetType::button : WidgetType::empty;
            widgets[WIDX_EDGE_HIGH_DOWN].type = edgeHighVisible ? WidgetType::button : WidgetType::empty;

            widgets[WIDX_LAND_STYLE_SCROLL].type = landStyleVisible ? WidgetType::scroll : WidgetType::empty;

            widgets[WIDX_FEATURE_LABEL].type = featureVisible ? WidgetType::label : WidgetType::empty;
            widgets[WIDX_FEATURE].type = featureVisible ? WidgetType::dropdownMenu : WidgetType::empty;
            widgets[WIDX_FEATURE_DROPDOWN].type = featureVisible ? WidgetType::button : WidgetType::empty;

            widgets[WIDX_HEIGHT_MODE_LABEL].type = heightDropdownsVisible ? WidgetType::label : WidgetType::empty;
            widgets[WIDX_HEIGHT_MODE].type = heightDropdownsVisible ? WidgetType::dropdownMenu : WidgetType::empty;
            widgets[WIDX_HEIGHT_MODE_DROPDOWN].type = heightDropdownsVisible ? WidgetType::button : WidgetType::empty;

            widgets[WIDX_HEIGHT_SOURCE_FIRST_LABEL].type = heightDropdownsVisible ? WidgetType::label : WidgetType::empty;
            widgets[WIDX_HEIGHT_SOURCE_FIRST].type = heightDropdownsVisible ? WidgetType::dropdownMenu : WidgetType::empty;
            widgets[WIDX_HEIGHT_SOURCE_FIRST_DROPDOWN].type = heightDropdownsVisible ? WidgetType::button : WidgetType::empty;

            widgets[WIDX_HEIGHT_SOURCE_SECOND_LABEL].type = heightDropdownsVisible ? WidgetType::label : WidgetType::empty;
            widgets[WIDX_HEIGHT_SOURCE_SECOND].type = heightDropdownsVisible ? WidgetType::dropdownMenu : WidgetType::empty;
            widgets[WIDX_HEIGHT_SOURCE_SECOND_DROPDOWN].type = heightDropdownsVisible ? WidgetType::button : WidgetType::empty;

            bool isInCond = condition.type == Type::LandStyle;
            switch (condition.predicate)
            {
                case Predicate::Equal:
                    widgets[WIDX_PREDICATE].text = isInCond ? STR_MAPGEN_RULE_PREDICATE_IN : STR_MAPGEN_RULE_PREDICATE_EQUAL;
                    break;
                case Predicate::NotEqual:
                    widgets[WIDX_PREDICATE].text = isInCond ? STR_MAPGEN_RULE_PREDICATE_NOT_IN
                                                            : STR_MAPGEN_RULE_PREDICATE_NOT_EQUAL;
                    break;
                case Predicate::LessThan:
                    widgets[WIDX_PREDICATE].text = STR_MAPGEN_RULE_PREDICATE_LESS_THAN;
                    break;
                case Predicate::GreaterThan:
                    widgets[WIDX_PREDICATE].text = STR_MAPGEN_RULE_PREDICATE_GREATER_THAN;
                    break;
                case Predicate::LessThanOrEqual:
                    widgets[WIDX_PREDICATE].text = STR_MAPGEN_RULE_PREDICATE_LESS_THAN_OR_EQUAL;
                    break;
                case Predicate::GreaterThanOrEqual:
                    widgets[WIDX_PREDICATE].text = STR_MAPGEN_RULE_PREDICATE_GREATER_THAN_OR_EQUAL;
                    break;
                default:
                    throw std::runtime_error("unknown predicate");
            }

            if (featureVisible)
            {
                widgets[WIDX_FEATURE].text = featureToStringId(getFeature());
            }

            if (heightDropdownsVisible)
            {
                auto& heightData = std::get<HeightData>(condition.data);
                bool isAbsMode = heightData.mode == HeightMode::Absolute;

                setWidgetDisabled(WIDX_HEIGHT_SOURCE_SECOND_LABEL, isAbsMode);
                setWidgetDisabled(WIDX_HEIGHT_SOURCE_SECOND, isAbsMode);
                setWidgetDisabled(WIDX_HEIGHT_SOURCE_SECOND_DROPDOWN, isAbsMode);

                if (isAbsMode)
                {
                    widgets[WIDX_HEIGHT_SOURCE_FIRST_LABEL].text = STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE;
                    widgets[WIDX_CONDITION_LABEL].text = STR_MAPGEN_RULE_CONDITION_HEIGHT_VERBOSE_ABSOLUTE;
                }
                else
                {
                    widgets[WIDX_HEIGHT_SOURCE_FIRST_LABEL].text = STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_MINUEND;
                    widgets[WIDX_CONDITION_LABEL].text = STR_MAPGEN_RULE_CONDITION_HEIGHT_VERBOSE_RELATIVE;
                }

                widgets[WIDX_HEIGHT_MODE].text = heightModeToStringId(heightData.mode);
                widgets[WIDX_HEIGHT_SOURCE_FIRST].text = heightSourceToStringId(heightData.sourceFirst);
                widgets[WIDX_HEIGHT_SOURCE_SECOND].text = heightSourceToStringId(heightData.sourceSecond);
            }
        }

        StringId heightModeToStringId(HeightMode& mode)
        {
            switch (mode)
            {
                case HeightMode::Absolute:
                    return STR_MAPGEN_RULE_CONDITION_HEIGHT_MODE_ABSOLUTE;
                case HeightMode::Relative:
                    return STR_MAPGEN_RULE_CONDITION_HEIGHT_MODE_RELATIVE;
                default:
                    throw std::runtime_error("unknown height mode");
            }
        }

        StringId heightSourceToStringId(HeightSource& source)
        {
            switch (source)
            {
                case HeightSource::SelfLand: return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_SELF_LAND;
                case HeightSource::SelfWater: return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_SELF_WATER;
                case HeightSource::NeighbourNWLand: return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_NW_LAND;
                case HeightSource::NeighbourNWWater: return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_NW_WATER;
                case HeightSource::NeighbourNELand: return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_NE_LAND;
                case HeightSource::NeighbourNEWater: return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_NE_WATER;
                case HeightSource::NeighbourSELand: return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_SE_LAND;
                case HeightSource::NeighbourSEWater: return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_SE_WATER;
                case HeightSource::NeighbourSWLand: return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_SW_LAND;
                case HeightSource::NeighbourSWWater: return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_SW_WATER;
                case HeightSource::GlobalMin: return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_GLOBAL_MIN;
                case HeightSource::GlobalMax: return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_GLOBAL_MAX;
                case HeightSource::GlobalWaterLevel: return STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_GLOBAL_WATER_LEVEL;
                default:
                    throw std::runtime_error("unknown height source");
            }
        }

        StringId featureToStringId(Feature& feature)
        {
            switch (feature)
            {
                case Feature::Water:
                    return STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_FEATURE_WATER;
                case Feature::River:
                    return STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_FEATURE_RIVER;
                case Feature::Sea:
                    return STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_FEATURE_SEA;
                case Feature::MapBorder:
                    return STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_FEATURE_BORDER;
                case Feature::Fill:
                    return STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_FEATURE_FILL;
                case Feature::Breach:
                    return STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_FEATURE_BREACH;
                case Feature::Land:
                    return STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_FEATURE_LAND;
                default:
                    throw std::runtime_error("unknown feature");
            }
        }

        void onDraw(Drawing::RenderTarget& rt) override
        {
            drawWidgets(rt);

            switch (condition.type)
            {
                case Type::Height:
                {
                    auto ft = Formatter();
                    ft.Add<int16_t>(static_cast<int16_t>(BaseZToMetres(std::get<HeightData>(condition.data).height)));
                    drawText(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_VALUE].left + 1, widgets[WIDX_VALUE].top + 1 },
                        STR_RIDE_LENGTH_ENTRY, ft, { colours[1] });
                    break;
                }
                case Type::Distance:
                {
                    auto ft = Formatter();
                    ft.Add<int16_t>(static_cast<int16_t>(std::get<DistanceData>(condition.data).distance));
                    drawText(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_VALUE].left + 1, widgets[WIDX_VALUE].top + 1 },
                        STR_RIDE_LENGTH_ENTRY, ft, { colours[1] });
                    break;
                }
                case Type::Noise:
                {
                    auto ft = Formatter();
                    auto& noiseData = std::get<NoiseData>(condition.data);
                    ft.Add<int32_t>(static_cast<int32_t>(noiseData.value * 100));
                    drawText(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_VALUE].left + 1, widgets[WIDX_VALUE].top + 1 },
                        STR_COMMA2DP32, ft, { colours[1] });

                    ft = Formatter();
                    ft.Add<int32_t>(static_cast<int32_t>(noiseData.frequency * 100));
                    drawText(
                        rt,
                        windowPos
                            + ScreenCoordsXY{ widgets[WIDX_BASE_FREQUENCY].left + 1, widgets[WIDX_BASE_FREQUENCY].top + 1 },
                        STR_COMMA2DP32, ft, { colours[1] });

                    ft = Formatter();
                    ft.Add<int32_t>(noiseData.octaves);
                    drawText(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_OCTAVES].left + 1, widgets[WIDX_OCTAVES].top + 1 },
                        STR_FORMAT_INTEGER, ft, { colours[1] });

                    ft = Formatter();
                    ft.Add<int32_t>(static_cast<int32_t>(noiseData.seedOffset));
                    drawText(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_SEED_OFFSET].left + 1, widgets[WIDX_SEED_OFFSET].top + 1 },
                        STR_FORMAT_INTEGER, ft, { colours[1] });
                    break;
                }
                case Type::NormalAngle:
                {
                    auto ft = Formatter();
                    ft.Add<int32_t>(static_cast<int32_t>(std::get<NormalAngleData>(condition.data).angle * 100));
                    drawText(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_VALUE].left + 1, widgets[WIDX_VALUE].top + 1 },
                        STR_COMMA2DP32, ft, { colours[1] });
                    break;
                }
                case Type::Random:
                {
                    auto ft = Formatter();
                    auto& prngData = std::get<RandomData>(condition.data);
                    ft.Add<int32_t>(static_cast<int32_t>(prngData.value * 100));
                    drawText(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_VALUE].left + 1, widgets[WIDX_VALUE].top + 1 },
                        STR_COMMA2DP32, ft, { colours[1] });

                    ft = Formatter();
                    ft.Add<int32_t>(static_cast<int32_t>(prngData.seedOffset));
                    drawText(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_SEED_OFFSET].left + 1, widgets[WIDX_SEED_OFFSET].top + 1 },
                        STR_FORMAT_INTEGER, ft, { colours[1] });
                    break;
                }
                case Type::BlendHeight:
                {
                    auto& blendHeightData = std::get<BlendHeightData>(condition.data);

                    auto ft = Formatter();
                    ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_PRNG);
                    drawText(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_VALUE].left + 1, widgets[WIDX_VALUE].top + 1 },
                        STR_STRINGID, ft, { colours[1] });

                    ft = Formatter();
                    ft.Add<int16_t>(static_cast<int16_t>(BaseZToMetres(blendHeightData.edgeLow)));
                    drawText(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_EDGE_LOW].left + 1, widgets[WIDX_EDGE_LOW].top + 1 },
                        STR_RIDE_LENGTH_ENTRY, ft, { colours[1] });

                    ft = Formatter();
                    ft.Add<int16_t>(static_cast<int16_t>(BaseZToMetres(blendHeightData.edgeHigh)));
                    drawText(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_EDGE_HIGH].left + 1, widgets[WIDX_EDGE_HIGH].top + 1 },
                        STR_RIDE_LENGTH_ENTRY, ft, { colours[1] });

                    ft = Formatter();
                    ft.Add<int32_t>(static_cast<int32_t>(blendHeightData.seedOffset));
                    drawText(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_SEED_OFFSET].left + 1, widgets[WIDX_SEED_OFFSET].top + 1 },
                        STR_FORMAT_INTEGER, ft, { colours[1] });
                    break;
                }
                case Type::BlendNoise:
                {
                    auto& blendNoiseData = std::get<BlendNoiseData>(condition.data);

                    auto ft = Formatter();
                    ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_PRNG);
                    drawText(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_VALUE].left + 1, widgets[WIDX_VALUE].top + 1 },
                        STR_STRINGID, ft, { colours[1] });

                    ft = Formatter();
                    ft.Add<int32_t>(static_cast<int32_t>(blendNoiseData.edgeLow * 100));
                    drawText(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_EDGE_LOW].left + 1, widgets[WIDX_EDGE_LOW].top + 1 },
                        STR_COMMA2DP32, ft, { colours[1] });

                    ft = Formatter();
                    ft.Add<int32_t>(static_cast<int32_t>(blendNoiseData.edgeHigh * 100));
                    drawText(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_EDGE_HIGH].left + 1, widgets[WIDX_EDGE_HIGH].top + 1 },
                        STR_COMMA2DP32, ft, { colours[1] });

                    ft = Formatter();
                    ft.Add<int32_t>(static_cast<int32_t>(blendNoiseData.frequency * 100));
                    drawText(
                        rt,
                        windowPos
                            + ScreenCoordsXY{ widgets[WIDX_BASE_FREQUENCY].left + 1, widgets[WIDX_BASE_FREQUENCY].top + 1 },
                        STR_COMMA2DP32, ft, { colours[1] });

                    ft = Formatter();
                    ft.Add<int32_t>(blendNoiseData.octaves);
                    drawText(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_OCTAVES].left + 1, widgets[WIDX_OCTAVES].top + 1 },
                        STR_FORMAT_INTEGER, ft, { colours[1] });

                    ft = Formatter();
                    ft.Add<int32_t>(static_cast<int32_t>(blendNoiseData.seedOffset));
                    drawText(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_SEED_OFFSET].left + 1, widgets[WIDX_SEED_OFFSET].top + 1 },
                        STR_FORMAT_INTEGER, ft, { colours[1] });

                    break;
                }
                case Type::BlendDistance:
                {
                    auto& blendDistanceData = std::get<BlendDistanceData>(condition.data);

                    auto ft = Formatter();
                    ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_PRNG);
                    drawText(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_VALUE].left + 1, widgets[WIDX_VALUE].top + 1 },
                        STR_STRINGID, ft, { colours[1] });

                    ft = Formatter();
                    ft.Add<int16_t>(static_cast<int16_t>(blendDistanceData.edgeLow));
                    drawText(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_EDGE_LOW].left + 1, widgets[WIDX_EDGE_LOW].top + 1 },
                        STR_RIDE_LENGTH_ENTRY, ft, { colours[1] });

                    ft = Formatter();
                    ft.Add<int16_t>(static_cast<int16_t>(blendDistanceData.edgeHigh));
                    drawText(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_EDGE_HIGH].left + 1, widgets[WIDX_EDGE_HIGH].top + 1 },
                        STR_RIDE_LENGTH_ENTRY, ft, { colours[1] });

                    ft = Formatter();
                    ft.Add<int32_t>(static_cast<int32_t>(blendDistanceData.seedOffset));
                    drawText(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_SEED_OFFSET].left + 1, widgets[WIDX_SEED_OFFSET].top + 1 },
                        STR_FORMAT_INTEGER, ft, { colours[1] });
                    break;
                }
                case Type::LandStyle:
                {
                    auto ft = Formatter();
                    ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_LAND_STYLE);
                    drawText(
                        rt,
                        windowPos
                            + ScreenCoordsXY{ widgets[WIDX_CONDITION_LABEL].left + 1, widgets[WIDX_CONDITION_LABEL].top + 1 },
                        STR_STRINGID, ft, { colours[1] });
                    break;
                }
            }
        }

        void SetCallingContext(WindowBase* _callWindow, uint16_t _callWidget)
        {
            callWidget.window.classification = _callWindow->classification;
            callWidget.window.number = _callWindow->number;
            callWidget.widgetIndex = _callWidget;

            colours[0] = _callWindow->colours[1];
            colours[1] = _callWindow->colours[1];
            colours[2] = _callWindow->colours[1];
        }

        void SetCondition(const Condition& _condition)
        {
            condition = _condition;
        }

        void SetCallback(const std::function<void(Condition&)>& _callback)
        {
            callback = _callback;
        }

        static int32_t MetresToBaseZCast(const int32_t metres)
        {
            return static_cast<int32_t>(MetresToBaseZ(static_cast<int16_t>(metres)));
        }

        template<typename T>
        static std::optional<T> mapOptional(std::optional<T> maybeT, const std::function<T(T)>& fn)
        {
            if (!maybeT.has_value())
            {
                return std::nullopt;
            }

            return fn(maybeT.value());
        }

        void HandleValueChange(const Change change, std::optional<int32_t> intValue, std::optional<float> floatValue)
        {
            switch (condition.type)
            {
                case Type::Height:
                {
                    auto& elevationData = std::get<HeightData>(condition.data);
                    elevationData.height.change(change, mapOptional<int32_t>(intValue, MetresToBaseZCast));
                    break;
                }
                case Type::Distance:
                {
                    auto& distanceData = std::get<DistanceData>(condition.data);
                    distanceData.distance.change(change, floatValue);
                    break;
                }
                case Type::Noise:
                {
                    auto& noiseData = std::get<NoiseData>(condition.data);
                    noiseData.value.change(change, floatValue);
                    break;
                }
                case Type::NormalAngle:
                {
                    auto& normalAngleData = std::get<NormalAngleData>(condition.data);
                    normalAngleData.angle.change(change, floatValue);
                    break;
                }
                case Type::Random:
                {
                    auto& prngData = std::get<RandomData>(condition.data);
                    prngData.value.change(change, floatValue);
                    break;
                }
                default:
                {
                    throw std::runtime_error("value change: unexpected condition type");
                }
            }
        }

        void HandleEdgeLowChange(const Change change, std::optional<int32_t> intValue, std::optional<float> floatValue)
        {
            switch (condition.type)
            {
                case Type::BlendHeight:
                {
                    auto& blendHeightData = std::get<BlendHeightData>(condition.data);
                    blendHeightData.edgeLow.change(change, mapOptional<int32_t>(intValue, MetresToBaseZCast));
                    if (blendHeightData.edgeLow > blendHeightData.edgeHigh)
                    {
                        blendHeightData.edgeHigh = blendHeightData.edgeLow;
                    }
                    break;
                }
                case Type::BlendNoise:
                {
                    auto& blendNoiseData = std::get<BlendNoiseData>(condition.data);
                    blendNoiseData.edgeLow.change(change, floatValue);
                    if (blendNoiseData.edgeLow > blendNoiseData.edgeHigh)
                    {
                        blendNoiseData.edgeHigh = blendNoiseData.edgeLow;
                    }
                    break;
                }
                case Type::BlendDistance:
                {
                    auto& blendDistanceData = std::get<BlendDistanceData>(condition.data);
                    blendDistanceData.edgeLow.change(change, floatValue);
                    if (blendDistanceData.edgeLow > blendDistanceData.edgeHigh)
                    {
                        blendDistanceData.edgeHigh = blendDistanceData.edgeLow;
                    }
                    break;
                }
                default:
                {
                    throw std::runtime_error("edge low change: unexpected condition type");
                }
            }
        }

        void HandleEdgeHighChange(const Change change, std::optional<int32_t> intValue, std::optional<float> floatValue)
        {
            switch (condition.type)
            {
                case Type::BlendHeight:
                {
                    auto& blendHeightData = std::get<BlendHeightData>(condition.data);
                    blendHeightData.edgeHigh.change(change, mapOptional<int32_t>(intValue, MetresToBaseZCast));
                    if (blendHeightData.edgeLow > blendHeightData.edgeHigh)
                    {
                        blendHeightData.edgeLow = blendHeightData.edgeHigh;
                    }
                    break;
                }
                case Type::BlendNoise:
                {
                    auto& blendNoiseData = std::get<BlendNoiseData>(condition.data);
                    blendNoiseData.edgeHigh.change(change, floatValue);
                    if (blendNoiseData.edgeLow > blendNoiseData.edgeHigh)
                    {
                        blendNoiseData.edgeLow = blendNoiseData.edgeHigh;
                    }
                    break;
                }
                case Type::BlendDistance:
                {
                    auto& blendDistanceData = std::get<BlendDistanceData>(condition.data);
                    blendDistanceData.edgeHigh.change(change, floatValue);
                    if (blendDistanceData.edgeLow > blendDistanceData.edgeHigh)
                    {
                        blendDistanceData.edgeHigh = blendDistanceData.edgeLow;
                    }
                    break;
                }
                default:
                {
                    throw std::runtime_error("edge high change: unexpected condition type");
                }
            }
        }

        void HandleSeedOffsetChange(const Change change, std::optional<int32_t> seedOffset)
        {
            switch (condition.type)
            {
                case Type::Noise:
                {
                    auto& noiseData = std::get<NoiseData>(condition.data);
                    noiseData.seedOffset.change(change, seedOffset);
                    break;
                }
                case Type::Random:
                {
                    auto& prngData = std::get<RandomData>(condition.data);
                    prngData.seedOffset.change(change, seedOffset);
                    break;
                }
                case Type::BlendHeight:
                {
                    auto& blendHeightData = std::get<BlendHeightData>(condition.data);
                    blendHeightData.seedOffset.change(change, seedOffset);
                    break;
                }
                case Type::BlendNoise:
                {
                    auto& blendNoiseData = std::get<BlendNoiseData>(condition.data);
                    blendNoiseData.seedOffset.change(change, seedOffset);
                    break;
                }
                default:
                {
                    throw std::runtime_error("seed offset change: unexpected condition type");
                }
            }
        }

        void HandleFrequencyChange(const Change change, std::optional<float> frequency)
        {
            switch (condition.type)
            {
                case Type::Noise:
                {
                    auto& noiseData = std::get<NoiseData>(condition.data);
                    noiseData.frequency.change(change, frequency);
                    break;
                }
                case Type::BlendNoise:
                {
                    auto& blendNoiseData = std::get<BlendNoiseData>(condition.data);
                    blendNoiseData.frequency.change(change, frequency);
                    break;
                }
                default:
                {
                    throw std::runtime_error("frequency change: unexpected condition type");
                }
            }
        }

        void HandleOctavesChange(const Change change, std::optional<int32_t> octaves)
        {
            switch (condition.type)
            {
                case Type::Noise:
                {
                    auto& noiseData = std::get<NoiseData>(condition.data);
                    noiseData.octaves.change(change, octaves);
                    break;
                }
                case Type::BlendNoise:
                {
                    auto& blendNoiseData = std::get<BlendNoiseData>(condition.data);
                    blendNoiseData.octaves.change(change, octaves);
                    break;
                }
                default:
                {
                    throw std::runtime_error("octaves change: unexpected condition type");
                }
            }
        }

        void ShowValueTextInput()
        {
            switch (condition.type)
            {
                case Type::Height:
                {
                    Formatter ft;
                    ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_HEIGHT);
                    ft.Add<int16_t>(static_cast<int16_t>(kHeightMin));
                    ft.Add<int16_t>(static_cast<int16_t>(kHeightMax));
                    WindowTextInputOpen(
                        this, WIDX_VALUE, STR_MAPGEN_RULE_CONDITION_HEIGHT, STR_MAPGEN_RULE_ENTER_LENGTH, ft,
                        STR_FORMAT_INTEGER, BaseZToMetres(std::get<HeightData>(condition.data).height), 3);
                    break;
                }
                case Type::Distance:
                {
                    Formatter ft;
                    ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_DISTANCE_TO);
                    ft.Add<int16_t>(static_cast<int16_t>(kDistanceMin));
                    ft.Add<int16_t>(static_cast<int16_t>(kDistanceMax));
                    WindowTextInputOpen(
                        this, WIDX_VALUE, STR_MAPGEN_RULE_CONDITION_DISTANCE_TO, STR_MAPGEN_RULE_ENTER_LENGTH, ft,
                        STR_FORMAT_INTEGER, std::get<DistanceData>(condition.data).distance, 4);
                    break;
                }
                case Type::Noise:
                {
                    Formatter ft;
                    ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_NOISE);
                    ft.Add<int32_t>(static_cast<int32_t>(kNoiseMin * 100));
                    ft.Add<int32_t>(static_cast<int32_t>(kNoiseMax * 100));
                    WindowTextInputOpen(
                        this, WIDX_VALUE, STR_MAPGEN_RULE_CONDITION_NOISE, STR_MAPGEN_RULE_ENTER_FLOAT, ft, STR_COMMA2DP32,
                        static_cast<int32_t>(std::get<NoiseData>(condition.data).value * 100), 4);
                    break;
                }
                case Type::NormalAngle:
                {
                    Formatter ft;
                    ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_NORMAL_ANGLE);
                    ft.Add<int32_t>(static_cast<int32_t>(kAngleMin * 100));
                    ft.Add<int32_t>(static_cast<int32_t>(kAngleMax * 100));
                    WindowTextInputOpen(
                        this, WIDX_VALUE, STR_MAPGEN_RULE_CONDITION_NORMAL_ANGLE, STR_MAPGEN_RULE_ENTER_FLOAT, ft,
                        STR_COMMA2DP32, static_cast<int32_t>(std::get<NormalAngleData>(condition.data).angle * 100), 5);
                    break;
                }
                case Type::Random:
                {
                    Formatter ft;
                    ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_PRNG);
                    ft.Add<int32_t>(static_cast<int32_t>(kAngleMin * 100));
                    ft.Add<int32_t>(static_cast<int32_t>(kAngleMax * 100));
                    WindowTextInputOpen(
                        this, WIDX_VALUE, STR_MAPGEN_RULE_CONDITION_PRNG, STR_MAPGEN_RULE_ENTER_FLOAT, ft, STR_COMMA2DP32,
                        static_cast<int32_t>(std::get<RandomData>(condition.data).value * 100), 4);
                    break;
                }
                default:
                    throw std::runtime_error("show value text input: unexpected condition type");
            }
        }

        void ShowFrequencyTextInput()
        {
            int32_t frequency = 0;
            switch (condition.type)
            {
                case Type::Noise:
                    frequency = static_cast<int32_t>(std::get<NoiseData>(condition.data).frequency * 100);
                    break;
                case Type::BlendNoise:
                    frequency = static_cast<int32_t>(std::get<BlendNoiseData>(condition.data).frequency * 100);
                    break;
                default:
                    throw std::runtime_error("show frequency text input: unexpected condition type");
            }

            Formatter ft;
            ft.Add<StringId>(STR_MAPGEN_SIMPLEX_NOISE_BASE_FREQUENCY);
            ft.Add<int32_t>(static_cast<int32_t>(kFrequencyMin * 100));
            ft.Add<int32_t>(static_cast<int32_t>(kFrequencyMax * 100));
            WindowTextInputOpen(
                this, WIDX_BASE_FREQUENCY, STR_MAPGEN_SIMPLEX_NOISE_BASE_FREQUENCY, STR_MAPGEN_RULE_ENTER_FLOAT, ft,
                STR_FORMAT_COMMA2DP32, frequency, 5);
        }

        void ShowOctavesTextInput()
        {
            int32_t octaves = 0;
            switch (condition.type)
            {
                case Type::Noise:
                    octaves = std::get<NoiseData>(condition.data).octaves;
                    break;
                case Type::BlendNoise:
                    octaves = std::get<BlendNoiseData>(condition.data).octaves;
                    break;
                default:
                    throw std::runtime_error("show octaves text input: unexpected condition type");
            }

            Formatter ft;
            ft.Add<StringId>(STR_MAPGEN_SIMPLEX_NOISE_OCTAVES);
            ft.Add<int32_t>(kOctavesMin);
            ft.Add<int32_t>(kOctavesMax);
            WindowTextInputOpen(
                this, WIDX_OCTAVES, STR_MAPGEN_SIMPLEX_NOISE_OCTAVES, STR_MAPGEN_RULE_ENTER_INTEGER, ft, STR_FORMAT_INTEGER,
                octaves, 2);
        }

        void ShowSeedOffsetTextInput()
        {
            uint32_t seedOffset = 0;
            switch (condition.type)
            {
                case Type::Noise:
                    seedOffset = std::get<NoiseData>(condition.data).seedOffset;
                    break;
                case Type::Random:
                    seedOffset = std::get<RandomData>(condition.data).seedOffset;
                    break;
                case Type::BlendHeight:
                    seedOffset = std::get<BlendHeightData>(condition.data).seedOffset;
                    break;
                case Type::BlendNoise:
                    seedOffset = std::get<BlendNoiseData>(condition.data).seedOffset;
                    break;
                case Type::BlendDistance:
                    seedOffset = std::get<BlendDistanceData>(condition.data).seedOffset;
                    break;
                default:
                    throw std::runtime_error("show seed offset text input: unexpected condition type");
            }

            Formatter ft;
            ft.Add<StringId>(STR_MAPGEN_SEED_OFFSET);
            ft.Add<int32_t>(kSeedOffsetMin);
            ft.Add<int32_t>(kSeedOffsetMax);
            WindowTextInputOpen(
                this, WIDX_SEED_OFFSET, STR_MAPGEN_SEED_OFFSET, STR_MAPGEN_RULE_ENTER_INTEGER, ft, STR_FORMAT_INTEGER,
                static_cast<int32_t>(seedOffset), 11);
        }

        void ShowEdgeLowTextInput()
        {
            switch (condition.type)
            {
                case Type::BlendHeight:
                {
                    Formatter ft;
                    ft.Add<StringId>(STR_MAPGEN_RULE_EDGE_LOW);
                    ft.Add<int16_t>(static_cast<int16_t>(kHeightMin));
                    ft.Add<int16_t>(static_cast<int16_t>(kHeightMax));
                    WindowTextInputOpen(
                        this, WIDX_EDGE_LOW, STR_MAPGEN_RULE_EDGE_LOW, STR_MAPGEN_RULE_ENTER_LENGTH, ft, STR_FORMAT_INTEGER,
                        BaseZToMetres(std::get<BlendHeightData>(condition.data).edgeLow), 3);
                    break;
                }
                case Type::BlendNoise:
                {
                    Formatter ft;
                    ft.Add<StringId>(STR_MAPGEN_RULE_EDGE_LOW);
                    ft.Add<int32_t>(static_cast<int32_t>(kNoiseMin * 100));
                    ft.Add<int32_t>(static_cast<int32_t>(kNoiseMax * 100));
                    WindowTextInputOpen(
                        this, WIDX_EDGE_LOW, STR_MAPGEN_RULE_EDGE_LOW, STR_MAPGEN_RULE_ENTER_FLOAT, ft, STR_COMMA2DP32,
                        static_cast<int32_t>(std::get<BlendNoiseData>(condition.data).edgeLow * 100), 4);
                    break;
                }
                case Type::BlendDistance:
                {
                    Formatter ft;
                    ft.Add<StringId>(STR_MAPGEN_RULE_EDGE_LOW);
                    ft.Add<int32_t>(static_cast<int32_t>(kDistanceMin));
                    ft.Add<int32_t>(static_cast<int32_t>(kDistanceMax));
                    WindowTextInputOpen(
                        this, WIDX_EDGE_LOW, STR_MAPGEN_RULE_EDGE_LOW, STR_MAPGEN_RULE_ENTER_LENGTH, ft, STR_FORMAT_INTEGER,
                        BaseZToMetres(std::get<BlendDistanceData>(condition.data).edgeLow), 3);
                    break;
                }
                default:
                    throw std::runtime_error("show edge low text input: unexpected condition type");
            }
        }

        void ShowEdgeHighTextInput()
        {
            switch (condition.type)
            {
                case Type::BlendHeight:
                {
                    Formatter ft;
                    ft.Add<StringId>(STR_MAPGEN_RULE_EDGE_HIGH);
                    ft.Add<int16_t>(static_cast<int16_t>(kHeightMin));
                    ft.Add<int16_t>(static_cast<int16_t>(kHeightMax));
                    WindowTextInputOpen(
                        this, WIDX_EDGE_HIGH, STR_MAPGEN_RULE_EDGE_HIGH, STR_MAPGEN_RULE_ENTER_LENGTH, ft, STR_FORMAT_INTEGER,
                        BaseZToMetres(std::get<BlendHeightData>(condition.data).edgeHigh), 3);
                    break;
                }
                case Type::BlendNoise:
                {
                    Formatter ft;
                    ft.Add<StringId>(STR_MAPGEN_RULE_EDGE_HIGH);
                    ft.Add<int32_t>(static_cast<int32_t>(kNoiseMin * 100));
                    ft.Add<int32_t>(static_cast<int32_t>(kNoiseMax * 100));
                    WindowTextInputOpen(
                        this, WIDX_EDGE_HIGH, STR_MAPGEN_RULE_EDGE_HIGH, STR_MAPGEN_RULE_ENTER_FLOAT, ft, STR_COMMA2DP32,
                        static_cast<int32_t>(std::get<BlendNoiseData>(condition.data).edgeHigh * 100), 4);
                    break;
                }
                case Type::BlendDistance:
                {
                    Formatter ft;
                    ft.Add<StringId>(STR_MAPGEN_RULE_EDGE_HIGH);
                    ft.Add<int32_t>(static_cast<int32_t>(kDistanceMin));
                    ft.Add<int32_t>(static_cast<int32_t>(kDistanceMax));
                    WindowTextInputOpen(
                        this, WIDX_EDGE_HIGH, STR_MAPGEN_RULE_EDGE_HIGH, STR_MAPGEN_RULE_ENTER_LENGTH, ft, STR_FORMAT_INTEGER,
                        BaseZToMetres(std::get<BlendDistanceData>(condition.data).edgeHigh), 3);
                    break;
                }
                default:
                    throw std::runtime_error("show edge high text input: unexpected condition type");
            }
        }

        void onMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CANCEL:
                    close();
                    break;
                case WIDX_CLOSE:
                    close();
                    break;
                case WIDX_OKAY:
                    if (callback)
                    {
                        callback(condition);
                    }
                    close();
                    break;
                case WIDX_VALUE:
                    ShowValueTextInput();
                    break;
                case WIDX_BASE_FREQUENCY:
                    ShowFrequencyTextInput();
                    break;
                case WIDX_OCTAVES:
                    ShowOctavesTextInput();
                    break;
                case WIDX_SEED_OFFSET:
                    ShowSeedOffsetTextInput();
                    break;
                case WIDX_EDGE_LOW:
                    ShowEdgeLowTextInput();
                    break;
                case WIDX_EDGE_HIGH:
                    ShowEdgeHighTextInput();
                    break;
            }
        }

        std::optional<int32_t> parseInt(std::string_view& text)
        {
            auto textStr = std::string(text);
            try
            {
                return std::make_optional(std::stoi(textStr));
            }
            catch (const std::invalid_argument& e)
            {
                LOG_VERBOSE("failed parsing int from %s (%s)", textStr.c_str(), e.what());
                return std::nullopt;
            }
        }

        std::optional<float> parseFloat(std::string_view& text)
        {
            auto textStr = std::string(text);
            try
            {
                return std::make_optional(std::stof(textStr));
            }
            catch (const std::invalid_argument& e)
            {
                LOG_VERBOSE("failed parsing float from %s (%s)", textStr.c_str(), e.what());
                return std::nullopt;
            }
        }

        void onTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            // TODO refactor
            switch (widgetIndex)
            {
                case WIDX_VALUE:
                    HandleValueChange(set, parseInt(text), parseFloat(text));
                    invalidate();
                    break;
                case WIDX_BASE_FREQUENCY:
                    HandleFrequencyChange(set, parseFloat(text));
                    invalidate();
                    break;
                case WIDX_OCTAVES:
                    HandleOctavesChange(set, parseInt(text));
                    invalidate();
                    break;
                case WIDX_SEED_OFFSET:
                    HandleSeedOffsetChange(set, parseInt(text));
                    invalidate();
                    break;
                case WIDX_EDGE_LOW:
                    HandleEdgeLowChange(set, parseInt(text), parseFloat(text));
                    invalidate();
                    break;
                case WIDX_EDGE_HIGH:
                    HandleEdgeHighChange(set, parseInt(text), parseFloat(text));
                    invalidate();
                    break;
            }
        }

        void onDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_PREDICATE_DROPDOWN:
                {
                    if (selectedIndex == -1)
                        selectedIndex = gDropdown.highlightedIndex;

                    if (selectedIndex == -1)
                        return;

                    condition.predicate = static_cast<Predicate>(selectedIndex);
                    invalidate();
                    break;
                }
                case WIDX_FEATURE_DROPDOWN:
                {
                    if (selectedIndex == -1)
                        selectedIndex = gDropdown.highlightedIndex;

                    if (selectedIndex == -1)
                        return;

                    getFeature() = static_cast<Feature>(selectedIndex);
                    invalidate();
                    break;
                }
                case WIDX_HEIGHT_MODE_DROPDOWN:
                {
                    if (selectedIndex == -1)
                        selectedIndex = gDropdown.highlightedIndex;

                    if (selectedIndex == -1)
                        return;


                    auto& heightData = std::get<HeightData>(condition.data);
                    heightData.mode = static_cast<HeightMode>(selectedIndex);
                    invalidate();
                    break;
                }
                case WIDX_HEIGHT_SOURCE_FIRST_DROPDOWN:
                {
                    if (selectedIndex == -1)
                        selectedIndex = gDropdown.highlightedIndex;

                    if (selectedIndex == -1)
                        return;

                    auto& heightData = std::get<HeightData>(condition.data);
                    heightData.sourceFirst = static_cast<HeightSource>(selectedIndex);
                    invalidate();
                    break;
                }
                case WIDX_HEIGHT_SOURCE_SECOND_DROPDOWN:
                {
                    if (selectedIndex == -1)
                        selectedIndex = gDropdown.highlightedIndex;

                    if (selectedIndex == -1)
                        return;

                    auto& heightData = std::get<HeightData>(condition.data);
                    heightData.sourceSecond = static_cast<HeightSource>(selectedIndex);
                    invalidate();
                    break;
                }
            }
        }

        Feature& getFeature()
        {
            switch (condition.type)
            {
                case Type::Distance:
                {
                    return std::get<DistanceData>(condition.data).feature;
                }
                case Type::BlendDistance:
                {
                    return std::get<BlendDistanceData>(condition.data).feature;
                }
                default:
                    throw std::runtime_error("getFeature called with bad condition type");
            }
        }

        void onMouseDown(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_PREDICATE_DROPDOWN:
                {
                    using namespace Dropdown;

                    size_t itemSize;
                    if (condition.type == Type::LandStyle)
                    {
                        constexpr ItemExt items[] = {
                            ItemExt(0, STR_STRINGID, STR_MAPGEN_RULE_PREDICATE_IN),
                            ItemExt(1, STR_STRINGID, STR_MAPGEN_RULE_PREDICATE_NOT_IN),
                        };

                        SetItems(items);
                        itemSize = std::size(items);
                    }
                    else
                    {
                        constexpr ItemExt items[] = {
                            ItemExt(0, STR_STRINGID, STR_MAPGEN_RULE_PREDICATE_EQUAL),
                            ItemExt(1, STR_STRINGID, STR_MAPGEN_RULE_PREDICATE_NOT_EQUAL),
                            ItemExt(2, STR_STRINGID, STR_MAPGEN_RULE_PREDICATE_LESS_THAN),
                            ItemExt(3, STR_STRINGID, STR_MAPGEN_RULE_PREDICATE_GREATER_THAN),
                            ItemExt(4, STR_STRINGID, STR_MAPGEN_RULE_PREDICATE_LESS_THAN_OR_EQUAL),
                            ItemExt(5, STR_STRINGID, STR_MAPGEN_RULE_PREDICATE_GREATER_THAN_OR_EQUAL),
                        };

                        SetItems(items);
                        itemSize = std::size(items);
                    }

                    Widget* ddWidget = &widgets[widgetIndex - 1];
                    WindowDropdownShowText(
                        { windowPos.x + ddWidget->left, windowPos.y + ddWidget->top }, ddWidget->height() + 1, colours[1],
                        StayOpen, itemSize);
                    break;
                }
                case WIDX_FEATURE_DROPDOWN:
                {
                    using namespace Dropdown;

                    int32_t selectedIndex = static_cast<int32_t>(getFeature());

                    constexpr ItemExt items[] = {
                        ItemExt(0, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_FEATURE_WATER),
                        ItemExt(1, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_FEATURE_RIVER),
                        ItemExt(2, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_FEATURE_SEA),
                        ItemExt(3, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_FEATURE_BORDER),
                        ItemExt(4, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_FEATURE_FILL),
                        ItemExt(5, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_FEATURE_BREACH),
                        ItemExt(6, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_FEATURE_LAND),
                    };

                    SetItems(items);
                    size_t itemSize = std::size(items);

                    gDropdown.items[selectedIndex].setChecked(true);

                    Widget* ddWidget = &widgets[widgetIndex - 1];
                    WindowDropdownShowText(
                        { windowPos.x + ddWidget->left, windowPos.y + ddWidget->top }, ddWidget->height() + 1, colours[1],
                        StayOpen, itemSize);
                    break;
                }
                case WIDX_HEIGHT_MODE_DROPDOWN:
                {
                    using namespace Dropdown;

                    auto& heightData = std::get<HeightData>(condition.data);
                    int32_t selectedIndex = static_cast<int32_t>(heightData.mode);

                    constexpr ItemExt items[] = {
                        ItemExt(0, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_MODE_ABSOLUTE),
                        ItemExt(1, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_MODE_RELATIVE),
                    };

                    SetItems(items);
                    size_t itemSize = std::size(items);

                    gDropdown.items[selectedIndex].setChecked(true);

                    Widget* ddWidget = &widgets[widgetIndex - 1];
                    WindowDropdownShowText(
                        { windowPos.x + ddWidget->left, windowPos.y + ddWidget->top }, ddWidget->height() + 1, colours[1],
                        StayOpen, itemSize);
                    break;
                }
                case WIDX_HEIGHT_SOURCE_FIRST_DROPDOWN:
                {
                    using namespace Dropdown;

                    auto& heightData = std::get<HeightData>(condition.data);
                    int32_t selectedIndex = static_cast<int32_t>(heightData.sourceFirst);

                    constexpr ItemExt items[] = {
                        ItemExt( 0, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_SELF_LAND),
                        ItemExt( 1, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_SELF_WATER),
                        ItemExt( 2, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_NW_LAND),
                        ItemExt( 3, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_NW_WATER),
                        ItemExt( 4, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_NE_LAND),
                        ItemExt( 5, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_NE_WATER),
                        ItemExt( 6, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_SE_LAND),
                        ItemExt( 7, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_SE_WATER),
                        ItemExt( 8, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_SW_LAND),
                        ItemExt( 9, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_SW_WATER),
                        ItemExt(10, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_GLOBAL_MIN),
                        ItemExt(11, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_GLOBAL_MAX),
                        ItemExt(12, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_GLOBAL_WATER_LEVEL),
                    };

                    SetItems(items);
                    size_t itemSize = std::size(items);

                    gDropdown.items[selectedIndex].setChecked(true);

                    Widget* ddWidget = &widgets[widgetIndex - 1];
                    WindowDropdownShowText(
                        { windowPos.x + ddWidget->left, windowPos.y + ddWidget->top }, ddWidget->height() + 1, colours[1],
                        StayOpen, itemSize);
                    break;
                }
                case WIDX_HEIGHT_SOURCE_SECOND_DROPDOWN:
                {
                    using namespace Dropdown;

                    auto& heightData = std::get<HeightData>(condition.data);
                    int32_t selectedIndex = static_cast<int32_t>(heightData.sourceSecond);

                    constexpr ItemExt items[] = {
                        ItemExt( 0, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_SELF_LAND),
                        ItemExt( 1, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_SELF_WATER),
                        ItemExt( 2, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_NW_LAND),
                        ItemExt( 3, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_NW_WATER),
                        ItemExt( 4, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_NE_LAND),
                        ItemExt( 5, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_NE_WATER),
                        ItemExt( 6, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_SE_LAND),
                        ItemExt( 7, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_SE_WATER),
                        ItemExt( 8, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_SW_LAND),
                        ItemExt( 9, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_NEIGHBOUR_SW_WATER),
                        ItemExt(10, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_GLOBAL_MIN),
                        ItemExt(11, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_GLOBAL_MAX),
                        ItemExt(12, STR_STRINGID, STR_MAPGEN_RULE_CONDITION_HEIGHT_SOURCE_GLOBAL_WATER_LEVEL),
                    };

                    SetItems(items);
                    size_t itemSize = std::size(items);

                    gDropdown.items[selectedIndex].setChecked(true);

                    Widget* ddWidget = &widgets[widgetIndex - 1];
                    WindowDropdownShowText(
                        { windowPos.x + ddWidget->left, windowPos.y + ddWidget->top }, ddWidget->height() + 1, colours[1],
                        StayOpen, itemSize);
                    break;
                }
                case WIDX_VALUE_DOWN:
                {
                    HandleValueChange(decrement, std::nullopt, std::nullopt);
                    invalidate();
                    break;
                }
                case WIDX_VALUE_UP:
                {
                    HandleValueChange(increment, std::nullopt, std::nullopt);
                    invalidate();
                    break;
                }
                case WIDX_SEED_OFFSET_DOWN:
                {
                    HandleSeedOffsetChange(decrement, std::nullopt);
                    invalidate();
                    break;
                }
                case WIDX_SEED_OFFSET_UP:
                {
                    HandleSeedOffsetChange(increment, std::nullopt);
                    invalidate();
                    break;
                }
                case WIDX_BASE_FREQUENCY_DOWN:
                {
                    HandleFrequencyChange(decrement, std::nullopt);
                    invalidate();
                    break;
                }
                case WIDX_BASE_FREQUENCY_UP:
                {
                    HandleFrequencyChange(increment, std::nullopt);
                    invalidate();
                    break;
                }
                case WIDX_OCTAVES_DOWN:
                {
                    HandleOctavesChange(decrement, std::nullopt);
                    invalidate();
                    break;
                }
                case WIDX_OCTAVES_UP:
                {
                    HandleOctavesChange(increment, std::nullopt);
                    invalidate();
                    break;
                }
                case WIDX_EDGE_LOW_DOWN:
                {
                    HandleEdgeLowChange(decrement, std::nullopt, std::nullopt);
                    invalidate();
                    break;
                }
                case WIDX_EDGE_LOW_UP:
                {
                    HandleEdgeLowChange(increment, std::nullopt, std::nullopt);
                    invalidate();
                    break;
                }
                case WIDX_EDGE_HIGH_DOWN:
                {
                    HandleEdgeHighChange(decrement, std::nullopt, std::nullopt);
                    invalidate();
                    break;
                }
                case WIDX_EDGE_HIGH_UP:
                {
                    HandleEdgeHighChange(increment, std::nullopt, std::nullopt);
                    invalidate();
                    break;
                }
            }
        }

        int32_t GetNumColumns() const
        {
            return (widgets[WIDX_LAND_STYLE_SCROLL].width() - kScrollBarWidth) / kItemSize.width;
        }

        int32_t GetNumRows() const
        {
            float items = static_cast<float>(landStyles.size());
            float columns = static_cast<float>(GetNumColumns());
            return static_cast<int32_t>(std::ceil<float>(items / columns));
        }

        ScreenSize onScrollGetSize(int32_t scrollIndex) override
        {
            return { 0, GetNumRows() * kItemSize.height };
        }

        ImageId LookupSurfaceImage(ObjectEntryIndex surfaceTexture)
        {
            auto& objManager = GetContext()->GetObjectManager();
            const auto* surfaceObj = objManager.GetLoadedObject<TerrainSurfaceObject>(surfaceTexture);
            ImageId surfaceImage;
            if (surfaceObj != nullptr)
            {
                surfaceImage = ImageId(surfaceObj->IconImageId);
                if (surfaceObj->Colour != Drawing::kColourNull)
                {
                    surfaceImage = surfaceImage.WithPrimary(surfaceObj->Colour);
                }
            }
            return surfaceImage;
        }

        void onScrollDraw(int32_t scrollIndex, Drawing::RenderTarget& rt) override
        {
            GfxClear(rt, Drawing::getColourMap(colours[1].colour).midLight);

            auto numColumns = GetNumColumns();

            ScreenCoordsXY pos{ 0, 0 };

            auto& selectedLandStyles = std::get<LandStyleData>(condition.data).styles;

            for (auto& itemIdx : landStyles)
            {
                auto itemRect = ScreenRect{ pos, pos + ScreenCoordsXY{ kItemSize.width - 1, kItemSize.height - 1 } };

                bool selected = selectedLandStyles.contains(itemIdx);
                if (selected)
                {
                    Drawing::Rectangle::fillInset(
                        rt, itemRect, colours[1], Drawing::Rectangle::BorderStyle::inset,
                        Drawing::Rectangle::FillBrightness::light);
                }
                else if (highlightedItem.has_value() && highlightedItem.value() == itemIdx)
                {
                    Drawing::Rectangle::fillInset(
                        rt, itemRect, colours[1], Drawing::Rectangle::BorderStyle::none,
                        Drawing::Rectangle::FillBrightness::light);
                }

                // draw sprite
                GfxDrawSprite(rt, LookupSurfaceImage(itemIdx), pos);

                pos.x += kItemSize.width;
                if (pos.x >= numColumns * kItemSize.width)
                {
                    pos.y += kItemSize.height;
                    pos.x = 0;
                }
            }
        }

        std::optional<ObjectEntryIndex> getItemIdxAt(const ScreenCoordsXY& screenCoords) const
        {
            const auto columns = GetNumColumns();
            const auto col = screenCoords.x / kItemSize.width;
            const auto row = screenCoords.y / kItemSize.height;

            if (col >= 0 && col < columns && row >= 0)
            {
                const auto idx = static_cast<size_t>(row * columns + col);
                if (idx < landStyles.size())
                {
                    return std::make_optional(landStyles[idx]);
                }
            }
            return std::nullopt;
        }

        void onScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            highlightedItem = getItemIdxAt(screenCoords);
            invalidate();
        }

        void onScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            auto maybeClicked = getItemIdxAt(screenCoords);
            if (!maybeClicked.has_value())
            {
                return;
            }

            auto& selectedLandStyles = std::get<LandStyleData>(condition.data).styles;
            auto& clicked = maybeClicked.value();

            if (selectedLandStyles.contains(clicked))
            {
                selectedLandStyles.erase(clicked);
            }
            else
            {
                selectedLandStyles.insert(clicked);
            }
            invalidate();
        }

        bool WasCalledFrom(const WindowBase* call_w, const WidgetIndex call_widget) const
        {
            if (call_w == nullptr)
            {
                return false;
            }
            return callWidget.window.classification == call_w->classification && callWidget.window.number == call_w->number
                && callWidget.widgetIndex == call_widget;
        }
    };

    WindowBase* MapGenRuleConditionOpen(
        WindowBase* callWindow, WidgetIndex callWidget, Condition& condition, std::function<void(Condition&)> callback)
    {
        auto* windowMgr = GetWindowManager();
        windowMgr->CloseByClass(WindowClass::mapgenRuleCondition);

        auto w = windowMgr->Create<MapGenRuleConditionWindow>(
            WindowClass::mapgenRuleCondition, { kWindowSize.width, kWindowSize.height + 10 },
            { WindowFlag::centreScreen, WindowFlag::stickToFront });
        if (w != nullptr)
        {
            w->SetCallingContext(callWindow, callWidget);
            w->SetCondition(condition);
            w->SetCallback(callback);
        }

        return w;
    }

    void MapGenRuleConditionCloseByCalling(WindowBase* call_w, WidgetIndex call_widget)
    {
        auto* windowMgr = GetWindowManager();
        auto* w = reinterpret_cast<MapGenRuleConditionWindow*>(windowMgr->FindByClass(WindowClass::mapgenRuleCondition));
        if (w == nullptr || !w->WasCalledFrom(call_w, call_widget))
        {
            return;
        }

        w->close();
    }

} // namespace OpenRCT2::Ui::Windows
