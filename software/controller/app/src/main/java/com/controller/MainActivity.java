package com.controller;

import android.content.Context;
import android.content.Intent;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.os.Handler;
import android.view.View;
import android.view.ViewGroup;
import android.widget.Button;
import android.widget.LinearLayout;

import java.util.LinkedList;

import com.controller.utils.BluetoothThread;
import com.controller.utils.DataFormat;
import com.controller.utils.MyGLSurfaceView;
import com.controller.utils.RenderingThread;

public class MainActivity extends AppCompatActivity {
    private static Handler handler = null;
    private RenderingThread renderingThread = null;

    private MyGLSurfaceView mGLView;

    private Button connectButton;
    private Button disconnectButton;
    private Button resetButton;
    private Button tuneButton;

    private float pseudoTime = 0;

    private boolean working = false;

    private static Context appContext;

    public void connect() {
        if (((Globals)this.getApplicationContext()).getBluetoothAdapter().connect()) {
            ((Globals)this.getApplicationContext()).getBluetoothAdapter().setHandler(handler);
            renderingThread = new RenderingThread(mGLView);
            renderingThread.start();
            connectButton.setEnabled(false);
            disconnectButton.setEnabled(true);
            tuneButton.setEnabled(true);
        }
    }

    public void disconnect() {
        ((Globals)this.getApplicationContext()).getBluetoothAdapter().disconnect();
        try {
            renderingThread.interrupt();
        } catch (Exception e) {}
        connectButton.setEnabled(true);
        disconnectButton.setEnabled(false);
        tuneButton.setEnabled(false);
    }

    @Override
    public void onCreate( Bundle savedInstanceState ) {
        super.onCreate(  savedInstanceState );
        mGLView = new MyGLSurfaceView( this );
        setContentView(R.layout.activity_main);

        appContext = this;

        //setContentView(mGLView);
        LinearLayout linearLayout = (LinearLayout)findViewById(R.id.openglLayout);
        linearLayout.addView(mGLView, 0, new ViewGroup.LayoutParams(ViewGroup.LayoutParams.WRAP_CONTENT,
                ViewGroup.LayoutParams.WRAP_CONTENT));;

        connectButton = (Button)findViewById(R.id.connectButton);
        disconnectButton = (Button)findViewById(R.id.disconnectButton);
        resetButton = (Button)findViewById(R.id.resetButton);
        tuneButton = (Button)findViewById(R.id.tuneButton);

        connectButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                working = true;
                connect();
            }
        });

        disconnectButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                working = false;
                disconnect();
            }
        });

        resetButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                mGLView.mRenderer.roll.clear();
                mGLView.mRenderer.pitch.clear();
                mGLView.mRenderer.yaw.clear();
                mGLView.requestRender();
            }
        });

        tuneButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                Intent intent = new Intent(MainActivity.this, TuneActivity.class);
                startActivity(intent);
            }
        });

        handler = new Handler(new Handler.Callback() {
            @Override
            public boolean handleMessage(android.os.Message message) {
                if (message.what == BluetoothThread.MESSAGE_RECEIVED) {
                    byte[] buffer = (byte[])message.obj;
                    ((Globals)appContext.getApplicationContext()).getSerialStreamParser().appendData(new String(buffer, 0, message.arg1));
                    LinkedList<DataFormat> data = ((Globals)appContext.getApplicationContext()).getSerialStreamParser().parseStream();

                    for (int i = 0; i < data.size(); i++) {
                        if (data.get(i).getDataSize() == 1) {
                            if (data.get(i).getId() == DataFormat.Q2C_CURRENT_ROLL) {
                                mGLView.mRenderer.roll.addPoint(pseudoTime, data.get(i).getData()[0] / 1.0f);
                            } else if (data.get(i).getId() == DataFormat.Q2C_CURRENT_PITCH) {
                                mGLView.mRenderer.pitch.addPoint(pseudoTime, data.get(i).getData()[0] / 1.0f);
                            } if (data.get(i).getId() == DataFormat.Q2C_CURRENT_YAW) {
                                mGLView.mRenderer.yaw.addPoint(pseudoTime, data.get(i).getData()[0] / 1.0f);
                            }
                            pseudoTime += 0.001;
                        }
                    }

                    return true;
                }
                return false;
            }
        });
    }

    @Override
    public void onResume( ) {
        super.onResume();

        if (working) {
            connect();
        }
    }

    @Override
    public void onPause( ){
        super.onPause();

//        if (working) {
//            disconnect();
//        }
    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_main, menu);
        return true;
    }

    @Override
    public boolean onOptionsItemSelected(MenuItem item) {
        // Handle action bar item clicks here. The action bar will
        // automatically handle clicks on the Home/Up button, so long
        // as you specify a parent activity in AndroidManifest.xml.
        int id = item.getItemId();

        //noinspection SimplifiableIfStatement
        if (id == R.id.action_settings) {
            return true;
        }

        return super.onOptionsItemSelected(item);
    }
}
