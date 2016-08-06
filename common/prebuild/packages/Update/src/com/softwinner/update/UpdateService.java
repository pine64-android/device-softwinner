
package com.softwinner.update;

import com.softwinner.update.ThreadTask;
import com.softwinner.update.UpdateActivity;
import com.softwinner.update.Utils;

import java.util.Calendar;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.app.Service;
import android.content.Context;
import android.content.Intent;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.Binder;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.util.Log;
import android.widget.RemoteViews;

public class UpdateService extends Service {
    private static final String TAG = "UpdateService";

    public static final String KEY_START_COMMAND = "start_command";

    private Context mContext;

    private Preferences mPreference;

    public static final int START_COMMAND_START_CHECKING = 101;

    public static final int START_COMMAND_DOWNLOAD = 102;

    public static final int TASK_ID_CHECKING = 101;

    public static final int TASK_ID_DOWNLOAD = 102;

    public static final String DOWNLOAD_OTA_PATH = Utils.DOWNLOAD_PATH;

    private CheckBinder mBinder = new CheckBinder();

    private static ThreadTask mCheckingTask;

    private static ThreadTask mDownloadTask;

    final Handler mHandler = new Handler();

    public class CheckBinder extends Binder {    
        public boolean resetTask(int taskId){
            boolean b = false;
            switch (taskId) {
                case TASK_ID_CHECKING:
                    b = mCheckingTask.reset();
                    break;
                case TASK_ID_DOWNLOAD:
                    b = mDownloadTask.reset();
                    break;
            }
            return b;
        }
        
        public void setTaskPause(int taskId){
            switch (taskId) {
                case TASK_ID_CHECKING:
                    mCheckingTask.pause();
                    break;
                case TASK_ID_DOWNLOAD:
                    mDownloadTask.pause();
                    break;
            }
        }
        
        public void setTaskResume(int taskId){
            switch (taskId) {
                case TASK_ID_CHECKING:
                    mCheckingTask.resume();
                    break;
                case TASK_ID_DOWNLOAD:
                    mDownloadTask.resume();
                    break;
            }
        }

        public int getTaskRunnningStatus(int taskId) {
            int status = -1;
            switch (taskId) {
                case TASK_ID_CHECKING:
                    status = mCheckingTask.getRunningStatus();
                    break;
                case TASK_ID_DOWNLOAD:
                    status = mDownloadTask.getRunningStatus();
                    break;
            }
            return status;
        }

        public Object getTaskResult(int taskId) {
            Object result = null;
            switch (taskId) {
                case TASK_ID_CHECKING:
                    result = mCheckingTask.getResult();
                    break;
                case TASK_ID_DOWNLOAD:
                    result = mDownloadTask.getResult();
                    break;
            }
            return result;
        }

        public int getTaskErrorCode(int taskId) {
            int errorCode = -1;
            switch (taskId) {
                case TASK_ID_CHECKING:
                    errorCode = mCheckingTask.getErrorCode();
                    break;
                case TASK_ID_DOWNLOAD:
                    errorCode = mDownloadTask.getErrorCode();
                    break;
            }
            return errorCode;
        }

        public int getTaskProgress(int taskId) {
            int progress = -1;
            switch (taskId) {
                case TASK_ID_CHECKING:
                    progress = mCheckingTask.getProgress();
                    break;
                case TASK_ID_DOWNLOAD:
                    progress = mDownloadTask.getProgress();
                    break;
            }
            return progress;
        }
    }

    @Override
    public IBinder onBind(Intent intent) {
        return mBinder;
    }

    private void sendNewVersionBroadcast() {
        Intent intent = new Intent(LoaderReceiver.UPDATE_GET_NEW_VERSION);
        sendBroadcast(intent);
    }

    public int onStartCommand(Intent intent, int paramInt1, int paramInt2) {
        mContext = getBaseContext();
        mPreference = new Preferences(mContext);
		Log.d(TAG,"onStartCommand enter");
		if(intent == null)
		{
		   Log.d(TAG,"intent is null!");
		   return 0;
		}
        int cmd = intent.getIntExtra(KEY_START_COMMAND, 0);
        if (Utils.DEBUG) {
            Log.i(TAG, "get a start cmd : " + cmd);
        }
        switch (cmd) {
            case START_COMMAND_START_CHECKING:
            	Log.v(TAG,"status="+mDownloadTask.getRunningStatus());
                if (mDownloadTask.getRunningStatus() == ThreadTask.RUNNING_STATUS_UNSTART) {
                    mCheckingTask.start();
                }
                break;
            case START_COMMAND_DOWNLOAD:                
                if(mCheckingTask.getRunningStatus() != ThreadTask.RUNNING_STATUS_RUNNING){
                    mDownloadTask.start();
                }
                break;
            default:
                break;

        }
        return 0;
    }

    private void initInstance() {
        if (mCheckingTask == null) {
            mCheckingTask = new CheckingTask(this, mHandler);
        }
        if (mDownloadTask == null) {
            mDownloadTask = new DownloadTask(this, mHandler);
        }
    }

    @Override
    public void onCreate() {
        super.onCreate();
        Log.v(TAG,"UpdateService onCreate");
        initInstance();
    }
}
