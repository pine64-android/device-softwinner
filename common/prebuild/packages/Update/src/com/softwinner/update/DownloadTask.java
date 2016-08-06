
package com.softwinner.update;

import com.softwinner.update.ThreadTask;
import com.softwinner.update.Utils;

import java.io.BufferedInputStream;
import java.io.File;
import java.io.FileNotFoundException;
import java.io.FileOutputStream;
import java.io.IOException;
import java.io.InputStream;
import java.io.RandomAccessFile;
import java.net.HttpURLConnection;
import java.net.MalformedURLException;
import java.net.URL;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.methods.HttpGet;
import org.apache.http.impl.client.DefaultHttpClient;

import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.os.Handler;
import android.os.Message;
import android.util.Log;

public class DownloadTask extends ThreadTask {
    private static final String TAG = "DownloadTask";

    public static final int ERROR_DOWNLOAD_FAILED = 1;
    
    public static final String FAIL_ACTION="com.softwinner.update.ACTION_DOWNLOAD_FAIL";
    public static final String SUCCEED_ACTION="com.softwinner.update.ACTION_DOWNLOAD_SUCCEED";

    private String mUrl;

    private String mTargetFile;

    private long mFileSize;

    private long mPosition = 0;

    private Handler mHandler;

    private Context mContext;

    private Preferences mPrefs;

    public boolean mDestoryDownloadThread = false;

    private long mCountSize = 0;

    private RandomAccessFile mRandomAccessFile;

    public DownloadTask(Context context, Handler handler) {
        mHandler = handler;
        mContext = context;
        mPrefs = new Preferences(context);
        mUrl = mPrefs.getDownloadURL();
        mTargetFile = mPrefs.getDownloadTarget();
        mPosition = mPrefs.getDownloadPos();
        mFileSize = mPrefs.getDownloadSize();
        if (mPosition == 0 || mFileSize == 0 || !checkCompleted()) {
            resetBreakpoint();
            mRunningStatus = RUNNING_STATUS_UNSTART;
        } else {
            mRunningStatus = RUNNING_STATUS_PAUSE;
            mProgress = mFileSize != 0 ? (int) (mPosition * 100 / mFileSize) : 0;
        }
    }

    public DownloadTask(Context context, Handler handler, String url, String targetFile) {
        this(context, handler);
        mUrl = url;
        mTargetFile = targetFile;
    }

    protected void onRunning() {
        mUrl = mPrefs.getDownloadURL();
        mTargetFile = mPrefs.getDownloadTarget();
        try {
            URL url = new URL(mUrl);
            mRandomAccessFile = new RandomAccessFile(mTargetFile, "rw");
            if (Utils.DEBUG) {
                Log.i(TAG, "start download, url=" + mUrl + " " + "target=" + mTargetFile);
            }
            byte[] buf = new byte[1024 * 8];
            HttpURLConnection cn = (HttpURLConnection) url.openConnection();
            mFileSize = cn.getContentLength(); 
            if (mFileSize < 0) {
                Log.e(TAG, "Download file " + mTargetFile + " from " + mUrl + " is failure");
                mErrorCode = ERROR_DOWNLOAD_FAILED;
                resetBreakpoint();
                throw new IOException("Something is wrong with network!");
            }
            if (Utils.DEBUG)
                Log.v(TAG, "FileSize=" + mFileSize);
            mRandomAccessFile.setLength(mFileSize);
            mRandomAccessFile.seek(mPosition);
            cn = (HttpURLConnection) url.openConnection();
            cn.setRequestProperty("Range", "bytes=" + mPosition + "-" + mFileSize);
            mCountSize = mPosition;
            BufferedInputStream bis = new BufferedInputStream(cn.getInputStream());
            int len;
            while ((len = bis.read(buf)) > 0) {
                while (mRunningStatus == RUNNING_STATUS_PAUSE||mRunningStatus==RUNNING_STATUS_UNSTART) {
                    try {
                        if (mRunningStatus==RUNNING_STATUS_UNSTART) {
                        	resetBreakpoint();
                            return;
                        }
                        Thread.sleep(500);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }
                synchronized (mRandomAccessFile) {
                    mRandomAccessFile.write(buf, 0, len);
                    mCountSize += len;
                    mProgress = (int) (mCountSize * 100 / mFileSize);
                    if (mFileSize == mCountSize) {
                        mProgress = 100;
                    }
                    mPosition = mCountSize;
                    mPrefs.setBreakpoint(mFileSize, mPosition);
                }
            }
            
            String md5 = mPrefs.getMd5();
            if (!MD5.checkMd5(md5, UpdateService.DOWNLOAD_OTA_PATH)) {
                Log.d(TAG,"md5 check failed");
                mErrorCode = ERROR_DOWNLOAD_FAILED;
                noticeUI(R.string.Download_failed,FAIL_ACTION);
            }else{
                mErrorCode = NO_ERROR; 
                noticeUI(R.string.Download_succeed,SUCCEED_ACTION);
            }    
            resetBreakpoint();
        } catch (MalformedURLException e) {
            Log.e(TAG, "Download is  failure\n" + e.toString());
            mErrorCode = ERROR_DOWNLOAD_FAILED;
            resetBreakpoint();
            e.printStackTrace();
        } catch (FileNotFoundException e) {
            Log.e(TAG, "URL is not exist\n" + e.toString());
            mErrorCode = ERROR_DOWNLOAD_FAILED;
            resetBreakpoint();
            e.printStackTrace();
        } catch (IOException e) {
            Log.e(TAG, "IO is exception\n" + e.toString());
            mErrorCode = ERROR_DOWNLOAD_FAILED;
            resetBreakpoint();
            e.printStackTrace();
        }
    }

    protected void onStart() {

    }

    private boolean checkCompleted() {
        File file = new File(mTargetFile);
        if (file.exists() && file.length() == mFileSize) {
            return true;
        }
        return false;
    }

    public void resetBreakpoint() {
        mPosition = 0;
        mFileSize = 0;
        mPrefs.setBreakpoint(0, 0);
    }
    private void noticeUI(int RString,String action){
    	NotificationManager notificationManager;
        Notification notification;
        Log.v(TAG, "Donwload finish");
        notificationManager = (NotificationManager) mContext
                .getSystemService(Context.NOTIFICATION_SERVICE);
        notification = new Notification(R.drawable.ic_title,
                mContext.getString(RString), System.currentTimeMillis());
        notification.flags |= Notification.FLAG_AUTO_CANCEL; 
        Intent mIntent=new Intent(mContext,DownloadActivity.class);
        mIntent.setAction(action);
        PendingIntent pi = PendingIntent.getActivity(mContext, 0, mIntent, 0);
        notification.setLatestEventInfo(mContext, "Update",
                mContext.getString(RString), pi);
        notificationManager.notify(R.drawable.ic_title, notification);
    }
}
