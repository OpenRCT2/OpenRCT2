/*****************************************************************************
 * Copyright (c) 2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include <openrct2-ui/interface/Dropdown.h>
#include <openrct2-ui/interface/Widget.h>
#include <openrct2-ui/interface/Window.h>
#include <openrct2-ui/windows/Windows.h>
#include <openrct2/Context.h>
#include <openrct2/Diagnostic.h>
#include <openrct2/core/UnitConversion.h>
#include <openrct2/localisation/StringIds.h>
#include <openrct2/object/ObjectLimits.h>
#include <openrct2/object/ObjectManager.h>
#include <openrct2/ui/WindowManager.h>
#include <openrct2/world/map_generator/rule/Rule.h>

using namespace OpenRCT2::World::MapGenerator::Rule;

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

        WIDX_LAND_STYLE_SCROLL
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
        
        makeWidget(         {   5,  39 }, { 290, 88 }, WidgetType::scroll, WindowColour::secondary, SCROLL_VERTICAL)
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
            return callWidget.window.classification != WindowClass::Null;
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
        void OnOpen() override
        {
            SetWidgets(_widgets);
            hold_down_widgets = (1uLL << WIDX_VALUE_UP) | (1uLL << WIDX_VALUE_DOWN);
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

        void OnUpdate() override
        {
            if (HasParentWindow())
            {
                // If the calling window is closed then close this window
                auto parentWindow = GetParentWindow();
                if (parentWindow == nullptr)
                {
                    Close();
                }
            }

            if (!widgetIsHighlighted(*this, WIDX_LAND_STYLE_SCROLL))
            {
                highlightedItem = std::nullopt;
                InvalidateWidget(WIDX_LAND_STYLE_SCROLL);
            }
        }

        void OnPrepareDraw() override
        {
            bool valueVisible = true;
            bool seedVisible = false;
            bool freqVisible = false;
            bool octaVisible = false;
            bool edgeLowVisible = false;
            bool edgeHighVisible = false;
            bool landStyleVisible = false;

            switch (condition.type)
            {
                case Type::HeightAbsolute:
                    widgets[WIDX_CONDITION_LABEL].text = STR_MAPGEN_RULE_CONDITION_ELEVATION_ABSOLUTE;
                    break;
                case Type::HeightRelativeToWater:
                    widgets[WIDX_CONDITION_LABEL].text = STR_MAPGEN_RULE_CONDITION_ELEVATION_RELATIVE_TO_WATER;
                    break;
                case Type::DistanceToWater:
                    widgets[WIDX_CONDITION_LABEL].text = STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_WATER;
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
                case Type::LandStyle:
                    valueVisible = false;
                    seedVisible = false;
                    freqVisible = false;
                    octaVisible = false;
                    edgeLowVisible = false;
                    edgeHighVisible = false;
                    landStyleVisible = true;
                    break;
            }

            //  widgets[WIDX_VALUE_LABEL] stays visible
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

            switch (condition.predicate)
            {
                case Predicate::Equal:
                    widgets[WIDX_PREDICATE].text = condition.type == Type::LandStyle ? STR_MAPGEN_RULE_PREDICATE_IN
                                                                                     : STR_MAPGEN_RULE_PREDICATE_EQUAL;
                    break;
                case Predicate::NotEqual:
                    widgets[WIDX_PREDICATE].text = condition.type == Type::LandStyle ? STR_MAPGEN_RULE_PREDICATE_NOT_IN
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
        }

        void OnDraw(RenderTarget& rt) override
        {
            DrawWidgets(rt);

            switch (condition.type)
            {
                case Type::HeightAbsolute:
                {
                    auto ft = Formatter();
                    ft.Add<int16_t>(static_cast<int16_t>(BaseZToMetres(std::get<HeightData>(condition.data).height)));
                    DrawTextBasic(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_VALUE].left + 1, widgets[WIDX_VALUE].top + 1 },
                        STR_RIDE_LENGTH_ENTRY, ft, { colours[1] });
                    break;
                }
                case Type::HeightRelativeToWater:
                {
                    auto ft = Formatter();
                    ft.Add<int16_t>(static_cast<int16_t>(HeightUnitsToMetres(std::get<HeightData>(condition.data).height)));
                    DrawTextBasic(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_VALUE].left + 1, widgets[WIDX_VALUE].top + 1 },
                        STR_RIDE_LENGTH_ENTRY, ft, { colours[1] });
                    break;
                }
                case Type::DistanceToWater:
                {
                    auto ft = Formatter();
                    ft.Add<int16_t>(static_cast<int16_t>(std::get<DistanceData>(condition.data).distance));
                    DrawTextBasic(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_VALUE].left + 1, widgets[WIDX_VALUE].top + 1 },
                        STR_RIDE_LENGTH_ENTRY, ft, { colours[1] });
                    break;
                }
                case Type::Noise:
                {
                    auto ft = Formatter();
                    auto& noiseData = std::get<NoiseData>(condition.data);
                    ft.Add<int32_t>(static_cast<int32_t>(noiseData.value * 100));
                    DrawTextBasic(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_VALUE].left + 1, widgets[WIDX_VALUE].top + 1 },
                        STR_COMMA2DP32, ft, { colours[1] });

                    ft = Formatter();
                    ft.Add<int32_t>(static_cast<int32_t>(noiseData.frequency * 100));
                    DrawTextBasic(
                        rt,
                        windowPos
                            + ScreenCoordsXY{ widgets[WIDX_BASE_FREQUENCY].left + 1, widgets[WIDX_BASE_FREQUENCY].top + 1 },
                        STR_COMMA2DP32, ft, { colours[1] });

                    ft = Formatter();
                    ft.Add<int32_t>(noiseData.octaves);
                    DrawTextBasic(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_OCTAVES].left + 1, widgets[WIDX_OCTAVES].top + 1 },
                        STR_FORMAT_INTEGER, ft, { colours[1] });

                    ft = Formatter();
                    ft.Add<int32_t>(static_cast<int32_t>(noiseData.seedOffset));
                    DrawTextBasic(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_SEED_OFFSET].left + 1, widgets[WIDX_SEED_OFFSET].top + 1 },
                        STR_FORMAT_INTEGER, ft, { colours[1] });
                    break;
                }
                case Type::NormalAngle:
                {
                    auto ft = Formatter();
                    ft.Add<int32_t>(static_cast<int32_t>(std::get<NormalAngleData>(condition.data).angle * 100));
                    DrawTextBasic(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_VALUE].left + 1, widgets[WIDX_VALUE].top + 1 },
                        STR_COMMA2DP32, ft, { colours[1] });
                    break;
                }
                case Type::Random:
                {
                    auto ft = Formatter();
                    auto& prngData = std::get<RandomData>(condition.data);
                    ft.Add<int32_t>(static_cast<int32_t>(prngData.value * 100));
                    DrawTextBasic(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_VALUE].left + 1, widgets[WIDX_VALUE].top + 1 },
                        STR_COMMA2DP32, ft, { colours[1] });

                    ft = Formatter();
                    ft.Add<int32_t>(static_cast<int32_t>(prngData.seedOffset));
                    DrawTextBasic(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_SEED_OFFSET].left + 1, widgets[WIDX_SEED_OFFSET].top + 1 },
                        STR_FORMAT_INTEGER, ft, { colours[1] });
                    break;
                }
                case Type::BlendHeight:
                {
                    auto& blendHeightData = std::get<BlendHeightData>(condition.data);

                    auto ft = Formatter();
                    ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_PRNG);
                    DrawTextBasic(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_VALUE].left + 1, widgets[WIDX_VALUE].top + 1 },
                        STR_STRINGID, ft, { colours[1] });

                    ft = Formatter();
                    ft.Add<int16_t>(static_cast<int16_t>(BaseZToMetres(blendHeightData.edgeLow)));
                    DrawTextBasic(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_EDGE_LOW].left + 1, widgets[WIDX_EDGE_LOW].top + 1 },
                        STR_RIDE_LENGTH_ENTRY, ft, { colours[1] });

                    ft = Formatter();
                    ft.Add<int16_t>(static_cast<int16_t>(BaseZToMetres(blendHeightData.edgeHigh)));
                    DrawTextBasic(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_EDGE_HIGH].left + 1, widgets[WIDX_EDGE_HIGH].top + 1 },
                        STR_RIDE_LENGTH_ENTRY, ft, { colours[1] });

                    ft = Formatter();
                    ft.Add<int32_t>(static_cast<int32_t>(blendHeightData.seedOffset));
                    DrawTextBasic(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_SEED_OFFSET].left + 1, widgets[WIDX_SEED_OFFSET].top + 1 },
                        STR_FORMAT_INTEGER, ft, { colours[1] });
                    break;
                }
                case Type::BlendNoise:
                {
                    auto& blendNoiseData = std::get<BlendNoiseData>(condition.data);

                    auto ft = Formatter();
                    ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_PRNG);
                    DrawTextBasic(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_VALUE].left + 1, widgets[WIDX_VALUE].top + 1 },
                        STR_STRINGID, ft, { colours[1] });

                    ft = Formatter();
                    ft.Add<int32_t>(static_cast<int32_t>(blendNoiseData.edgeLow * 100));
                    DrawTextBasic(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_EDGE_LOW].left + 1, widgets[WIDX_EDGE_LOW].top + 1 },
                        STR_COMMA2DP32, ft, { colours[1] });

                    ft = Formatter();
                    ft.Add<int32_t>(static_cast<int32_t>(blendNoiseData.edgeHigh * 100));
                    DrawTextBasic(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_EDGE_HIGH].left + 1, widgets[WIDX_EDGE_HIGH].top + 1 },
                        STR_COMMA2DP32, ft, { colours[1] });

                    ft = Formatter();
                    ft.Add<int32_t>(static_cast<int32_t>(blendNoiseData.frequency * 100));
                    DrawTextBasic(
                        rt,
                        windowPos
                            + ScreenCoordsXY{ widgets[WIDX_BASE_FREQUENCY].left + 1, widgets[WIDX_BASE_FREQUENCY].top + 1 },
                        STR_COMMA2DP32, ft, { colours[1] });

                    ft = Formatter();
                    ft.Add<int32_t>(blendNoiseData.octaves);
                    DrawTextBasic(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_OCTAVES].left + 1, widgets[WIDX_OCTAVES].top + 1 },
                        STR_FORMAT_INTEGER, ft, { colours[1] });

                    ft = Formatter();
                    ft.Add<int32_t>(static_cast<int32_t>(blendNoiseData.seedOffset));
                    DrawTextBasic(
                        rt, windowPos + ScreenCoordsXY{ widgets[WIDX_SEED_OFFSET].left + 1, widgets[WIDX_SEED_OFFSET].top + 1 },
                        STR_FORMAT_INTEGER, ft, { colours[1] });

                    break;
                }
                case Type::LandStyle:
                {
                    auto ft = Formatter();
                    ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_LAND_STYLE);
                    DrawTextBasic(
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
            callWidget.widget_index = _callWidget;

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

        void HandleValueChange(int32_t changeMultiplier, std::optional<int32_t> intValue, std::optional<float> floatValue)
        {
            switch (condition.type)
            {
                case Type::HeightAbsolute:
                {
                    auto& elevationData = std::get<HeightData>(condition.data);
                    auto elevationValue = intValue.has_value() ? MetresToBaseZ(intValue.value())
                                                               : elevationData.height + 2 * changeMultiplier;
                    elevationData.height = std::clamp(elevationValue, kHeightMin, kHeightMax);
                    break;
                }
                case Type::HeightRelativeToWater:
                {
                    auto& elevationData = std::get<HeightData>(condition.data);
                    auto elevationValue = intValue.has_value() ? MetresToHeightUnits(intValue.value())
                                                               : elevationData.height + 2 * changeMultiplier;
                    elevationData.height = std::clamp(elevationValue, -kHeightMax, kHeightMax);
                    break;
                }
                case Type::DistanceToWater:
                {
                    auto& distanceData = std::get<DistanceData>(condition.data);
                    auto distanceValue = intValue.value_or(distanceData.distance + 1 * changeMultiplier);
                    distanceData.distance = std::clamp(distanceValue, kDistanceMin, kDistanceMax);
                    break;
                }
                case Type::Noise:
                {
                    auto& noiseData = std::get<NoiseData>(condition.data);
                    auto noiseValue = floatValue.has_value() ? floatValue.value()
                                                             : noiseData.value + 0.01f * static_cast<float>(changeMultiplier);
                    noiseData.value = std::clamp(noiseValue, kNoiseMin, kNoiseMax);
                    break;
                }
                case Type::NormalAngle:
                {
                    auto& normalAngleData = std::get<NormalAngleData>(condition.data);
                    auto angleValue = floatValue.has_value()
                        ? floatValue.value()
                        : normalAngleData.angle + 0.5f * static_cast<float>(changeMultiplier);
                    normalAngleData.angle = std::clamp(angleValue, kAngleMin, kAngleMax);
                    break;
                }
                case Type::Random:
                {
                    auto& prngData = std::get<RandomData>(condition.data);
                    auto prngValue = floatValue.has_value() ? floatValue.value()
                                                            : prngData.value + 0.01f * static_cast<float>(changeMultiplier);
                    prngData.value = std::clamp(prngValue, kRandomMin, kRandomMax);
                    break;
                }
                default:
                {
                    throw std::runtime_error("value change: unexpected condition type");
                }
            }
        }

        void HandleEdgeLowChange(int32_t changeMultiplier, std::optional<int32_t> intValue, std::optional<float> floatValue)
        {
            switch (condition.type)
            {
                case Type::BlendHeight:
                {
                    auto& blendHeightData = std::get<BlendHeightData>(condition.data);
                    auto edgeLow = intValue.has_value() ? MetresToBaseZ(intValue.value())
                                                        : blendHeightData.edgeLow + 2 * changeMultiplier;
                    blendHeightData.edgeLow = std::clamp(edgeLow, kHeightMin, kHeightMax);
                    if (blendHeightData.edgeLow > blendHeightData.edgeHigh)
                    {
                        blendHeightData.edgeHigh = blendHeightData.edgeLow;
                    }
                    break;
                }
                case Type::BlendNoise:
                {
                    auto& blendNoiseData = std::get<BlendNoiseData>(condition.data);
                    auto edgeLow = floatValue.has_value()
                        ? floatValue.value()
                        : blendNoiseData.edgeLow + 0.01f * static_cast<float>(changeMultiplier);
                    blendNoiseData.edgeLow = std::clamp(edgeLow, kNoiseMin, kNoiseMax);
                    if (blendNoiseData.edgeLow > blendNoiseData.edgeHigh)
                    {
                        blendNoiseData.edgeHigh = blendNoiseData.edgeLow;
                    }
                    break;
                }
                default:
                {
                    throw std::runtime_error("edge low change: unexpected condition type");
                }
            }
        }

        void HandleEdgeHighChange(int32_t changeMultiplier, std::optional<int32_t> intValue, std::optional<float> floatValue)
        {
            switch (condition.type)
            {
                case Type::BlendHeight:
                {
                    auto& blendHeightData = std::get<BlendHeightData>(condition.data);
                    auto edgeHigh = intValue.has_value() ? MetresToBaseZ(intValue.value())
                                                         : blendHeightData.edgeHigh + 2 * changeMultiplier;
                    blendHeightData.edgeHigh = std::clamp(edgeHigh, kHeightMin, kHeightMax);
                    if (blendHeightData.edgeLow > blendHeightData.edgeHigh)
                    {
                        blendHeightData.edgeLow = blendHeightData.edgeHigh;
                    }
                    break;
                }
                case Type::BlendNoise:
                {
                    auto& blendNoiseData = std::get<BlendNoiseData>(condition.data);
                    auto edgeHigh = floatValue.has_value()
                        ? floatValue.value()
                        : blendNoiseData.edgeHigh + 0.01f * static_cast<float>(changeMultiplier);
                    blendNoiseData.edgeHigh = std::clamp(edgeHigh, kNoiseMin, kNoiseMax);
                    if (blendNoiseData.edgeLow > blendNoiseData.edgeHigh)
                    {
                        blendNoiseData.edgeLow = blendNoiseData.edgeHigh;
                    }
                    break;
                }
                default:
                {
                    throw std::runtime_error("edge high change: unexpected condition type");
                }
            }
        }

        void HandleSeedOffsetChange(int32_t changeMultiplier, std::optional<int32_t> seedOffset)
        {
            switch (condition.type)
            {
                case Type::Noise:
                {
                    auto& noiseData = std::get<NoiseData>(condition.data);
                    auto noiseSeedOffset = seedOffset.has_value()
                        ? seedOffset.value()
                        : static_cast<int32_t>(noiseData.seedOffset) + 1 * changeMultiplier;
                    noiseData.seedOffset = static_cast<uint32_t>(noiseSeedOffset);
                    break;
                }
                case Type::Random:
                {
                    auto& prngData = std::get<RandomData>(condition.data);
                    auto prngSeedOffset = seedOffset.has_value()
                        ? seedOffset.value()
                        : static_cast<int32_t>(prngData.seedOffset) + 1 * changeMultiplier;
                    prngData.seedOffset = static_cast<uint32_t>(prngSeedOffset);
                    break;
                }
                case Type::BlendHeight:
                {
                    auto& blendHeightData = std::get<BlendHeightData>(condition.data);
                    auto prngSeedOffset = seedOffset.has_value()
                        ? seedOffset.value()
                        : static_cast<int32_t>(blendHeightData.seedOffset) + 1 * changeMultiplier;
                    blendHeightData.seedOffset = static_cast<uint32_t>(prngSeedOffset);
                    break;
                }
                case Type::BlendNoise:
                {
                    auto& blendNoiseData = std::get<BlendNoiseData>(condition.data);
                    auto prngSeedOffset = seedOffset.has_value()
                        ? seedOffset.value()
                        : static_cast<int32_t>(blendNoiseData.seedOffset) + 1 * changeMultiplier;
                    blendNoiseData.seedOffset = static_cast<uint32_t>(prngSeedOffset);
                    break;
                }
                default:
                {
                    throw std::runtime_error("seed offset change: unexpected condition type");
                }
            }
        }

        void HandleFrequencyChange(int32_t changeMultiplier, std::optional<float> frequency)
        {
            switch (condition.type)
            {
                case Type::Noise:
                {
                    auto& noiseData = std::get<NoiseData>(condition.data);
                    auto noiseFrequency = frequency.has_value() ? frequency.value()
                                                                : noiseData.frequency + 0.05f * changeMultiplier;
                    noiseData.frequency = std::clamp(noiseFrequency, kFrequencyMin, kFrequencyMax);
                    break;
                }
                case Type::BlendNoise:
                {
                    auto& blendNoiseData = std::get<BlendNoiseData>(condition.data);
                    auto noiseFrequency = frequency.has_value() ? frequency.value()
                                                                : blendNoiseData.frequency + 0.05f * changeMultiplier;
                    blendNoiseData.frequency = std::clamp(noiseFrequency, kFrequencyMin, kFrequencyMax);
                    break;
                }
                default:
                {
                    throw std::runtime_error("frequency change: unexpected condition type");
                }
            }
        }

        void HandleOctavesChange(int32_t changeMultiplier, std::optional<int32_t> octaves)
        {
            switch (condition.type)
            {
                case Type::Noise:
                {
                    auto& noiseData = std::get<NoiseData>(condition.data);
                    auto noiseOctaves = octaves.value_or(noiseData.octaves + 1 * changeMultiplier);
                    noiseData.octaves = std::clamp(noiseOctaves, kOctavesMin, kOctavesMax);
                    break;
                }
                case Type::BlendNoise:
                {
                    auto& blendNoiseData = std::get<BlendNoiseData>(condition.data);
                    auto noiseOctaves = octaves.value_or(blendNoiseData.octaves + 1 * changeMultiplier);
                    blendNoiseData.octaves = std::clamp(noiseOctaves, kOctavesMin, kOctavesMax);
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
                case Type::HeightAbsolute:
                {
                    Formatter ft;
                    ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_ELEVATION_ABSOLUTE);
                    ft.Add<int16_t>(static_cast<int16_t>(kHeightMin));
                    ft.Add<int16_t>(static_cast<int16_t>(kHeightMax));
                    WindowTextInputOpen(
                        this, WIDX_VALUE, STR_MAPGEN_RULE_CONDITION_ELEVATION_ABSOLUTE, STR_MAPGEN_RULE_ENTER_LENGTH, ft,
                        STR_FORMAT_INTEGER, BaseZToMetres(std::get<HeightData>(condition.data).height), 3);
                    break;
                }
                case Type::HeightRelativeToWater:
                {
                    Formatter ft;
                    ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_ELEVATION_RELATIVE_TO_WATER);
                    ft.Add<int16_t>(static_cast<int16_t>(-kHeightMax));
                    ft.Add<int16_t>(static_cast<int16_t>(kHeightMax));
                    WindowTextInputOpen(
                        this, WIDX_VALUE, STR_MAPGEN_RULE_CONDITION_ELEVATION_RELATIVE_TO_WATER, STR_MAPGEN_RULE_ENTER_LENGTH,
                        ft, STR_FORMAT_INTEGER, HeightUnitsToMetres(std::get<HeightData>(condition.data).height), 4);
                    break;
                }
                case Type::DistanceToWater:
                {
                    Formatter ft;
                    ft.Add<StringId>(STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_WATER);
                    ft.Add<int16_t>(static_cast<int16_t>(kDistanceMin));
                    ft.Add<int16_t>(static_cast<int16_t>(kDistanceMax));
                    WindowTextInputOpen(
                        this, WIDX_VALUE, STR_MAPGEN_RULE_CONDITION_DISTANCE_TO_WATER, STR_MAPGEN_RULE_ENTER_LENGTH, ft,
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
                default:
                    throw std::runtime_error("show edge high text input: unexpected condition type");
            }
        }

        void OnMouseUp(WidgetIndex widgetIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_CANCEL:
                    Close();
                    break;
                case WIDX_CLOSE:
                    Close();
                    break;
                case WIDX_OKAY:
                    if (callback)
                    {
                        callback(condition);
                    }
                    Close();
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

        void OnTextInput(WidgetIndex widgetIndex, std::string_view text) override
        {
            // TODO refactor
            switch (widgetIndex)
            {
                case WIDX_VALUE:
                    HandleValueChange(0, parseInt(text), parseFloat(text));
                    Invalidate();
                    break;
                case WIDX_BASE_FREQUENCY:
                    HandleFrequencyChange(0, parseFloat(text));
                    Invalidate();
                    break;
                case WIDX_OCTAVES:
                    HandleOctavesChange(0, parseInt(text));
                    Invalidate();
                    break;
                case WIDX_SEED_OFFSET:
                    HandleSeedOffsetChange(0, parseInt(text));
                    Invalidate();
                    break;
                case WIDX_EDGE_LOW:
                    HandleEdgeLowChange(0, parseInt(text), parseFloat(text));
                    Invalidate();
                    break;
                case WIDX_EDGE_HIGH:
                    HandleEdgeHighChange(0, parseInt(text), parseFloat(text));
                    Invalidate();
                    break;
            }
        }

        void OnDropdown(WidgetIndex widgetIndex, int32_t selectedIndex) override
        {
            switch (widgetIndex)
            {
                case WIDX_PREDICATE_DROPDOWN:
                {
                    if (selectedIndex == -1)
                        selectedIndex = gDropdownHighlightedIndex;

                    if (selectedIndex == -1)
                        return;

                    condition.predicate = static_cast<Predicate>(selectedIndex);
                    Invalidate();
                    break;
                }
            }
        }

        void OnMouseDown(WidgetIndex widgetIndex) override
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
                        Dropdown::Flag::StayOpen, itemSize);
                    break;
                }
                case WIDX_VALUE_DOWN:
                {
                    HandleValueChange(-1, std::nullopt, std::nullopt);
                    Invalidate();
                    break;
                }
                case WIDX_VALUE_UP:
                {
                    HandleValueChange(1, std::nullopt, std::nullopt);
                    Invalidate();
                    break;
                }
                case WIDX_SEED_OFFSET_DOWN:
                {
                    HandleSeedOffsetChange(-1, std::nullopt);
                    Invalidate();
                    break;
                }
                case WIDX_SEED_OFFSET_UP:
                {
                    HandleSeedOffsetChange(1, std::nullopt);
                    Invalidate();
                    break;
                }
                case WIDX_BASE_FREQUENCY_DOWN:
                {
                    HandleFrequencyChange(-1, std::nullopt);
                    Invalidate();
                    break;
                }
                case WIDX_BASE_FREQUENCY_UP:
                {
                    HandleFrequencyChange(1, std::nullopt);
                    Invalidate();
                    break;
                }
                case WIDX_OCTAVES_DOWN:
                {
                    HandleOctavesChange(-1, std::nullopt);
                    Invalidate();
                    break;
                }
                case WIDX_OCTAVES_UP:
                {
                    HandleOctavesChange(1, std::nullopt);
                    Invalidate();
                    break;
                }
                case WIDX_EDGE_LOW_DOWN:
                {
                    HandleEdgeLowChange(-1, std::nullopt, std::nullopt);
                    Invalidate();
                    break;
                }
                case WIDX_EDGE_LOW_UP:
                {
                    HandleEdgeLowChange(1, std::nullopt, std::nullopt);
                    Invalidate();
                    break;
                }
                case WIDX_EDGE_HIGH_DOWN:
                {
                    HandleEdgeHighChange(-1, std::nullopt, std::nullopt);
                    Invalidate();
                    break;
                }
                case WIDX_EDGE_HIGH_UP:
                {
                    HandleEdgeHighChange(1, std::nullopt, std::nullopt);
                    Invalidate();
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

        ScreenSize OnScrollGetSize(int32_t scrollIndex) override
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
                if (surfaceObj->Colour != TerrainSurfaceObject::kNoValue)
                {
                    surfaceImage = surfaceImage.WithPrimary(surfaceObj->Colour);
                }
            }
            return surfaceImage;
        }

        void OnScrollDraw(int32_t scrollIndex, RenderTarget& rt) override
        {
            GfxClear(rt, ColourMapA[colours[1].colour].mid_light);

            auto numColumns = GetNumColumns();

            ScreenCoordsXY pos{ 0, 0 };

            auto& selectedLandStyles = std::get<LandStyleData>(condition.data).styles;

            for (auto& itemIdx : landStyles)
            {
                auto itemRect = ScreenRect{ pos, pos + ScreenCoordsXY{ kItemSize.width - 1, kItemSize.height - 1 } };

                bool selected = selectedLandStyles.contains(itemIdx);
                if (selected)
                {
                    GfxFillRectInset(rt, itemRect, colours[1], INSET_RECT_FLAG_BORDER_INSET | INSET_RECT_FLAG_FILL_MID_LIGHT);
                }
                else if (highlightedItem.has_value() && highlightedItem.value() == itemIdx)
                {
                    GfxFillRectInset(rt, itemRect, colours[1], INSET_RECT_FLAG_FILL_MID_LIGHT);
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

        void OnScrollMouseOver(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
        {
            highlightedItem = getItemIdxAt(screenCoords);
            Invalidate();
        }

        void OnScrollMouseDown(int32_t scrollIndex, const ScreenCoordsXY& screenCoords) override
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
            Invalidate();
        }

        bool WasCalledFrom(const WindowBase* call_w, const WidgetIndex call_widget) const
        {
            if (call_w == nullptr)
            {
                return false;
            }
            return callWidget.window.classification == call_w->classification && callWidget.window.number == call_w->number
                && callWidget.widget_index == call_widget;
        }
    };

    WindowBase* MapGenRuleConditionOpen(
        WindowBase* callWindow, WidgetIndex callWidget, Condition& condition, std::function<void(Condition&)> callback)
    {
        auto* windowMgr = GetWindowManager();
        windowMgr->CloseByClass(WindowClass::MapgenRuleCondition);

        auto w = windowMgr->Create<MapGenRuleConditionWindow>(
            WindowClass::MapgenRuleCondition, { kWindowSize.width, kWindowSize.height + 10 },
            WF_CENTRE_SCREEN | WF_STICK_TO_FRONT);
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
        auto* w = reinterpret_cast<MapGenRuleConditionWindow*>(windowMgr->FindByClass(WindowClass::MapgenRuleCondition));
        if (w == nullptr || !w->WasCalledFrom(call_w, call_widget))
        {
            return;
        }

        w->Close();
    }

} // namespace OpenRCT2::Ui::Windows
