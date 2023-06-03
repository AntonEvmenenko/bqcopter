package com.controller.utils;

import android.os.SystemClock;

public class RenderingThread extends Thread {
    private MyGLSurfaceView mGLView;

    private boolean working = true;

    public RenderingThread( MyGLSurfaceView mGLView ) {
        this.mGLView = mGLView;
    }

    public void run() {
        while (true) {
            if (!Thread.interrupted()) {
                mGLView.requestRender();
                try {
                    Thread.sleep(10);
                } catch (InterruptedException e) {
                    return;
                }
            } else {
                return;
            }
        }
    }
}
