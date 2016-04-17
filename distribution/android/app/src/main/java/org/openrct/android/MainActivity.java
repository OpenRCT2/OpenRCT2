package org.openrct.android;

import android.Manifest;
import android.content.pm.PackageManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;

import org.libsdl.app.SDLActivity;

public class MainActivity extends SDLActivity {

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {

        boolean canRead = false;
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED) {
            canRead = true;
        }

        boolean canWrite = false;
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_GRANTED) {
            canWrite = true;
        }

        if (!canRead || !canWrite) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1);
            return;
        }

        super.onCreate(savedInstanceState);
    }

    public float getDefaultScale() {
        return getResources().getDisplayMetrics().density;
    }


    @Override
    protected String[] getLibraries() {
        return new String[]{
                "c++_shared",

                "libspeexdsp",
                "libjansson",
                "libpng",

                "SDL2",

                "freetype",
                "SDL2_ttf",

                "main"
        };
    }

}
