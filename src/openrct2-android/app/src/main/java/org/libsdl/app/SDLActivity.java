package org.libsdl.app;

import android.app.Activity;
import android.app.AlertDialog;
import android.app.Dialog;
import android.app.UiModeManager;
import android.content.ClipboardManager;
import android.content.ClipData;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.pm.ActivityInfo;
import android.content.pm.ApplicationInfo;
import android.content.pm.PackageManager;
import android.content.res.Configuration;
import android.graphics.Bitmap;
import android.graphics.Color;
import android.graphics.PorterDuff;
import android.graphics.drawable.Drawable;
import android.hardware.Sensor;
import android.net.Uri;
import android.os.Build;
import android.os.Bundle;
import android.os.Handler;
import android.os.Message;
import android.text.Editable;
import android.text.InputType;
import android.text.Selection;
import android.util.DisplayMetrics;
import android.util.Log;
import android.util.SparseArray;
import android.view.Display;
import android.view.Gravity;
import android.view.InputDevice;
import android.view.KeyEvent;
import android.view.PointerIcon;
import android.view.Surface;
import android.view.View;
import android.view.ViewGroup;
import android.view.Window;
import android.view.WindowManager;
import android.view.inputmethod.BaseInputConnection;
import android.view.inputmethod.EditorInfo;
import android.view.inputmethod.InputConnection;
import android.view.inputmethod.InputMethodManager;
import android.widget.Button;
import android.widget.EditText;
import android.widget.LinearLayout;
import android.widget.RelativeLayout;
import android.widget.TextView;
import android.widget.Toast;

import java.util.Hashtable;
import java.util.Locale;


/**
    SDL Activity
*/
public class SDLActivity extends Activity implements View.OnSystemUiVisibilityChangeListener {
    private static final String TAG = "SDL";
    private static final int SDL_MAJOR_VERSION = 2;
    private static final int SDL_MINOR_VERSION = 30;
    private static final int SDL_MICRO_VERSION = 9;
/*
    // Display InputType.SOURCE/CLASS of events and devices
    //
    // SDLActivity.debugSource(device.getSources(), "device[" + device.getName() + "]");
    // SDLActivity.debugSource(event.getSource(), "event");
    public static void debugSource(int sources, String prefix) {
        int s = sources;
        int s_copy = sources;
        String cls = "";
        String src = "";
        int tst = 0;
        int FLAG_TAINTED = 0x80000000;

        if ((s & InputDevice.SOURCE_CLASS_BUTTON) != 0)     cls += " BUTTON";
        if ((s & InputDevice.SOURCE_CLASS_JOYSTICK) != 0)   cls += " JOYSTICK";
        if ((s & InputDevice.SOURCE_CLASS_POINTER) != 0)    cls += " POINTER";
        if ((s & InputDevice.SOURCE_CLASS_POSITION) != 0)   cls += " POSITION";
        if ((s & InputDevice.SOURCE_CLASS_TRACKBALL) != 0)  cls += " TRACKBALL";


        int s2 = s_copy & ~InputDevice.SOURCE_ANY; // keep class bits
        s2 &= ~(  InputDevice.SOURCE_CLASS_BUTTON
                | InputDevice.SOURCE_CLASS_JOYSTICK
                | InputDevice.SOURCE_CLASS_POINTER
                | InputDevice.SOURCE_CLASS_POSITION
                | InputDevice.SOURCE_CLASS_TRACKBALL);

        if (s2 != 0) cls += "Some_Unknown";

        s2 = s_copy & InputDevice.SOURCE_ANY; // keep source only, no class;

        if (Build.VERSION.SDK_INT >= 23) {
            tst = InputDevice.SOURCE_BLUETOOTH_STYLUS;
            if ((s & tst) == tst) src += " BLUETOOTH_STYLUS";
            s2 &= ~tst;
        }

        tst = InputDevice.SOURCE_DPAD;
        if ((s & tst) == tst) src += " DPAD";
        s2 &= ~tst;

        tst = InputDevice.SOURCE_GAMEPAD;
        if ((s & tst) == tst) src += " GAMEPAD";
        s2 &= ~tst;

        if (Build.VERSION.SDK_INT >= 21) {
            tst = InputDevice.SOURCE_HDMI;
            if ((s & tst) == tst) src += " HDMI";
            s2 &= ~tst;
        }

        tst = InputDevice.SOURCE_JOYSTICK;
        if ((s & tst) == tst) src += " JOYSTICK";
        s2 &= ~tst;

        tst = InputDevice.SOURCE_KEYBOARD;
        if ((s & tst) == tst) src += " KEYBOARD";
        s2 &= ~tst;

        tst = InputDevice.SOURCE_MOUSE;
        if ((s & tst) == tst) src += " MOUSE";
        s2 &= ~tst;

        if (Build.VERSION.SDK_INT >= 26) {
            tst = InputDevice.SOURCE_MOUSE_RELATIVE;
            if ((s & tst) == tst) src += " MOUSE_RELATIVE";
            s2 &= ~tst;

            tst = InputDevice.SOURCE_ROTARY_ENCODER;
            if ((s & tst) == tst) src += " ROTARY_ENCODER";
            s2 &= ~tst;
        }
        tst = InputDevice.SOURCE_STYLUS;
        if ((s & tst) == tst) src += " STYLUS";
        s2 &= ~tst;

        tst = InputDevice.SOURCE_TOUCHPAD;
        if ((s & tst) == tst) src += " TOUCHPAD";
        s2 &= ~tst;

        tst = InputDevice.SOURCE_TOUCHSCREEN;
        if ((s & tst) == tst) src += " TOUCHSCREEN";
        s2 &= ~tst;

        if (Build.VERSION.SDK_INT >= 18) {
            tst = InputDevice.SOURCE_TOUCH_NAVIGATION;
            if ((s & tst) == tst) src += " TOUCH_NAVIGATION";
            s2 &= ~tst;
        }

        tst = InputDevice.SOURCE_TRACKBALL;
        if ((s & tst) == tst) src += " TRACKBALL";
        s2 &= ~tst;

        tst = InputDevice.SOURCE_ANY;
        if ((s & tst) == tst) src += " ANY";
        s2 &= ~tst;

        if (s == FLAG_TAINTED) src += " FLAG_TAINTED";
        s2 &= ~FLAG_TAINTED;

        if (s2 != 0) src += " Some_Unknown";

        Log.v(TAG, prefix + "int=" + s_copy + " CLASS={" + cls + " } source(s):" + src);
    }
*/

    public static boolean mIsResumedCalled, mHasFocus;
    public static final boolean mHasMultiWindow = (Build.VERSION.SDK_INT >= 24  /* Android 7.0 (N) */);

    // Cursor types
    // private static final int SDL_SYSTEM_CURSOR_NONE = -1;
    private static final int SDL_SYSTEM_CURSOR_ARROW = 0;
    private static final int SDL_SYSTEM_CURSOR_IBEAM = 1;
    private static final int SDL_SYSTEM_CURSOR_WAIT = 2;
    private static final int SDL_SYSTEM_CURSOR_CROSSHAIR = 3;
    private static final int SDL_SYSTEM_CURSOR_WAITARROW = 4;
    private static final int SDL_SYSTEM_CURSOR_SIZENWSE = 5;
    private static final int SDL_SYSTEM_CURSOR_SIZENESW = 6;
    private static final int SDL_SYSTEM_CURSOR_SIZEWE = 7;
    private static final int SDL_SYSTEM_CURSOR_SIZENS = 8;
    private static final int SDL_SYSTEM_CURSOR_SIZEALL = 9;
    private static final int SDL_SYSTEM_CURSOR_NO = 10;
    private static final int SDL_SYSTEM_CURSOR_HAND = 11;

    protected static final int SDL_ORIENTATION_UNKNOWN = 0;
    protected static final int SDL_ORIENTATION_LANDSCAPE = 1;
    protected static final int SDL_ORIENTATION_LANDSCAPE_FLIPPED = 2;
    protected static final int SDL_ORIENTATION_PORTRAIT = 3;
    protected static final int SDL_ORIENTATION_PORTRAIT_FLIPPED = 4;

    protected static int mCurrentOrientation;
    protected static Locale mCurrentLocale;

    // Handle the state of the native layer
    public enum NativeState {
           INIT, RESUMED, PAUSED
    }

    public static NativeState mNextNativeState;
    public static NativeState mCurrentNativeState;

    /** If shared libraries (e.g. SDL or the native application) could not be loaded. */
    public static boolean mBrokenLibraries = true;

    // Main components
    protected static SDLActivity mSingleton;
    protected static SDLSurface mSurface;
    protected static DummyEdit mTextEdit;
    protected static boolean mScreenKeyboardShown;
    protected static ViewGroup mLayout;
    protected static SDLClipboardHandler mClipboardHandler;
    protected static Hashtable<Integer, PointerIcon> mCursors;
    protected static int mLastCursorID;
    protected static SDLGenericMotionListener_API12 mMotionListener;
    protected static HIDDeviceManager mHIDDeviceManager;

    // This is what SDL runs in. It invokes SDL_main(), eventually
    protected static Thread mSDLThread;

    protected static SDLGenericMotionListener_API12 getMotionListener() {
        if (mMotionListener == null) {
            if (Build.VERSION.SDK_INT >= 26 /* Android 8.0 (O) */) {
                mMotionListener = new SDLGenericMotionListener_API26();
            } else if (Build.VERSION.SDK_INT >= 24 /* Android 7.0 (N) */) {
                mMotionListener = new SDLGenericMotionListener_API24();
            } else {
                mMotionListener = new SDLGenericMotionListener_API12();
            }
        }

        return mMotionListener;
    }

    /**
     * This method returns the name of the shared object with the application entry point
     * It can be overridden by derived classes.
     */
    protected String getMainSharedObject() {
        String library;
        String[] libraries = SDLActivity.mSingleton.getLibraries();
        if (libraries.length > 0) {
            library = "lib" + libraries[libraries.length - 1] + ".so";
        } else {
            library = "libmain.so";
        }
        return getContext().getApplicationInfo().nativeLibraryDir + "/" + library;
    }

    /**
     * This method returns the name of the application entry point
     * It can be overridden by derived classes.
     */
    protected String getMainFunction() {
        return "SDL_main";
    }

    /**
     * This method is called by SDL before loading the native shared libraries.
     * It can be overridden to provide names of shared libraries to be loaded.
     * The default implementation returns the defaults. It never returns null.
     * An array returned by a new implementation must at least contain "SDL2".
     * Also keep in mind that the order the libraries are loaded may matter.
     * @return names of shared libraries to be loaded (e.g. "SDL2", "main").
     */
    protected String[] getLibraries() {
        return new String[] {
            "SDL2",
            // "SDL2_image",
            // "SDL2_mixer",
            // "SDL2_net",
            // "SDL2_ttf",
            "main"
        };
    }

    // Load the .so
    public void loadLibraries() {
       for (String lib : getLibraries()) {
          SDL.loadLibrary(lib, this);
       }
    }

    /**
     * This method is called by SDL before starting the native application thread.
     * It can be overridden to provide the arguments after the application name.
     * The default implementation returns an empty array. It never returns null.
     * @return arguments for the native application.
     */
    protected String[] getArguments() {
        return new String[0];
    }

    public static void initialize() {
        // The static nature of the singleton and Android quirkyness force us to initialize everything here
        // Otherwise, when exiting the app and returning to it, these variables *keep* their pre exit values
        mSingleton = null;
        mSurface = null;
        mTextEdit = null;
        mLayout = null;
        mClipboardHandler = null;
        mCursors = new Hashtable<Integer, PointerIcon>();
        mLastCursorID = 0;
        mSDLThread = null;
        mIsResumedCalled = false;
        mHasFocus = true;
        mNextNativeState = NativeState.INIT;
        mCurrentNativeState = NativeState.INIT;
    }
    
    protected SDLSurface createSDLSurface(Context context) {
        return new SDLSurface(context);
    }

    // Setup
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        Log.v(TAG, "Device: " + Build.DEVICE);
        Log.v(TAG, "Model: " + Build.MODEL);
        Log.v(TAG, "onCreate()");
        super.onCreate(savedInstanceState);

        try {
            Thread.currentThread().setName("SDLActivity");
        } catch (Exception e) {
            Log.v(TAG, "modify thread properties failed " + e.toString());
        }

        // Load shared libraries
        String errorMsgBrokenLib = "";
        try {
            loadLibraries();
            mBrokenLibraries = false; /* success */
        } catch(UnsatisfiedLinkError e) {
            System.err.println(e.getMessage());
            mBrokenLibraries = true;
            errorMsgBrokenLib = e.getMessage();
        } catch(Exception e) {
            System.err.println(e.getMessage());
            mBrokenLibraries = true;
            errorMsgBrokenLib = e.getMessage();
        }

        if (!mBrokenLibraries) {
            String expected_version = String.valueOf(SDL_MAJOR_VERSION) + "." +
                                      String.valueOf(SDL_MINOR_VERSION) + "." +
                                      String.valueOf(SDL_MICRO_VERSION);
            String version = nativeGetVersion();
            if (!version.equals(expected_version)) {
                mBrokenLibraries = true;
                errorMsgBrokenLib = "SDL C/Java version mismatch (expected " + expected_version + ", got " + version + ")";
            }
        }

        if (mBrokenLibraries) {
            mSingleton = this;
            AlertDialog.Builder dlgAlert  = new AlertDialog.Builder(this);
            dlgAlert.setMessage("An error occurred while trying to start the application. Please try again and/or reinstall."
                  + System.getProperty("line.separator")
                  + System.getProperty("line.separator")
                  + "Error: " + errorMsgBrokenLib);
            dlgAlert.setTitle("SDL Error");
            dlgAlert.setPositiveButton("Exit",
                new DialogInterface.OnClickListener() {
                    @Override
                    public void onClick(DialogInterface dialog,int id) {
                        // if this button is clicked, close current activity
                        SDLActivity.mSingleton.finish();
                    }
                });
           dlgAlert.setCancelable(false);
           dlgAlert.create().show();

           return;
        }

        // Set up JNI
        SDL.setupJNI();

        // Initialize state
        SDL.initialize();

        // So we can call stuff from static callbacks
        mSingleton = this;
        SDL.setContext(this);

        mClipboardHandler = new SDLClipboardHandler();

        mHIDDeviceManager = HIDDeviceManager.acquire(this);

        // Set up the surface
        mSurface = createSDLSurface(this);

        mLayout = new RelativeLayout(this);
        mLayout.addView(mSurface);

        // Get our current screen orientation and pass it down.
        mCurrentOrientation = SDLActivity.getCurrentOrientation();
        // Only record current orientation
        SDLActivity.onNativeOrientationChanged(mCurrentOrientation);

        try {
            if (Build.VERSION.SDK_INT < 24 /* Android 7.0 (N) */) {
                mCurrentLocale = getContext().getResources().getConfiguration().locale;
            } else {
                mCurrentLocale = getContext().getResources().getConfiguration().getLocales().get(0);
            }
        } catch(Exception ignored) {
        }

        setContentView(mLayout);

        setWindowStyle(false);

        getWindow().getDecorView().setOnSystemUiVisibilityChangeListener(this);

        // Get filename from "Open with" of another application
        Intent intent = getIntent();
        if (intent != null && intent.getData() != null) {
            String filename = intent.getData().getPath();
            if (filename != null) {
                Log.v(TAG, "Got filename: " + filename);
                SDLActivity.onNativeDropFile(filename);
            }
        }
    }

    protected void pauseNativeThread() {
        mNextNativeState = NativeState.PAUSED;
        mIsResumedCalled = false;

        if (SDLActivity.mBrokenLibraries) {
            return;
        }

        SDLActivity.handleNativeState();
    }

    protected void resumeNativeThread() {
        mNextNativeState = NativeState.RESUMED;
        mIsResumedCalled = true;

        if (SDLActivity.mBrokenLibraries) {
           return;
        }

        SDLActivity.handleNativeState();
    }

    // Events
    @Override
    protected void onPause() {
        Log.v(TAG, "onPause()");
        super.onPause();

        if (mHIDDeviceManager != null) {
            mHIDDeviceManager.setFrozen(true);
        }
        if (!mHasMultiWindow) {
            pauseNativeThread();
        }
    }

    @Override
    protected void onResume() {
        Log.v(TAG, "onResume()");
        super.onResume();

        if (mHIDDeviceManager != null) {
            mHIDDeviceManager.setFrozen(false);
        }
        if (!mHasMultiWindow) {
            resumeNativeThread();
        }
    }

    @Override
    protected void onStop() {
        Log.v(TAG, "onStop()");
        super.onStop();
        if (mHasMultiWindow) {
            pauseNativeThread();
        }
    }

    @Override
    protected void onStart() {
        Log.v(TAG, "onStart()");
        super.onStart();
        if (mHasMultiWindow) {
            resumeNativeThread();
        }
    }

    public static int getCurrentOrientation() {
        int result = SDL_ORIENTATION_UNKNOWN;

        Activity activity = (Activity)getContext();
        if (activity == null) {
            return result;
        }
        Display display = activity.getWindowManager().getDefaultDisplay();

        switch (display.getRotation()) {
            case Surface.ROTATION_0:
                result = SDL_ORIENTATION_PORTRAIT;
                break;

            case Surface.ROTATION_90:
                result = SDL_ORIENTATION_LANDSCAPE;
                break;

            case Surface.ROTATION_180:
                result = SDL_ORIENTATION_PORTRAIT_FLIPPED;
                break;

            case Surface.ROTATION_270:
                result = SDL_ORIENTATION_LANDSCAPE_FLIPPED;
                break;
        }

        return result;
    }

    @Override
    public void onWindowFocusChanged(boolean hasFocus) {
        super.onWindowFocusChanged(hasFocus);
        Log.v(TAG, "onWindowFocusChanged(): " + hasFocus);

        if (SDLActivity.mBrokenLibraries) {
           return;
        }

        mHasFocus = hasFocus;
        if (hasFocus) {
           mNextNativeState = NativeState.RESUMED;
           SDLActivity.getMotionListener().reclaimRelativeMouseModeIfNeeded();

           SDLActivity.handleNativeState();
           nativeFocusChanged(true);

        } else {
           nativeFocusChanged(false);
           if (!mHasMultiWindow) {
               mNextNativeState = NativeState.PAUSED;
               SDLActivity.handleNativeState();
           }
        }
    }

    @Override
    public void onLowMemory() {
        Log.v(TAG, "onLowMemory()");
        super.onLowMemory();

        if (SDLActivity.mBrokenLibraries) {
           return;
        }

        SDLActivity.nativeLowMemory();
    }

    @Override
    public void onConfigurationChanged(Configuration newConfig) {
        Log.v(TAG, "onConfigurationChanged()");
        super.onConfigurationChanged(newConfig);

        if (SDLActivity.mBrokenLibraries) {
           return;
        }

        if (mCurrentLocale == null || !mCurrentLocale.equals(newConfig.locale)) {
            mCurrentLocale = newConfig.locale;
            SDLActivity.onNativeLocaleChanged();
        }
    }

    @Override
    protected void onDestroy() {
        Log.v(TAG, "onDestroy()");

        if (mHIDDeviceManager != null) {
            HIDDeviceManager.release(mHIDDeviceManager);
            mHIDDeviceManager = null;
        }

        SDLAudioManager.release(this);

        if (SDLActivity.mBrokenLibraries) {
           super.onDestroy();
           return;
        }

        if (SDLActivity.mSDLThread != null) {

            // Send Quit event to "SDLThread" thread
            SDLActivity.nativeSendQuit();

            // Wait for "SDLThread" thread to end
            try {
                SDLActivity.mSDLThread.join();
            } catch(Exception e) {
                Log.v(TAG, "Problem stopping SDLThread: " + e);
            }
        }

        SDLActivity.nativeQuit();

        super.onDestroy();
    }

    @Override
    public void onBackPressed() {
        // Check if we want to block the back button in case of mouse right click.
        //
        // If we do, the normal hardware back button will no longer work and people have to use home,
        // but the mouse right click will work.
        //
        boolean trapBack = SDLActivity.nativeGetHintBoolean("SDL_ANDROID_TRAP_BACK_BUTTON", false);
        if (trapBack) {
            // Exit and let the mouse handler handle this button (if appropriate)
            return;
        }

        // Default system back button behavior.
        if (!isFinishing()) {
            super.onBackPressed();
        }
    }

    // Called by JNI from SDL.
    public static void manualBackButton() {
        mSingleton.pressBackButton();
    }

    // Used to get us onto the activity's main thread
    public void pressBackButton() {
        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                if (!SDLActivity.this.isFinishing()) {
                    SDLActivity.this.superOnBackPressed();
                }
            }
        });
    }

    // Used to access the system back behavior.
    public void superOnBackPressed() {
        super.onBackPressed();
    }

    @Override
    public boolean dispatchKeyEvent(KeyEvent event) {

        if (SDLActivity.mBrokenLibraries) {
           return false;
        }

        int keyCode = event.getKeyCode();
        // Ignore certain special keys so they're handled by Android
        if (keyCode == KeyEvent.KEYCODE_VOLUME_DOWN ||
            keyCode == KeyEvent.KEYCODE_VOLUME_UP ||
            keyCode == KeyEvent.KEYCODE_CAMERA ||
            keyCode == KeyEvent.KEYCODE_ZOOM_IN || /* API 11 */
            keyCode == KeyEvent.KEYCODE_ZOOM_OUT /* API 11 */
            ) {
            return false;
        }
        return super.dispatchKeyEvent(event);
    }

    /* Transition to next state */
    public static void handleNativeState() {

        if (mNextNativeState == mCurrentNativeState) {
            // Already in same state, discard.
            return;
        }

        // Try a transition to init state
        if (mNextNativeState == NativeState.INIT) {

            mCurrentNativeState = mNextNativeState;
            return;
        }

        // Try a transition to paused state
        if (mNextNativeState == NativeState.PAUSED) {
            if (mSDLThread != null) {
                nativePause();
            }
            if (mSurface != null) {
                mSurface.handlePause();
            }
            mCurrentNativeState = mNextNativeState;
            return;
        }

        // Try a transition to resumed state
        if (mNextNativeState == NativeState.RESUMED) {
            if (mSurface.mIsSurfaceReady && mHasFocus && mIsResumedCalled) {
                if (mSDLThread == null) {
                    // This is the entry point to the C app.
                    // Start up the C app thread and enable sensor input for the first time
                    // FIXME: Why aren't we enabling sensor input at start?

                    mSDLThread = new Thread(new SDLMain(), "SDLThread");
                    mSurface.enableSensor(Sensor.TYPE_ACCELEROMETER, true);
                    mSDLThread.start();

                    // No nativeResume(), don't signal Android_ResumeSem
                } else {
                    nativeResume();
                }
                mSurface.handleResume();

                mCurrentNativeState = mNextNativeState;
            }
        }
    }

    // Messages from the SDLMain thread
    static final int COMMAND_CHANGE_TITLE = 1;
    static final int COMMAND_CHANGE_WINDOW_STYLE = 2;
    static final int COMMAND_TEXTEDIT_HIDE = 3;
    static final int COMMAND_SET_KEEP_SCREEN_ON = 5;

    protected static final int COMMAND_USER = 0x8000;

    protected static boolean mFullscreenModeActive;

    /**
     * This method is called by SDL if SDL did not handle a message itself.
     * This happens if a received message contains an unsupported command.
     * Method can be overwritten to handle Messages in a different class.
     * @param command the command of the message.
     * @param param the parameter of the message. May be null.
     * @return if the message was handled in overridden method.
     */
    protected boolean onUnhandledMessage(int command, Object param) {
        return false;
    }

    /**
     * A Handler class for Messages from native SDL applications.
     * It uses current Activities as target (e.g. for the title).
     * static to prevent implicit references to enclosing object.
     */
    protected static class SDLCommandHandler extends Handler {
        @Override
        public void handleMessage(Message msg) {
            Context context = SDL.getContext();
            if (context == null) {
                Log.e(TAG, "error handling message, getContext() returned null");
                return;
            }
            switch (msg.arg1) {
            case COMMAND_CHANGE_TITLE:
                if (context instanceof Activity) {
                    ((Activity) context).setTitle((String)msg.obj);
                } else {
                    Log.e(TAG, "error handling message, getContext() returned no Activity");
                }
                break;
            case COMMAND_CHANGE_WINDOW_STYLE:
                if (Build.VERSION.SDK_INT >= 19 /* Android 4.4 (KITKAT) */) {
                    if (context instanceof Activity) {
                        Window window = ((Activity) context).getWindow();
                        if (window != null) {
                            if ((msg.obj instanceof Integer) && ((Integer) msg.obj != 0)) {
                                int flags = View.SYSTEM_UI_FLAG_FULLSCREEN |
                                        View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
                                        View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY |
                                        View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |
                                        View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION |
                                        View.SYSTEM_UI_FLAG_LAYOUT_STABLE | View.INVISIBLE;
                                window.getDecorView().setSystemUiVisibility(flags);
                                window.addFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
                                window.clearFlags(WindowManager.LayoutParams.FLAG_FORCE_NOT_FULLSCREEN);
                                SDLActivity.mFullscreenModeActive = true;
                            } else {
                                int flags = View.SYSTEM_UI_FLAG_LAYOUT_STABLE | View.SYSTEM_UI_FLAG_VISIBLE;
                                window.getDecorView().setSystemUiVisibility(flags);
                                window.addFlags(WindowManager.LayoutParams.FLAG_FORCE_NOT_FULLSCREEN);
                                window.clearFlags(WindowManager.LayoutParams.FLAG_FULLSCREEN);
                                SDLActivity.mFullscreenModeActive = false;
                            }
                        }
                    } else {
                        Log.e(TAG, "error handling message, getContext() returned no Activity");
                    }
                }
                break;
            case COMMAND_TEXTEDIT_HIDE:
                if (mTextEdit != null) {
                    // Note: On some devices setting view to GONE creates a flicker in landscape.
                    // Setting the View's sizes to 0 is similar to GONE but without the flicker.
                    // The sizes will be set to useful values when the keyboard is shown again.
                    mTextEdit.setLayoutParams(new RelativeLayout.LayoutParams(0, 0));

                    InputMethodManager imm = (InputMethodManager) context.getSystemService(Context.INPUT_METHOD_SERVICE);
                    imm.hideSoftInputFromWindow(mTextEdit.getWindowToken(), 0);

                    mScreenKeyboardShown = false;

                    mSurface.requestFocus();
                }
                break;
            case COMMAND_SET_KEEP_SCREEN_ON:
            {
                if (context instanceof Activity) {
                    Window window = ((Activity) context).getWindow();
                    if (window != null) {
                        if ((msg.obj instanceof Integer) && ((Integer) msg.obj != 0)) {
                            window.addFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
                        } else {
                            window.clearFlags(WindowManager.LayoutParams.FLAG_KEEP_SCREEN_ON);
                        }
                    }
                }
                break;
            }
            default:
                if ((context instanceof SDLActivity) && !((SDLActivity) context).onUnhandledMessage(msg.arg1, msg.obj)) {
                    Log.e(TAG, "error handling message, command is " + msg.arg1);
                }
            }
        }
    }

    // Handler for the messages
    Handler commandHandler = new SDLCommandHandler();

    // Send a message from the SDLMain thread
    boolean sendCommand(int command, Object data) {
        Message msg = commandHandler.obtainMessage();
        msg.arg1 = command;
        msg.obj = data;
        boolean result = commandHandler.sendMessage(msg);

        if (Build.VERSION.SDK_INT >= 19 /* Android 4.4 (KITKAT) */) {
            if (command == COMMAND_CHANGE_WINDOW_STYLE) {
                // Ensure we don't return until the resize has actually happened,
                // or 500ms have passed.

                boolean bShouldWait = false;

                if (data instanceof Integer) {
                    // Let's figure out if we're already laid out fullscreen or not.
                    Display display = ((WindowManager) getSystemService(Context.WINDOW_SERVICE)).getDefaultDisplay();
                    DisplayMetrics realMetrics = new DisplayMetrics();
                    display.getRealMetrics(realMetrics);

                    boolean bFullscreenLayout = ((realMetrics.widthPixels == mSurface.getWidth()) &&
                            (realMetrics.heightPixels == mSurface.getHeight()));

                    if ((Integer) data == 1) {
                        // If we aren't laid out fullscreen or actively in fullscreen mode already, we're going
                        // to change size and should wait for surfaceChanged() before we return, so the size
                        // is right back in native code.  If we're already laid out fullscreen, though, we're
                        // not going to change size even if we change decor modes, so we shouldn't wait for
                        // surfaceChanged() -- which may not even happen -- and should return immediately.
                        bShouldWait = !bFullscreenLayout;
                    } else {
                        // If we're laid out fullscreen (even if the status bar and nav bar are present),
                        // or are actively in fullscreen, we're going to change size and should wait for
                        // surfaceChanged before we return, so the size is right back in native code.
                        bShouldWait = bFullscreenLayout;
                    }
                }

                if (bShouldWait && (SDLActivity.getContext() != null)) {
                    // We'll wait for the surfaceChanged() method, which will notify us
                    // when called.  That way, we know our current size is really the
                    // size we need, instead of grabbing a size that's still got
                    // the navigation and/or status bars before they're hidden.
                    //
                    // We'll wait for up to half a second, because some devices
                    // take a surprisingly long time for the surface resize, but
                    // then we'll just give up and return.
                    //
                    synchronized (SDLActivity.getContext()) {
                        try {
                            SDLActivity.getContext().wait(500);
                        } catch (InterruptedException ie) {
                            ie.printStackTrace();
                        }
                    }
                }
            }
        }

        return result;
    }

    // C functions we call
    public static native String nativeGetVersion();
    public static native int nativeSetupJNI();
    public static native int nativeRunMain(String library, String function, Object arguments);
    public static native void nativeLowMemory();
    public static native void nativeSendQuit();
    public static native void nativeQuit();
    public static native void nativePause();
    public static native void nativeResume();
    public static native void nativeFocusChanged(boolean hasFocus);
    public static native void onNativeDropFile(String filename);
    public static native void nativeSetScreenResolution(int surfaceWidth, int surfaceHeight, int deviceWidth, int deviceHeight, float rate);
    public static native void onNativeResize();
    public static native void onNativeKeyDown(int keycode);
    public static native void onNativeKeyUp(int keycode);
    public static native boolean onNativeSoftReturnKey();
    public static native void onNativeKeyboardFocusLost();
    public static native void onNativeMouse(int button, int action, float x, float y, boolean relative);
    public static native void onNativeTouch(int touchDevId, int pointerFingerId,
                                            int action, float x,
                                            float y, float p);
    public static native void onNativeAccel(float x, float y, float z);
    public static native void onNativeClipboardChanged();
    public static native void onNativeSurfaceCreated();
    public static native void onNativeSurfaceChanged();
    public static native void onNativeSurfaceDestroyed();
    public static native String nativeGetHint(String name);
    public static native boolean nativeGetHintBoolean(String name, boolean default_value);
    public static native void nativeSetenv(String name, String value);
    public static native void onNativeOrientationChanged(int orientation);
    public static native void nativeAddTouch(int touchId, String name);
    public static native void nativePermissionResult(int requestCode, boolean result);
    public static native void onNativeLocaleChanged();

    /**
     * This method is called by SDL using JNI.
     */
    public static boolean setActivityTitle(String title) {
        // Called from SDLMain() thread and can't directly affect the view
        return mSingleton.sendCommand(COMMAND_CHANGE_TITLE, title);
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static void setWindowStyle(boolean fullscreen) {
        // Called from SDLMain() thread and can't directly affect the view
        mSingleton.sendCommand(COMMAND_CHANGE_WINDOW_STYLE, fullscreen ? 1 : 0);
    }

    /**
     * This method is called by SDL using JNI.
     * This is a static method for JNI convenience, it calls a non-static method
     * so that is can be overridden
     */
    public static void setOrientation(int w, int h, boolean resizable, String hint)
    {
        if (mSingleton != null) {
            mSingleton.setOrientationBis(w, h, resizable, hint);
        }
    }

    /**
     * This can be overridden
     */
    public void setOrientationBis(int w, int h, boolean resizable, String hint)
    {
        int orientation_landscape = -1;
        int orientation_portrait = -1;

        /* If set, hint "explicitly controls which UI orientations are allowed". */
        if (hint.contains("LandscapeRight") && hint.contains("LandscapeLeft")) {
            orientation_landscape = ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE;
        } else if (hint.contains("LandscapeLeft")) {
            orientation_landscape = ActivityInfo.SCREEN_ORIENTATION_LANDSCAPE;
        } else if (hint.contains("LandscapeRight")) {
            orientation_landscape = ActivityInfo.SCREEN_ORIENTATION_REVERSE_LANDSCAPE;
        }

        /* exact match to 'Portrait' to distinguish with PortraitUpsideDown */
        boolean contains_Portrait = hint.contains("Portrait ") || hint.endsWith("Portrait");

        if (contains_Portrait && hint.contains("PortraitUpsideDown")) {
            orientation_portrait = ActivityInfo.SCREEN_ORIENTATION_SENSOR_PORTRAIT;
        } else if (contains_Portrait) {
            orientation_portrait = ActivityInfo.SCREEN_ORIENTATION_PORTRAIT;
        } else if (hint.contains("PortraitUpsideDown")) {
            orientation_portrait = ActivityInfo.SCREEN_ORIENTATION_REVERSE_PORTRAIT;
        }

        boolean is_landscape_allowed = (orientation_landscape != -1);
        boolean is_portrait_allowed = (orientation_portrait != -1);
        int req; /* Requested orientation */

        /* No valid hint, nothing is explicitly allowed */
        if (!is_portrait_allowed && !is_landscape_allowed) {
            if (resizable) {
                /* All orientations are allowed, respecting user orientation lock setting */
                req = ActivityInfo.SCREEN_ORIENTATION_FULL_USER;
            } else {
                /* Fixed window and nothing specified. Get orientation from w/h of created window */
                req = (w > h ? ActivityInfo.SCREEN_ORIENTATION_SENSOR_LANDSCAPE : ActivityInfo.SCREEN_ORIENTATION_SENSOR_PORTRAIT);
            }
        } else {
            /* At least one orientation is allowed */
            if (resizable) {
                if (is_portrait_allowed && is_landscape_allowed) {
                    /* hint allows both landscape and portrait, promote to full user */
                    req = ActivityInfo.SCREEN_ORIENTATION_FULL_USER;
                } else {
                    /* Use the only one allowed "orientation" */
                    req = (is_landscape_allowed ? orientation_landscape : orientation_portrait);
                }
            } else {
                /* Fixed window and both orientations are allowed. Choose one. */
                if (is_portrait_allowed && is_landscape_allowed) {
                    req = (w > h ? orientation_landscape : orientation_portrait);
                } else {
                    /* Use the only one allowed "orientation" */
                    req = (is_landscape_allowed ? orientation_landscape : orientation_portrait);
                }
            }
        }

        Log.v(TAG, "setOrientation() requestedOrientation=" + req + " width=" + w +" height="+ h +" resizable=" + resizable + " hint=" + hint);
        mSingleton.setRequestedOrientation(req);
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static void minimizeWindow() {

        if (mSingleton == null) {
            return;
        }

        Intent startMain = new Intent(Intent.ACTION_MAIN);
        startMain.addCategory(Intent.CATEGORY_HOME);
        startMain.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
        mSingleton.startActivity(startMain);
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static boolean shouldMinimizeOnFocusLoss() {
/*
        if (Build.VERSION.SDK_INT >= 24) {
            if (mSingleton == null) {
                return true;
            }

            if (mSingleton.isInMultiWindowMode()) {
                return false;
            }

            if (mSingleton.isInPictureInPictureMode()) {
                return false;
            }
        }

        return true;
*/
        return false;
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static boolean isScreenKeyboardShown()
    {
        if (mTextEdit == null) {
            return false;
        }

        if (!mScreenKeyboardShown) {
            return false;
        }

        InputMethodManager imm = (InputMethodManager) SDL.getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
        return imm.isAcceptingText();

    }

    /**
     * This method is called by SDL using JNI.
     */
    public static boolean supportsRelativeMouse()
    {
        // DeX mode in Samsung Experience 9.0 and earlier doesn't support relative mice properly under
        // Android 7 APIs, and simply returns no data under Android 8 APIs.
        //
        // This is fixed in Samsung Experience 9.5, which corresponds to Android 8.1.0, and
        // thus SDK version 27.  If we are in DeX mode and not API 27 or higher, as a result,
        // we should stick to relative mode.
        //
        if (Build.VERSION.SDK_INT < 27 /* Android 8.1 (O_MR1) */ && isDeXMode()) {
            return false;
        }

        return SDLActivity.getMotionListener().supportsRelativeMouse();
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static boolean setRelativeMouseEnabled(boolean enabled)
    {
        if (enabled && !supportsRelativeMouse()) {
            return false;
        }

        return SDLActivity.getMotionListener().setRelativeMouseEnabled(enabled);
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static boolean sendMessage(int command, int param) {
        if (mSingleton == null) {
            return false;
        }
        return mSingleton.sendCommand(command, param);
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static Context getContext() {
        return SDL.getContext();
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static boolean isAndroidTV() {
        UiModeManager uiModeManager = (UiModeManager) getContext().getSystemService(UI_MODE_SERVICE);
        if (uiModeManager.getCurrentModeType() == Configuration.UI_MODE_TYPE_TELEVISION) {
            return true;
        }
        if (Build.MANUFACTURER.equals("MINIX") && Build.MODEL.equals("NEO-U1")) {
            return true;
        }
        if (Build.MANUFACTURER.equals("Amlogic") && Build.MODEL.equals("X96-W")) {
            return true;
        }
        return Build.MANUFACTURER.equals("Amlogic") && Build.MODEL.startsWith("TV");
    }

    public static double getDiagonal()
    {
        DisplayMetrics metrics = new DisplayMetrics();
        Activity activity = (Activity)getContext();
        if (activity == null) {
            return 0.0;
        }
        activity.getWindowManager().getDefaultDisplay().getMetrics(metrics);

        double dWidthInches = metrics.widthPixels / (double)metrics.xdpi;
        double dHeightInches = metrics.heightPixels / (double)metrics.ydpi;

        return Math.sqrt((dWidthInches * dWidthInches) + (dHeightInches * dHeightInches));
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static boolean isTablet() {
        // If our diagonal size is seven inches or greater, we consider ourselves a tablet.
        return (getDiagonal() >= 7.0);
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static boolean isChromebook() {
        if (getContext() == null) {
            return false;
        }
        return getContext().getPackageManager().hasSystemFeature("org.chromium.arc.device_management");
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static boolean isDeXMode() {
        if (Build.VERSION.SDK_INT < 24 /* Android 7.0 (N) */) {
            return false;
        }
        try {
            final Configuration config = getContext().getResources().getConfiguration();
            final Class<?> configClass = config.getClass();
            return configClass.getField("SEM_DESKTOP_MODE_ENABLED").getInt(configClass)
                    == configClass.getField("semDesktopModeEnabled").getInt(config);
        } catch(Exception ignored) {
            return false;
        }
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static DisplayMetrics getDisplayDPI() {
        return getContext().getResources().getDisplayMetrics();
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static boolean getManifestEnvironmentVariables() {
        try {
            if (getContext() == null) {
                return false;
            }

            ApplicationInfo applicationInfo = getContext().getPackageManager().getApplicationInfo(getContext().getPackageName(), PackageManager.GET_META_DATA);
            Bundle bundle = applicationInfo.metaData;
            if (bundle == null) {
                return false;
            }
            String prefix = "SDL_ENV.";
            final int trimLength = prefix.length();
            for (String key : bundle.keySet()) {
                if (key.startsWith(prefix)) {
                    String name = key.substring(trimLength);
                    String value = bundle.get(key).toString();
                    nativeSetenv(name, value);
                }
            }
            /* environment variables set! */
            return true;
        } catch (Exception e) {
           Log.v(TAG, "exception " + e.toString());
        }
        return false;
    }

    // This method is called by SDLControllerManager's API 26 Generic Motion Handler.
    public static View getContentView() {
        return mLayout;
    }

    static class ShowTextInputTask implements Runnable {
        /*
         * This is used to regulate the pan&scan method to have some offset from
         * the bottom edge of the input region and the top edge of an input
         * method (soft keyboard)
         */
        static final int HEIGHT_PADDING = 15;

        public int x, y, w, h;

        public ShowTextInputTask(int x, int y, int w, int h) {
            this.x = x;
            this.y = y;
            this.w = w;
            this.h = h;

            /* Minimum size of 1 pixel, so it takes focus. */
            if (this.w <= 0) {
                this.w = 1;
            }
            if (this.h + HEIGHT_PADDING <= 0) {
                this.h = 1 - HEIGHT_PADDING;
            }
        }

        @Override
        public void run() {
            RelativeLayout.LayoutParams params = new RelativeLayout.LayoutParams(w, h + HEIGHT_PADDING);
            params.leftMargin = x;
            params.topMargin = y;

            if (mTextEdit == null) {
                mTextEdit = new DummyEdit(SDL.getContext());

                mLayout.addView(mTextEdit, params);
            } else {
                mTextEdit.setLayoutParams(params);
            }

            mTextEdit.setVisibility(View.VISIBLE);
            mTextEdit.requestFocus();

            InputMethodManager imm = (InputMethodManager) SDL.getContext().getSystemService(Context.INPUT_METHOD_SERVICE);
            imm.showSoftInput(mTextEdit, 0);

            mScreenKeyboardShown = true;
        }
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static boolean showTextInput(int x, int y, int w, int h) {
        // Transfer the task to the main thread as a Runnable
        return mSingleton.commandHandler.post(new ShowTextInputTask(x, y, w, h));
    }

    public static boolean isTextInputEvent(KeyEvent event) {

        // Key pressed with Ctrl should be sent as SDL_KEYDOWN/SDL_KEYUP and not SDL_TEXTINPUT
        if (event.isCtrlPressed()) {
            return false;
        }

        return event.isPrintingKey() || event.getKeyCode() == KeyEvent.KEYCODE_SPACE;
    }

    public static boolean handleKeyEvent(View v, int keyCode, KeyEvent event, InputConnection ic) {
        int deviceId = event.getDeviceId();
        int source = event.getSource();

        if (source == InputDevice.SOURCE_UNKNOWN) {
            InputDevice device = InputDevice.getDevice(deviceId);
            if (device != null) {
                source = device.getSources();
            }
        }

//        if (event.getAction() == KeyEvent.ACTION_DOWN) {
//            Log.v("SDL", "key down: " + keyCode + ", deviceId = " + deviceId + ", source = " + source);
//        } else if (event.getAction() == KeyEvent.ACTION_UP) {
//            Log.v("SDL", "key up: " + keyCode + ", deviceId = " + deviceId + ", source = " + source);
//        }

        // Dispatch the different events depending on where they come from
        // Some SOURCE_JOYSTICK, SOURCE_DPAD or SOURCE_GAMEPAD are also SOURCE_KEYBOARD
        // So, we try to process them as JOYSTICK/DPAD/GAMEPAD events first, if that fails we try them as KEYBOARD
        //
        // Furthermore, it's possible a game controller has SOURCE_KEYBOARD and
        // SOURCE_JOYSTICK, while its key events arrive from the keyboard source
        // So, retrieve the device itself and check all of its sources
        if (SDLControllerManager.isDeviceSDLJoystick(deviceId)) {
            // Note that we process events with specific key codes here
            if (event.getAction() == KeyEvent.ACTION_DOWN) {
                if (SDLControllerManager.onNativePadDown(deviceId, keyCode) == 0) {
                    return true;
                }
            } else if (event.getAction() == KeyEvent.ACTION_UP) {
                if (SDLControllerManager.onNativePadUp(deviceId, keyCode) == 0) {
                    return true;
                }
            }
        }

        if ((source & InputDevice.SOURCE_MOUSE) == InputDevice.SOURCE_MOUSE) {
            // on some devices key events are sent for mouse BUTTON_BACK/FORWARD presses
            // they are ignored here because sending them as mouse input to SDL is messy
            if ((keyCode == KeyEvent.KEYCODE_BACK) || (keyCode == KeyEvent.KEYCODE_FORWARD)) {
                switch (event.getAction()) {
                case KeyEvent.ACTION_DOWN:
                case KeyEvent.ACTION_UP:
                    // mark the event as handled or it will be handled by system
                    // handling KEYCODE_BACK by system will call onBackPressed()
                    return true;
                }
            }
        }

        if (event.getAction() == KeyEvent.ACTION_DOWN) {
            if (isTextInputEvent(event)) {
                if (ic != null) {
                    ic.commitText(String.valueOf((char) event.getUnicodeChar()), 1);
                } else {
                    SDLInputConnection.nativeCommitText(String.valueOf((char) event.getUnicodeChar()), 1);
                }
            }
            onNativeKeyDown(keyCode);
            return true;
        } else if (event.getAction() == KeyEvent.ACTION_UP) {
            onNativeKeyUp(keyCode);
            return true;
        }

        return false;
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static Surface getNativeSurface() {
        if (SDLActivity.mSurface == null) {
            return null;
        }
        return SDLActivity.mSurface.getNativeSurface();
    }

    // Input

    /**
     * This method is called by SDL using JNI.
     */
    public static void initTouch() {
        int[] ids = InputDevice.getDeviceIds();

        for (int id : ids) {
            InputDevice device = InputDevice.getDevice(id);
            /* Allow SOURCE_TOUCHSCREEN and also Virtual InputDevices because they can send TOUCHSCREEN events */
            if (device != null && ((device.getSources() & InputDevice.SOURCE_TOUCHSCREEN) == InputDevice.SOURCE_TOUCHSCREEN
                    || device.isVirtual())) {

                int touchDevId = device.getId();
                /*
                 * Prevent id to be -1, since it's used in SDL internal for synthetic events
                 * Appears when using Android emulator, eg:
                 *  adb shell input mouse tap 100 100
                 *  adb shell input touchscreen tap 100 100
                 */
                if (touchDevId < 0) {
                    touchDevId -= 1;
                }
                nativeAddTouch(touchDevId, device.getName());
            }
        }
    }

    // Messagebox

    /** Result of current messagebox. Also used for blocking the calling thread. */
    protected final int[] messageboxSelection = new int[1];

    /**
     * This method is called by SDL using JNI.
     * Shows the messagebox from UI thread and block calling thread.
     * buttonFlags, buttonIds and buttonTexts must have same length.
     * @param buttonFlags array containing flags for every button.
     * @param buttonIds array containing id for every button.
     * @param buttonTexts array containing text for every button.
     * @param colors null for default or array of length 5 containing colors.
     * @return button id or -1.
     */
    public int messageboxShowMessageBox(
            final int flags,
            final String title,
            final String message,
            final int[] buttonFlags,
            final int[] buttonIds,
            final String[] buttonTexts,
            final int[] colors) {

        messageboxSelection[0] = -1;

        // sanity checks

        if ((buttonFlags.length != buttonIds.length) && (buttonIds.length != buttonTexts.length)) {
            return -1; // implementation broken
        }

        // collect arguments for Dialog

        final Bundle args = new Bundle();
        args.putInt("flags", flags);
        args.putString("title", title);
        args.putString("message", message);
        args.putIntArray("buttonFlags", buttonFlags);
        args.putIntArray("buttonIds", buttonIds);
        args.putStringArray("buttonTexts", buttonTexts);
        args.putIntArray("colors", colors);

        // trigger Dialog creation on UI thread

        runOnUiThread(new Runnable() {
            @Override
            public void run() {
                messageboxCreateAndShow(args);
            }
        });

        // block the calling thread

        synchronized (messageboxSelection) {
            try {
                messageboxSelection.wait();
            } catch (InterruptedException ex) {
                ex.printStackTrace();
                return -1;
            }
        }

        // return selected value

        return messageboxSelection[0];
    }

    protected void messageboxCreateAndShow(Bundle args) {

        // TODO set values from "flags" to messagebox dialog

        // get colors

        int[] colors = args.getIntArray("colors");
        int backgroundColor;
        int textColor;
        int buttonBorderColor;
        int buttonBackgroundColor;
        int buttonSelectedColor;
        if (colors != null) {
            int i = -1;
            backgroundColor = colors[++i];
            textColor = colors[++i];
            buttonBorderColor = colors[++i];
            buttonBackgroundColor = colors[++i];
            buttonSelectedColor = colors[++i];
        } else {
            backgroundColor = Color.TRANSPARENT;
            textColor = Color.TRANSPARENT;
            buttonBorderColor = Color.TRANSPARENT;
            buttonBackgroundColor = Color.TRANSPARENT;
            buttonSelectedColor = Color.TRANSPARENT;
        }

        // create dialog with title and a listener to wake up calling thread

        final AlertDialog dialog = new AlertDialog.Builder(this).create();
        dialog.setTitle(args.getString("title"));
        dialog.setCancelable(false);
        dialog.setOnDismissListener(new DialogInterface.OnDismissListener() {
            @Override
            public void onDismiss(DialogInterface unused) {
                synchronized (messageboxSelection) {
                    messageboxSelection.notify();
                }
            }
        });

        // create text

        TextView message = new TextView(this);
        message.setGravity(Gravity.CENTER);
        message.setText(args.getString("message"));
        if (textColor != Color.TRANSPARENT) {
            message.setTextColor(textColor);
        }

        // create buttons

        int[] buttonFlags = args.getIntArray("buttonFlags");
        int[] buttonIds = args.getIntArray("buttonIds");
        String[] buttonTexts = args.getStringArray("buttonTexts");

        final SparseArray<Button> mapping = new SparseArray<Button>();

        LinearLayout buttons = new LinearLayout(this);
        buttons.setOrientation(LinearLayout.HORIZONTAL);
        buttons.setGravity(Gravity.CENTER);
        for (int i = 0; i < buttonTexts.length; ++i) {
            Button button = new Button(this);
            final int id = buttonIds[i];
            button.setOnClickListener(new View.OnClickListener() {
                @Override
                public void onClick(View v) {
                    messageboxSelection[0] = id;
                    dialog.dismiss();
                }
            });
            if (buttonFlags[i] != 0) {
                // see SDL_messagebox.h
                if ((buttonFlags[i] & 0x00000001) != 0) {
                    mapping.put(KeyEvent.KEYCODE_ENTER, button);
                }
                if ((buttonFlags[i] & 0x00000002) != 0) {
                    mapping.put(KeyEvent.KEYCODE_ESCAPE, button); /* API 11 */
                }
            }
            button.setText(buttonTexts[i]);
            if (textColor != Color.TRANSPARENT) {
                button.setTextColor(textColor);
            }
            if (buttonBorderColor != Color.TRANSPARENT) {
                // TODO set color for border of messagebox button
            }
            if (buttonBackgroundColor != Color.TRANSPARENT) {
                Drawable drawable = button.getBackground();
                if (drawable == null) {
                    // setting the color this way removes the style
                    button.setBackgroundColor(buttonBackgroundColor);
                } else {
                    // setting the color this way keeps the style (gradient, padding, etc.)
                    drawable.setColorFilter(buttonBackgroundColor, PorterDuff.Mode.MULTIPLY);
                }
            }
            if (buttonSelectedColor != Color.TRANSPARENT) {
                // TODO set color for selected messagebox button
            }
            buttons.addView(button);
        }

        // create content

        LinearLayout content = new LinearLayout(this);
        content.setOrientation(LinearLayout.VERTICAL);
        content.addView(message);
        content.addView(buttons);
        if (backgroundColor != Color.TRANSPARENT) {
            content.setBackgroundColor(backgroundColor);
        }

        // add content to dialog and return

        dialog.setView(content);
        dialog.setOnKeyListener(new Dialog.OnKeyListener() {
            @Override
            public boolean onKey(DialogInterface d, int keyCode, KeyEvent event) {
                Button button = mapping.get(keyCode);
                if (button != null) {
                    if (event.getAction() == KeyEvent.ACTION_UP) {
                        button.performClick();
                    }
                    return true; // also for ignored actions
                }
                return false;
            }
        });

        dialog.show();
    }

    private final Runnable rehideSystemUi = new Runnable() {
        @Override
        public void run() {
            if (Build.VERSION.SDK_INT >= 19 /* Android 4.4 (KITKAT) */) {
                int flags = View.SYSTEM_UI_FLAG_FULLSCREEN |
                        View.SYSTEM_UI_FLAG_HIDE_NAVIGATION |
                        View.SYSTEM_UI_FLAG_IMMERSIVE_STICKY |
                        View.SYSTEM_UI_FLAG_LAYOUT_FULLSCREEN |
                        View.SYSTEM_UI_FLAG_LAYOUT_HIDE_NAVIGATION |
                        View.SYSTEM_UI_FLAG_LAYOUT_STABLE | View.INVISIBLE;

                SDLActivity.this.getWindow().getDecorView().setSystemUiVisibility(flags);
            }
        }
    };

    public void onSystemUiVisibilityChange(int visibility) {
        if (SDLActivity.mFullscreenModeActive && ((visibility & View.SYSTEM_UI_FLAG_FULLSCREEN) == 0 || (visibility & View.SYSTEM_UI_FLAG_HIDE_NAVIGATION) == 0)) {

            Handler handler = getWindow().getDecorView().getHandler();
            if (handler != null) {
                handler.removeCallbacks(rehideSystemUi); // Prevent a hide loop.
                handler.postDelayed(rehideSystemUi, 2000);
            }

        }
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static boolean clipboardHasText() {
        return mClipboardHandler.clipboardHasText();
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static String clipboardGetText() {
        return mClipboardHandler.clipboardGetText();
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static void clipboardSetText(String string) {
        mClipboardHandler.clipboardSetText(string);
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static int createCustomCursor(int[] colors, int width, int height, int hotSpotX, int hotSpotY) {
        Bitmap bitmap = Bitmap.createBitmap(colors, width, height, Bitmap.Config.ARGB_8888);
        ++mLastCursorID;

        if (Build.VERSION.SDK_INT >= 24 /* Android 7.0 (N) */) {
            try {
                mCursors.put(mLastCursorID, PointerIcon.create(bitmap, hotSpotX, hotSpotY));
            } catch (Exception e) {
                return 0;
            }
        } else {
            return 0;
        }
        return mLastCursorID;
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static void destroyCustomCursor(int cursorID) {
        if (Build.VERSION.SDK_INT >= 24 /* Android 7.0 (N) */) {
            try {
                mCursors.remove(cursorID);
            } catch (Exception e) {
            }
        }
        return;
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static boolean setCustomCursor(int cursorID) {

        if (Build.VERSION.SDK_INT >= 24 /* Android 7.0 (N) */) {
            try {
                mSurface.setPointerIcon(mCursors.get(cursorID));
            } catch (Exception e) {
                return false;
            }
        } else {
            return false;
        }
        return true;
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static boolean setSystemCursor(int cursorID) {
        int cursor_type = 0; //PointerIcon.TYPE_NULL;
        switch (cursorID) {
        case SDL_SYSTEM_CURSOR_ARROW:
            cursor_type = 1000; //PointerIcon.TYPE_ARROW;
            break;
        case SDL_SYSTEM_CURSOR_IBEAM:
            cursor_type = 1008; //PointerIcon.TYPE_TEXT;
            break;
        case SDL_SYSTEM_CURSOR_WAIT:
            cursor_type = 1004; //PointerIcon.TYPE_WAIT;
            break;
        case SDL_SYSTEM_CURSOR_CROSSHAIR:
            cursor_type = 1007; //PointerIcon.TYPE_CROSSHAIR;
            break;
        case SDL_SYSTEM_CURSOR_WAITARROW:
            cursor_type = 1004; //PointerIcon.TYPE_WAIT;
            break;
        case SDL_SYSTEM_CURSOR_SIZENWSE:
            cursor_type = 1017; //PointerIcon.TYPE_TOP_LEFT_DIAGONAL_DOUBLE_ARROW;
            break;
        case SDL_SYSTEM_CURSOR_SIZENESW:
            cursor_type = 1016; //PointerIcon.TYPE_TOP_RIGHT_DIAGONAL_DOUBLE_ARROW;
            break;
        case SDL_SYSTEM_CURSOR_SIZEWE:
            cursor_type = 1014; //PointerIcon.TYPE_HORIZONTAL_DOUBLE_ARROW;
            break;
        case SDL_SYSTEM_CURSOR_SIZENS:
            cursor_type = 1015; //PointerIcon.TYPE_VERTICAL_DOUBLE_ARROW;
            break;
        case SDL_SYSTEM_CURSOR_SIZEALL:
            cursor_type = 1020; //PointerIcon.TYPE_GRAB;
            break;
        case SDL_SYSTEM_CURSOR_NO:
            cursor_type = 1012; //PointerIcon.TYPE_NO_DROP;
            break;
        case SDL_SYSTEM_CURSOR_HAND:
            cursor_type = 1002; //PointerIcon.TYPE_HAND;
            break;
        }
        if (Build.VERSION.SDK_INT >= 24 /* Android 7.0 (N) */) {
            try {
                mSurface.setPointerIcon(PointerIcon.getSystemIcon(SDL.getContext(), cursor_type));
            } catch (Exception e) {
                return false;
            }
        }
        return true;
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static void requestPermission(String permission, int requestCode) {
        if (Build.VERSION.SDK_INT < 23 /* Android 6.0 (M) */) {
            nativePermissionResult(requestCode, true);
            return;
        }

        Activity activity = (Activity)getContext();
        if (activity.checkSelfPermission(permission) != PackageManager.PERMISSION_GRANTED) {
            activity.requestPermissions(new String[]{permission}, requestCode);
        } else {
            nativePermissionResult(requestCode, true);
        }
    }

    @Override
    public void onRequestPermissionsResult(int requestCode, String[] permissions, int[] grantResults) {
        boolean result = (grantResults.length > 0 && grantResults[0] == PackageManager.PERMISSION_GRANTED);
        nativePermissionResult(requestCode, result);
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static int openURL(String url)
    {
        try {
            Intent i = new Intent(Intent.ACTION_VIEW);
            i.setData(Uri.parse(url));

            int flags = Intent.FLAG_ACTIVITY_NO_HISTORY | Intent.FLAG_ACTIVITY_MULTIPLE_TASK;
            if (Build.VERSION.SDK_INT >= 21 /* Android 5.0 (LOLLIPOP) */) {
                flags |= Intent.FLAG_ACTIVITY_NEW_DOCUMENT;
            } else {
                flags |= Intent.FLAG_ACTIVITY_CLEAR_WHEN_TASK_RESET;
            }
            i.addFlags(flags);

            mSingleton.startActivity(i);
        } catch (Exception ex) {
            return -1;
        }
        return 0;
    }

    /**
     * This method is called by SDL using JNI.
     */
    public static int showToast(String message, int duration, int gravity, int xOffset, int yOffset)
    {
        if(null == mSingleton) {
            return - 1;
        }

        try
        {
            class OneShotTask implements Runnable {
                String mMessage;
                int mDuration;
                int mGravity;
                int mXOffset;
                int mYOffset;

                OneShotTask(String message, int duration, int gravity, int xOffset, int yOffset) {
                    mMessage  = message;
                    mDuration = duration;
                    mGravity  = gravity;
                    mXOffset  = xOffset;
                    mYOffset  = yOffset;
                }

                public void run() {
                    try
                    {
                        Toast toast = Toast.makeText(mSingleton, mMessage, mDuration);
                        if (mGravity >= 0) {
                            toast.setGravity(mGravity, mXOffset, mYOffset);
                        }
                        toast.show();
                    } catch(Exception ex) {
                        Log.e(TAG, ex.getMessage());
                    }
                }
            }
            mSingleton.runOnUiThread(new OneShotTask(message, duration, gravity, xOffset, yOffset));
        } catch(Exception ex) {
            return -1;
        }
        return 0;
    }
}

/**
    Simple runnable to start the SDL application
*/
class SDLMain implements Runnable {
    @Override
    public void run() {
        // Runs SDL_main()
        String library = SDLActivity.mSingleton.getMainSharedObject();
        String function = SDLActivity.mSingleton.getMainFunction();
        String[] arguments = SDLActivity.mSingleton.getArguments();

        try {
            android.os.Process.setThreadPriority(android.os.Process.THREAD_PRIORITY_DISPLAY);
        } catch (Exception e) {
            Log.v("SDL", "modify thread properties failed " + e.toString());
        }

        Log.v("SDL", "Running main function " + function + " from library " + library);

        SDLActivity.nativeRunMain(library, function, arguments);

        Log.v("SDL", "Finished main function");

        if (SDLActivity.mSingleton != null && !SDLActivity.mSingleton.isFinishing()) {
            // Let's finish the Activity
            SDLActivity.mSDLThread = null;
            SDLActivity.mSingleton.finish();
        }  // else: Activity is already being destroyed

    }
}

/* This is a fake invisible editor view that receives the input and defines the
 * pan&scan region
 */
class DummyEdit extends View implements View.OnKeyListener {
    InputConnection ic;

    public DummyEdit(Context context) {
        super(context);
        setFocusableInTouchMode(true);
        setFocusable(true);
        setOnKeyListener(this);
    }

    @Override
    public boolean onCheckIsTextEditor() {
        return true;
    }

    @Override
    public boolean onKey(View v, int keyCode, KeyEvent event) {
        return SDLActivity.handleKeyEvent(v, keyCode, event, ic);
    }

    //
    @Override
    public boolean onKeyPreIme (int keyCode, KeyEvent event) {
        // As seen on StackOverflow: http://stackoverflow.com/questions/7634346/keyboard-hide-event
        // FIXME: Discussion at http://bugzilla.libsdl.org/show_bug.cgi?id=1639
        // FIXME: This is not a 100% effective solution to the problem of detecting if the keyboard is showing or not
        // FIXME: A more effective solution would be to assume our Layout to be RelativeLayout or LinearLayout
        // FIXME: And determine the keyboard presence doing this: http://stackoverflow.com/questions/2150078/how-to-check-visibility-of-software-keyboard-in-android
        // FIXME: An even more effective way would be if Android provided this out of the box, but where would the fun be in that :)
        if (event.getAction()==KeyEvent.ACTION_UP && keyCode == KeyEvent.KEYCODE_BACK) {
            if (SDLActivity.mTextEdit != null && SDLActivity.mTextEdit.getVisibility() == View.VISIBLE) {
                SDLActivity.onNativeKeyboardFocusLost();
            }
        }
        return super.onKeyPreIme(keyCode, event);
    }

    @Override
    public InputConnection onCreateInputConnection(EditorInfo outAttrs) {
        ic = new SDLInputConnection(this, true);

        outAttrs.inputType = InputType.TYPE_CLASS_TEXT |
                             InputType.TYPE_TEXT_FLAG_MULTI_LINE;
        outAttrs.imeOptions = EditorInfo.IME_FLAG_NO_EXTRACT_UI |
                              EditorInfo.IME_FLAG_NO_FULLSCREEN /* API 11 */;

        return ic;
    }
}

class SDLInputConnection extends BaseInputConnection {

    protected EditText mEditText;
    protected String mCommittedText = "";

    public SDLInputConnection(View targetView, boolean fullEditor) {
        super(targetView, fullEditor);
        mEditText = new EditText(SDL.getContext());
    }

    @Override
    public Editable getEditable() {
        return mEditText.getEditableText();
    }

    @Override
    public boolean sendKeyEvent(KeyEvent event) {
        /*
         * This used to handle the keycodes from soft keyboard (and IME-translated input from hardkeyboard)
         * However, as of Ice Cream Sandwich and later, almost all soft keyboard doesn't generate key presses
         * and so we need to generate them ourselves in commitText.  To avoid duplicates on the handful of keys
         * that still do, we empty this out.
         */

        /*
         * Return DOES still generate a key event, however.  So rather than using it as the 'click a button' key
         * as we do with physical keyboards, let's just use it to hide the keyboard.
         */

        if (event.getKeyCode() == KeyEvent.KEYCODE_ENTER) {
            if (SDLActivity.onNativeSoftReturnKey()) {
                return true;
            }
        }

        return super.sendKeyEvent(event);
    }

    @Override
    public boolean commitText(CharSequence text, int newCursorPosition) {
        if (!super.commitText(text, newCursorPosition)) {
            return false;
        }
        updateText();
        return true;
    }

    @Override
    public boolean setComposingText(CharSequence text, int newCursorPosition) {
        if (!super.setComposingText(text, newCursorPosition)) {
            return false;
        }
        updateText();
        return true;
    }

    @Override
    public boolean deleteSurroundingText(int beforeLength, int afterLength) {
        if (Build.VERSION.SDK_INT <= 29 /* Android 10.0 (Q) */) {
            // Workaround to capture backspace key. Ref: http://stackoverflow.com/questions>/14560344/android-backspace-in-webview-baseinputconnection
            // and https://bugzilla.libsdl.org/show_bug.cgi?id=2265
            if (beforeLength > 0 && afterLength == 0) {
                // backspace(s)
                while (beforeLength-- > 0) {
                    nativeGenerateScancodeForUnichar('\b');
                }
                return true;
           }
        }

        if (!super.deleteSurroundingText(beforeLength, afterLength)) {
            return false;
        }
        updateText();
        return true;
    }

    protected void updateText() {
        final Editable content = getEditable();
        if (content == null) {
            return;
        }

        String text = content.toString();
        int compareLength = Math.min(text.length(), mCommittedText.length());
        int matchLength, offset;

        /* Backspace over characters that are no longer in the string */
        for (matchLength = 0; matchLength < compareLength; ) {
            int codePoint = mCommittedText.codePointAt(matchLength);
            if (codePoint != text.codePointAt(matchLength)) {
                break;
            }
            matchLength += Character.charCount(codePoint);
        }
        /* FIXME: This doesn't handle graphemes, like '🌬️' */
        for (offset = matchLength; offset < mCommittedText.length(); ) {
            int codePoint = mCommittedText.codePointAt(offset);
            nativeGenerateScancodeForUnichar('\b');
            offset += Character.charCount(codePoint);
        }

        if (matchLength < text.length()) {
            String pendingText = text.subSequence(matchLength, text.length()).toString();
            for (offset = 0; offset < pendingText.length(); ) {
                int codePoint = pendingText.codePointAt(offset);
                if (codePoint == '\n') {
                    if (SDLActivity.onNativeSoftReturnKey()) {
                        return;
                    }
                }
                /* Higher code points don't generate simulated scancodes */
                if (codePoint < 128) {
                    nativeGenerateScancodeForUnichar((char)codePoint);
                }
                offset += Character.charCount(codePoint);
            }
            SDLInputConnection.nativeCommitText(pendingText, 0);
        }
        mCommittedText = text;
    }

    public static native void nativeCommitText(String text, int newCursorPosition);

    public static native void nativeGenerateScancodeForUnichar(char c);
}

class SDLClipboardHandler implements
    ClipboardManager.OnPrimaryClipChangedListener {

    protected ClipboardManager mClipMgr;

    SDLClipboardHandler() {
       mClipMgr = (ClipboardManager) SDL.getContext().getSystemService(Context.CLIPBOARD_SERVICE);
       mClipMgr.addPrimaryClipChangedListener(this);
    }

    public boolean clipboardHasText() {
       return mClipMgr.hasPrimaryClip();
    }

    public String clipboardGetText() {
        ClipData clip = mClipMgr.getPrimaryClip();
        if (clip != null) {
            ClipData.Item item = clip.getItemAt(0);
            if (item != null) {
                CharSequence text = item.getText();
                if (text != null) {
                    return text.toString();
                }
            }
        }
        return null;
    }

    public void clipboardSetText(String string) {
       mClipMgr.removePrimaryClipChangedListener(this);
       ClipData clip = ClipData.newPlainText(null, string);
       mClipMgr.setPrimaryClip(clip);
       mClipMgr.addPrimaryClipChangedListener(this);
    }

    @Override
    public void onPrimaryClipChanged() {
        SDLActivity.onNativeClipboardChanged();
    }
}

