/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#pragma once

#ifdef ENABLE_SCRIPTING

    #include "../../../Context.h"
    #include "../../Duktape.hpp"
    #include "ScParkMessage.hpp"
    #include "ScResearch.hpp"

    #include <vector>

namespace OpenRCT2::Scripting
{
    class ScPark
    {
    private:
        duk_context* _context;

    public:
        ScPark(duk_context* ctx);

        money64 cash_get() const;
        void cash_set(money64 value);

        int32_t rating_get() const;
        void rating_set(int32_t value);

        money64 bankLoan_get() const;
        void bankLoan_set(money64 value);

        money64 maxBankLoan_get() const;
        void maxBankLoan_set(money64 value);

        money64 entranceFee_get() const;
        void entranceFee_set(money64 value);

        uint32_t guests_get() const;

        uint32_t suggestedGuestMaximum_get() const;

        int32_t guestGenerationProbability_get() const;

        DukValue generateGuest();

        money64 guestInitialCash_get() const;

        uint8_t guestInitialHappiness_get() const;

        uint8_t guestInitialHunger_get() const;

        uint8_t guestInitialThirst_get() const;

        money64 value_get() const;
        void value_set(money64 value);

        money64 companyValue_get() const;
        void companyValue_set(money64 value);

        money64 totalRideValueForMoney_get() const;

        uint32_t totalAdmissions_get() const;
        void totalAdmissions_set(uint32_t value);

        money64 totalIncomeFromAdmissions_get() const;
        void totalIncomeFromAdmissions_set(money64 value);

        money64 landPrice_get() const;
        void landPrice_set(money64 value);

        money64 constructionRightsPrice_get() const;
        void constructionRightsPrice_set(money64 value);

        int16_t casualtyPenalty_get() const;
        void casualtyPenalty_set(int16_t value);

        uint16_t parkSize_get() const;

        std::string name_get() const;
        void name_set(std::string value);

        bool getFlag(const std::string& key) const;

        void setFlag(const std::string& key, bool value);

        std::shared_ptr<ScResearch> research_get() const;

        std::vector<std::shared_ptr<ScParkMessage>> messages_get() const;

        void messages_set(const std::vector<DukValue>& value);

        void postMessage(DukValue message);

        std::vector<int32_t> getMonthlyExpenditure(const std::string& expenditureType) const;

        static void Register(duk_context* ctx);
    };
} // namespace OpenRCT2::Scripting

#endif
