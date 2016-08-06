
package com.softwinner.update;

import android.app.ActivityManagerNative;
import android.content.Context;
//import android.net.ethernet.EthernetNative;
import android.net.InterfaceConfiguration;
import android.os.Build;
import android.os.IBinder;
import android.os.INetworkManagementService;
import android.os.RemoteException;
import android.os.ServiceManager;
import android.os.SystemProperties;

public class UpdaterInfo {
    /**
     * info include: 1. id --> ro.build.id 2. brand --> ro.product.brand 3.
     * device --> ro.product.device 4. board --> Build.BOARD 5. mac -->
     * EthernetNative.getEthHwaddr("eth0") 6. android open version -->
     * ro.build.version.release 7. build time --> Build.UTC 8. builder -->
     * ro.build.user 9. fingerprint --> Build.FINGERPRINT
     */
    public static final String UNKNOWN = "unknown";

    public static final String DEFAULT_SERVER_IP="192.168.1.239";
    public static final String DEFAULT_SERVER_PORT="";

   // public static final String postUrl = Utils.SERVER_URL_USE_DOMAIN;

    public static String updating_apk_version;// = "1";

    public static String brand;// = "softwinner";

    public static String device;// = "G10";

    public static String board;// = "CRANE-V1.2";

    public static String mac;//= "00.11.22.33.44.55";

    public static String firmware /*= "1.0"*/;
    public static String server_ip /*= "192.168.1.239"*/;
    public static String server_port /*= ":8080"*/;

    public static String android;// = "2.3.4";

    public static String time;// = "20120301.092251";

    public static String builder;// = "ygwang";

    public static String fingerprint;// = "softwinners/apollo_mele/G10:2.3.4/GRJ22/eng.ygwang.20120301.092251:eng/test-keys";

    public static String country;
    
    public static String chip_id;
    
    public static String guid;
    
    public static String service_type;
    
    public static String para_serial;

    private Context mContext;

    public UpdaterInfo(Context mContext) {
        this.mContext = mContext;
        onInit();
    }

    private void getcountry() {
        try {
            country = ActivityManagerNative.getDefault().getConfiguration().locale.getCountry();
        } catch (RemoteException e) {

        }
    }

    public static String makePostString() {
        return null;
    }

    private static String getString(String property) {
        return SystemProperties.get(property, UNKNOWN);
    }
    
    private String getMacAddr(){
        IBinder b = ServiceManager.getService(Context.NETWORKMANAGEMENT_SERVICE);
        INetworkManagementService networkManagement = INetworkManagementService.Stub.asInterface(b);
        if(networkManagement != null){
            InterfaceConfiguration iconfig = null;
            try{
                iconfig= networkManagement.getInterfaceConfig("wlan0");
                if (iconfig == null) {
                    iconfig = networkManagement.getInterfaceConfig("eth0");
                }
            }catch(Exception e){            	 
                e.printStackTrace();
            }finally{
                if(iconfig != null){
               	 	return iconfig.getHardwareAddress();
              	}else{
              		return "";
              	}
            }
        }else{
            return "";
        }
    }
    private String getVersionCode(){
	String packageName = mContext.getPackageName();
	int versionCode = 0;
	try{
	    versionCode = mContext.getPackageManager().getPackageInfo(
			packageName, 0).versionCode;
	}catch(Exception e){
            
	}
	return String.valueOf(versionCode);
    }
    private void onInit(){
    	getcountry();
    	updating_apk_version = getVersionCode();
    	brand=getString("ro.product.brand");
    	device=getString("ro.product.device");
    	board=getString("ro.product.board");
    	mac = getMacAddr();
    	firmware=getString("ro.product.firmware");

    	server_ip=getString("persist.ota.server.ip");
        if(UNKNOWN.equals(server_ip)) {
            server_ip = DEFAULT_SERVER_IP;
        }

    	server_port=getString("persist.ota.server.port");
        if(UNKNOWN.equals(server_port)) {
            server_port = DEFAULT_SERVER_PORT;//default do'nt determin port number
        } else {
            server_port = ":" + server_port;
        }

    	android=getString("ro.build.version.release");
    	time=getString("ro.build.date.utc");
    	builder=getString("ro.build.user");
    	fingerprint=getString("ro.build.fingerprint");
    	  	
    	chip_id = "ffee";//ProcCpuInfo.getChipIDHex();
    	guid = "646212C46B774f95BBBE2F9CCFF32797";
    	service_type = "FIRMWARE_UPDATE";
    //para_serial = "customer=test;product=pad;device=vi40;hardware=a10;os=android4.0.4;firmware=1.4;time=2012-07-19;other=null";
    	para_serial = "customer="+getString("ro.product.manufacturer")+";" + "product="+getString("ro.product.brand")+";" 
    	              + "device="+getString("ro.product.device")+";" + "hardware=NULL;"+ "chip_id="+chip_id+";"
    	              + "os="+getString("ro.build.version.release")+";" + "firmware="+getString("ro.product.firmware");
    	              
    
    }
}
