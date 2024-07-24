#include "GameActionResult.h"

#include "../localisation/Formatting.h"

#include <algorithm>

using namespace OpenRCT2;

namespace OpenRCT2::GameActions
{
    Result::Result(GameActions::Status error, StringId title, StringId message, uint8_t* args /*= nullptr*/)
        : Error(error)
        , ErrorTitle(title)
        , ErrorMessage(message)
    {
        if (args != nullptr)
        {
            std::copy_n(args, ErrorMessageArgs.size(), ErrorMessageArgs.begin());
        }
    }

    struct StringVariantVisitor
    {
        const void* ErrorMessageArgs{};

        std::string operator()(const std::string& str) const
        {
            return str;
        }
        std::string operator()(const StringId strId) const
        {
            return FormatStringIDLegacy(strId, ErrorMessageArgs);
        }
    };

    std::string GameActions::Result::GetErrorTitle() const
    {
        return std::visit(StringVariantVisitor{ ErrorMessageArgs.data() }, ErrorTitle);
    }

    std::string GameActions::Result::GetErrorMessage() const
    {
        return std::visit(StringVariantVisitor{ ErrorMessageArgs.data() }, ErrorMessage);
    }

} // namespace OpenRCT2::GameActions
