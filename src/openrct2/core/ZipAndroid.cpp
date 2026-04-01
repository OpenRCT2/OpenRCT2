/*****************************************************************************
 * Copyright (c) 2014-2026 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef __ANDROID__

    #include "../Diagnostic.h"
    #include "../platform/Platform.h"
    #include "IStream.hpp"
    #include "Memory.hpp"
    #include "MemoryStream.h"
    #include "Zip.h"

    #include <SDL.h>
    #include <jni.h>
    #include <string>

using namespace OpenRCT2;

class ZipArchive final : public IZipArchive
{
private:
    jobject _zip;

public:
    ZipArchive(std::string_view path, ZipAccess access)
    {
        // retrieve the JNI environment.
        JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();

        jclass jniClass = Platform::AndroidFindClass(env, "io/openrct2/ZipArchive");
        jmethodID constructor = env->GetMethodID(jniClass, "<init>", "(Landroid/content/Context;Ljava/lang/String;)V");

        jobject activity = (jobject)SDL_AndroidGetActivity();
        jstring jniPath = env->NewStringUTF(std::string(path).c_str());

        jobject zip = env->NewObject(jniClass, constructor, activity, jniPath);
        if (env->ExceptionCheck())
        {
            LOG_ERROR("JNI exception occurred while opening zip archive '%s':", std::string(path).c_str());
            env->ExceptionDescribe();
            env->ExceptionClear();
            env->DeleteLocalRef(jniClass);
            env->DeleteLocalRef(jniPath);
            env->DeleteLocalRef(activity);
            throw std::runtime_error("Failed to open zip archive: " + std::string(path));
        }

        _zip = env->NewGlobalRef(zip);

        env->DeleteLocalRef(zip);
        env->DeleteLocalRef(jniClass);
        env->DeleteLocalRef(jniPath);
        env->DeleteLocalRef(activity);
    }

    ~ZipArchive() override
    {
        if (_zip == nullptr)
        {
            return;
        }

        // retrieve the JNI environment.
        JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();

        jclass zipClass = env->GetObjectClass(_zip);
        jmethodID closeMethod = env->GetMethodID(zipClass, "close", "()V");

        env->CallVoidMethod(_zip, closeMethod);
        if (env->ExceptionCheck())
        {
            LOG_ERROR("JNI exception occurred while closing zip archive:");
            env->ExceptionDescribe();
            env->ExceptionClear();
        }

        env->DeleteLocalRef(zipClass);
        env->DeleteGlobalRef(_zip);
    }

    size_t GetNumFiles() const override
    {
        if (_zip == nullptr)
        {
            return 0;
        }

        // retrieve the JNI environment.
        JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();

        jclass zipClass = env->GetObjectClass(_zip);
        jmethodID fileCountMethod = env->GetMethodID(zipClass, "getNumFiles", "()I");

        jint count = env->CallIntMethod(_zip, fileCountMethod);
        if (env->ExceptionCheck())
        {
            LOG_ERROR("JNI exception occurred while getting file count:");
            env->ExceptionDescribe();
            env->ExceptionClear();
            count = 0;
        }

        env->DeleteLocalRef(zipClass);

        return static_cast<size_t>(count);
    }

    std::string GetFileName(size_t index) const override
    {
        if (_zip == nullptr)
        {
            return std::string();
        }

        // retrieve the JNI environment.
        JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();

        jclass zipClass = env->GetObjectClass(_zip);
        jmethodID fileNameMethod = env->GetMethodID(zipClass, "getFileName", "(I)Ljava/lang/String;");

        jstring jniString = (jstring)env->CallObjectMethod(_zip, fileNameMethod, (jint)index);
        if (env->ExceptionCheck())
        {
            LOG_ERROR("JNI exception occurred while getting file name at index %zu:", index);
            env->ExceptionDescribe();
            env->ExceptionClear();
            env->DeleteLocalRef(zipClass);
            return std::string();
        }

        std::string string;
        if (jniString != nullptr)
        {
            const char* jniChars = env->GetStringUTFChars(jniString, nullptr);
            string = jniChars;
            env->ReleaseStringUTFChars(jniString, jniChars);
            env->DeleteLocalRef(jniString);
        }

        env->DeleteLocalRef(zipClass);

        return string;
    }

    uint64_t GetFileSize(size_t index) const override
    {
        if (_zip == nullptr)
        {
            return 0;
        }

        // retrieve the JNI environment.
        JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();

        jclass zipClass = env->GetObjectClass(_zip);
        jmethodID fileSizeMethod = env->GetMethodID(zipClass, "getFileSize", "(I)J");

        jlong size = env->CallLongMethod(_zip, fileSizeMethod, (jint)index);
        if (env->ExceptionCheck())
        {
            LOG_ERROR("JNI exception occurred while getting file size at index %zu:", index);
            env->ExceptionDescribe();
            env->ExceptionClear();
            size = 0;
        }

        env->DeleteLocalRef(zipClass);

        return static_cast<uint64_t>(size);
    }

    std::vector<uint8_t> GetFileData(std::string_view path) const override
    {
        if (_zip == nullptr)
        {
            return {};
        }

        // retrieve the JNI environment.
        JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();

        jclass zipClass = env->GetObjectClass(_zip);
        jstring javaPath = env->NewStringUTF(std::string(path).c_str());
        jmethodID indexMethod = env->GetMethodID(zipClass, "getFileIndex", "(Ljava/lang/String;)I");
        jint index = env->CallIntMethod(_zip, indexMethod, javaPath);
        if (env->ExceptionCheck())
        {
            LOG_ERROR("JNI exception occurred while getting file index for path: %s", std::string(path).c_str());
            env->ExceptionDescribe();
            env->ExceptionClear();
            index = -1;
        }

        std::vector<uint8_t> result;
        if (index != -1)
        {
            jmethodID fileMethod = env->GetMethodID(zipClass, "getFile", "(I)[B");
            jbyteArray array = (jbyteArray)env->CallObjectMethod(_zip, fileMethod, index);
            if (env->ExceptionCheck())
            {
                LOG_ERROR("JNI exception occurred while getting file data at index %d:", index);
                env->ExceptionDescribe();
                env->ExceptionClear();
                array = nullptr;
            }

            if (array != nullptr)
            {
                jsize len = env->GetArrayLength(array);
                result.resize(len);
                env->GetByteArrayRegion(array, 0, len, reinterpret_cast<jbyte*>(result.data()));
                env->DeleteLocalRef(array);
            }
        }

        env->DeleteLocalRef(javaPath);
        env->DeleteLocalRef(zipClass);

        return result;
    }

    std::unique_ptr<IStream> GetFileStream(std::string_view path) const override
    {
        auto data = GetFileData(path);
        return std::make_unique<MemoryStream>(std::move(data));
    }

    void SetFileData(std::string_view path, std::vector<uint8_t>&& data) override
    {
        LOG_WARNING("Function %s at %s:%d is a stub.", __PRETTY_FUNCTION__, __FILE__, __LINE__);
    }

    void DeleteFile(std::string_view) override
    {
        LOG_WARNING("Function %s at %s:%d is a stub.", __PRETTY_FUNCTION__, __FILE__, __LINE__);
    }

    void RenameFile(std::string_view, std::string_view) override
    {
        LOG_WARNING("Function %s at %s:%d is a stub.", __PRETTY_FUNCTION__, __FILE__, __LINE__);
    }
};

namespace OpenRCT2::Zip
{
    std::unique_ptr<IZipArchive> Open(std::string_view path, ZipAccess access)
    {
        return std::make_unique<ZipArchive>(path, access);
    }

    std::unique_ptr<IZipArchive> TryOpen(std::string_view path, ZipAccess access)
    {
        std::unique_ptr<IZipArchive> result;
        try
        {
            result = std::make_unique<ZipArchive>(path, access);
        }
        catch (const std::exception&)
        {
        }
        return result;
    }
} // namespace OpenRCT2::Zip

#endif // __ANDROID__
