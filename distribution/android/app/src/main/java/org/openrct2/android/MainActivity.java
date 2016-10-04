package org.openrct2.android;

import android.Manifest;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.util.Log;

import org.apache.commons.io.IOUtils;
import org.libsdl.app.SDLActivity;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class MainActivity extends SDLActivity {

    public static final String TAG = "OpenRCT2";

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

        File dataDir = new File("/sdcard/openrct2/");
        try {
            copyAsset(getAssets(), "data", dataDir, "");
        } catch (IOException e) {
            Log.e(TAG, "Error extracting files", e);
            finish();
            return;
        }


        super.onCreate(savedInstanceState);
    }

    private void copyAsset(AssetManager assets, String srcPath, File dataDir, String destPath) throws IOException {
        String[] list = assets.list(srcPath);

        if (list.length == 0) {
            InputStream input = assets.open(srcPath);
            File extractedFile = new File(dataDir, destPath);
            File parentFile = extractedFile.getParentFile();
            if (!parentFile.exists()) {
                boolean success = parentFile.mkdirs();
                if (!success) {
                    Log.d(TAG, String.format("Error creating folder '%s'", parentFile));
                }
            }
            FileOutputStream output = new FileOutputStream(extractedFile);
            IOUtils.copyLarge(input, output);
            output.close();
            input.close();
            return;
        }

        for (String fileName : list) {
            copyAsset(assets, srcPath + File.separator + fileName, dataDir, destPath + File.separator + fileName);
        }
    }

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
