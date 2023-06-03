package com.controller;

import android.app.Application;

import com.controller.utils.MyBluetoothAdapter;
import com.controller.utils.SerialStreamParser;

public class Globals extends Application {
    private MyBluetoothAdapter myBluetoothAdapter = new MyBluetoothAdapter();
    private SerialStreamParser serialStreamParser = new SerialStreamParser();

    @Override
    public void onCreate() {
        super.onCreate();
    }

    public MyBluetoothAdapter getBluetoothAdapter() {
        return myBluetoothAdapter;
    }
    public SerialStreamParser getSerialStreamParser() {
        return serialStreamParser;
    }
}
