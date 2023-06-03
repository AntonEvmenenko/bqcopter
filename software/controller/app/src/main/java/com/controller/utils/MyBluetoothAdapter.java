package com.controller.utils;

import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothSocket;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.util.Log;

import java.io.IOException;
import java.util.UUID;

public class MyBluetoothAdapter {
    private static final String MAC_ADDRESS = "98:D3:31:40:20:83";
    private static final UUID MY_UUID = UUID.fromString("00001101-0000-1000-8000-00805F9B34FB");
    private static final int REQUEST_ENABLE_BLUETOOTH = 1;

    private BluetoothAdapter bluetoothAdapter = null;
    private BluetoothSocket bluetoothSocket = null;

    private BluetoothThread bluetoothThread = null;

    private boolean connected = false;

    public MyBluetoothAdapter() {

    }

    public boolean connect() {
        if (connected) {
            return true;
        }

        if (bluetoothAdapter == null) {
            bluetoothAdapter = BluetoothAdapter.getDefaultAdapter();
        }

        if (bluetoothAdapter != null) {
            if (!bluetoothAdapter.isEnabled()) {
                return false;
            }
        }

        BluetoothDevice bluetoothDevice = bluetoothAdapter.getRemoteDevice(MAC_ADDRESS);

        try {
            bluetoothSocket = bluetoothDevice.createRfcommSocketToServiceRecord(MY_UUID);
        } catch (IOException e) {
            bluetoothSocket = null;
               return false;
        }

        //bluetoothAdapter.cancelDiscovery();

        try {
            bluetoothSocket.connect();
        } catch (IOException e) {
            try {
                bluetoothSocket.close();
            } catch (IOException e1) {
                return false;
            }
            return false;
        }

        bluetoothThread = new BluetoothThread(getBluetoothSocket());
        bluetoothThread.start();

        connected = true;

        return true;
    }

    public void disconnect() {
        if (!connected) {
            return;
        }

        try {
            bluetoothThread.interrupt();
        } catch (Exception e) {}

        try {
            if (bluetoothSocket != null) {
                bluetoothSocket.close();
                connected = false;
            }
        } catch ( IOException e ) { }
    }

    public BluetoothSocket getBluetoothSocket() {
        return bluetoothSocket;
    }

    public void setHandler(Handler handler) {
        bluetoothThread.setHandler(handler);
    }

    public boolean write(String message) {
        if (bluetoothThread != null) {
            Log.d("AZAZA", message);
            bluetoothThread.write(message);
            return true;
        }
        return false;
    }
}
