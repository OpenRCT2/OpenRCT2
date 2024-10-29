/*****************************************************************************
 * Copyright (c) 2014-2024 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#include "Util.h"

#include "../Diagnostic.h"
#include "../core/Guard.hpp"
#include "../core/Path.hpp"
#include "../core/UTF8.h"
#include "../interface/Window.h"
#include "../platform/Platform.h"
#include "../scenes/title/TitleScene.h"

#include <algorithm>
#include <cassert>
#include <cctype>
#include <cmath>
#include <ctime>
#include <random>

/* Case insensitive logical compare */
// Example:
// - Guest 10
// - Guest 99
// - Guest 100
// - John v2.0
// - John v2.1
int32_t StrLogicalCmp(const char* s1, const char* s2)
{
    for (;;)
    {
        if (*s2 == '\0')
            return *s1 != '\0';
        if (*s1 == '\0')
            return -1;
        if (!(isdigit(static_cast<unsigned char>(*s1)) && isdigit(static_cast<unsigned char>(*s2))))
        {
            if (toupper(*s1) != toupper(*s2))
                return toupper(*s1) - toupper(*s2);

            ++s1;
            ++s2;
        }
        else
        {
            char *lim1, *lim2;
            unsigned long n1 = strtoul(s1, &lim1, 10);
            unsigned long n2 = strtoul(s2, &lim2, 10);
            if (n1 > n2)
                return 1;
            if (n1 < n2)
                return -1;

            s1 = lim1;
            s2 = lim2;
        }
    }
}

char* SafeStrCpy(char* destination, const char* source, size_t size)
{
    assert(destination != nullptr);
    assert(source != nullptr);

    if (size == 0)
        return destination;

    char* result = destination;

    bool truncated = false;
    const char* sourceLimit = source + size - 1;
    const char* ch = source;
    uint32_t codepoint;
    while ((codepoint = UTF8GetNext(ch, &ch)) != 0)
    {
        if (ch <= sourceLimit)
        {
            destination = UTF8WriteCodepoint(destination, codepoint);
        }
        else
        {
            truncated = true;
        }
    }
    *destination = 0;

    if (truncated)
    {
        LOG_WARNING("Truncating string \"%s\" to %d bytes.", result, size);
    }
    return result;
}

char* SafeStrCat(char* destination, const char* source, size_t size)
{
    assert(destination != nullptr);
    assert(source != nullptr);

    if (size == 0)
    {
        return destination;
    }

    char* result = destination;

    size_t i;
    for (i = 0; i < size; i++)
    {
        if (*destination == '\0')
        {
            break;
        }

        destination++;
    }

    bool terminated = false;
    for (; i < size; i++)
    {
        if (*source != '\0')
        {
            *destination++ = *source++;
        }
        else
        {
            *destination = *source;
            terminated = true;
            break;
        }
    }

    if (!terminated)
    {
        result[size - 1] = '\0';
        LOG_WARNING("Truncating string \"%s\" to %d bytes.", result, size);
    }

    return result;
}

uint32_t UtilRand()
{
    thread_local std::mt19937 _prng(std::random_device{}());
    return _prng();
}

// Returns a random floating point number from the Standard Normal Distribution; mean of 0 and standard deviation of 1.
// TODO: In C++20 this can be templated, where the standard deviation is passed as a value template argument.
float UtilRandNormalDistributed()
{
    thread_local std::mt19937 _prng{ std::random_device{}() };
    thread_local std::normal_distribution<float> _distributor{ 0.0f, 1.0f };
    return _distributor(_prng);
}

uint8_t Lerp(uint8_t a, uint8_t b, float t)
{
    if (t <= 0)
        return a;
    if (t >= 1)
        return b;

    int32_t range = b - a;
    int32_t amount = static_cast<int32_t>(range * t);
    return static_cast<uint8_t>(a + amount);
}

float FLerp(float a, float b, float f)
{
    return (a * (1.0f - f)) + (b * f);
}

uint8_t SoftLight(uint8_t a, uint8_t b)
{
    float fa = a / 255.0f;
    float fb = b / 255.0f;
    float fr;
    if (fb < 0.5f)
    {
        fr = (2 * fa * fb) + ((fa * fa) * (1 - (2 * fb)));
    }
    else
    {
        fr = (2 * fa * (1 - fb)) + (std::sqrt(fa) * ((2 * fb) - 1));
    }
    return static_cast<uint8_t>(std::clamp(fr, 0.0f, 1.0f) * 255.0f);
}

/**
 * strftime wrapper which appends to an existing string.
 */
size_t StrCatFTime(char* buffer, size_t bufferSize, const char* format, const struct tm* tp)
{
    size_t stringLen = strnlen(buffer, bufferSize);
    if (stringLen < bufferSize)
    {
        char* dst = buffer + stringLen;
        size_t dstMaxSize = bufferSize - stringLen;
        return strftime(dst, dstMaxSize, format, tp);
    }
    return 0;
}
