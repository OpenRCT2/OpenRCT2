/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef __ANDROID__

    #include "Platform.h"

    #include "../Diagnostic.h"
    #include "../core/File.h"
    #include "../core/Guard.hpp"
    #include "../core/IStream.hpp"
    #include "../core/String.hpp"
    #include "../localisation/Language.h"

    #include <SDL.h>
    #include <algorithm>
    #include <android/asset_manager.h>
    #include <android/asset_manager_jni.h>
    #include <jni.h>
    #include <memory>
    #include <mutex>
    #include <sys/stat.h>

AndroidClassLoader::~AndroidClassLoader()
{
    JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
    env->DeleteGlobalRef(_classLoader);
}

jobject AndroidClassLoader::_classLoader;
jmethodID AndroidClassLoader::_findClassMethod;
static AAssetManager* _assetManager;
static std::vector<OpenRCT2::Platform::AssetInfo> _assetList;
static std::once_flag _assetManagerInitialized;
static std::once_flag _assetListInitialized;

// Initialized in JNI_OnLoad. Cannot be initialized here as JVM is not
// available until after JNI_OnLoad is called.
static std::shared_ptr<AndroidClassLoader> acl;

namespace OpenRCT2::Platform
{
    std::string GetFolderPath(SpecialFolder folder)
    {
        // Android builds currently only read from /sdcard/openrct2*
        switch (folder)
        {
            case SpecialFolder::userCache:
            case SpecialFolder::userConfig:
            case SpecialFolder::userData:
            case SpecialFolder::userHome:
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
        // If assets are bundled, use them as install path
        if (File::Exists(std::string(Platform::kAndroidAssetPathPrefix) + "openrct2/data/g2.dat"))
        {
            return std::string(Platform::kAndroidAssetPathPrefix) + "openrct2/data";
        }

        // Fallback to external storage for backward compatibility
        return GetFolderPath(SpecialFolder::userData);
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

        jstring jniString = static_cast<jstring>(env->CallObjectMethod(activity, getDefaultLocale, jLanguageTags));

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

        jstring jniString = static_cast<jstring>(env->CallObjectMethod(activity, getDefaultLocale));

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
        jmethodID getIsImperialLocaleMeasurementFormat = env->GetMethodID(
            activityClass, "isImperialLocaleMeasurementFormat", "()Z");

        jboolean isImperial = env->CallBooleanMethod(activity, getIsImperialLocaleMeasurementFormat);

        env->DeleteLocalRef(activity);
        env->DeleteLocalRef(activityClass);
        return isImperial == JNI_TRUE ? MeasurementFormat::Imperial : MeasurementFormat::Metric;
    }

    SteamPaths GetSteamPaths()
    {
        return {};
    }

    uint64_t GetLastModified(std::string_view path)
    {
        if (String::startsWith(path, Platform::kAndroidAssetPathPrefix))
        {
            // Assets don't have a modification time in the traditional sense.
            return 0;
        }

        uint64_t lastModified = 0;
        struct stat statInfo{};
        if (stat(std::string(path).c_str(), &statInfo) == 0)
        {
            lastModified = statInfo.st_mtime;
        }
        return lastModified;
    }

    uint64_t GetFileSize(std::string_view path)
    {
        if (String::startsWith(path, Platform::kAndroidAssetPathPrefix))
        {
            auto assetManager = static_cast<AAssetManager*>(GetAssetManager());
            if (assetManager != nullptr)
            {
                std::string assetPath = std::string(path).substr(Platform::kAndroidAssetPathPrefix.length());
                auto asset = AAssetManager_open(assetManager, assetPath.c_str(), AASSET_MODE_UNKNOWN);
                if (asset != nullptr)
                {
                    auto size = AAsset_getLength64(asset);
                    AAsset_close(asset);
                    return size;
                }
            }
            return 0;
        }

        uint64_t size = 0;
        struct stat statInfo{};
        if (stat(std::string(path).c_str(), &statInfo) == 0)
        {
            size = statInfo.st_size;
        }
        return size;
    }

    #ifndef DISABLE_TTF
    std::string GetFontPath(const TTFFontDescriptor& font)
    {
        auto expectedPath = std::string("/system/fonts/") + std::string(font.filename);
        if (File::Exists(expectedPath))
        {
            return expectedPath;
        }

        return {};
    }
    #endif

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

    jclass AndroidFindClass(JNIEnv* env, std::string_view name)
    {
        return static_cast<jclass>(env->CallObjectMethod(
            AndroidClassLoader::_classLoader, AndroidClassLoader::_findClassMethod,
            env->NewStringUTF(std::string(name).c_str())));
    }

    void* GetAssetManager()
    {
        std::call_once(_assetManagerInitialized, []() {
            JNIEnv* env = static_cast<JNIEnv*>(SDL_AndroidGetJNIEnv());
            jobject activity = static_cast<jobject>(SDL_AndroidGetActivity());
            jclass activityClass = env->GetObjectClass(activity);
            jmethodID getAssetsMethod = env->GetMethodID(activityClass, "getAssets", "()Landroid/content/res/AssetManager;");
            jobject assetManagerObj = env->CallObjectMethod(activity, getAssetsMethod);
            _assetManager = AAssetManager_fromJava(env, assetManagerObj);

            env->DeleteLocalRef(assetManagerObj);
            env->DeleteLocalRef(activityClass);
            env->DeleteLocalRef(activity);
        });

        return _assetManager;
    }

    static AssetCheckResult CheckAssetExistsInList(u8string_view path, bool directoryOnly)
    {
        if (!String::startsWith(path, Platform::kAndroidAssetPathPrefix))
        {
            return AssetCheckResult::NotApplicable;
        }

        const auto& assetList = GetAssetList();
        std::string assetPath = std::string(path.substr(Platform::kAndroidAssetPathPrefix.length()));
        if (assetPath.empty())
        {
            return assetList.empty() ? AssetCheckResult::NotFound : AssetCheckResult::Found;
        }

        if (!directoryOnly)
        {
            auto it = std::lower_bound(
                assetList.begin(), assetList.end(), assetPath,
                [](const AssetInfo& a, const std::string& b) { return a.Path < b; });
            if (it != assetList.end() && it->Path == assetPath)
            {
                return AssetCheckResult::Found;
            }
            return AssetCheckResult::NotFound;
        }

        // Only check for directory/prefix matches if directoryOnly is true
        std::string prefix = assetPath;
        if (!prefix.empty() && prefix.back() != '/')
        {
            prefix += '/';
        }

        auto it = std::lower_bound(
            assetList.begin(), assetList.end(), prefix, [](const AssetInfo& a, const std::string& b) { return a.Path < b; });
        if (it != assetList.end() && String::startsWith(it->Path, prefix))
        {
            return AssetCheckResult::Found;
        }

        return AssetCheckResult::NotFound;
    }

    AssetCheckResult CheckAssetDirectoryExists(u8string_view path)
    {
        return CheckAssetExistsInList(path, true);
    }

    AssetCheckResult CheckAssetExists(u8string_view path)
    {
        return CheckAssetExistsInList(path, false);
    }

    AssetFileOpenResult OpenAssetFile(u8string_view path)
    {
        if (!String::startsWith(path, Platform::kAndroidAssetPathPrefix))
        {
            return AssetFileOpenResult{ AssetCheckResult::NotApplicable, nullptr, 0 };
        }

        auto assetManager = static_cast<AAssetManager*>(GetAssetManager());
        if (assetManager == nullptr)
        {
            return AssetFileOpenResult{ AssetCheckResult::NotFound, nullptr, 0 };
        }

        std::string assetPath = std::string(path.substr(Platform::kAndroidAssetPathPrefix.length()));
        auto asset = AAssetManager_open(assetManager, assetPath.c_str(), AASSET_MODE_RANDOM);
        if (asset == nullptr)
        {
            return AssetFileOpenResult{ AssetCheckResult::NotFound, nullptr, 0 };
        }

        uint64_t assetSize = AAsset_getLength64(asset);
        return AssetFileOpenResult{ AssetCheckResult::Found, asset, assetSize };
    }

    void CloseAssetFile(AssetHandle handle)
    {
        if (handle != nullptr)
        {
            AAsset_close(static_cast<AAsset*>(handle));
        }
    }

    uint64_t GetAssetPosition(AssetHandle handle)
    {
        if (handle == nullptr)
        {
            return 0;
        }
        return static_cast<uint64_t>(AAsset_seek64(static_cast<AAsset*>(handle), 0, SEEK_CUR));
    }

    void SeekAsset(AssetHandle handle, int64_t offset, int32_t origin)
    {
        if (handle == nullptr)
        {
            return;
        }

        int whence;
        switch (origin)
        {
            case STREAM_SEEK_BEGIN:
                whence = SEEK_SET;
                break;
            case STREAM_SEEK_CURRENT:
                whence = SEEK_CUR;
                break;
            case STREAM_SEEK_END:
                whence = SEEK_END;
                break;
            default:
                return;
        }
        AAsset_seek64(static_cast<AAsset*>(handle), static_cast<off64_t>(offset), whence);
    }

    uint64_t ReadAsset(AssetHandle handle, void* buffer, uint64_t length)
    {
        if (handle == nullptr)
        {
            return 0;
        }
        int readBytes = AAsset_read(static_cast<AAsset*>(handle), buffer, static_cast<size_t>(length));
        if (readBytes < 0)
        {
            return 0;
        }
        return static_cast<uint64_t>(readBytes);
    }

    uint64_t TryReadAsset(AssetHandle handle, void* buffer, uint64_t length)
    {
        return ReadAsset(handle, buffer, length);
    }

    u8string GetAssetPath()
    {
        return std::string(Platform::kAndroidAssetPathPrefix);
    }

    const std::vector<AssetInfo>& GetAssetList()
    {
        std::call_once(_assetListInitialized, []() {
            AAssetManager* am = static_cast<AAssetManager*>(GetAssetManager());
            if (am != nullptr)
            {
                AAsset* asset = AAssetManager_open(am, "openrct2/manifest.txt", AASSET_MODE_BUFFER);
                if (asset != nullptr)
                {
                    size_t size = AAsset_getLength64(asset);
                    std::string content;
                    content.resize(size);
                    AAsset_read(asset, content.data(), size);
                    AAsset_close(asset);

                    auto processLine = [](std::string line) {
                        if (!line.empty() && line.back() == '\r')
                        {
                            line.pop_back();
                        }
                        if (!line.empty())
                        {
                            auto sep = line.find('|');
                            if (sep != std::string::npos)
                            {
                                try
                                {
                                    AssetInfo info;
                                    info.Path = line.substr(0, sep);
                                    info.Size = std::stoull(line.substr(sep + 1));
                                    _assetList.push_back(std::move(info));
                                }
                                catch (const std::exception&)
                                {
                                    LOG_WARNING("Failed to parse asset entry: %s", line.c_str());
                                }
                            }
                        }
                    };

                    size_t start = 0;
                    size_t end = content.find('\n');
                    while (end != std::string::npos)
                    {
                        processLine(content.substr(start, end - start));
                        start = end + 1;
                        end = content.find('\n', start);
                    }
                    processLine(content.substr(start));
                    std::sort(_assetList.begin(), _assetList.end(), [](const AssetInfo& a, const AssetInfo& b) {
                        return a.Path < b.Path;
                    });
                }
            }
        });
        return _assetList;
    }

    std::vector<std::string_view> GetSearchablePathsRCT1()
    {
        return { "/sdcard/rct1" };
    }

    std::vector<std::string_view> GetSearchablePathsRCT2()
    {
        return { "/sdcard/rct2" };
    }

    time_t FileGetModifiedTime(u8string_view path)
    {
        return static_cast<time_t>(GetLastModified(path));
    }
} // namespace OpenRCT2::Platform

/**
 * JNI function to expose the Android asset path prefix constant to Java.
 * Called from io.openrct2.PlatformConstants.getAndroidAssetPathPrefix()
 */
extern "C" JNIEXPORT jstring JNICALL
    Java_io_openrct2_PlatformConstants_getAndroidAssetPathPrefix(JNIEnv* env, jclass /* clazz */)
{
    return env->NewStringUTF(std::string(OpenRCT2::Platform::kAndroidAssetPathPrefix).c_str());
}

JNIEXPORT jint JNICALL JNI_OnLoad(JavaVM* pjvm, void* reserved)
{
    // Due to an issue where JNI_OnLoad could be called multiple times, we need
    // to make sure it is only initialized once.
    // https://issuetracker.google.com/issues/220523932
    // Otherwise JVM complains about jobject-s having incorrect serial numbers.
    if (!acl)
    {
        acl = std::make_shared<AndroidClassLoader>();
    }
    return JNI_VERSION_1_6;
}

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
