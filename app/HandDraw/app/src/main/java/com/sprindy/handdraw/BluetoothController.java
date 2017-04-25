package com.sprindy.handdraw;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.content.Intent;
import android.util.Log;
import android.widget.Toast;

/**
 * Created by sprindy on 4/25/17.
 */

public class BluetoothController {
        private BluetoothAdapter mAdapter;
        public String TAG = "sprindy_ble";
        public BluetoothController(){
            mAdapter = BluetoothAdapter.getDefaultAdapter();
        }

        /**
         * 打开蓝牙
         * @param activity
         * @param requestCode
         */
        public boolean turnOnBluetooth(Activity activity, int requestCode){
            if(mAdapter!=null){
                if(!mAdapter.isEnabled()) {
                    Intent intent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                    activity.startActivityForResult(intent, requestCode);
                }
                else {
                    Toast.makeText(activity, "BLE opened", Toast.LENGTH_SHORT).show();
                }

                return true;
            }
            else {
                Toast.makeText(activity, "BLE not support", Toast.LENGTH_SHORT).show();
                return false;
            }
        }

        public void startScanBluetooth(Activity activity){
            Log.d(TAG, "startScanBluetooth()");
            mAdapter.startDiscovery();
        }
}
