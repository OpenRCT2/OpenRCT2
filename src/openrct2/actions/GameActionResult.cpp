#include "GameActionResult.h"

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

    Result::Result(Status status, StringId title, StringId message, const Formatter& args)
        : error(status)
        , errorTitle(title)
        , errorMessage(message)
        , errorMessageArgs(args)
    {
    }

    struct StringVariantVisitor
    {
        const Formatter& ErrorMessageArgs;

        std::string operator()(const std::string& str) const
        {
            return str;
        }
        std::string operator()(const StringId strId) const
        {
            return FormatStringIDLegacy(strId, ErrorMessageArgs.Data());
        }
    };

    std::string Result::getErrorTitle() const
    {
        return std::visit(StringVariantVisitor{ errorMessageArgs }, errorTitle);
    }

    std::string Result::getErrorMessage() const
    {
        return std::visit(StringVariantVisitor{ errorMessageArgs }, errorMessage);
    }

} // namespace OpenRCT2::GameActions
