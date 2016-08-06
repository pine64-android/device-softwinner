
package com.softwinner.update;

import com.softwinner.update.Utils;

import java.util.Calendar;

import android.app.AlarmManager;
import android.app.PendingIntent;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.util.Log;

public class LoaderReceiver extends BroadcastReceiver {
    private static final String TAG = "LoaderReceiver";

    public static final int CHECK_BEGIN_HOUR = 8;

    public static final int CHECK_ENT_HOUR = 24;

    public static final String UPDATE_GET_NEW_VERSION = "com.softwinner.update.UPDATE_GET_NEW_VERSION";

    public static final String CHECKING_TASK_COMPLETED = "com.softwinner.update.CHECKING_TASK_COMPLETED";

    public long CHECK_REPEATE_TIME = Utils.CHECK_CYCLE_DAY*24 * 60 * 60 * 1000;
    
    private static boolean rebootShallCheck=false;

    private void DailyCheck(Context context, Preferences prefs) {
        AlarmManager alarmManager = (AlarmManager) context.getSystemService(Context.ALARM_SERVICE);
        Calendar mCalendar = Calendar.getInstance();
        mCalendar.setTimeInMillis(System.currentTimeMillis());
        if (!Utils.DEBUG) {
            Log.i(TAG, "data now is :" + mCalendar.getTime());
            mCalendar.set(mCalendar.DAY_OF_YEAR, mCalendar.get(mCalendar.DAY_OF_YEAR) + Utils.CHECK_CYCLE_DAY);

            PendingIntent mPendingIntent = getPendingIntent(context, 0);
            alarmManager.setRepeating(AlarmManager.RTC, mCalendar.getTimeInMillis(),
                    CHECK_REPEATE_TIME, mPendingIntent);
        } else {
            mCalendar.set(mCalendar.MINUTE, mCalendar.get(mCalendar.MINUTE) + 10);
            PendingIntent mPendingIntent = getPendingIntent(context, 0);
            alarmManager.setRepeating(AlarmManager.RTC, mCalendar.getTimeInMillis(), 10 * 60 * 1000,
                    mPendingIntent);
        }
        Log.i(TAG, "check will at time:" + mCalendar.getTime());
    }

    private PendingIntent getPendingIntent(Context paramContext, int paramInt) {
        Intent localIntent = new Intent(paramContext, UpdateService.class);
        localIntent.putExtra(UpdateService.KEY_START_COMMAND,
                UpdateService.START_COMMAND_START_CHECKING);
        return PendingIntent.getService(paramContext, 0, localIntent, 0);
    }

    private void startService(Context context) {
    	if(Utils.DEBUG)Log.d(TAG,"startService");
        Intent mIntent = new Intent(context, UpdateService.class);
        mIntent.putExtra(UpdateService.KEY_START_COMMAND, UpdateService.START_COMMAND_START_CHECKING);
        context.startService(mIntent);
    }

    public boolean checkConnectivity(Context context) {
        ConnectivityManager cm = (ConnectivityManager) context
                .getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo info = cm.getActiveNetworkInfo();
        return info != null && info.isConnected();
    }

    @Override
    public void onReceive(Context context, Intent intent) {
        Preferences prefs = new Preferences(context);
        String action = intent.getAction();
        if (Utils.DEBUG)
            Log.i(TAG, "receive a new action : " + action);
        if (action.equals(ConnectivityManager.CONNECTIVITY_ACTION) && checkConnectivity(context)
                &&rebootShallCheck) {
        	rebootShallCheck=false;
            startService(context);
        } else if (action.equals(Intent.ACTION_BOOT_COMPLETED)) {
        	rebootShallCheck=true;
            DailyCheck(context, prefs);
        }
    }

}
