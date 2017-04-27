package com.sprindy.handdraw;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Intent;
import android.os.Handler;
import android.util.Log;
import android.view.View;
import android.widget.AdapterView;
import android.widget.ArrayAdapter;
import android.widget.ImageView;
import android.widget.ListView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;

/**
 * Created by sprindy on 4/26/17.
 */

public class BleController{
    public String TAG = "sprindy_ble";
    private BluetoothAdapter mBleAdapter;
    private boolean mScanning;
    private Handler mHandler;
    private ListView ble_listView;
    private BleAdapter bleListAdapter;
    private List<Ble> bleListData = new ArrayList<Ble>();

    // Stops scanning after 10 seconds.
    private static final long SCAN_PERIOD = 10000;

    public BleController(){
        mBleAdapter = BluetoothAdapter.getDefaultAdapter();
    }

    public void bleListInit(final Activity activity){
        // the sample offer by AndroidDevelop: "android.R.layout.simple_list_item_1"
        // do not change to "R.id.list_ble", or will be kill process when List.add
        bleListAdapter = new BleAdapter(activity,
                R.layout.ble_item, bleListData);
        ble_listView = (ListView)activity.findViewById(R.id.list_ble);
        ble_listView.setAdapter(bleListAdapter);

//        ble_listView.setOnClickListener(new AdapterView.OnItemClickListener() {
//            @Override
//            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
//                Ble ble = bleListData.get(position);
////                Toast.makeText(activity.this, ble.getDeviceName() + " " + ble.getDeviceHwAddress(),
////                        Toast.LENGTH_SHORT).show();
//                Log.d(TAG, ble.getDeviceName() + " " + ble.getDeviceHwAddress());
//            }
//        });
    }

    public void connectBle(Activity activity) {
        int position = 0;
        Ble ble = bleListData.get(position);
        Toast.makeText(activity, ble.getDeviceName() + " " + ble.getDeviceHwAddress(),
                        Toast.LENGTH_SHORT).show();
        Log.d(TAG, "connectBle(): " + ble.getDeviceName() + " " + ble.getDeviceHwAddress());
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
                            Ble ble = new Ble(deviceName, deviceHardwareAddress);
                            bleListData.add(ble);
                            //android.view.ViewRootImpl$CalledFromWrongThreadException: Only the
                            // original thread that created a view hierarchy can touch its views.
                            bleListAdapter.notifyDataSetChanged();
//                        }
//                    }).start();
                }
            };

}
