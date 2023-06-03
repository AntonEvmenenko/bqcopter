package com.controller.utils;

/*

struct Data {
    int16_t id;
    float data[n];
    int16_t checksum;
};

s.length() = 8 + n * 8

*/

import android.util.Log;

public class DataFormat {
    public static final short C2Q_WANT_ALL = 1;

    public static final short C2Q_NEW_ROLL_P = 2;
    public static final short C2Q_NEW_ROLL_D = 3;
    public static final short C2Q_NEW_PITCH_P = 4;
    public static final short C2Q_NEW_PITCH_D = 5;
    public static final short C2Q_NEW_YAW_P = 6;
    public static final short C2Q_NEW_YAW_D = 7;

    public static final short Q2C_CURRENT_ROLL_P = 8;
    public static final short Q2C_CURRENT_ROLL_D = 9;
    public static final short Q2C_CURRENT_PITCH_P = 10;
    public static final short Q2C_CURRENT_PITCH_D = 11;
    public static final short Q2C_CURRENT_YAW_P = 12;
    public static final short Q2C_CURRENT_YAW_D = 13;

    public static final short Q2C_CURRENT_ROLL = 14;
    public static final short Q2C_CURRENT_PITCH = 15;
    public static final short Q2C_CURRENT_YAW = 16;

    private static final int MAX_SIZE = 100;

    private short id;
    private float data[] = new float[MAX_SIZE];
    private int n = 0;

    private boolean error = false;

    public DataFormat(String s) {
        error = false;

        if (s.length() < 4 || (s.length() - 8) % 8 != 0) {
            error = true;
            return;
        }

        n = (s.length() - 8) / 8;

        try {
            short tempChecksum = 0;
            id = (short)Integer.parseInt(s.substring(0, 4), 16);
            for (int i = 0; i < n; ++i) {
                tempChecksum = (short)(tempChecksum + (short)Integer.parseInt(s.substring(i * 8 + 4, i * 8 + 8), 16) +
                        (short)Integer.parseInt(s.substring(i * 8 + 8, i * 8 + 12), 16));
                Long temp = Long.parseLong(s.substring(i * 8 + 4, i * 8 + 12), 16);
                data[i] = Float.intBitsToFloat(temp.intValue());
            }
            tempChecksum = (short)(tempChecksum + id);
            short checksum = (short)Integer.parseInt(s.substring(s.length() - 4, s.length()), 16);
            if (tempChecksum != checksum) {
                error = true;
                return;
            }
        } catch (Exception e) {
            error = true;
            return;
        }
    }

    public boolean parsingError() {
        return error;
    }

    public float[] getData() {
        return data;
    }

    public int getDataSize() {
        return n;
    }

    public short getId() {
        return id;
    }

    private static String integerToHexString(int x) {
        String result = Integer.toHexString(x);
        result = result.toUpperCase();
        while (result.length() < 8) {
            result = "0" + result;
        }
        return result;
    }

    public static String format(short id, float[] data, int n) {
        short checksum = id;
        String result = integerToHexString(id).substring(4);
        for (int i = 0; i < n; ++i) {
            String current = Integer.toHexString(Float.floatToIntBits(data[i])).toUpperCase();
            while (current.length() < 8) {
                current = "0" + current;
            }
            checksum = (short)(checksum + (short)Integer.parseInt(current.substring(0, 4), 16) +
                    (short)Integer.parseInt(current.substring(4, 8), 16));

            result += current;
        }
        result += integerToHexString(checksum).substring(4) + "#";
        return result;
    }

    public static String format(short id) {
        float temp[] = {0.0f};
        return format(id, temp, 1);
    }

    public static String format(short id, float x) {
        float temp[] = {x};
        return format(id, temp, 1);
    }

    public static String format(short id, float x, float y) {
        float temp[] = {x, y};
        return format(id, temp, 2);
    }

    public static String format(short id, float x, float y, float z) {
        float temp[] = {x, y, z};
        return format(id, temp, 3);
    }
}
