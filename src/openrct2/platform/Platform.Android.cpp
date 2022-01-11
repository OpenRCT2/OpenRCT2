/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef __ANDROID__

#    include "../core/Guard.hpp"
#    include "../localisation/Language.h"
#    include "Platform2.h"

namespace Platform
{
    std::string GetFolderPath(SPECIAL_FOLDER folder)
    {
        // Android builds currently only read from /sdcard/openrct2*
        switch (folder)
        {
            case SPECIAL_FOLDER::USER_CACHE:
            case SPECIAL_FOLDER::USER_CONFIG:
            case SPECIAL_FOLDER::USER_DATA:
            case SPECIAL_FOLDER::USER_HOME:
                return "/sdcard";
            default:
                return std::string();
        }
    }

    std::string GetDocsPath()
    {
        return std::string();
    }

    std::string GetInstallPath()
    {
        return "/sdcard/openrct2";
    }

    std::string GetCurrentExecutablePath()
    {
        Guard::Assert(false, "GetCurrentExecutablePath() not implemented for Android.");
        return std::string();
    }

    utf8* StrDecompToPrecomp(utf8* input)
    {
        return input;
    }

    bool HandleSpecialCommandLineArgument(const char* argument)
    {
        return false;
    }

    uint16_t GetLocaleLanguage()
    {
        return LANGUAGE_ENGLISH_UK;
    }

    CurrencyType GetLocaleCurrency()
    {
        return Platform::GetCurrencyValue(NULL);
    }

    MeasurementFormat GetLocaleMeasurementFormat()
    {
        return MeasurementFormat::Metric;
    }

    std::string GetSteamPath()
    {
        return "";
    }

#    ifndef NO_TTF
    std::string GetFontPath(const TTFFontDescriptor& font)
    {
        STUB();
        return "";
    }
#    endif

    float GetDefaultScale()
    {
        JNIEnv* env = static_cast<JNIEnv*>(SDL_AndroidGetJNIEnv());

        jobject activity = static_cast<jobject>(SDL_AndroidGetActivity());
        jclass activityClass = env->GetObjectClass(activity);
        jmethodID getDefaultScale = env->GetMethodID(activityClass, "getDefaultScale", "()F");

        jfloat displayScale = env->CallFloatMethod(activity, getDefaultScale);

        env->DeleteLocalRef(activity);
        env->DeleteLocalRef(activityClass);

        return displayScale;
    }
} // namespace Platform

#endif
