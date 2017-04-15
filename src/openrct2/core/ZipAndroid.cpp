#pragma region Copyright (c) 2014-2017 OpenRCT2 Developers
/*****************************************************************************
 * OpenRCT2, an open source clone of Roller Coaster Tycoon 2.
 *
 * OpenRCT2 is the work of many authors, a full list can be found in contributors.md
 * For more information, visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * A full copy of the GNU General Public License can be found in licence.txt
 *****************************************************************************/
#pragma endregion

#include <SDL.h>
#include <jni.h>
#include "IStream.hpp"
#include "Zip.h"

class ZipArchive final : public IZipArchive
{
private:
    jobject _zip;

public:
    ZipArchive(const utf8 * path, ZIP_ACCESS access)
    {
        // retrieve the JNI environment.
        JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();
        
        jclass jniClass = env->FindClass("org/openrct2/android/ZipArchive");
        jmethodID constructor = env->GetMethodID(jniClass, "<init>", "(Ljava/lang/String;)V");

        jstring jniPath = env->NewStringUTF(path);

        // TODO: Catch exceptions. Should probably be done on Java side, and just return null from a static method
        jobject zip = env->NewObject(jniClass, constructor, jniPath);
        
        _zip = env->NewGlobalRef(zip);

    }

    ~ZipArchive() override
    {
        // retrieve the JNI environment.
        JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();

        jclass zipClass = env->GetObjectClass(_zip);
        jmethodID closeMethod = env->GetMethodID(zipClass, "close", "()V");

        env->CallVoidMethod(zipClass, closeMethod);

        env->DeleteGlobalRef(_zip);
    }

    size_t GetNumFiles() const override
    {
        // retrieve the JNI environment.
        JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();

        jclass zipClass = env->GetObjectClass(_zip);
        jmethodID fileCountMethod = env->GetMethodID(zipClass, "getNumFiles", "()I");

        return (size_t) env->CallIntMethod(_zip, fileCountMethod);
    }

    const utf8 * GetFileName(size_t index) const override
    {
        // retrieve the JNI environment.
        JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();

        jclass zipClass = env->GetObjectClass(_zip);
        jmethodID fileNameMethod = env->GetMethodID(zipClass, "getFileName", "(I)Ljava/lang/String;");

        jstring jniString = (jstring) env->CallObjectMethod(_zip, fileNameMethod, (jint)index);

        const char *jniChars = env->GetStringUTFChars(jniString, nullptr);

        // TODO: Malloc our own copy

        env->ReleaseStringChars(jniString, jniChars);
        return nullptr;
    }

    uint64 GetFileSize(size_t index) const override
    {

        // retrieve the JNI environment.
        JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();

        jclass zipClass = env->GetObjectClass(_zip);
        jmethodID fileSizeMethod = env->GetMethodID(zipClass, "getFileSize", "(I)J");

        return (size_t) env->CallLongMethod(_zip, fileSizeMethod, (jint)index);
    }

    void * GetFileData(const utf8 * path, size_t * outSize) const override
    {
        // TODO: This should probably get implemented...
        STUB();
        return nullptr;
    }

    IStream * GetFileStream(const utf8 * path) const override
    {
        STUB();
        return nullptr;
    }

    void SetFileData(const utf8 * path, void * data, size_t dataSize) override
    {
        STUB();
    }

    void DeleteFile(const utf8 * path) override
    {
        STUB();
    }

    void RenameFile(const utf8 * path, const utf8 * newPath) override
    {
        STUB();
    }
};

namespace Zip
{
    IZipArchive * Open(const utf8 * path, ZIP_ACCESS access)
    {
        return new ZipArchive(path, access);
    }

    IZipArchive * TryOpen(const utf8 * path, ZIP_ACCESS access)
    {
        IZipArchive * result = nullptr;
        try
        {
            result = new ZipArchive(path, access);
        }
        catch (Exception)
        {
        }
        return result;
    }
}
