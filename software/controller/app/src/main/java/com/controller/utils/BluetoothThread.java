package com.controller.utils;

import android.bluetooth.BluetoothSocket;

import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import android.os.Handler;
import android.os.SystemClock;

public class BluetoothThread extends Thread {
    private final BluetoothSocket socket;
    private final InputStream inputStream;
    private final OutputStream outputStream;
    private Handler handler = null;

    public static final int MESSAGE_RECEIVED = 1;

    public BluetoothThread( BluetoothSocket socket ) {
        this.socket = socket;

        InputStream tempInputStream = null;
        OutputStream tempOutputStream = null;

        try {
            tempInputStream = socket.getInputStream( );
            tempOutputStream = socket.getOutputStream( );
        } catch ( IOException e ) { }

        inputStream = tempInputStream;
        outputStream = tempOutputStream;
    }

    public void run( ) {
        byte[ ] buffer = new byte[ 256 ];
        int bytes;

        while ( true ) {
            if (!Thread.interrupted()) {
                try {
                    if (inputStream.available() > 0) {
                        bytes = inputStream.read(buffer);
                        if (bytes > 0 && handler != null) {
                            handler.obtainMessage(MESSAGE_RECEIVED, bytes, -1, buffer).sendToTarget();
                        }
                    } else {
                        try {
                            Thread.sleep(10);
                        } catch (InterruptedException e) {
                            return;
                        }

                    }
                } catch (IOException e) {
                    break;
                }
            } else {
                return;
            }
        }
    }

    /* Call this from the main activity to send data to the remote device */
    public void write( String message ) {
        byte[ ] msgBuffer = message.getBytes( );
        try {
            outputStream.write( msgBuffer );
            outputStream.flush();
        } catch ( IOException e ) { }
    }

    /* Call this from the main activity to shutdown the connection */
    public void cancel() {
        try {
            socket.close();
        } catch (IOException e) { }
    }

    public void setHandler(Handler handler) {
        this.handler = handler;
    }
}
