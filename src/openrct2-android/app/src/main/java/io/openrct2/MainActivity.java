package io.openrct2;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.graphics.Point;
import android.graphics.PointF;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;

import androidx.annotation.NonNull;
import androidx.core.app.ActivityCompat;
import androidx.core.content.ContextCompat;
import androidx.appcompat.app.AppCompatActivity;

import android.os.Environment;
import android.text.TextUtils;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.Surface;

import org.apache.commons.io.IOUtils;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;

public class MainActivity extends AppCompatActivity {

    public static final String TAG = "OpenRCT2";

    @Override
    public void onRequestPermissionsResult(
        int requestCode,
        @NonNull String[] permissions,
        @NonNull int[] grantResults) {
        super.onRequestPermissionsResult(requestCode, permissions, grantResults);

        if (hasRequiredPermissions()) {
            startGame();
        }
    }

    private String[] getSupportedAbis() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.LOLLIPOP) {
            return Build.SUPPORTED_ABIS;
        }

        if (!TextUtils.isEmpty(Build.CPU_ABI2)) {
            return new String[]{Build.CPU_ABI, Build.CPU_ABI2};
        }

        return new String[]{Build.CPU_ABI};
    }

    private PointF getResolutionDips() {
        PointF out = new PointF();
        Point pixelSize = new Point();

        Display display = getWindowManager().getDefaultDisplay();

        DisplayMetrics metrics = new DisplayMetrics();
        display.getMetrics(metrics);
        display.getRealSize(pixelSize);
        display.getRealMetrics(metrics);

        int rotation = display.getRotation();
        if (rotation == Surface.ROTATION_90 || rotation == Surface.ROTATION_270) {
            pixelSize = new Point(pixelSize.y, pixelSize.x);
        }

        out.x = ((float) pixelSize.x) / metrics.density;
        out.y = ((float) pixelSize.y) / metrics.density;

        return out;
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);

        String[] supportedAbis = getSupportedAbis();
        PointF resolution = getResolutionDips();
        DisplayMetrics displayMetrics = getResources().getDisplayMetrics();
    }

    @Override
    protected void onStart() {
        super.onStart();

        if (!hasRequiredPermissions()) {
            if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
                /*
                TODO - This is a temporary solution
                Ideally, OpenRCT2 assets should be shipped in the apk and we should ask
                the user for their rct2 assets directory on boot (similar to windows).
                Save data should be stored under the "external data directory",
                which you can get via `getExternalFilesDir(null).getAbsolutePath()`
                 */
                Intent intent = new Intent(android.provider.Settings.ACTION_MANAGE_APP_ALL_FILES_ACCESS_PERMISSION);
                intent.addCategory("android.intent.category.DEFAULT");
                intent.setData(Uri.fromParts("package", getPackageName(), null));
                startActivity(intent);
            } else {
                ActivityCompat.requestPermissions(this, new String[]{Manifest.permission.READ_EXTERNAL_STORAGE, Manifest.permission.WRITE_EXTERNAL_STORAGE}, 1);
            }
        } else {
            startGame();
        }
    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if (requestCode == 0) {
            if (!hasRequiredPermissions()) {
                Log.d(TAG, "User denied storage permission!");
            } else {
                startGame();
            }
        }
    }

    private boolean hasRequiredPermissions() {
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.R) {
            return Environment.isExternalStorageManager();
        } else {
            if (ContextCompat.checkSelfPermission(this, Manifest.permission.READ_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED) {
                return false;
            }

            if (ContextCompat.checkSelfPermission(this, Manifest.permission.WRITE_EXTERNAL_STORAGE) == PackageManager.PERMISSION_DENIED) {
                return false;
            }

            return true;
        }
    }

    private void startGame() {
        Intent intent = new Intent(this, GameActivity.class);
        if (getIntent().hasExtra("commandLineArgs")) {
            intent.putExtra("commandLineArgs", getIntent().getStringArrayExtra("commandLineArgs"));
        }
        startActivity(intent);
        finish();
    }
}
