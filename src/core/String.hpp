#pragma once

extern "C"
{
    #include "../common.h"
    #include "../localisation/localisation.h"
    #include "../util/util.h"
}

namespace String
{
    bool Equals(const utf8 * a, const utf8 * b, bool ignoreCase = false)
    {
        if (a == b) return true;
        if (a == nullptr || b == nullptr) return false;

        if (ignoreCase)
        {
            return _strcmpi(a, b) == 0;
        }
        else
        {
            return strcmp(a, b) == 0;
        }
    }

    size_t LengthOf(const utf8 * str)
    {
        return utf8_length(str);
    }

    size_t SizeOf(const utf8 * str)
    {
        return strlen(str);
    }

    utf8 * Append(utf8 * buffer, size_t bufferSize, const utf8 * src)
    {
        return safe_strcat(buffer, src, bufferSize);
    }

    utf8 * Format(utf8 * buffer, size_t bufferSize, const utf8 * format, ...)
    {
	    va_list args;

        va_start(args, format);
        vsnprintf(buffer, bufferSize, format, args);
        va_end(args);

        // Terminate buffer in case formatted string overflowed
        buffer[bufferSize - 1] = '\0';

        return buffer;
    }

    utf8 * AppendFormat(utf8 * buffer, size_t bufferSize, const utf8 * format, ...)
    {
	    va_list args;

        utf8 * dst = buffer;
        size_t i;
        for (i = 0; i < bufferSize; i++)
        {
            if (*dst == '\0') break;
            dst++;
        }

        if (i < bufferSize - 1)
        {
            va_start(args, format);
            vsnprintf(buffer, bufferSize - i - 1, format, args);
            va_end(args);

            // Terminate buffer in case formatted string overflowed
            buffer[bufferSize - 1] = '\0';
        }

        return buffer;
    }
}
