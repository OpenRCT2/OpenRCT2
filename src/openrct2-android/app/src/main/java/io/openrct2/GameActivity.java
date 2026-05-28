package io.openrct2;

import android.annotation.SuppressLint;
import android.content.res.Configuration;
import android.icu.util.Currency;
import android.icu.util.LocaleData;
import android.icu.util.ULocale;
import android.os.Build;
import android.os.Bundle;
import android.view.DisplayCutout;
import android.view.RoundedCorner;
import android.view.View;
import android.view.WindowInsets;

import org.libsdl.app.SDLActivity;

import java.util.Locale;

public class GameActivity extends SDLActivity {

    private static native void nativeSetSafeAreaInsets(int left, int top, int right, int bottom);

    public float getDefaultScale() {
        return getResources().getDisplayMetrics().density;
    }

    private void updateSafeAreaInsets(WindowInsets windowInsets) {
        int left = 0;
        int top = 0;
        int right = 0;
        int bottom = 0;

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            DisplayCutout displayCutout = windowInsets.getDisplayCutout();
            if (displayCutout != null) {
                left = Math.max(left, displayCutout.getSafeInsetLeft());
                top = Math.max(top, displayCutout.getSafeInsetTop());
                right = Math.max(right, displayCutout.getSafeInsetRight());
                bottom = Math.max(bottom, displayCutout.getSafeInsetBottom());
            }
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            RoundedCorner topLeft = windowInsets.getRoundedCorner(RoundedCorner.POSITION_TOP_LEFT);
            RoundedCorner topRight = windowInsets.getRoundedCorner(RoundedCorner.POSITION_TOP_RIGHT);
            RoundedCorner bottomLeft = windowInsets.getRoundedCorner(RoundedCorner.POSITION_BOTTOM_LEFT);
            RoundedCorner bottomRight = windowInsets.getRoundedCorner(RoundedCorner.POSITION_BOTTOM_RIGHT);

            if (topLeft != null) {
                left = Math.max(left, topLeft.getRadius());
                top = Math.max(top, topLeft.getRadius());
            }
            if (topRight != null) {
                right = Math.max(right, topRight.getRadius());
                top = Math.max(top, topRight.getRadius());
            }
            if (bottomLeft != null) {
                left = Math.max(left, bottomLeft.getRadius());
                bottom = Math.max(bottom, bottomLeft.getRadius());
            }
            if (bottomRight != null) {
                right = Math.max(right, bottomRight.getRadius());
                bottom = Math.max(bottom, bottomRight.getRadius());
            }
        }

        nativeSetSafeAreaInsets(left, top, right, bottom);
    }

    private void configureSafeAreaHandling() {
        if (mLayout == null) {
            return;
        }

        mLayout.setOnApplyWindowInsetsListener((view, windowInsets) -> {
            updateSafeAreaInsets(windowInsets);
            return windowInsets;
        });
        mLayout.requestApplyInsets();
    }

    private void applyImmersiveMode() {
        getWindow().getDecorView().setSystemUiVisibility(
            View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
            | View.SYSTEM_UI_FLAG_FULLSCREEN
            | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
            | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
            | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
            | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
        );
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        configureSafeAreaHandling();
    }

    @SuppressLint("ObsoleteSdkInt")
    private Locale getDeviceLocale() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            return getResources().getConfiguration().getLocales().get(0);
        } else {
            return getResources().getConfiguration().locale;
        }
    }
    public String getDefaultLocale(String[] supportedTags) {
        Locale deviceLocale = getDeviceLocale();

        for (String supportedTag : supportedTags) {
            if (supportedTag.isEmpty()) continue;
            String[] splits = supportedTag.split("-");
            String language = splits[0];
            String country = splits[1];
            if (deviceLocale.getLanguage().equals(language) && deviceLocale.getCountry().equals(country)) {
                return supportedTag;
            }
        }

        Locale canadaEn = Locale.CANADA;
        if (canadaEn.getLanguage().equals(deviceLocale.getLanguage()) && canadaEn.getCountry().equals(deviceLocale.getCountry())) {
            return "en-US";
        }

        for (String supportedTag : supportedTags) {
            if (supportedTag.isEmpty()) continue;
            String[] splits = supportedTag.split("-");
            String language = splits[0];
            if (deviceLocale.getLanguage().equals(language)) {
                return supportedTag;
            }
        }
        return "en-UK";
    }

    public String getLocaleCurrency() {
        Locale deviceLocale = getDeviceLocale();
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            return Currency.getInstance(deviceLocale).getCurrencyCode();
        } else {
            return java.util.Currency.getInstance(deviceLocale).getCurrencyCode();
        }
    }

    public boolean isImperialLocaleMeasurementFormat() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            return LocaleData.getMeasurementSystem(ULocale.forLocale(getResources().getConfiguration().getLocales().get(0))) == LocaleData.MeasurementSystem.US;
        }
        Locale deviceLocale = getDeviceLocale();
        String localeCountry = deviceLocale.getCountry();
        return localeCountry.equals(Locale.US.getCountry()) || localeCountry.equals(new Locale("xx", "LR").getCountry()) || localeCountry.equals(new Locale("xx", "MM").getCountry());
    }

    @Override
    protected String[] getLibraries() {
        return new String[]{
                "c++_shared",
                "bz2",
                "freetype",
                "z",
                "png16",
                "SDL2",
                "icudata",
                "icuuc",
                "icui18n",

                "openrct2",
                "openrct2-ui"
        };
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);

        // Set app to fullscreen mode
        if (hasFocus) {
            applyImmersiveMode();
            if (mLayout != null) {
                mLayout.requestApplyInsets();
            }
        }
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        super.onConfigurationChanged(newConfig);

        if (hasWindowFocus()) {
            applyImmersiveMode();
            if (mLayout != null) {
                mLayout.requestApplyInsets();
            }
        }
    }

    protected String[] getArguments() {
        if (getIntent().hasExtra("commandLineArgs")) {
            return getIntent().getStringArrayExtra("commandLineArgs");
        }
        return new String[0];
    }
}
