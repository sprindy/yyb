package com.sprindy.handdraw;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.os.Handler;
import android.util.Log;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by sprindy on 4/26/17.
 */

public class BleController {
    public String TAG = "sprindy_ble";
    private BluetoothAdapter mBleAdapter;
    private boolean mScanning;
    private Handler mHandler;
    private ListView ble_listView;
    private ArrayAdapter<String> bleListAdapter;
    private List<String> bleListData = new ArrayList<String>();

    // Stops scanning after 10 seconds.
    private static final long SCAN_PERIOD = 10000;

    public BleController(){
        mBleAdapter = BluetoothAdapter.getDefaultAdapter();
    }

    public void bleListInit(Activity activity){
        // the sample offer by AndroidDevelop: "android.R.layout.simple_list_item_1"
        // do not change to "R.id.list_ble", or will be kill process when List.add
        bleListAdapter = new ArrayAdapter<String>(activity,
                android.R.layout.simple_list_item_1, bleListData);
        ble_listView = (ListView)activity.findViewById(R.id.list_ble);
        ble_listView.setAdapter(bleListAdapter);
    }

    /**
     * turn on BLE
     * @param activity
     * @param requestCode
     */
    public boolean turnOnBluetooth(Activity activity, int requestCode){
        if(mBleAdapter!=null){
            if(!mBleAdapter.isEnabled()) {
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

    public void startScanBle(Activity activity, boolean enable){
        Log.d(TAG, "startScanBle()");
        if (enable) {
            // Stops scanning after a pre-defined scan period.
//            mHandler.postDelayed(new Runnable() {
//                @Override
//                public void run() {
//                    mScanning = false;
//                    mBleAdapter.stopLeScan(mLeScanCallback);
//                }
//            }, SCAN_PERIOD);

            mScanning = true;
            mBleAdapter.startLeScan(mLeScanCallback);
        } else {
            mScanning = false;
            mBleAdapter.stopLeScan(mLeScanCallback);
        }
    }

    // Device scan callback.
    private BluetoothAdapter.LeScanCallback mLeScanCallback =
            new BluetoothAdapter.LeScanCallback() {
                @Override
                public void onLeScan(final BluetoothDevice device, int rssi,
                                     byte[] scanRecord) {
//                    runOnUiThread(new Runnable() {
//                        @Override
//                        public void run() {
                            String deviceName = device.getName();
                            String deviceHardwareAddress = device.getAddress(); // MAC address

                            Log.d(TAG, "mLeScanCallback():" + deviceName + " " + deviceHardwareAddress);
                            bleListData.add(deviceName + "\n" + deviceHardwareAddress);
                            //android.view.ViewRootImpl$CalledFromWrongThreadException: Only the
                            // original thread that created a view hierarchy can touch its views.
                            bleListAdapter.notifyDataSetChanged();
//                        }
//                    }).start();
                }
            };

}
