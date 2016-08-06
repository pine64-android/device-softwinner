
package com.softwinner.shared;

import com.softwinner.update.R;

import android.app.Activity;
import android.content.Context;
import android.content.Intent;
import android.os.Bundle;
import android.util.AttributeSet;
import android.view.KeyEvent;
import android.view.LayoutInflater;
import android.view.View;
import android.view.ViewGroup;
import android.widget.AdapterView;
import android.widget.AdapterView.OnItemClickListener;
import android.widget.BaseAdapter;
import android.widget.ListView;
import android.widget.TextView;

import java.io.File;

public class FileSelector extends Activity implements OnItemClickListener {

    public static final String FILE = "file";

    private File mCurrentDirectory;

    private LayoutInflater mInflater;

    private FileAdapter mAdapter = new FileAdapter();

    private ListView mListView;

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        mInflater = LayoutInflater.from(this);
        setContentView(R.layout.file_list);
        mListView = (ListView) findViewById(R.id.file_list);
        mListView.setAdapter(mAdapter);
        mAdapter.setCurrentList(new File("/sdcard"));
        mListView.setOnItemClickListener(this);
    }

    @Override
    public void onItemClick(AdapterView<?> adapterView, View view, int position, long id) {
        File selectFile = (File) adapterView.getItemAtPosition(position);
        if (selectFile.isDirectory()) {
            mCurrentDirectory = selectFile;
            FileAdapter adapter = (FileAdapter) adapterView.getAdapter();
            adapter.setCurrentList(selectFile);
        } else if (selectFile.isFile()) {
            Intent intent = new Intent();
            intent.putExtra(FILE, selectFile.getPath());
            setResult(0, intent);
            finish();
        }
    }

    @Override
    public void onBackPressed() {
        if (mCurrentDirectory == null || mCurrentDirectory.getPath().equals("/sdcard")) {
            super.onBackPressed();
        } else {
            mCurrentDirectory = mCurrentDirectory.getParentFile();
            mAdapter.setCurrentList(mCurrentDirectory);
        }
    }

//    @Override
//    public boolean onKeyDown(int keyCode, KeyEvent event) {
//        if (keyCode == KeyEvent.KEYCODE_BACK) {
//            if (mCurrentDirectory == null || mCurrentDirectory.getPath().equals("/sdcard")) {
//                return super.onKeyDown(keyCode, event);
//            } else {
//                mAdapter.setCurrentList(mCurrentDirectory.getParentFile());
//                return false;
//            }
//        }
//        return super.onKeyDown(keyCode, event)
//    }

    private class FileAdapter extends BaseAdapter {

        private File mFiles[];

        public void setCurrentList(File directory) {
            mFiles = directory.listFiles();
            notifyDataSetChanged();
        }

        @Override
        public int getCount() {
            return mFiles == null ? 0 : mFiles.length;
        }

        @Override
        public File getItem(int position) {
            File file = mFiles == null ? null : mFiles[position];
            return file;
        }

        @Override
        public long getItemId(int position) {
            return position;
        }

        @Override
        public View getView(int position, View convertView, ViewGroup parent) {
            if (convertView == null) {
                convertView = mInflater.inflate(R.layout.large_text, null);
            }
            TextView tv = (TextView) convertView;
            File file = mFiles[position];
            String name = file.getName();
            tv.setText(name);
            return tv;
        }

    }
}
