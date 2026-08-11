package io.openrct2;

/**
 * Platform constants exposed from C++ via JNI.
 * These constants are defined in the native code and exposed to Java to ensure
 * the same definitions are used in both languages.
 */
public class PlatformConstants {
    static {
        System.loadLibrary("openrct2");
    }

    /**
     * Android asset path prefix.
     * This constant is retrieved from the native C++ code to ensure consistency.
     */
    public static final String ANDROID_ASSET_PATH_PREFIX = getAndroidAssetPathPrefix();

    /**
     * Native method to get the Android asset path prefix from C++.
     * Defined in src/openrct2/platform/Platform.Android.cpp
     */
    private static native String getAndroidAssetPathPrefix();
}
