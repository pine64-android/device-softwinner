
package com.softwinner.update;

import com.softwinner.update.ThreadTask;
import com.softwinner.update.Utils;

import java.io.BufferedReader;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileNotFoundException;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.UnsupportedEncodingException;
import java.util.ArrayList;
import java.util.List;

import javax.xml.parsers.DocumentBuilder;
import javax.xml.parsers.DocumentBuilderFactory;
import javax.xml.parsers.ParserConfigurationException;

import org.apache.http.HttpEntity;
import org.apache.http.HttpResponse;
import org.apache.http.NameValuePair;
import org.apache.http.client.ClientProtocolException;
import org.apache.http.client.HttpClient;
import org.apache.http.client.entity.UrlEncodedFormEntity;
import org.apache.http.client.methods.HttpPost;
import org.apache.http.impl.client.DefaultHttpClient;
import org.apache.http.message.BasicNameValuePair;
import org.apache.http.params.BasicHttpParams;
import org.apache.http.params.CoreConnectionPNames;
import org.apache.http.params.HttpConnectionParams;
import org.apache.http.params.HttpParams;
import org.apache.http.protocol.HTTP;
import org.apache.http.util.EntityUtils;
import org.w3c.dom.Document;
import org.w3c.dom.Element;
import org.w3c.dom.Node;
import org.w3c.dom.NodeList;
import org.xml.sax.SAXException;

import android.app.DownloadManager;
import android.app.DownloadManager.Request;
import android.app.Notification;
import android.app.NotificationManager;
import android.app.PendingIntent;
import android.content.Context;
import android.content.Intent;
import android.database.Cursor;
import android.net.Uri;
import android.os.Environment;
import android.os.Handler;
import android.os.Message;
import android.util.Log;


public class CheckingTask extends ThreadTask {
    private final static String TAG = "CheckingTask";

    public static final int ERROR_UNDISCOVERY_NEW_VERSION = 1;

    public static final int ERROR_NETWORK_UNAVAIBLE = 2;

    public static final int ERROR_UNKNOWN = 3;

    Context mContext;

    Handler mServiceHandler;

    public static String XML_DOWNLOAD_DIRECTORY = "/mnt/sdcard/Download";

    public static String XML_NAME = "ota_update.xml";
    
    private static final String STATUS="success";
    
    private static final String COMMAND="update_with_inc_ota";

    private long mDownloadId;

    private static final int HANDLE_XML_DOWNLOAD_FINISH = 100;

    private static final int HANDLE_XML_DOWNLOAD_FAIL = 101;
    
    private static final int CHECK_TIMEOUT=60*1000*5;

    private String command;
    
    private String force;
    
    private String status;

    private String zipUrl;

    private String md5;

    private String description = null;

    private String country = null;

    private DownloadManager mDownload;

    private UpdaterInfo mUpdaterInfo;

    private Preferences mPreferences;
    
    //private InputStream filein;

    public CheckingTask(Context context, Handler handler) {
        mServiceHandler = handler;
        mContext = context;
        mUpdaterInfo = new UpdaterInfo(mContext);
        mPreferences = new Preferences(mContext);
    }

    private void handleDownloadResult(int msg, Object obj) {
        switch (msg) {
            case HANDLE_XML_DOWNLOAD_FINISH:
                if (Utils.DEBUG)
                    Log.i(TAG, "xml " + obj.toString() + " download finish");
                parserXml(XML_DOWNLOAD_DIRECTORY, XML_NAME);
                mDownload.remove(mDownloadId);
                mErrorCode = NO_ERROR;
                break;
            case HANDLE_XML_DOWNLOAD_FAIL:
                mDownload.remove(mDownloadId);
                if (Utils.DEBUG)
                    Log.i(TAG, "xml download fail");
                mErrorCode = ERROR_NETWORK_UNAVAIBLE;
                break;
        }
    }

    private void downloadXML(String url, String xmlName) {
        if (Utils.DEBUG)
            Log.i(TAG, "start download a xml file:" + xmlName);
        Uri uri = Uri.parse(url);
        Request request = new DownloadManager.Request(uri);
        File file = new File(XML_DOWNLOAD_DIRECTORY + "/" + xmlName);
        File dir = new File(XML_DOWNLOAD_DIRECTORY);
        Log.i(TAG, "file:" + file.getAbsolutePath());
        if (file.exists()) {
            file.delete();
        }
        Log.i(TAG,
                "dir:" + dir.getAbsolutePath() + " exixts:" + dir.exists() + " mkdir:"
                        + dir.mkdirs());
        if (!dir.exists()) {
            dir.mkdirs();
        }
        request.setDestinationInExternalPublicDir(Environment.DIRECTORY_DOWNLOADS, xmlName);
        request.setShowRunningNotification(false);
        mDownloadId = mDownload.enqueue(request);
        while (true) {
            try {
                Cursor c = mDownload.query(new DownloadManager.Query().setFilterById(mDownloadId));
                if (c != null) {
                    c.moveToFirst();
                    int status = c.getInt(c.getColumnIndex(DownloadManager.COLUMN_STATUS));
                    if (status == DownloadManager.STATUS_SUCCESSFUL) {
                        handleDownloadResult(HANDLE_XML_DOWNLOAD_FINISH, xmlName);
                        break;
                    } else if (status == DownloadManager.STATUS_FAILED) {
                        handleDownloadResult(HANDLE_XML_DOWNLOAD_FAIL, null);
                        break;
                    }
                    c.close();
                }
                Thread.sleep(300);
            } catch (InterruptedException e) {
                mErrorCode = ERROR_UNKNOWN;
                e.printStackTrace();
            }
        }
    }

    protected void onRunning() {
        mDownload = (DownloadManager) mContext.getSystemService(mContext.DOWNLOAD_SERVICE);
        sendPost(Utils.SERVER_URL_USE_DOMAIN);
        if(mErrorCode != NO_ERROR )
        {
        		mErrorCode = NO_ERROR;
        		sendPost(Utils.SERVER_URL_USE_IP);
        } 
    }

    private void parserXml(String xmlPath, String xmlName) {
        DocumentBuilderFactory domfac = DocumentBuilderFactory.newInstance();
        DocumentBuilder domBuilder;
        try {
            domBuilder = domfac.newDocumentBuilder();
            InputStream in = new FileInputStream(xmlPath + "/" + xmlName);
            Document doc = domBuilder.parse(in);
            Element root = doc.getDocumentElement();
            NodeList nodelist_1 = root.getChildNodes();
            if (nodelist_1 != null) {
                for (int i = 0; i < nodelist_1.getLength(); i++) {
                    Node node_1 = nodelist_1.item(i);
                    if (node_1.getNodeName().equals("command")) {
                        command = node_1.getAttributes().getNamedItem("name").getNodeValue();
                        force = node_1.getAttributes().getNamedItem("force").getNodeValue();
                        if (Utils.DEBUG){
                        	 Log.i(TAG, "get xml command:" + command);
                        	 Log.i(TAG, "get xml force:" + force);
                        }  
                        if(!command.equals(COMMAND)){ 
                        	mErrorCode = ERROR_UNDISCOVERY_NEW_VERSION;
                            return;
                        }
                        NodeList nodelist_2 = node_1.getChildNodes();
                        if (nodelist_2 != null) {
                            for (int j = 0; j < nodelist_2.getLength(); j++) {
                                Node node_2 = nodelist_2.item(j);
                                if (node_2.getNodeName().equals("url")) {
                                    zipUrl = node_2.getFirstChild().getNodeValue();
                                    if (Utils.DEBUG)
                                        Log.i(TAG, "get xml zipUrl:" + zipUrl);
                                }
                                if (node_2.getNodeName().equals("md5")) {
                                    md5 = node_2.getFirstChild().getNodeValue();
                                    if (Utils.DEBUG)
                                        Log.i(TAG, "get xml md5:" + md5);
                                }
                                if (node_2.getNodeName().equals("description")) {
                                    country = node_2.getAttributes().getNamedItem("country")
                                            .getNodeValue();
                                    if (description == null) {
                                        if (country.equals(mUpdaterInfo.country)
                                                || country.equals("ELSE"))
                                            description = node_2.getFirstChild().getNodeValue();
                                    }
                                }
                            }
                            if (Utils.DEBUG) {
                                Log.i(TAG, "get xml description:" + description);
                            }
                        }
                    } 
                }

            }
        } catch (Exception e) {
            e.printStackTrace();
            mErrorCode = ERROR_UNDISCOVERY_NEW_VERSION;
            return;
        }
        String[] checkInfo = new String[4];
        checkInfo[0] = md5;
        checkInfo[1] = zipUrl;
        checkInfo[2] = description;
        //checkInfo[3] = force;
        mResult = checkInfo;
        if (Utils.DEBUG) {
            int i = 0;
            Log.d(TAG, "checkInfo.length = " + checkInfo.length);
            if (i < checkInfo.length) Log.d(TAG, "md5 = " + checkInfo[i++]);
            if (i < checkInfo.length) Log.d(TAG, "zipUrl = " + checkInfo[i++]);
            if (i < checkInfo.length) Log.d(TAG, "description = " + checkInfo[i++]);
            if (i < checkInfo.length) Log.d(TAG, "force = " + checkInfo[i++]);
        }
    }

    private void sendPost(String server_url) {
    	Log.v(TAG,"send post to server >> " + server_url);
        HttpPost post = new HttpPost(server_url);
        List<NameValuePair> params = new ArrayList<NameValuePair>();
        
        params.add(new BasicNameValuePair("updating_apk_version", UpdaterInfo.updating_apk_version));
        params.add(new BasicNameValuePair("brand", UpdaterInfo.brand));
        params.add(new BasicNameValuePair("device", UpdaterInfo.device));
        params.add(new BasicNameValuePair("board", UpdaterInfo.board));
        params.add(new BasicNameValuePair("mac", UpdaterInfo.mac));
        params.add(new BasicNameValuePair("firmware", UpdaterInfo.firmware));
        params.add(new BasicNameValuePair("android", UpdaterInfo.android));
        params.add(new BasicNameValuePair("time", UpdaterInfo.time));
        params.add(new BasicNameValuePair("builder", UpdaterInfo.builder));
        params.add(new BasicNameValuePair("fingerprint", UpdaterInfo.fingerprint));
        params.add(new BasicNameValuePair("id", mPreferences.getID() + ""));
        /*
        params.add(new BasicNameValuePair("guid", UpdaterInfo.guid));
        params.add(new BasicNameValuePair("service_type", UpdaterInfo.service_type));
        params.add(new BasicNameValuePair("para_serial", UpdaterInfo.para_serial));
        */
        if (Utils.DEBUG) Log.i(TAG, "params: " + params);//
        
        HttpParams httpParameters = new BasicHttpParams();
        HttpConnectionParams.setSoTimeout(httpParameters,CHECK_TIMEOUT);
        HttpClient httpClient = new DefaultHttpClient(httpParameters);
        try {
            post.setEntity(new UrlEncodedFormEntity(params, HTTP.UTF_8));
            HttpResponse response = httpClient.execute(post);
            Log.i(TAG, "response status:  " + response.getStatusLine().getStatusCode());
            if (response.getStatusLine().getStatusCode() == 200) {
                HttpEntity entity = response.getEntity();
                String msg = EntityUtils.toString(entity);
                
                if (Utils.DEBUG)Log.i(TAG, "get data:  " + msg);
                String url[] = msg.split("=");
                if (url.length==2&&url[0].equals("url")&&url[1].length()>10) {
                    if (Utils.DEBUG)Log.i(TAG, "xml url:" + url[1]);
                    url[1] = url[1].replace(" ", "");
                    url[1] = url[1].replace("\r\n", "");
                    downloadXML(Utils.SERVER_ADDRESS + url[1], XML_NAME);
                } else {
                    if (Utils.DEBUG)Log.i(TAG, "Can'n find new firmware");
                    mErrorCode = ERROR_UNDISCOVERY_NEW_VERSION;
                }
                
                //String msg = EntityUtils.toString(entity);
                //if (Utils.DEBUG)Log.i(TAG, "get data:  " + msg);
                //InputStream filein = entity.getContent();
                //parser(filein);
				//String msg = EntityUtils.toString(entity);
                //if (Utils.DEBUG)Log.i(TAG, "get data:  " + msg);

            }else{
            	mErrorCode = ERROR_UNKNOWN;
            }
        } catch (Exception e) {
            mErrorCode = ERROR_UNKNOWN;
            e.printStackTrace();
        }

    }
    
    private void parser(InputStream in) {
        DocumentBuilderFactory domfac = DocumentBuilderFactory.newInstance();
        DocumentBuilder domBuilder;
        try {
            domBuilder = domfac.newDocumentBuilder();
            //if (filein != null)
            //InputStream in = filein;
            Document doc = domBuilder.parse(in);
            Element root = doc.getDocumentElement();
            NodeList nodelist_root = root.getChildNodes();
            /*
            if (node_child != null) {
                for (int i = 0; i < node_root.getLength(); i++) {
                    Node node_child = node_root.item(i);
                        if (node_child.getNodeName().equals("command")) {
                        command = node_child.getAttributes().getNamedItem("name").getNodeValue();
                        force = node_child.getAttributes().getNamedItem("force").getNodeValue();
                        if (Utils.DEBUG){
                        	 Log.i(TAG, "get xml command:" + command);
                        	 Log.i(TAG, "get xml force:" + force);
                        }  
                        if(!command.equals(COMMAND)){ 
                        	mErrorCode = ERROR_UNDISCOVERY_NEW_VERSION;
                            return;
                        }
                        */
                        //NodeList nodelist_2 = node_child.getChildNodes();
                        if (nodelist_root != null) {
                            for (int i = 0; i < nodelist_root.getLength(); i++) {
                                Node node_child = nodelist_root.item(i);
                                if (node_child.getNodeName().equals("status")) {
                                    status = node_child.getFirstChild().getNodeValue();
                                    if (Utils.DEBUG)
                                        Log.i(TAG, "get status:" + status);
                                    if(!status.equals(STATUS)){ 
                                    	mErrorCode = ERROR_UNDISCOVERY_NEW_VERSION;
                                        return;
                                    }
                                }
                                if (node_child.getNodeName().equals("url")) {
                                    zipUrl = node_child.getFirstChild().getNodeValue();
                                    if (Utils.DEBUG)
                                        Log.i(TAG, "get xml zipUrl:" + zipUrl);
                                }
                                if (node_child.getNodeName().equals("md5")) {
                                    md5 = node_child.getFirstChild().getNodeValue();
                                    if (Utils.DEBUG)
                                        Log.i(TAG, "get xml md5:" + md5);
                                }
                                if (node_child.getNodeName().equals("description")) {
                                    country = node_child.getAttributes().getNamedItem("country")
                                            .getNodeValue();
                                    if (description == null) {
                                        if (country.equals(mUpdaterInfo.country)
                                                || country.equals("ELSE"))
                                            description = node_child.getFirstChild().getNodeValue();
                                    }
                                }
                            }
                            if (Utils.DEBUG) {
                                Log.i(TAG, "get xml description:" + description);
                            }
            }
        } catch (Exception e) {
            e.printStackTrace();
            mErrorCode = ERROR_UNDISCOVERY_NEW_VERSION;
            return;
        }
        String[] checkInfo = new String[4];
        checkInfo[0] = md5;
        checkInfo[1] = zipUrl;
        checkInfo[2] = description;
        //checkInfo[3] = force;
        mResult = checkInfo;
    }

    protected void onStop() {
    	Log.v(TAG,"ErrorCode="+mErrorCode);
        if (mErrorCode == NO_ERROR) {
            NotificationManager notificationManager;
            Notification notification;
            Log.v(TAG, "Discover new version");
            mPreferences.setDownloadTarget(UpdateService.DOWNLOAD_OTA_PATH);
            String[] result = (String[]) mResult;
            //download url
            mPreferences.setDownloadURL(result[1]);
            mPreferences.setMd5(result[0]);
            Intent intent= new Intent(mContext,DownloadActivity.class);
            intent.setAction(DownloadActivity.ACTION_DOWNLOAD);
            notificationManager = (NotificationManager) mContext
                    .getSystemService(Context.NOTIFICATION_SERVICE);
            notification = new Notification(R.drawable.ic_title,
                    mContext.getString(R.string.check_succeed), System.currentTimeMillis());
            notification.flags |= Notification.FLAG_AUTO_CANCEL;
            PendingIntent pi = PendingIntent.getActivity(mContext, 0, intent, 0);
            notification.setLatestEventInfo(mContext, "Update",
                    mContext.getString(R.string.check_succeed), pi);
            notificationManager.notify(R.drawable.ic_title, notification);
            mPreferences.setPackageDescriptor(result[2]);
        }
    }
}
