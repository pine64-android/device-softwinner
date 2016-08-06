
package com.softwinner.update;

import com.softwinner.update.UpdateService.CheckBinder;
import com.softwinner.update.ui.InstallPackage;

import android.app.Activity;
import android.app.Dialog;
import android.app.NotificationManager;
import android.app.Service;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.SharedPreferences;
import android.os.Bundle;
import android.os.Handler;
import android.os.IBinder;
import android.util.Log;
import android.view.LayoutInflater;
import android.view.View;
import android.view.View.OnClickListener;
import android.widget.Button;
import android.widget.ProgressBar;
import android.widget.TextView;
import android.widget.Toast;

public class DownloadActivity extends Activity {

    public static final String ACTION_CHECK = "com.softwinner.update.ACTION_CHECK";

    public static final String ACTION_DOWNLOAD = "com.softwinner.update.ACTION_DOWNLOAD";

    private TextView mDescription;

    private Button mCancel;

    private Button mCombineBtn;

    private ProgressBar mProgress,mCheckProgress;

    private TextView mCheckingStatus;
    
    private TextView mPercent;

    private CheckBinder mServiceBinder;

    private Handler mHandler = new Handler();

    private QueryThread[] mQueryThread = new QueryThread[2];

    private Preferences mPreference;
    

    /** ------------------------------------------------------ */
    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);    
        Intent mIntent = new Intent(this, UpdateService.class);
        bindService(mIntent, mConn, Service.BIND_AUTO_CREATE);
        mPreference = new Preferences(this);
        mQueryThread[0] = new QueryCheck();
        mQueryThread[1] = new QueryDownload(); 

    }

    @Override
    protected void onDestroy() {
        super.onDestroy();
        mQueryThread[0].stop();
        mQueryThread[1].stop();
        unbindService(mConn);
        Log.v("DownloadActivity","transfer onDestroy");
    }
    private void setCheckView() {
        setContentView(R.layout.update_checking);
        mCheckProgress=(ProgressBar)findViewById(R.id.checking_progress);
        mCheckingStatus = (TextView) findViewById(R.id.checking_status);
        mCheckingStatus.setText(R.string.check_connecting);
        mQueryThread[0].start();
    }
    private String setStr(long size,int status){
      String str=null;
	  long k= (size/1024);
	  if(k<1024){
	     str=status*k/100+"K/"+k+"K";
	   }else if(k>=1024){
		    long m=k/1024;
		    k=k%1024;
	    str=m*status/100+"."+ status*k/100+"Mb/"+m+"."+k+"Mb";
	  }
	  return str;
    }
    private void setDownloadView() {
        setContentView(R.layout.update_download);
        mDescription = (TextView) findViewById(R.id.new_version_description);
        mCombineBtn = (Button) findViewById(R.id.download_pause_and_update);
        mCancel = (Button) findViewById(R.id.cancel);
        mProgress = (ProgressBar) findViewById(R.id.download_progress);
        mPercent=(TextView)findViewById(R.id.percent);
        final NotificationManager notificationManager = (NotificationManager)getSystemService(Context.NOTIFICATION_SERVICE);
        mCombineBtn.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View v) {
            	notificationManager.cancel(R.drawable.ic_title);
                Object tag = v.getTag();    
                if (Integer.valueOf(R.string.download_download).equals(tag)) {
                    Intent intent = new Intent(DownloadActivity.this, UpdateService.class);
                    intent.putExtra(UpdateService.KEY_START_COMMAND,
                            UpdateService.START_COMMAND_DOWNLOAD);
                    startService(intent);
                    mQueryThread[1].start();  
                } else if (Integer.valueOf(R.string.download_pause).equals(tag)) {
                    mServiceBinder.setTaskPause(UpdateService.TASK_ID_DOWNLOAD);
                } else if (Integer.valueOf(R.string.download_resume).equals(tag)) {
                    mServiceBinder.setTaskResume(UpdateService.TASK_ID_DOWNLOAD);
                } else if (Integer.valueOf(R.string.download_update).equals(tag)) {
                    final Dialog dlg = new Dialog(DownloadActivity.this);
                    dlg.setTitle(R.string.confirm_update);
                    LayoutInflater inflater = LayoutInflater.from(DownloadActivity.this);
                    InstallPackage dlgView = (InstallPackage) inflater.inflate(
                            R.layout.install_ota, null, false);
                    dlgView.setPackagePath(UpdateService.DOWNLOAD_OTA_PATH);
                    dlgView.deleteSource(true);
                    dlg.setContentView(dlgView);
                    dlg.findViewById(R.id.confirm_cancel).setOnClickListener(
                            new View.OnClickListener() {
                                @Override
                                public void onClick(View v) {
                                    dlg.dismiss();
                                }
                            });
                    dlg.show();
                }
            }
        });
        mCancel.setOnClickListener(new OnClickListener() {
            @Override
            public void onClick(View arg0) {
            	notificationManager.cancel(R.drawable.ic_title);
            	mServiceBinder.resetTask(UpdateService.TASK_ID_DOWNLOAD);
                DownloadActivity.this.finish();
            }
        });
        mDescription.setText(mPreference.getPackageDescriptor().replace("\\n","\n"));
        mQueryThread[1].start();
    }

    /** -----------------Internal Class-------------------------- */
    private ServiceConnection mConn = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            mServiceBinder = (CheckBinder) service;
            Intent extrasIntent = getIntent();
            String action = extrasIntent.getAction();
            boolean b = mServiceBinder.getTaskRunnningStatus(UpdateService.TASK_ID_DOWNLOAD) == ThreadTask.RUNNING_STATUS_UNSTART;
            if (ACTION_CHECK.equals(action) && b) {
                Intent intent = new Intent(DownloadActivity.this, UpdateService.class);
                intent.putExtra(UpdateService.KEY_START_COMMAND,
                        UpdateService.START_COMMAND_START_CHECKING);
                startService(intent);
                setCheckView();
            } else if (ACTION_DOWNLOAD.equals(action) || !b) {
                setDownloadView();
            } 
            if(DownloadTask.FAIL_ACTION.equals(action)){        	
            		setDownloadView();
                    mPercent.setVisibility(View.VISIBLE);
                	mPercent.setText(R.string.download_error);
            }else if(DownloadTask.SUCCEED_ACTION.equals(action)){
            	setDownloadView();
            	mPercent.setVisibility(View.VISIBLE);
            	mPercent.setText(getString(R.string.Download_succeed));
            	mProgress.setProgress(100);
            	mCombineBtn.setText(R.string.download_update);
                mCombineBtn.setTag(Integer.valueOf(R.string.download_update)); 
            }
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {

        }
    };

    private class QueryDownload extends QueryThread {

        int lastProgress = -1;

        protected void loop() {
            int status = mServiceBinder.getTaskRunnningStatus(UpdateService.TASK_ID_DOWNLOAD);
            Log.d(Utils.GROBLE_TAG, "query status " + status);
            switch (status) {
                case DownloadTask.RUNNING_STATUS_UNSTART:
                    mHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            mCombineBtn.setText(R.string.download_download);
                            mCombineBtn.setTag(Integer.valueOf(R.string.download_download));
                        }
                    });
                    stop();
                    break;
                case DownloadTask.RUNNING_STATUS_RUNNING:
                    mHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            mCombineBtn.setText(R.string.download_pause);
                            mCombineBtn.setTag(Integer.valueOf(R.string.download_pause));
                        }
                    });
                    break;
                case DownloadTask.RUNNING_STATUS_PAUSE:
                    mHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            mCombineBtn.setText(R.string.download_resume);
                            mCombineBtn.setTag(Integer.valueOf(R.string.download_resume));
                        }
                    });
                    break;
                case DownloadTask.RUNNING_STATUS_FINISH:
                    int errorCode = mServiceBinder.getTaskErrorCode(UpdateService.TASK_ID_DOWNLOAD);
                    if (errorCode == DownloadTask.NO_ERROR) {
                        mHandler.post(new Runnable() {
                            @Override
                            public void run() {
                                mCombineBtn.setText(R.string.download_update);
                                mCombineBtn.setTag(Integer.valueOf(R.string.download_update));
                            }
                        });
                    } else {
                        mHandler.post(new Runnable() {
                            @Override
                            public void run() {
                               // Toast.makeText(DownloadActivity.this, R.string.download_error,
                                 //       Toast.LENGTH_SHORT).show();
                                mServiceBinder.resetTask(UpdateService.TASK_ID_DOWNLOAD);
                                mPercent.setText(R.string.download_error);
                            }
                        });
                    }
                    break;
            }
            int progress = mServiceBinder.getTaskProgress(UpdateService.TASK_ID_DOWNLOAD);
            if (lastProgress != progress) {
                lastProgress = progress;
               
                mHandler.post(new Runnable() {

                    @Override
                    public void run() {
                    	 if(mPreference.getDownloadSize()>0){
                         	mPercent.setVisibility(View.VISIBLE);
                         	mPercent.setText(setStr(mPreference.getDownloadSize(),lastProgress));
                         }
                        mProgress.setProgress(lastProgress);
                        if(lastProgress==100){
                        	mPercent.setText(getString(R.string.Download_succeed));
                        }
                         
                    }
                });
           }
        }
    }

    private class QueryCheck extends QueryThread {
        protected void loop() {
            int status = mServiceBinder.getTaskRunnningStatus(UpdateService.TASK_ID_CHECKING);
            switch (status) {
                case CheckingTask.RUNNING_STATUS_UNSTART:
                    stop();
                    break;
                case CheckingTask.RUNNING_STATUS_RUNNING:
                    mHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            mCheckingStatus.setText(R.string.check_now);
                        }
                    });
                    break;
                case CheckingTask.RUNNING_STATUS_FINISH:
                    int errorCode = mServiceBinder.getTaskErrorCode(UpdateService.TASK_ID_CHECKING);
                    measureError(errorCode);
                    break;
            }
        }

        private void measureError(int errorCode) {
            switch (errorCode) {
                case CheckingTask.ERROR_UNDISCOVERY_NEW_VERSION:
                    mHandler.post(new Runnable() {
                        @Override
                        public void run() {
                        	mCheckProgress.setVisibility(View.INVISIBLE);
                            mCheckingStatus.setText(R.string.check_failed);
                        }
                    });
                    break;
                case CheckingTask.ERROR_UNKNOWN:
                    mHandler.post(new Runnable() {
                        @Override
                        public void run() {
                        	mCheckProgress.setVisibility(View.INVISIBLE);
                            mCheckingStatus.setText(R.string.check_unknown);
                        }
                    });
                    break;
                case CheckingTask.NO_ERROR:
                    mHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            setDownloadView();
                        }
                    });
                    mServiceBinder.resetTask(UpdateService.TASK_ID_CHECKING);
                    stop();
                    break;
                case CheckingTask.ERROR_NETWORK_UNAVAIBLE:
                    mHandler.post(new Runnable() {
                        @Override
                        public void run() {
                            mCheckingStatus.setText(R.string.net_error);
                        }
                    });
                    break;
            }
        }
    }
}

class QueryThread implements Runnable {

    private boolean mStop = false;

    public void start() {
        mStop = false;
        new Thread(this).start();
    }

    @Override
    public void run() {
        while (!mStop) {
            try {
                Thread.sleep(500);
            } catch (Exception e) {
                e.printStackTrace();
            }
            loop();
        }
    }

    public void stop() {
        mStop = true;
    }

    protected void loop() {
    }

}
