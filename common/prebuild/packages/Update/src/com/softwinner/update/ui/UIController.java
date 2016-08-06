
package com.softwinner.update.ui;

import com.softwinner.shared.FileSelector;
import com.softwinner.update.DownloadActivity;
import com.softwinner.update.R;
import com.softwinner.update.UpdateActivity;
import com.softwinner.update.UpdateService;
import com.softwinner.update.UpdateService.CheckBinder;

import android.app.Activity;
import android.app.Dialog;
import android.app.Service;
import android.content.ComponentName;
import android.content.Context;
import android.content.Intent;
import android.content.ServiceConnection;
import android.content.res.AssetManager;
import android.net.ConnectivityManager;
import android.net.NetworkInfo;
import android.os.IBinder;
import android.util.Log;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.view.animation.AlphaAnimation;
import android.view.animation.Animation;
import android.view.animation.LayoutAnimationController;
import android.webkit.WebView;
import android.widget.Button;
import android.widget.Toast;

import java.io.File;
import java.io.InputStream;
import java.util.Locale;

public class UIController implements View.OnClickListener, View.OnKeyListener {

    private Context mContext;

    private View mRootView;

    private ViewGroup mContentView;

    private ViewGroup mMenuView;

    private ViewGroup mMenuMain;

    private ViewGroup mMenuLocal;

    private ViewGroup mMenuOnline;

    private View mCurrentMenuFieldView;

    private View mCurrentContentFieldView;

    private CheckBinder mServiceBinder;
    
    private static final String HELP_URL = "file:///android_asset/html/%y%z/Help.htm";

    public UIController(Context context) {
        mContext = context;
    }

    public UIController(Context context, View rootView) {
        this(context);
        setContentView(rootView);
    }

    public void setBinder(IBinder binder) {
        mServiceBinder = (CheckBinder) binder;
    }

    public void onCreate() {

    }

    public void onDestroy() {

    }

    public void onResume() {
        Button btn = (Button) mMenuOnline.findViewById(R.id.update_or_download);
        if (haveNewVersion()) {
            btn.setEnabled(true);
            btn.setText(R.string.menu_has_newversion);
        } else {
            btn.setEnabled(false);
            btn.setText(R.string.menu_has_not_newnewsion);
        }
    }

    private boolean haveNewVersion() {
        return true;
    }

    public void onPause() {
    }

    public void setContentView(View rootView) {
        mRootView = rootView;
        
        Locale locale = Locale.getDefault();
        //check for the full language + country resource, if not there, try just language
        String path = HELP_URL.replace("%y", locale.getLanguage().toLowerCase());
        path = path.replace("%z", '_'+locale.getCountry().toLowerCase());

        mContentView = (ViewGroup) rootView.findViewById(R.id.help_field);
        WebView wv = new WebView(mContext);
        wv.loadUrl(path);
        switchContentView(wv);

        mMenuView = (ViewGroup) rootView.findViewById(R.id.menu_field);
        mMenuMain = (ViewGroup) LayoutInflater.from(mContext).inflate(R.layout.menu_main, null);
        mMenuLocal = (ViewGroup) LayoutInflater.from(mContext).inflate(R.layout.menu_update_local,
                null);
        mMenuOnline = (ViewGroup) LayoutInflater.from(mContext).inflate(
                R.layout.menu_update_online, null);

        mMenuMain.findViewById(R.id.update_online).setOnClickListener(this);
        mMenuMain.findViewById(R.id.update_local).setOnClickListener(this);
        mMenuLocal.findViewById(R.id.update_by_mkimg).setOnClickListener(this);
        mMenuLocal.findViewById(R.id.update_by_ota).setOnClickListener(this);
        mMenuOnline.findViewById(R.id.update_check).setOnClickListener(this);
        mMenuOnline.findViewById(R.id.update_or_download).setOnClickListener(this);

        mMenuMain.findViewById(R.id.update_online).setOnKeyListener(this);
        mMenuMain.findViewById(R.id.update_local).setOnKeyListener(this);
        mMenuLocal.findViewById(R.id.update_by_mkimg).setOnKeyListener(this);
        mMenuLocal.findViewById(R.id.update_by_ota).setOnKeyListener(this);
        mMenuOnline.findViewById(R.id.update_check).setOnKeyListener(this);
        mMenuOnline.findViewById(R.id.update_or_download).setOnKeyListener(this);
        switchMenuView(mMenuMain);
        mMenuMain.requestFocus();
    }

    public void switchContentView(View v) {
        mContentView.removeAllViews();
        Animation animation = new AlphaAnimation(0.0f, 1.0f);
        animation.setDuration(400);
        LayoutAnimationController controller = new LayoutAnimationController(animation);
        mContentView.setLayoutAnimation(controller);
        mContentView.addView(v);
        mCurrentContentFieldView = v;
    }

    public void switchMenuView(View v) {
        mMenuView.removeAllViews();
        Animation animation = new AlphaAnimation(0.0f, 1.0f);
        animation.setDuration(400);
        LayoutAnimationController controller = new LayoutAnimationController(animation);
        mMenuView.setLayoutAnimation(controller);
        mMenuView.addView(v);
        mCurrentMenuFieldView = v;
    }

    public void switchContentView(int id) {
        View v = LayoutInflater.from(mContext).inflate(id, null);
        switchContentView(v);
    }

    public void switchMenuView(int id) {
        View v = LayoutInflater.from(mContext).inflate(id, null);
        switchMenuView(v);
    }

    public void init() {

    }

    @Override
    public boolean onKey(View v, int keyCode, KeyEvent event) {
        if (keyCode == KeyEvent.KEYCODE_ENTER) {
            enterEvent(v, v.getId());
        } else if (keyCode == KeyEvent.KEYCODE_BACK) {
            return true;
        }
        return false;
    }

    @Override
    public void onClick(View v) {
        enterEvent(v, v.getId());
    }

    private void enterEvent(View view, int viewId) {
        switch (viewId) {
            case R.id.update_local:
                Intent intent0 = new Intent(mContext, FileSelector.class);
                Activity activity = (Activity) mContext;
                activity.startActivityForResult(intent0, 0);
                break;
            case R.id.update_online:
                if (!checkInternet()) {
                    Toast.makeText(mContext, mContext.getString(R.string.net_error), 2000).show();
                    return;
                }
                Intent intent1 = new Intent(DownloadActivity.ACTION_CHECK);
                mContext.startActivity(intent1);
                break;
            case R.id.update_by_mkimg:
                break;
            case R.id.update_by_ota:
                break;
            case R.id.update_check:
                Intent intent = new Intent(DownloadActivity.ACTION_CHECK);
                mContext.startActivity(intent);
                break;
            case R.id.update_or_download:
                break;
        }
    }

    private boolean checkInternet() {
        ConnectivityManager cm = (ConnectivityManager) mContext
                .getSystemService(Context.CONNECTIVITY_SERVICE);
        NetworkInfo info = cm.getActiveNetworkInfo();
        if (info != null && info.isConnected()) {
            return true;
        } else {
            Log.v("UIController", "It's can't connect the Internet!");
            return false;
        }
    }
}
