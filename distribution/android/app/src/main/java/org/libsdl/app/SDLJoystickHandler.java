package org.libsdl.app;

import android.view.MotionEvent;

/* A null joystick handler for API level < 12 devices (the accelerometer is handled separately) */
class SDLJoystickHandler {

    /**
     * Handles given MotionEvent.
     * @param event the event to be handled.
     * @return if given event was processed.
     */
    public boolean handleMotionEvent(MotionEvent event) {
        return false;
    }

    /**
     * Handles adding and removing of input devices.
     */
    public void pollInputDevices() {
    }
}
