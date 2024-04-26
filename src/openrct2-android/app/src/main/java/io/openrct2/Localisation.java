package io.openrct2;

import java.io.UnsupportedEncodingException;

public class Localisation {

    static final String TAG = Localisation.class.getSimpleName();

    public static String win1252ToUtf8(byte[] src) {
        try {
            return new String(src, "CP1252");
        } catch (UnsupportedEncodingException e) {
            e.printStackTrace();
        }

        return "";
    }
}
