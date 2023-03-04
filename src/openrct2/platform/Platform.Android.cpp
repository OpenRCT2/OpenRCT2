/*****************************************************************************
 * Copyright (c) 2014-2023 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef __ANDROID__

#    include "Platform.h"

#    include "../core/Guard.hpp"
#    include "../localisation/Language.h"

#    include <SDL.h>
#    include <jni.h>
#    include <memory>

AndroidClassLoader::~AndroidClassLoader()
{
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    env->DeleteGlobalRef(_classLoader);
}

jobject AndroidClassLoader::_classLoader;
jmethodID AndroidClassLoader::_findClassMethod;

static std::shared_ptr<AndroidClassLoader> acl;

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

    u8string StrDecompToPrecomp(u8string_view input)
    {
        return u8string(input);
    }

    bool HandleSpecialCommandLineArgument(const char* argument)
    {
        return false;
    }

    uint16_t GetLocaleLanguage()
    {
        JNIEnv* env = static_cast<JNIEnv*>(SDL_AndroidGetJNIEnv());

        jobject activity = static_cast<jobject>(SDL_AndroidGetActivity());
        jclass activityClass = env->GetObjectClass(activity);
        jmethodID getDefaultLocale = env->GetMethodID(
            activityClass, "getDefaultLocale", "([Ljava/lang/String;)Ljava/lang/String;");

        jobjectArray jLanguageTags = env->NewObjectArray(
            LANGUAGE_COUNT, env->FindClass("java/lang/String"), env->NewStringUTF(""));

        for (int32_t i = 1; i < LANGUAGE_COUNT; ++i)
        {
            jstring jTag = env->NewStringUTF(LanguagesDescriptors[i].locale);
            env->SetObjectArrayElement(jLanguageTags, i, jTag);
        }

        jstring jniString = (jstring)env->CallObjectMethod(activity, getDefaultLocale, jLanguageTags);

        const char* jniChars = env->GetStringUTFChars(jniString, nullptr);
        std::string defaultLocale = jniChars;

        env->ReleaseStringUTFChars(jniString, jniChars);
        for (int32_t i = 0; i < LANGUAGE_COUNT; ++i)
        {
            jobject strToFree = env->GetObjectArrayElement(jLanguageTags, i);
            env->DeleteLocalRef(strToFree);
        }
        env->DeleteLocalRef(jLanguageTags);
        env->DeleteLocalRef(activity);
        env->DeleteLocalRef(activityClass);

        return LanguageGetIDFromLocale(defaultLocale.c_str());
    }

    CurrencyType GetLocaleCurrency()
    {
        JNIEnv* env = static_cast<JNIEnv*>(SDL_AndroidGetJNIEnv());

        jobject activity = static_cast<jobject>(SDL_AndroidGetActivity());
        jclass activityClass = env->GetObjectClass(activity);
        jmethodID getDefaultLocale = env->GetMethodID(activityClass, "getLocaleCurrency", "()Ljava/lang/String;");

        jstring jniString = (jstring)env->CallObjectMethod(activity, getDefaultLocale);

        const char* jniChars = env->GetStringUTFChars(jniString, nullptr);
        std::string localeCurrencyCode = jniChars;

        env->ReleaseStringUTFChars(jniString, jniChars);
        env->DeleteLocalRef(activity);
        env->DeleteLocalRef(activityClass);

        return Platform::GetCurrencyValue(localeCurrencyCode.c_str());
    }

    MeasurementFormat GetLocaleMeasurementFormat()
    {
        JNIEnv* env = static_cast<JNIEnv*>(SDL_AndroidGetJNIEnv());

        jobject activity = static_cast<jobject>(SDL_AndroidGetActivity());
        jclass activityClass = env->GetObjectClass(activity);
        jmethodID getLocaleMeasurementFormat = env->GetMethodID(activityClass, "getLocaleMeasurementFormat", "()Z");

        jboolean isImperial = env->CallBooleanMethod(activity, getLocaleMeasurementFormat);

        env->DeleteLocalRef(activity);
        env->DeleteLocalRef(activityClass);
        bool result = isImperial == JNI_TRUE;
        if (result)
        {
            return MeasurementFormat::Imperial;
        }
        return MeasurementFormat::Metric;
    }

    std::string GetSteamPath()
    {
        return {};
    }

#    ifndef NO_TTF
    std::string GetFontPath(const TTFFontDescriptor& font)
    {
        STUB();
        return {};
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

    void AndroidInitClassLoader()
    {
        acl = std::make_shared<AndroidClassLoader>();
    }

    jclass AndroidFindClass(JNIEnv* env, std::string_view name)
    {
        return static_cast<jclass>(env->CallObjectMethod(
            AndroidClassLoader::_classLoader, AndroidClassLoader::_findClassMethod,
            env->NewStringUTF(std::string(name).c_str())));
    }
} // namespace Platform

AndroidClassLoader::AndroidClassLoader()
{
    LOG_INFO("Obtaining JNI class loader");

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

#endif
