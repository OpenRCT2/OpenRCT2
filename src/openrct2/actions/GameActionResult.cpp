#include "GameActionResult.h"

#include "../localisation/Localisation.h"

namespace GameActions
{
    Result::Result(GameActions::Status error, rct_string_id message)
    {
        Error = error;
        ErrorMessage = message;
    }

    Result::Result(GameActions::Status error, rct_string_id title, rct_string_id message)
    {
        Error = error;
        ErrorTitle = title;
        ErrorMessage = message;
    }

    Result::Result(GameActions::Status error, rct_string_id title, rct_string_id message, uint8_t* args)
    {
        Error = error;
        ErrorTitle = title;
        ErrorMessage = message;
        std::copy_n(args, ErrorMessageArgs.size(), ErrorMessageArgs.begin());
    }

    std::string GameActions::Result::GetErrorTitle() const
    {
        std::string title;
        if (auto error = ErrorTitle.AsString())
        {
            title = *error;
        }
        else
        {
            title = format_string(ErrorTitle.GetStringId(), ErrorMessageArgs.data());
        }
        return title;
    }

    std::string GameActions::Result::GetErrorMessage() const
    {
        std::string message;
        if (auto error = ErrorMessage.AsString())
        {
            message = *error;
        }
        else
        {
            message = format_string(ErrorMessage.GetStringId(), ErrorMessageArgs.data());
        }
        return message;
    }

} // namespace GameActions
