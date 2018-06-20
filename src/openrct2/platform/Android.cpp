/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef __ANDROID__

#include "platform.h"
#include "../config/Config.h"
#include "../localisation/Language.h"
#include "../util/Util.h"
#include <wchar.h>
#include <jni.h>
#include <SDL.h>

#ifndef NO_TTF
bool platform_get_font_path(TTFFontDescriptor *font, utf8 *buffer, size_t size)
{
    STUB();
    return false;
}
#endif

uint16_t platform_get_locale_language() {
    return LANGUAGE_ENGLISH_UK;
}

uint8_t platform_get_locale_currency() {
    return platform_get_currency_value(NULL);
}

uint8_t platform_get_locale_measurement_format() {
    return MEASUREMENT_FORMAT_METRIC;
}

float platform_get_default_scale() {
    JNIEnv *env = static_cast<JNIEnv *>(SDL_AndroidGetJNIEnv());

    jobject activity = static_cast<jobject>(SDL_AndroidGetActivity());
    jclass activityClass = env->GetObjectClass(activity);
    jmethodID getDefaultScale = env->GetMethodID(activityClass, "getDefaultScale", "()F");

    jfloat displayScale = env->CallFloatMethod(activity, getDefaultScale);

    env->DeleteLocalRef(activity);
    env->DeleteLocalRef(activityClass);

    return displayScale;
}

bool platform_get_steam_path(utf8 * outPath, size_t outSize)
{
    return false;
}

#endif
