package com.softwinner.update;

import java.io.FileInputStream;
import java.io.IOException;

public class ProcCpuInfo {
    
    /* get 32 bit string */
    public static String getChipIDHex() {
        
        FileInputStream fis = null;
        String chipid = null;
        StringBuilder cpuinfo = new StringBuilder(); 
        
        try {
            fis = new FileInputStream("/proc/cpuinfo");
            byte[] buf = new byte[1024];
            int len=0;
            
            while((len = fis.read(buf))>0) {
                cpuinfo.append(new String(buf, 0, len));
            }    
            chipid = cpuinfo.toString();
            if( fis != null )
                fis.close();
        }catch(IOException io){
            io.printStackTrace();
        }
       
        int index = chipid.indexOf("Serial");
        chipid  = chipid.substring(index); 
        index   = chipid.indexOf(": ");
        chipid  = chipid.substring(index+2,index+34);        
        return chipid;
    }  
    
    /* get 128 bit string */
    public static String getChipID() {        
        StringBuilder chipId = new StringBuilder();  
        int intValue = 0;
        
        String hexString = getChipIDHex();               
        //String hexString="01234567890123456789012345678901"; /* for test */
        //String hexString="f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0f0"; /* for test */
        int hexLen = hexString.length();       
        for(int i=0; i< hexLen; i++){
            int k;
            intValue  = Integer.parseInt(hexString.substring(i,i+1),16);      
            
            k = (intValue & 8)>>3;
            chipId.append(k);
            
            k = (intValue & 4)>>2;
            chipId.append(k);
            
            k = (intValue & 2)>>1;
            chipId.append(k);
            
            k = intValue & 1;
            chipId.append(k);            
        }
        
        return chipId.toString();        
    }
}

