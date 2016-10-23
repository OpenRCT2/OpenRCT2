package org.openrct2.android;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;

import org.apache.commons.io.IOUtils;
import org.libsdl.app.SDLActivity;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class MainActivity extends AppCompatActivity {

    public static final String TAG = "OpenRCT2";
    private boolean assetsCopied = false;

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        if (hasRequiredPermissions()) {
            startGame();
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);


        if (!hasRequiredPermissions()) {
            ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1);
        } else {
            startGame();
        }
    }

    private boolean hasRequiredPermissions() {
        if (ContextCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED) {
            return false;
        }

        if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED) {
            return false;
        }

        return true;
    }

    private void startGame() {
        copyAssets();
        startActivity(new Intent(this, GameActivity.class));
        finish();
    }

    private void copyAssets() {
        File dataDir = new File("/sdcard/openrct2/");
        try {
            copyAsset(getAssets(), "data", dataDir, "");
        } catch (IOException e) {
            Log.e(TAG, "Error extracting files", e);
            return;
        }

        assetsCopied = true;
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

}
