
package com.softwinner.update;

import com.softwinner.shared.FileSelector;
import com.softwinner.update.ui.InstallPackage;
import com.softwinner.update.ui.UIController;
import com.softwinner.update.DownloadActivity;

import android.app.Activity;
import android.app.Dialog;
import android.app.Service;
import android.content.ComponentName;
import android.content.Intent;
import android.content.ServiceConnection;
import android.os.Bundle;
import android.os.IBinder;
import android.view.LayoutInflater;
import android.view.View;
import android.view.Window;

import java.io.File;

public class UpdateActivity extends Activity {
    /** Called when the activity is first created. */
    private String TAG = "UpdateActivity";

    private UIController mUIController;

    private ServiceConnection mConn = new ServiceConnection() {
        @Override
        public void onServiceConnected(ComponentName name, IBinder service) {
            mUIController.setBinder(service);
        }

        @Override
        public void onServiceDisconnected(ComponentName name) {

        }
    };

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.main);
        mUIController = new UIController(this, findViewById(R.id.root));
        Intent intent = new Intent(this, UpdateService.class);
        bindService(intent, mConn, Service.BIND_AUTO_CREATE);
        Intent intent1 = new Intent(DownloadActivity.ACTION_CHECK);
                 startActivity(intent1);
    }

    @Override
    protected void onActivityResult(int requestCode, int resultCode, Intent data) {
        super.onActivityResult(requestCode, resultCode, data);
        if(data != null){
	        Bundle bundle = data.getExtras();
	        String file = bundle.getString("file");
	        if (file != null) {
	            final Dialog dlg = new Dialog(this);
	            dlg.setTitle(R.string.confirm_update);
	            LayoutInflater inflater = LayoutInflater.from(this);
	            InstallPackage dlgView = (InstallPackage) inflater.inflate(R.layout.install_ota, null,
	                    false);
	            dlgView.setPackagePath(file);
	            dlg.setContentView(dlgView);
	            dlg.findViewById(R.id.confirm_cancel).setOnClickListener(new View.OnClickListener() {
	                @Override
	                public void onClick(View v) {
	                    dlg.dismiss();
	                }
	            });
                dlg.setCancelable(false);
	            dlg.show();
	        }
        }
    }

    @Override
    public void onResume() {
        super.onResume();
        mUIController.onResume();
    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        unbindService(mConn);
    }
}
