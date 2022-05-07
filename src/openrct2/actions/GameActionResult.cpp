#include "GameActionResult.h"

#include "../localisation/Localisation.h"

namespace GameActions
{
    Result::Result(GameActions::Status error, rct_string_id title, rct_string_id message, uint8_t* args /*= nullptr*/)
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
        std::string operator()(const rct_string_id strId) const
        {
            return format_string(strId, ErrorMessageArgs);
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

} // namespace GameActions
