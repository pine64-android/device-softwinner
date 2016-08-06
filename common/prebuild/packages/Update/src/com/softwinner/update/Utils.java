package com.softwinner.update;

import android.os.SystemProperties;

public class Utils {    
    public static final String GROBLE_TAG = "SoftwinnerUpdater";
    public static final String SERVER_ADDRESS="http://" + UpdaterInfo.server_ip + UpdaterInfo.server_port +"/";
    public static final String SERVER_URL_USE_DOMAIN = SERVER_ADDRESS+"db1000/update";
    public static final String SERVER_URL_USE_IP = SERVER_ADDRESS+"db1000/update";

    public static final String DOWNLOAD_PATH = "/sdcard/ota.zip";
    public static final boolean DEBUG = true;  //false
    public static final int CHECK_CYCLE_DAY=1;
}
