package com.pozhu.painterengine;

import android.Manifest;
import android.app.NativeActivity;
import android.content.pm.PackageManager;
import android.os.Build;
import android.os.Bundle;

public class PE_NativeActivity extends NativeActivity {
    private PermissionHelper mPermissionHelper;	//权限

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        //给与权限
        mPermissionHelper=new PermissionHelper(this);
        if (Build.VERSION.SDK_INT >= 23&&!mPermissionHelper.isAllRequestedPermissionGranted()) {
            mPermissionHelper.applyPermissions();
        }
    }

}
