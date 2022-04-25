/*****************************************************************************
 * Copyright (c) 2014-2021 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef ENABLE_SCRIPTING

#    include "ScParkMessage.hpp"

#    include "../../../Context.h"
#    include "../../../GameState.h"
#    include "../../../common.h"
#    include "../../../core/String.hpp"
#    include "../../../entity/Peep.h"
#    include "../../../management/Finance.h"
#    include "../../../management/NewsItem.h"
#    include "../../../windows/Intent.h"
#    include "../../../world/Park.h"
#    include "../../Duktape.hpp"
#    include "../../ScriptEngine.h"

#    include <algorithm>

namespace OpenRCT2::Scripting
{
    ScParkMessage::ScParkMessage(size_t index)
        : _index(index)
    {
    }

    void ScParkMessage::Register(duk_context* ctx)
    {
        dukglue_register_property(ctx, &ScParkMessage::isArchived_get, nullptr, "isArchived");
        dukglue_register_property(ctx, &ScParkMessage::month_get, &ScParkMessage::month_set, "month");
        dukglue_register_property(ctx, &ScParkMessage::day_get, &ScParkMessage::day_set, "day");
        dukglue_register_property(ctx, &ScParkMessage::tickCount_get, &ScParkMessage::tickCount_set, "tickCount");
        dukglue_register_property(ctx, &ScParkMessage::type_get, &ScParkMessage::type_set, "type");
        dukglue_register_property(ctx, &ScParkMessage::subject_get, &ScParkMessage::subject_set, "subject");
        dukglue_register_property(ctx, &ScParkMessage::text_get, &ScParkMessage::text_set, "text");
        dukglue_register_method(ctx, &ScParkMessage::remove, "remove");
    }

    News::Item* ScParkMessage::GetMessage() const
    {
        return &gNewsItems[_index];
    }

    bool ScParkMessage::isArchived_get() const
    {
        return _index >= News::ItemHistoryStart;
    }

    uint16_t ScParkMessage::month_get() const
    {
        auto msg = GetMessage();
        if (msg != nullptr)
        {
            return msg->MonthYear;
        }
        return 0;
    }

    void ScParkMessage::month_set(uint16_t value)
    {
        ThrowIfGameStateNotMutable();
        auto msg = GetMessage();
        if (msg != nullptr)
        {
            msg->MonthYear = value;
        }
    }

    uint8_t ScParkMessage::day_get() const
    {
        auto msg = GetMessage();
        if (msg != nullptr)
        {
            return msg->Day;
        }
        return 0;
    }

    void ScParkMessage::day_set(uint8_t value)
    {
        ThrowIfGameStateNotMutable();
        auto msg = GetMessage();
        if (msg != nullptr)
        {
            msg->Day = value;
        }
    }

    uint16_t ScParkMessage::tickCount_get() const
    {
        auto msg = GetMessage();
        if (msg != nullptr)
        {
            return msg->Ticks;
        }
        return 0;
    }

    void ScParkMessage::tickCount_set(uint16_t value)
    {
        ThrowIfGameStateNotMutable();
        auto msg = GetMessage();
        if (msg != nullptr)
        {
            msg->Ticks = value;
        }
    }

    std::string ScParkMessage::type_get() const
    {
        auto msg = GetMessage();
        if (msg != nullptr)
        {
            return GetParkMessageType(msg->Type);
        }
        return {};
    }

    void ScParkMessage::type_set(const std::string& value)
    {
        ThrowIfGameStateNotMutable();
        auto msg = GetMessage();
        if (msg != nullptr)
        {
            msg->Type = GetParkMessageType(value);
        }
    }

    uint32_t ScParkMessage::subject_get() const
    {
        auto msg = GetMessage();
        if (msg != nullptr)
        {
            return msg->Assoc;
        }
        return 0;
    }

    void ScParkMessage::subject_set(uint32_t value)
    {
        ThrowIfGameStateNotMutable();
        auto msg = GetMessage();
        if (msg != nullptr)
        {
            msg->Assoc = value;
        }
    }

    std::string ScParkMessage::text_get() const
    {
        auto msg = GetMessage();
        if (msg != nullptr)
        {
            return msg->Text;
        }
        return {};
    }

    void ScParkMessage::text_set(const std::string& value)
    {
        ThrowIfGameStateNotMutable();
        auto msg = GetMessage();
        if (msg != nullptr)
        {
            msg->Text = value;
        }
    }

    void ScParkMessage::remove()
    {
        News::RemoveItem(static_cast<int32_t>(_index));
    }

} // namespace OpenRCT2::Scripting

#endif
