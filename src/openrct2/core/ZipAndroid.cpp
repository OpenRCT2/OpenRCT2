/*****************************************************************************
 * Copyright (c) 2014-2018 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#ifdef __ANDROID__

#include <SDL.h>
#include <jni.h>
#include "IStream.hpp"
#include "Zip.h"

class ZipArchive final : public IZipArchive
{
private:
    jobject _zip;

public:
    ZipArchive(const std::string_view& path, ZIP_ACCESS access)
    {
        // retrieve the JNI environment.
        JNIEnv *env = (JNIEnv *) SDL_AndroidGetJNIEnv();

        jclass jniClass = env->FindClass("website/openrct2/ZipArchive");
        jmethodID constructor = env->GetMethodID(jniClass, "<init>", "(Ljava/lang/String;)V");

        jstring jniPath = env->NewStringUTF(path.data());

        // TODO: Catch exceptions. Should probably be done on Java side, and just return null from a static method
        jobject zip = env->NewObject(jniClass, constructor, jniPath);

        _zip = env->NewGlobalRef(zip);
    }

    ~ZipArchive() override
    {
        // retrieve the JNI environment.
        JNIEnv *env = (JNIEnv *) SDL_AndroidGetJNIEnv();

        jclass zipClass = env->GetObjectClass(_zip);
        jmethodID closeMethod = env->GetMethodID(zipClass, "close", "()V");

        env->CallVoidMethod(_zip, closeMethod);

        env->DeleteGlobalRef(_zip);
    }

    size_t GetNumFiles() const override
    {
        // retrieve the JNI environment.
        JNIEnv *env = (JNIEnv *) SDL_AndroidGetJNIEnv();

        jclass zipClass = env->GetObjectClass(_zip);
        jmethodID fileCountMethod = env->GetMethodID(zipClass, "getNumFiles", "()I");

        return (size_t) env->CallIntMethod(_zip, fileCountMethod);
    }

    std::string GetFileName(size_t index) const override
    {
        // retrieve the JNI environment.
        JNIEnv *env = (JNIEnv *) SDL_AndroidGetJNIEnv();

        jclass zipClass = env->GetObjectClass(_zip);
        jmethodID fileNameMethod = env->GetMethodID(zipClass, "getFileName",
                                                    "(I)Ljava/lang/String;");

        jstring jniString = (jstring) env->CallObjectMethod(_zip, fileNameMethod, (jint) index);

        const char *jniChars = env->GetStringUTFChars(jniString, nullptr);

        utf8 *string = (char *) malloc(strlen(jniChars) + 1);
        memcpy((void *) string, jniChars, strlen(jniChars));
        string[strlen(jniChars)] = 0x00;

        env->ReleaseStringUTFChars(jniString, jniChars);

        return string;
    }

    uint64_t GetFileSize(size_t index) const override
    {
        // retrieve the JNI environment.
        JNIEnv *env = (JNIEnv *) SDL_AndroidGetJNIEnv();

        jclass zipClass = env->GetObjectClass(_zip);
        jmethodID fileSizeMethod = env->GetMethodID(zipClass, "getFileSize", "(I)J");

        return (size_t) env->CallLongMethod(_zip, fileSizeMethod, (jint) index);
    }

    std::vector<uint8_t> GetFileData(const std::string_view& path) const override
    {
        // retrieve the JNI environment.
        JNIEnv *env = (JNIEnv *) SDL_AndroidGetJNIEnv();

        jclass zipClass = env->GetObjectClass(_zip);
        jstring javaPath = env->NewStringUTF(path.data());
        jmethodID indexMethod = env->GetMethodID(zipClass, "getFileIndex", "(Ljava/lang/String;)I");
        jint index = env->CallIntMethod(_zip, indexMethod, javaPath);

        jmethodID fileMethod = env->GetMethodID(zipClass, "getFile", "(I)J");
        jlong ptr = env->CallLongMethod(_zip, fileMethod, index);

        auto dataPtr = reinterpret_cast<uint8_t *>(ptr);
        auto dataSize = this->GetFileSize(index);

        return std::vector<uint8_t>(dataPtr, dataPtr + dataSize);
    }

    void SetFileData(const std::string_view& path, std::vector<uint8_t>&& data) override
    {
        STUB();
    }

    void DeleteFile(const std::string_view&) override
    {
        STUB();
    }

    void RenameFile(const std::string_view&, const std::string_view&) override
    {
        STUB();
    }
};

namespace Zip
{
    std::unique_ptr<IZipArchive> Open(const std::string_view& path, ZIP_ACCESS access)
    {
        return std::make_unique<ZipArchive>(path, access);
    }

    std::unique_ptr<IZipArchive> TryOpen(const std::string_view& path, ZIP_ACCESS access)
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
}

extern "C" {
JNIEXPORT jlong JNICALL
Java_website_openrct2_ZipArchive_allocBytes(JNIEnv *env, jclass, jbyteArray input,
                                                jint numBytes);
}

JNIEXPORT jlong JNICALL
Java_website_openrct2_ZipArchive_allocBytes(JNIEnv *env, jclass, jbyteArray input,
                                                jint numBytes) {

    jbyte *bufferPtr = env->GetByteArrayElements(input, nullptr);

    void *data = Memory::Allocate<void>((size_t) numBytes);
    memcpy(data, bufferPtr, numBytes);

    env->ReleaseByteArrayElements(input, bufferPtr, 0);

    return (uintptr_t) data;
}

#endif // __ANDROID__
