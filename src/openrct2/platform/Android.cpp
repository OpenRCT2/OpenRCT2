/*****************************************************************************
 * Copyright (c) 2014-2020 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef __ANDROID__

#    include "../config/Config.h"
#    include "../localisation/Language.h"
#    include "../util/Util.h"
#    include "platform.h"

#    include <SDL.h>
#    include <jni.h>
#    include <wchar.h>

#    ifndef NO_TTF
bool platform_get_font_path(TTFFontDescriptor* font, utf8* buffer, size_t size)
{
    STUB();
    return false;
}
#    endif

uint16_t platform_get_locale_language()
{
    return LANGUAGE_ENGLISH_UK;
}

uint8_t platform_get_locale_currency()
{
    return platform_get_currency_value(NULL);
}

MeasurementFormat platform_get_locale_measurement_format()
{
    return MeasurementFormat::Metric;
}

float platform_get_default_scale()
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

bool platform_get_steam_path(utf8* outPath, size_t outSize)
{
    return false;
}

AndroidClassLoader::AndroidClassLoader()
{
    log_info("Obtaining JNI class loader");

    // This is a workaround to be able to call JNI's ClassLoader from non-main
    // thread, based on https://stackoverflow.com/a/16302771

    // Apparently it's OK to use it from across different thread, but JNI
    // only looks for ClassLoader in the _current_ thread and fails to find
    // it when searched for from a native library's non-main thread.

    // The solution below works by obtaining a ClassLoader reference in main
    // thread and caching it for future use from any thread, instead of using
    // it via env->FindClass(). ClassLoader itself is abstract, so we cannot
    // create it directly; instead we take an arbitrary class and call
    // getClassLoader() on it to create a reference that way.

    // If we're here, SDL's JNI_OnLoad has already been called and set env
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();

    // Take an arbitrary class. While the class does not really matter, it
    // makes sense to use one that's most likely already loaded and is unlikely
    // to be removed from code.
    auto randomClass = env->FindClass("io/openrct2/MainActivity");
    jclass classClass = env->GetObjectClass(randomClass);

    // Get its class loader
    auto classLoaderClass = env->FindClass("java/lang/ClassLoader");
    auto getClassLoaderMethod = env->GetMethodID(classClass, "getClassLoader", "()Ljava/lang/ClassLoader;");

    // Store the class loader and its findClass method for future use
    _classLoader = env->NewGlobalRef(env->CallObjectMethod(randomClass, getClassLoaderMethod));
    _findClassMethod = env->GetMethodID(classLoaderClass, "findClass", "(Ljava/lang/String;)Ljava/lang/Class;");
}

AndroidClassLoader::~AndroidClassLoader()
{
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    env->DeleteGlobalRef(_classLoader);
}

jobject AndroidClassLoader::_classLoader;
jmethodID AndroidClassLoader::_findClassMethod;

static std::shared_ptr<AndroidClassLoader> acl;

void platform_android_init_class_loader()
{
    acl = std::make_shared<AndroidClassLoader>();
}

jclass platform_android_find_class(JNIEnv* env, const char* name)
{
    return static_cast<jclass>(
        env->CallObjectMethod(AndroidClassLoader::_classLoader, AndroidClassLoader::_findClassMethod, env->NewStringUTF(name)));
}
#endif
