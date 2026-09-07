package io.openrct2;

import android.annotation.SuppressLint;
import android.icu.util.Currency;
import android.icu.util.LocaleData;
import android.icu.util.ULocale;
import android.os.Build;
import android.os.CombinedVibration;
import android.os.VibrationEffect;
import android.os.Vibrator;
import android.os.VibratorManager;
import android.view.DisplayCutout;
import android.view.View;
import android.view.WindowInsets;

import org.libsdl.app.SDLActivity;

import java.util.Locale;

public class GameActivity extends SDLActivity {

    public float getDefaultScale() {
        return getResources().getDisplayMetrics().density;
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

    /**
     * Called from native code. Returns {left, top, right, bottom} in pixels.
     *
     * The game runs edge to edge in immersive mode, so the camera cutout and the rounded corners
     * sit inside the drawable area. Anything drawn in these margins is physically not visible.
     */
    @SuppressLint("ObsoleteSdkInt")
    public int[] getSafeAreaInsets() {
        int[] insets = new int[]{ 0, 0, 0, 0 };

        View decorView = getWindow().getDecorView();
        WindowInsets windowInsets = decorView.getRootWindowInsets();
        if (windowInsets == null) {
            return insets;
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            android.graphics.Insets cutout = windowInsets.getInsets(WindowInsets.Type.displayCutout());
            insets[0] = cutout.left;
            insets[1] = cutout.top;
            insets[2] = cutout.right;
            insets[3] = cutout.bottom;
            return insets;
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.P) {
            DisplayCutout cutout = windowInsets.getDisplayCutout();
            if (cutout != null) {
                insets[0] = cutout.getSafeInsetLeft();
                insets[1] = cutout.getSafeInsetTop();
                insets[2] = cutout.getSafeInsetRight();
                insets[3] = cutout.getSafeInsetBottom();
            }
        }

        return insets;
    }

    /**
     * Called from native code. A touchscreen has no click and no cursor, so a gesture that fires
     * without moving anything visible gives no sign that it registered.
     */
    @SuppressLint("ObsoleteSdkInt")
    public void vibrate(int milliseconds) {
        if (milliseconds <= 0) {
            return;
        }

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.S) {
            VibratorManager manager = (VibratorManager) getSystemService(VIBRATOR_MANAGER_SERVICE);
            if (manager == null) {
                return;
            }
            manager.vibrate(CombinedVibration.createParallel(
                    VibrationEffect.createOneShot(milliseconds, VibrationEffect.DEFAULT_AMPLITUDE)));
            return;
        }

        Vibrator vibrator = (Vibrator) getSystemService(VIBRATOR_SERVICE);
        if (vibrator == null || !vibrator.hasVibrator()) {
            return;
        }
        vibrator.vibrate(VibrationEffect.createOneShot(milliseconds, VibrationEffect.DEFAULT_AMPLITUDE));
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
            getWindow().getDecorView().setSystemUiVisibility(
                View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY
                | View.SYSTEM_UI_FLAG_FULLSCREEN
                | View.SYSTEM_UI_FLAG_HIDE_NAVIGATION
                | View.SYSTEM_UI_FLAG_LAYOUT_STABLE
                | View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN
                | View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION
            );
        }
    }

    protected String[] getArguments() {
        if (getIntent().hasExtra("commandLineArgs")) {
            return getIntent().getStringArrayExtra("commandLineArgs");
        }
        return new String[0];
    }
}
