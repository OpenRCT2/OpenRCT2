package io.openrct2;

import android.os.Build;
import android.os.LocaleList;
import android.view.View;

import androidx.annotation.NonNull;
import androidx.annotation.RequiresApi;
import androidx.core.os.LocaleListCompat;

import org.libsdl.app.SDLActivity;

import java.util.Arrays;
import java.util.Locale;

public class GameActivity extends SDLActivity {

    public float getDefaultScale() {
        return getResources().getDisplayMetrics().density;
    }

    public String getDefaultLocale() {
        Locale locale;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.N) {
            locale = getResources().getConfiguration().getLocales().get(0);
        } else {
            locale = getResources().getConfiguration().locale;
        }
        return locale.getLanguage() + "-" + locale.getCountry();
    }

    @Override
    protected String[] getLibraries() {
        return new String[]{
                "c++_shared",
                "speexdsp",
                "png16",
                "SDL2-2.0",

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
