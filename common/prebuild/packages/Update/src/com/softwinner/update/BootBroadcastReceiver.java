package com.softwinner.update;
 
import com.softwinner.update.Utils;
import android.content.BroadcastReceiver;  
import android.content.Context;  
import android.content.Intent;  
  
public class BootBroadcastReceiver extends BroadcastReceiver {  
  
 static final String ACTION = "android.intent.action.BOOT_COMPLETED";  
   
 @Override  
 public void onReceive(Context context, Intent intent) {  
    
  if (intent.getAction().equals(ACTION)){  
   Intent UpdateActivity=new Intent(context,UpdateActivity.class);  
   UpdateActivity.addFlags(Intent.FLAG_ACTIVITY_NEW_TASK);  
   context.startActivity(UpdateActivity);  
  }  
 }  
}