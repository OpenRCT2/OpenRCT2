#include "GameActionResult.h"

#include "../localisation/Formatter.h"
#include "../localisation/Formatting.h"

using namespace OpenRCT2;

namespace OpenRCT2::GameActions
{
    Result::Result(Status status, StringId title, StringId message)
        : error(status)
        , errorTitle(title)
        , errorMessage(message)
    {
    }

    struct StringVariantVisitor
    {
        std::string operator()(const std::string& str) const
        {
            return str;
        }
        std::string operator()(const StringId strId) const
        {
            const Formatter args;
            return FormatStringIDLegacy(strId, args.Data());
        }
    };

    std::string Result::getErrorTitle() const
    {
        return std::visit(StringVariantVisitor{}, errorTitle);
    }

    std::string Result::getErrorMessage() const
    {
        if (!errorMessageText.empty())
        {
            return errorMessageText;
        }
        return std::visit(StringVariantVisitor{}, errorMessage);
    }

} // namespace OpenRCT2::GameActions
