#include "GameActionResult.h"

#include "../localisation/Formatting.h"

#include <algorithm>

using namespace OpenRCT2;

namespace OpenRCT2::GameActions
{
    Result::Result(Status status, StringId title, StringId message, uint8_t* args /*= nullptr*/)
        : error(status)
        , errorTitle(title)
        , errorMessage(message)
    {
        if (args != nullptr)
        {
            std::copy_n(args, errorMessageArgs.size(), errorMessageArgs.begin());
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

    std::string Result::getErrorTitle() const
    {
        return std::visit(StringVariantVisitor{ errorMessageArgs.data() }, errorTitle);
    }

    std::string Result::getErrorMessage() const
    {
        return std::visit(StringVariantVisitor{ errorMessageArgs.data() }, errorMessage);
    }

} // namespace OpenRCT2::GameActions
