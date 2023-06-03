package com.controller;

import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuItem;
import android.view.View;
import android.widget.Button;
import android.widget.EditText;

import com.controller.utils.BluetoothThread;
import com.controller.utils.DataFormat;

import java.util.LinkedList;

public class TuneActivity extends AppCompatActivity {
    private static Context appContext;

    private static Handler handler = null;

    private Button writeButton;
    private Button readButton;

    private EditText rollP;
    private EditText rollD;
    private EditText pitchP;
    private EditText pitchD;
    private EditText yawP;
    private EditText yawD;

    private Float currentRollP = 0.0f;
    private Float currentRollD = 0.0f;
    private Float currentPitchP = 0.0f;
    private Float currentPitchD = 0.0f;
    private Float currentYawP = 0.0f;
    private Float currentYawD = 0.0f;

    private void sendValueIfChanged(Float currentValue, String newValueString, Short id) {
        boolean newValueExist = !newValueString.equals("");
        Float newValue = newValueExist ? Float.parseFloat(newValueString) : 0.0f;
        if (newValueExist && currentValue.compareTo(newValue) != 0) {
            String message = DataFormat.format(id, newValue);
            ((Globals)appContext.getApplicationContext()).getBluetoothAdapter().write(message);
        }
    }

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_tune);

        appContext = this;

        writeButton = (Button)findViewById(R.id.writeButton);
        readButton = (Button)findViewById(R.id.readButton);

        rollP = (EditText)findViewById(R.id.rollP);
        rollD = (EditText)findViewById(R.id.rollD);
        pitchP = (EditText)findViewById(R.id.pitchP);
        pitchD = (EditText)findViewById(R.id.pitchD);
        yawP = (EditText)findViewById(R.id.yawP);
        yawD = (EditText)findViewById(R.id.yawD);

        writeButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {

                sendValueIfChanged(currentRollP, rollP.getText().toString(), DataFormat.C2Q_NEW_ROLL_P);
                sendValueIfChanged(currentRollD, rollD.getText().toString(), DataFormat.C2Q_NEW_ROLL_D);
                sendValueIfChanged(currentPitchP, pitchP.getText().toString(), DataFormat.C2Q_NEW_PITCH_P);
                sendValueIfChanged(currentPitchD, pitchD.getText().toString(), DataFormat.C2Q_NEW_PITCH_D);
                sendValueIfChanged(currentYawP, yawP.getText().toString(), DataFormat.C2Q_NEW_YAW_P);
                sendValueIfChanged(currentYawD, yawD.getText().toString(), DataFormat.C2Q_NEW_YAW_D);
            }
        });

        readButton.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                String message = DataFormat.format(DataFormat.C2Q_WANT_ALL);
                ((Globals)appContext.getApplicationContext()).getBluetoothAdapter().write(message);
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
                            short id = data.get(i).getId();
                            float value = data.get(i).getData()[0];
                            if (id == DataFormat.Q2C_CURRENT_ROLL_P) {
                                currentRollP = value;
                                rollP.setText(String.valueOf(value));
                            } else if (id == DataFormat.Q2C_CURRENT_ROLL_D) {
                                currentRollD = value;
                                rollD.setText(String.valueOf(value));
                            } else if (id == DataFormat.Q2C_CURRENT_PITCH_P) {
                                currentPitchP = value;
                                pitchP.setText(String.valueOf(value));
                            } else if (id == DataFormat.Q2C_CURRENT_PITCH_D) {
                                currentPitchD = value;
                                pitchD.setText(String.valueOf(value));
                            } else if (id == DataFormat.Q2C_CURRENT_YAW_P) {
                                currentYawP = value;
                                yawP.setText(String.valueOf(value));
                            } else if (id == DataFormat.Q2C_CURRENT_YAW_D) {
                                currentYawD = value;
                                yawD.setText(String.valueOf(value));
                            }
                        }
                    }

                    return true;
                }
                return false;
            }
        });

        if (((Globals)this.getApplicationContext()).getBluetoothAdapter().connect()) {
            ((Globals)this.getApplicationContext()).getBluetoothAdapter().setHandler(handler);
        }

//        ((Globals)this.getApplicationContext()).getBluetoothAdapter().setHandler(handler);

    }

    @Override
    public boolean onCreateOptionsMenu(Menu menu) {
        // Inflate the menu; this adds items to the action bar if it is present.
        getMenuInflater().inflate(R.menu.menu_tune, menu);
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
