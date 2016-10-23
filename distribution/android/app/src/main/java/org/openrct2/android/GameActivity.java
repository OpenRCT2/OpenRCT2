package org.openrct2.android;

import org.libsdl.app.SDLActivity;

public class GameActivity extends SDLActivity {

    public float getDefaultScale() {
        return getResources().getDisplayMetrics().density;
    }

    @Override
    protected String[] getLibraries() {
        return new String[]{
                "speexdsp",
                "jansson",

                "SDL2-2.0",

                "freetyped",
                "SDL2_ttf",

                "openrct2"
        };
    }
}
