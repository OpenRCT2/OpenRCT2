/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#include "../world/Climate.h"
#include "Object.h"

namespace OpenRCT2
{
    struct IReadObjectContext;

    using YearlyDistribution = std::array<uint8_t, EnumValue(WeatherType::Count)>;

    class ClimateObject final : public Object
    {
    private:
        Climate _climate;
        std::string _scriptName;

    public:
        static constexpr ObjectType kObjectType = ObjectType::climate;

        void ReadJson(IReadObjectContext* context, json_t& root) override;
        void Load() override;
        void Unload() override;

        void DrawPreview(Drawing::RenderTarget& rt, int32_t width, int32_t height) const override;

        const TemperatureThresholds& getItemThresholds() const;
        const WeatherPattern& getPatternForMonth(uint8_t month) const;
        std::string getScriptName() const;
        YearlyDistribution getYearlyDistribution() const;
    };
} // namespace OpenRCT2
