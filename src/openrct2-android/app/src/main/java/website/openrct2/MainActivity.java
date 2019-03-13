package website.openrct2;

import android.Manifest;
import android.content.Intent;
import android.content.pm.PackageManager;
import android.content.res.AssetManager;
import android.graphics.Point;
import android.graphics.PointF;
import android.os.Build;
import android.os.Bundle;
import android.support.v4.app.ActivityCompat;
import android.support.v4.content.ContextCompat;
import android.support.v7.app.AppCompatActivity;
import android.text.TextUtils;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Display;
import android.view.Surface;

import com.google.android.gms.analytics.HitBuilders;
import com.google.android.gms.analytics.Tracker;

import org.apache.commons.io.IOUtils;

import java.io.File;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.lang.reflect.InvocationTargetException;
import java.lang.reflect.Method;

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

        boolean success = false;
        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.JELLY_BEAN_MR1) {
            display.getRealSize(pixelSize);
            display.getRealMetrics(metrics);

            success = true;
        } else {
            try {
                Method getRawHeight = Display.class.getMethod("getRawHeight");
                Method getRawWidth = Display.class.getMethod("getRawWidth");
                pixelSize.x = (Integer) getRawWidth.invoke(display);
                pixelSize.y = (Integer) getRawHeight.invoke(display);

                success = true;
            } catch (NoSuchMethodException e) {
                e.printStackTrace();
            } catch (InvocationTargetException e) {
                e.printStackTrace();
            } catch (IllegalAccessException e) {
                e.printStackTrace();
            }
        }

        if (!success) {
            // Fall back to viewport size
            display.getSize(pixelSize);
        }

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

        Tracker tracker = ((OpenRCT2App) getApplication()).getDefaultTracker();
        tracker.setScreenName("Main");
        tracker.setScreenResolution(Math.round(resolution.x), Math.round(resolution.y));
        tracker.send(new HitBuilders.ScreenViewBuilder()
                .setCustomDimension(1, Float.toString(displayMetrics.density))
                .setCustomDimension(2, TextUtils.join(", ", supportedAbis))
                .build()
        );
    }

    @Override
    protected void onStart() {
        super.onStart();

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
        Intent intent = new Intent(this, GameActivity.class);
        if (getIntent().hasExtra("commandLineArgs")) {
            intent.putExtra("commandLineArgs", getIntent().getStringArrayExtra("commandLineArgs"));
        }
        startActivity(intent);
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
