package com.controller.utils;

import java.util.LinkedList;

public class SerialStreamParser {
    private StringBuilder stringBuilder = new StringBuilder();

    public SerialStreamParser() {
    }

    public void appendData(String s) {
        stringBuilder.append(s);
    }

    public LinkedList<DataFormat> parseStream() {
        LinkedList<DataFormat> result = new LinkedList<DataFormat>();

        while (stringBuilder.length() > 0 && stringBuilder.charAt(0) == '\n') {
            stringBuilder.deleteCharAt(0);
        }

        String a[ ] = stringBuilder.toString().split( "\n" );
        for (int i = 0; i < a.length; ++i) {
            DataFormat data = new DataFormat(a[i]);

            if (!data.parsingError()) {
                result.add(data);
            }
        }

        if (stringBuilder.lastIndexOf("\n") > 0) {
            stringBuilder.delete(0, stringBuilder.lastIndexOf("\n"));
        }

        while (stringBuilder.length() > 0 && stringBuilder.charAt(0) == '\n') {
            stringBuilder.deleteCharAt(0);
        }

        return result;
    }
}
