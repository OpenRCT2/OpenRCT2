/*****************************************************************************
 * Copyright (c) 2014-2025 OpenRCT2 developers
 *
 * For a complete list of all authors, please refer to contributors.md
 * Interested in contributing? Visit https://github.com/OpenRCT2/OpenRCT2
 *
 * OpenRCT2 is licensed under the GNU General Public License version 3.
 *****************************************************************************/

#if !defined(DISABLE_HTTP) && defined(__ANDROID__)

    #include "Http.h"

    #include "../Version.h"
    #include "../platform/Platform.h"

    #include <SDL.h>
    #include <android/log.h>
    #include <jni.h>

    #define kOpenRCT2UserAgent "OpenRCT2/" kOpenRCT2Version

namespace OpenRCT2::Http
{
    Response Do(const Request& req)
    {
        std::map<std::string, std::string> headers = req.header;
        headers["User-Agent"] = kOpenRCT2UserAgent;
        // Lambda to convert jstring to string
        auto jstringToString = [](JNIEnv* env, jstring jstr) -> std::string {
            if (jstr == nullptr)
            {
                return "";
            }
            const char* cstr = env->GetStringUTFChars(jstr, nullptr);
            std::string str = cstr;
            env->ReleaseStringUTFChars(jstr, cstr);
            return str;
        };

        JNIEnv* env = (JNIEnv*)SDL_AndroidGetJNIEnv();

        // Create request object

        jclass HttpAndroidClass = Platform::AndroidFindClass(env, "io/openrct2/HttpAndroid");
        jclass req_class = Platform::AndroidFindClass(env, "io/openrct2/HttpAndroid$Request");
        // New request object
        jobject jniRequest = env->NewObject(req_class, env->GetMethodID(req_class, "<init>", "()V"));
        // Create request's headers map
        jobject jniHeaders = env->NewObject(
            env->FindClass("java/util/HashMap"),
            env->GetMethodID(env->GetObjectClass(env->FindClass("java/util/HashMap")), "<init>", "()V"));
        // HashMap's put method
        jmethodID putMethod = env->GetMethodID(
            env->FindClass("java/util/HashMap"), "put", "(Ljava/lang/Object;Ljava/lang/Object;)Ljava/lang/Object;");
        for (const auto& header : headers)
        {
            jstring jniKey = env->NewStringUTF(header.first.c_str());
            jstring jniValue = env->NewStringUTF(header.second.c_str());
            env->CallObjectMethod(jniHeaders, putMethod, jniKey, jniValue);
        }
        env->SetObjectField(jniRequest, env->GetFieldID(req_class, "headers", "Ljava/util/Map;"), jniHeaders);

        jstring req_body = req.body.empty() ? nullptr : env->NewStringUTF(req.body.c_str());
        jstring url = req.url.empty() ? nullptr : env->NewStringUTF(req.url.c_str());
        env->SetObjectField(jniRequest, env->GetFieldID(req_class, "body", "Ljava/lang/String;"), req_body);
        env->SetObjectField(jniRequest, env->GetFieldID(req_class, "url", "Ljava/lang/String;"), url);
        if (req_body != nullptr)
        {
            env->ReleaseStringUTFChars(req_body, env->GetStringUTFChars(req_body, nullptr));
        }
        if (url != nullptr)
        {
            env->ReleaseStringUTFChars(url, env->GetStringUTFChars(url, nullptr));
        }
        std::string method = "GET";
        switch (req.method)
        {
            case Method::GET:
                method = "GET";
                break;
            case Method::POST:
                method = "POST";
                break;
            case Method::PUT:
                method = "PUT";
                break;
        }
        env->SetObjectField(
            jniRequest, env->GetFieldID(req_class, "method", "Lio/openrct2/HttpAndroid$Method;"),
            env->GetStaticObjectField(
                Platform::AndroidFindClass(env, "io/openrct2/HttpAndroid$Method"),
                env->GetStaticFieldID(
                    Platform::AndroidFindClass(env, "io/openrct2/HttpAndroid$Method"), method.c_str(),
                    "Lio/openrct2/HttpAndroid$Method;")));
        // Call request method
        jmethodID requestMethod = env->GetStaticMethodID(
            HttpAndroidClass, "request", "(Lio/openrct2/HttpAndroid$Request;)Lio/openrct2/HttpAndroid$Response;");
        jobject jniResponse = env->CallStaticObjectMethod(HttpAndroidClass, requestMethod, jniRequest);
        jfieldID statusField = env->GetFieldID(env->GetObjectClass(jniResponse), "status", "Lio/openrct2/HttpAndroid$Status;");
        jobject jniStatus = env->GetObjectField(jniResponse, statusField);
        jclass statusClass = Platform::AndroidFindClass(env, "io/openrct2/HttpAndroid$Status");
        jmethodID getCodeMethod = env->GetMethodID(statusClass, "getCode", "()I");
        int code = env->CallIntMethod(jniStatus, getCodeMethod);

        Response response;

        // Get response's headers field
        jclass responseClass = env->GetObjectClass(jniResponse);
        jfieldID headersField = env->GetFieldID(responseClass, "headers", "Ljava/util/Map;");
        jobject jniHeadersMap = env->GetObjectField(jniResponse, headersField);

        jmethodID entrySetMethod = env->GetMethodID(env->GetObjectClass(jniHeadersMap), "entrySet", "()Ljava/util/Set;");
        jobject jniEntrySet = env->CallObjectMethod(jniHeadersMap, entrySetMethod);
        jmethodID iteratorMethod = env->GetMethodID(env->GetObjectClass(jniEntrySet), "iterator", "()Ljava/util/Iterator;");
        jobject jniIterator = env->CallObjectMethod(jniEntrySet, iteratorMethod);
        jmethodID hasNextMethod = env->GetMethodID(env->GetObjectClass(jniIterator), "hasNext", "()Z");
        jmethodID nextMethod = env->GetMethodID(env->GetObjectClass(jniIterator), "next", "()Ljava/lang/Object;");
        while (env->CallBooleanMethod(jniIterator, hasNextMethod))
        {
            jobject jniNext = env->CallObjectMethod(jniIterator, nextMethod);
            jmethodID getKeyMethod = env->GetMethodID(env->GetObjectClass(jniNext), "getKey", "()Ljava/lang/Object;");
            jmethodID getValueMethod = env->GetMethodID(env->GetObjectClass(jniNext), "getValue", "()Ljava/lang/Object;");
            jobject jniKey = env->CallObjectMethod(jniNext, getKeyMethod);
            // The first key is always null for some reason, its value contains the status code
            if (jniKey == nullptr)
            {
                env->DeleteLocalRef(jniKey);
                env->DeleteLocalRef(jniNext);
                continue;
            }
            jobject jniValue = env->CallObjectMethod(jniNext, getValueMethod);
            // convert jniValue to string
            jmethodID valueToStringMethod = env->GetMethodID(env->GetObjectClass(jniValue), "toString", "()Ljava/lang/String;");
            jstring jniValueString = (jstring)env->CallObjectMethod(jniValue, valueToStringMethod);
            jstring jniKeyString = (jstring)jniKey;
            const char* jniKeyChars = env->GetStringUTFChars(jniKeyString, nullptr);
            const char* jniValueChars = env->GetStringUTFChars(jniValueString, nullptr);
            std::string value = jniValueChars;
            std::string key = jniKeyChars;
            env->ReleaseStringUTFChars(jniKeyString, jniKeyChars);
            env->ReleaseStringUTFChars(jniValueString, jniValueChars);
            response.header[key] = value;

            // Cleanup
            env->DeleteLocalRef(jniValue);
            env->DeleteLocalRef(jniKey);
            env->DeleteLocalRef(jniNext);
        }
        // Get response's body field and convert it to string
        jfieldID bodyField = env->GetFieldID(responseClass, "body", "Ljava/lang/String;");
        jstring jniBody = (jstring)env->GetObjectField(jniResponse, bodyField);
        // Get response's error field and convert it to string
        jfieldID errorField = env->GetFieldID(responseClass, "error", "Ljava/lang/String;");
        jstring jniError = (jstring)env->GetObjectField(jniResponse, errorField);
        // Get response's content type field and convert it to string
        jfieldID contentTypeField = env->GetFieldID(responseClass, "contentType", "Ljava/lang/String;");
        jstring jniContentType = (jstring)env->GetObjectField(jniResponse, contentTypeField);
        // Prepare response
        response.error = jstringToString(env, jniError);
        response.content_type = jstringToString(env, jniContentType);
        response.status = static_cast<Status>(code);
        response.body = jstringToString(env, jniBody);

        env->DeleteLocalRef(jniResponse);
        env->DeleteLocalRef(jniStatus);
        env->DeleteLocalRef(jniHeadersMap);
        env->DeleteLocalRef(jniBody);
        env->DeleteLocalRef(jniContentType);
        env->DeleteLocalRef(jniError);
        env->DeleteLocalRef(jniHeaders);
        env->DeleteLocalRef(jniRequest);

        return response;
    }

} // namespace OpenRCT2::Http

#endif
