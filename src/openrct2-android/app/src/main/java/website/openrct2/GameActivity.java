package website.openrct2;

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
                "jansson",
                "png16",
                "SDL2-2.0",

                "openrct2",
                "openrct2-ui"
        };
    }

    protected String[] getArguments() {
        if (getIntent().hasExtra("commandLineArgs")) {
            return getIntent().getStringArrayExtra("commandLineArgs");
        }
        return new String[0];
    }
}
