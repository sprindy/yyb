package com.sprindy.handdraw;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.util.Log;
import android.widget.ArrayAdapter;
import android.widget.ListView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by sprindy on 4/25/17.
 */

public class BluetoothController {
        private BluetoothAdapter mBtAdapter;
        public String TAG = "sprindy_bt";
        private ListView bt_listView;
        private ArrayAdapter<String> btListAdapter;
        private List<String> btListData = new ArrayList<String>();

        public BluetoothController(){
            mBtAdapter = BluetoothAdapter.getDefaultAdapter();
        }

//        public void btListInit(Activity activity){
//            // the sample offer by AndroidDevelop: "android.R.layout.simple_list_item_1"
//            // do not change to "R.id.list_ble", or will be kill process when List.add
//            btListAdapter = new ArrayAdapter<String>(activity,
//                    android.R.layout.simple_list_item_1, btListData);
//            bt_listView = (ListView)activity.findViewById(R.id.list_ble);
//            bt_listView.setAdapter(btListAdapter);
//        }

        public void btRegister(Activity activity) {
            // Register for broadcasts when a device is discovered.
            IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
            activity.registerReceiver(mReceiver, filter);
        }

        // Create a BroadcastReceiver for ACTION_FOUND.
        private final BroadcastReceiver mReceiver = new BroadcastReceiver() {
            public void onReceive(Context context, Intent intent) {
                String action = intent.getAction();
                if (BluetoothDevice.ACTION_FOUND.equals(action)) {
                    // Discovery has found a device. Get the BluetoothDevice
                    // object and its info from the Intent.
                    BluetoothDevice device = intent.getParcelableExtra(BluetoothDevice.EXTRA_DEVICE);
                    String deviceName = device.getName();
                    String deviceHardwareAddress = device.getAddress(); // MAC address

                    Log.d(TAG, "onReceive():" + deviceName + " " + deviceHardwareAddress);
                    btListData.add(deviceName + "\n" + deviceHardwareAddress);
                    btListAdapter.notifyDataSetChanged();
                }
            }
        };

        /**
         * open BlueTooth
         * @param activity
         * @param requestCode
         */
        public boolean turnOnBluetooth(Activity activity, int requestCode){
            if(mBtAdapter!=null){
                if(!mBtAdapter.isEnabled()) {
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
            mBtAdapter.startDiscovery();
        }
}
