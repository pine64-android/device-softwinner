
package com.softwinner.update;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.math.BigInteger;
import java.security.MessageDigest;
import java.security.NoSuchAlgorithmException;

import android.util.Log;

public class MD5 {
    static String TAG = "MD5";

    private static String createMd5(String str) {
        MessageDigest mMDigest;
        FileInputStream Input;
        File file = new File(str);
        byte buffer[] = new byte[1024];
        int len;
        if (!file.exists())
            return null;
        try {
            mMDigest = MessageDigest.getInstance("MD5");
            Input = new FileInputStream(file);
            while ((len = Input.read(buffer, 0, 1024)) != -1) {
                mMDigest.update(buffer, 0, len);
            }
            Input.close();
        } catch (NoSuchAlgorithmException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
            return null;
        } catch (FileNotFoundException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
            return null;
        } catch (IOException e) {
            // TODO Auto-generated catch block
            e.printStackTrace();
            return null;
        }
        BigInteger mBInteger = new BigInteger(1, mMDigest.digest());
        Log.v(TAG, "create_MD5=" + mBInteger.toString(16));
        return mBInteger.toString(16);

    }

    public static boolean checkMd5(String Md5, String file) {
        String str = createMd5(file);
        Log.d(TAG,"md5sum = " + str);
        if(Md5.compareTo(str) == 0) return true;
        else
         return false;
    }
}
