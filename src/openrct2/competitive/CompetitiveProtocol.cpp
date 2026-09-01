/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "CompetitiveProtocol.h"

#include "../core/Json.hpp"

#include <limits>

namespace OpenRCT2::Competitive
{
    namespace
    {
        template<typename T>
        T Number(const json_t& object, const char* key, T fallback = {})
        {
            const auto found = object.find(key);
            return found != object.end() && found->is_number() ? found->get<T>() : fallback;
        }

        std::string String(const json_t& object, const char* key)
        {
            const auto found = object.find(key);
            return found != object.end() && found->is_string() ? found->get<std::string>() : std::string{};
        }

        bool Boolean(const json_t& object, const char* key, bool fallback = false)
        {
            const auto found = object.find(key);
            return found != object.end() && found->is_boolean() ? found->get<bool>() : fallback;
        }

        json_t AbilityRuleToJson(const AbilityRule& value)
        {
            return {
                { "enabled", value.enabled },
                { "cost", value.cost },
                { "cooldownYears", value.cooldownYears },
                { "durationDays", value.durationDays },
                { "potency", value.potency },
            };
        }

        AbilityRule AbilityRuleFromJson(const json_t& value, const AbilityRule& fallback)
        {
            if (!value.is_object())
            {
                return fallback;
            }
            return {
                Boolean(value, "enabled", fallback.enabled),
                Number<money64>(value, "cost", fallback.cost),
                Number<uint8_t>(value, "cooldownYears", fallback.cooldownYears),
                Number<uint16_t>(value, "durationDays", fallback.durationDays),
                Number<uint16_t>(value, "potency", fallback.potency),
            };
        }

        json_t ParticipantToJson(const Participant& value)
        {
            return {
                { "id", value.id },
                { "identityKey", value.identityKey },
                { "name", value.name },
                { "role", value.role },
                { "online", value.online },
                { "ready", value.ready },
                { "finished", value.finished },
                { "forfeited", value.forfeited },
                { "joinedOrder", value.joinedOrder },
                { "currentYear", value.currentYear },
                { "scenario", ToJson(value.scenario) },
            };
        }

        std::optional<Participant> ParticipantFromJson(const json_t& value)
        {
            if (!value.is_object())
            {
                return std::nullopt;
            }
            auto scenario = ScenarioIdentityFromJson(value["scenario"]);
            const auto id = Number<ParticipantId>(value, "id");
            const auto role = Number<Role>(value, "role", Role::player);
            if (id == kInvalidParticipantId || !scenario.has_value() || role > Role::spectator)
            {
                return std::nullopt;
            }
            return Participant{
                id,
                String(value, "identityKey"),
                String(value, "name"),
                role,
                Boolean(value, "online"),
                Boolean(value, "ready"),
                Boolean(value, "finished"),
                Boolean(value, "forfeited"),
                Number<uint32_t>(value, "joinedOrder"),
                Number<uint16_t>(value, "currentYear", 1),
                std::move(*scenario),
            };
        }

        json_t ScoreToJson(const Score& value)
        {
            json_t result = {
                { "participantId", value.participantId },
                { "points", value.points },
                { "rating", value.rating },
                { "guests", value.guests },
                { "parkValue", value.parkValue },
                { "competitiveCash", value.competitiveCash },
                { "lifetimeIncome", value.lifetimeIncome },
                { "lifetimeSpend", value.lifetimeSpend },
            };
            result["frozenAtYear"] = value.frozenAtYear.has_value() ? json_t(*value.frozenAtYear) : json_t(nullptr);
            return result;
        }

        std::optional<Score> ScoreFromJson(const json_t& value)
        {
            if (!value.is_object())
            {
                return std::nullopt;
            }
            Score result{
                .participantId = Number<ParticipantId>(value, "participantId"),
                .points = Number<int64_t>(value, "points"),
                .rating = Number<uint16_t>(value, "rating"),
                .guests = Number<uint32_t>(value, "guests"),
                .parkValue = Number<money64>(value, "parkValue"),
                .competitiveCash = Number<money64>(value, "competitiveCash"),
                .lifetimeIncome = Number<money64>(value, "lifetimeIncome"),
                .lifetimeSpend = Number<money64>(value, "lifetimeSpend"),
            };
            if (value.contains("frozenAtYear") && value["frozenAtYear"].is_number())
            {
                result.frozenAtYear = value["frozenAtYear"].get<uint16_t>();
            }
            return result.participantId == kInvalidParticipantId ? std::nullopt : std::optional<Score>{ result };
        }
    } // namespace

    json_t ToJson(const ScenarioIdentity& value)
    {
        return {
            { "fileName", value.fileName },
            { "name", value.name },
            { "contentHash", value.contentHash },
            { "mapWidth", value.mapWidth },
            { "mapHeight", value.mapHeight },
            { "noMoney", value.noMoney },
        };
    }

    json_t ToJson(const MatchRules& value)
    {
        return {
            { "victoryMode", value.victoryMode },
            { "metric", value.metric },
            { "target", value.target },
            { "deadlineYear", value.deadlineYear },
            { "maxPlayers", value.maxPlayers },
            { "allowLateJoin", value.allowLateJoin },
            { "maxGameSpeed", value.maxGameSpeed },
            { "economy",
              {
                  { "startingCash", value.economy.startingCash },
                  { "incomePerArrival", value.economy.incomePerArrival },
                  { "incomePerRideCustomer", value.economy.incomePerRideCustomer },
                  { "incomePerStallCustomer", value.economy.incomePerStallCustomer },
              } },
            { "vandal", AbilityRuleToJson(value.vandal) },
            { "misinformation", AbilityRuleToJson(value.misinformation) },
            { "poison", AbilityRuleToJson(value.poison) },
        };
    }

    json_t ToJson(const ParkMetrics& value)
    {
        json_t stalls = json_t::array();
        for (const auto& stall : value.openFoodDrinkStalls)
        {
            stalls.push_back({ { "rideId", stall.rideId }, { "name", stall.name } });
        }
        return {
            { "localYear", value.localYear },
            { "monthsElapsed", value.monthsElapsed },
            { "localDay", value.localDay },
            { "rating", value.rating },
            { "guests", value.guests },
            { "parkValue", value.parkValue },
            { "meanHappiness", value.meanHappiness },
            { "arrivalsGenerated", value.arrivalsGenerated },
            { "constructionSpend", value.constructionSpend },
            { "rideCustomers", value.rideCustomers },
            { "stallCustomers", value.stallCustomers },
            { "openFoodDrinkStalls", std::move(stalls) },
        };
    }

    json_t ToJson(const ActiveEffect& value)
    {
        return {
            { "id", value.id },
            { "ability", value.ability },
            { "sourceId", value.sourceId },
            { "targetId", value.targetId },
            { "targetRideId", value.targetRideId },
            { "delivered", value.delivered },
            { "reservedCost", value.reservedCost },
            { "startsAtDay", value.startsAtDay },
            { "endsAtDay", value.endsAtDay },
            { "potency", value.potency },
        };
    }

    json_t ToJson(const MatchState& value)
    {
        json_t participants = json_t::array();
        for (const auto& participant : value.participants)
        {
            participants.push_back(ParticipantToJson(participant));
        }
        json_t scores = json_t::array();
        for (const auto& score : value.scores)
        {
            scores.push_back(ScoreToJson(score));
        }
        json_t reports = json_t::array();
        for (const auto& report : value.reports)
        {
            reports.push_back({
                { "participantId", report.participantId },
                { "metrics", ToJson(report.metrics) },
                { "acceptedEconomy",
                  {
                      { "arrivalsGenerated", report.acceptedEconomy.arrivalsGenerated },
                      { "constructionSpend", report.acceptedEconomy.constructionSpend },
                      { "rideCustomers", report.acceptedEconomy.rideCustomers },
                      { "stallCustomers", report.acceptedEconomy.stallCustomers },
                  } },
                { "lastScoredDay", report.lastScoredDay },
            });
        }
        json_t cooldowns = json_t::array();
        for (const auto& cooldown : value.cooldowns)
        {
            cooldowns.push_back({
                { "participantId", cooldown.participantId },
                { "ability", cooldown.ability },
                { "availableYear", cooldown.availableYear },
            });
        }
        json_t effects = json_t::array();
        for (const auto& effect : value.effects)
        {
            effects.push_back(ToJson(effect));
        }

        json_t result = {
            { "protocol", value.protocol },
            { "matchId", value.matchId },
            { "name", value.name },
            { "epoch", value.epoch },
            { "revision", value.revision },
            { "phase", value.phase },
            { "hostId", value.hostId },
            { "startLocalDay", value.startLocalDay },
            { "rules", ToJson(value.rules) },
            { "scenario", ToJson(value.scenario) },
            { "participants", std::move(participants) },
            { "scores", std::move(scores) },
            { "reports", std::move(reports) },
            { "cooldowns", std::move(cooldowns) },
            { "effects", std::move(effects) },
            { "nextEffectId", value.nextEffectId },
            { "closedEarly", value.closedEarly },
        };
        result["winnerId"] = value.winnerId.has_value() ? json_t(*value.winnerId) : json_t(nullptr);
        return result;
    }

    std::optional<ScenarioIdentity> ScenarioIdentityFromJson(const json_t& value)
    {
        if (!value.is_object())
        {
            return std::nullopt;
        }
        ScenarioIdentity result{
            String(value, "fileName"),
            String(value, "name"),
            String(value, "contentHash"),
            Number<uint16_t>(value, "mapWidth"),
            Number<uint16_t>(value, "mapHeight"),
            Boolean(value, "noMoney"),
        };
        return result.fileName.empty() || result.name.empty() ? std::nullopt : std::optional<ScenarioIdentity>{ result };
    }

    std::optional<MatchRules> MatchRulesFromJson(const json_t& value)
    {
        if (!value.is_object())
        {
            return std::nullopt;
        }
        MatchRules result;
        result.victoryMode = Number<VictoryMode>(value, "victoryMode", result.victoryMode);
        result.metric = Number<Metric>(value, "metric", result.metric);
        result.target = Number<int64_t>(value, "target", result.target);
        result.deadlineYear = Number<uint16_t>(value, "deadlineYear", result.deadlineYear);
        result.maxPlayers = Number<uint8_t>(value, "maxPlayers", result.maxPlayers);
        result.allowLateJoin = Boolean(value, "allowLateJoin", result.allowLateJoin);
        result.maxGameSpeed = Number<uint8_t>(value, "maxGameSpeed", result.maxGameSpeed);
        if (value.contains("economy") && value["economy"].is_object())
        {
            const auto& economy = value["economy"];
            result.economy.startingCash = Number<money64>(economy, "startingCash", result.economy.startingCash);
            result.economy.incomePerArrival = Number<money64>(economy, "incomePerArrival", result.economy.incomePerArrival);
            result.economy.incomePerRideCustomer = Number<money64>(
                economy, "incomePerRideCustomer", result.economy.incomePerRideCustomer);
            result.economy.incomePerStallCustomer = Number<money64>(
                economy, "incomePerStallCustomer", result.economy.incomePerStallCustomer);
        }
        result.vandal = AbilityRuleFromJson(value["vandal"], result.vandal);
        result.misinformation = AbilityRuleFromJson(value["misinformation"], result.misinformation);
        result.poison = AbilityRuleFromJson(value["poison"], result.poison);

        if (result.victoryMode > VictoryMode::target || result.metric > Metric::parkValue || result.maxPlayers < 2
            || result.maxPlayers > 32 || result.deadlineYear == 0 || result.maxGameSpeed == 0 || result.maxGameSpeed > 4)
        {
            return std::nullopt;
        }
        return result;
    }

    std::optional<ParkMetrics> ParkMetricsFromJson(const json_t& value)
    {
        if (!value.is_object())
        {
            return std::nullopt;
        }
        ParkMetrics result{
            Number<uint16_t>(value, "localYear", 1),
            Number<uint32_t>(value, "monthsElapsed"),
            Number<uint32_t>(value, "localDay", 1),
            Number<uint16_t>(value, "rating"),
            Number<uint32_t>(value, "guests"),
            Number<money64>(value, "parkValue"),
            Number<uint8_t>(value, "meanHappiness"),
            Number<uint64_t>(value, "arrivalsGenerated"),
            Number<money64>(value, "constructionSpend"),
            Number<uint64_t>(value, "rideCustomers"),
            Number<uint64_t>(value, "stallCustomers"),
        };
        if (value.contains("openFoodDrinkStalls") && value["openFoodDrinkStalls"].is_array())
        {
            for (const auto& item : value["openFoodDrinkStalls"])
            {
                if (!item.is_object())
                    continue;
                ParkMetrics::Stall stall{ Number<int32_t>(item, "rideId", -1), String(item, "name") };
                if (stall.rideId >= 0 && !stall.name.empty() && stall.name.size() <= 128)
                    result.openFoodDrinkStalls.push_back(std::move(stall));
            }
        }
        if (result.localYear == 0 || result.rating > 999 || result.localDay == 0 || result.constructionSpend < 0)
        {
            return std::nullopt;
        }
        return result;
    }

    std::optional<ActiveEffect> ActiveEffectFromJson(const json_t& value)
    {
        if (!value.is_object())
            return std::nullopt;
        ActiveEffect effect{
            Number<uint32_t>(value, "id"),
            Number<Ability>(value, "ability", Ability::vandal),
            Number<ParticipantId>(value, "sourceId"),
            Number<ParticipantId>(value, "targetId"),
            Number<int32_t>(value, "targetRideId", -1),
            Boolean(value, "delivered"),
            Number<money64>(value, "reservedCost"),
            Number<uint32_t>(value, "startsAtDay"),
            Number<uint32_t>(value, "endsAtDay"),
            Number<uint16_t>(value, "potency"),
        };
        if (effect.id == 0 || effect.ability > Ability::poison || effect.sourceId == kInvalidParticipantId
            || effect.targetId == kInvalidParticipantId || effect.sourceId == effect.targetId
            || effect.endsAtDay <= effect.startsAtDay || effect.reservedCost < 0)
        {
            return std::nullopt;
        }
        return effect;
    }

    std::optional<MatchState> MatchStateFromJson(const json_t& value)
    {
        if (!value.is_object() || Number<uint16_t>(value, "protocol") != kProtocolVersion)
        {
            return std::nullopt;
        }
        auto rules = MatchRulesFromJson(value["rules"]);
        auto scenario = ScenarioIdentityFromJson(value["scenario"]);
        if (!rules.has_value() || !scenario.has_value())
        {
            return std::nullopt;
        }

        MatchState result;
        result.protocol = kProtocolVersion;
        result.matchId = String(value, "matchId");
        result.name = String(value, "name");
        result.epoch = Number<uint32_t>(value, "epoch", 1);
        result.revision = Number<uint64_t>(value, "revision");
        result.phase = Number<Phase>(value, "phase", Phase::none);
        result.hostId = Number<ParticipantId>(value, "hostId");
        result.startLocalDay = Number<uint32_t>(value, "startLocalDay", 1);
        result.rules = std::move(*rules);
        result.scenario = std::move(*scenario);
        result.nextEffectId = Number<uint32_t>(value, "nextEffectId", 1);
        result.closedEarly = Boolean(value, "closedEarly");
        if (value.contains("winnerId") && value["winnerId"].is_number_unsigned())
        {
            result.winnerId = value["winnerId"].get<ParticipantId>();
        }

        if (result.matchId.empty() || result.name.empty() || result.phase > Phase::finished
            || result.hostId == kInvalidParticipantId || !value["participants"].is_array()
            || !value["scores"].is_array() || !value["reports"].is_array())
        {
            return std::nullopt;
        }

        for (const auto& item : value["participants"])
        {
            auto parsed = ParticipantFromJson(item);
            if (!parsed.has_value())
                return std::nullopt;
            result.participants.push_back(std::move(*parsed));
        }
        for (const auto& item : value["scores"])
        {
            auto parsed = ScoreFromJson(item);
            if (!parsed.has_value())
                return std::nullopt;
            result.scores.push_back(std::move(*parsed));
        }
        for (const auto& item : value["reports"])
        {
            if (!item.is_object())
                return std::nullopt;
            auto metrics = ParkMetricsFromJson(item["metrics"]);
            const auto id = Number<ParticipantId>(item, "participantId");
            if (!metrics.has_value() || id == kInvalidParticipantId)
                return std::nullopt;
            ParticipantReport report{ id, std::move(*metrics), {}, Number<uint32_t>(item, "lastScoredDay") };
            if (item.contains("acceptedEconomy") && item["acceptedEconomy"].is_object())
            {
                const auto& economy = item["acceptedEconomy"];
                report.acceptedEconomy.arrivalsGenerated = Number<uint64_t>(economy, "arrivalsGenerated");
                report.acceptedEconomy.constructionSpend = Number<money64>(economy, "constructionSpend");
                report.acceptedEconomy.rideCustomers = Number<uint64_t>(economy, "rideCustomers");
                report.acceptedEconomy.stallCustomers = Number<uint64_t>(economy, "stallCustomers");
            }
            result.reports.push_back(std::move(report));
        }
        if (value.contains("cooldowns") && value["cooldowns"].is_array())
        {
            for (const auto& item : value["cooldowns"])
            {
                const AbilityCooldown cooldown{
                    Number<ParticipantId>(item, "participantId"),
                    Number<Ability>(item, "ability", Ability::vandal),
                    Number<uint16_t>(item, "availableYear"),
                };
                if (cooldown.participantId == kInvalidParticipantId || cooldown.ability > Ability::poison)
                    return std::nullopt;
                result.cooldowns.push_back(cooldown);
            }
        }
        if (value.contains("effects") && value["effects"].is_array())
        {
            for (const auto& item : value["effects"])
            {
                auto effect = ActiveEffectFromJson(item);
                if (!effect.has_value())
                    return std::nullopt;
                result.effects.push_back(*effect);
            }
        }
        return result;
    }
} // namespace OpenRCT2::Competitive
