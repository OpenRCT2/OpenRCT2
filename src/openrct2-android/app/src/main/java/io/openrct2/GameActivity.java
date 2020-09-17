package io.openrct2;

import android.view.View;

import org.libsdl.app.SDLActivity;

public class GameActivity extends SDLActivity {

    public float getDefaultScale() {
        return getResources().getDisplayMetrics().density;
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
