package com.sprindy.handdraw;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.bluetooth.BluetoothGatt;
import android.bluetooth.BluetoothGattCallback;
import android.bluetooth.BluetoothGattCharacteristic;
import android.bluetooth.BluetoothGattDescriptor;
import android.bluetooth.BluetoothGattService;
import android.bluetooth.BluetoothProfile;
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
import java.util.UUID;

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
    private BluetoothGatt bluetoothGatt;
    private BluetoothGattCharacteristic mUuidCharacteristic;
    private BluetoothGattCharacteristic mMajorMinorCharacteristic;
    private BluetoothGattCharacteristic mRssiCharacteristic;
    private BluetoothGattCharacteristic mManufacturerIdCharacteristic;
    private BluetoothGattCharacteristic mAdvIntervalCharacteristic;
    private BluetoothGattCharacteristic mLedSettingsCharacteristic;

    private int mConnectionState;
    public final static int STATE_DISCONNECTED = 0;
    public final static int STATE_CONNECTING = 1;
    public final static int STATE_DISCOVERING_SERVICES = 2;
    public final static int STATE_CONNECTED = 3;
    public final static int STATE_DISCONNECTING = 4;

    public final static int SERVICE_UUID = 1;
    public final static int SERVICE_MAJOR_MINOR = 2;
    public final static int SERVICE_CALIBRATION = 3;

    public static final UUID CONFIG_SERVICE_UUID = new UUID(0x955A15230FE2F5AAl, 0xA09484B8D4F3E8ADl);
    //#define BLE_GAP_AD_TYPE_SERVICE_DATA_128BIT_UUID            0x21 //< Service Data - 128-bit UUID.
    private static final UUID CONFIG_UUID_CHARACTERISTIC_UUID = new UUID(0x955A15240FE2F5AAl, 0xA09484B8D4F3E8ADl);
    //#define BLE_GAP_AD_TYPE_TX_POWER_LEVEL                      0x0A //< Transmit power level.
    private static final UUID CONFIG_RSSI_CHARACTERISTIC_UUID = new UUID(0x955A15250FE2F5AAl, 0xA09484B8D4F3E8ADl);
    private static final UUID CONFIG_MAJOR_MINOR_CHARACTERISTIC_UUID = new UUID(0x955A15260FE2F5AAl, 0xA09484B8D4F3E8ADl);
    private static final UUID CONFIG_MANUFACTURER_ID_CHARACTERISTIC_UUID = new UUID(0x955A15270FE2F5AAl, 0xA09484B8D4F3E8ADl);
    //#define BLE_GAP_AD_TYPE_ADVERTISING_INTERVAL                0x1A //< Advertising Interval.
    private static final UUID CONFIG_ADV_INTERVAL_CHARACTERISTIC_UUID = new UUID(0x955A15280FE2F5AAl, 0xA09484B8D4F3E8ADl);
    private static final UUID CONFIG_LED_SETTINGS_CHARACTERISTIC_UUID = new UUID(0x955A15290FE2F5AAl, 0xA09484B8D4F3E8ADl);

    public static final UUID TX_POWER_UUID = UUID.fromString("00001804-0000-1000-8000-00805f9b34fb");
    public static final UUID TX_POWER_LEVEL_UUID = UUID.fromString("00002a07-0000-1000-8000-00805f9b34fb");
    public static final UUID CCCD = UUID.fromString("00002902-0000-1000-8000-00805f9b34fb");
    public static final UUID FIRMWARE_REVISON_UUID = UUID.fromString("00002a26-0000-1000-8000-00805f9b34fb");
    public static final UUID DIS_UUID = UUID.fromString("0000180a-0000-1000-8000-00805f9b34fb");
    public static final UUID RX_SERVICE_UUID = UUID.fromString("6e400001-b5a3-f393-e0a9-e50e24dcca9e");
    public static final UUID RX_CHAR_UUID = UUID.fromString("6e400002-b5a3-f393-e0a9-e50e24dcca9e");
    public static final UUID TX_CHAR_UUID = UUID.fromString("6e400003-b5a3-f393-e0a9-e50e24dcca9e");


    // Stops scanning after 10 seconds.
    private static final long SCAN_PERIOD = 10000;
    private int[] fontArray ;

    public int[] getFontArray() {
        return fontArray;
    }

    public void setFontArray(int[] fontArray) {
        this.fontArray = fontArray;
    }

    public BleController(){
        mBleAdapter = BluetoothAdapter.getDefaultAdapter();
    }


//    public void bleListInit(final Activity activity){
//        // the sample offer by AndroidDevelop: "android.R.layout.simple_list_item_1"
//        // do not change to "R.id.list_ble", or will be kill process when List.add
//        bleListAdapter = new BleAdapter(activity,
//                R.layout.ble_item, bleListData);
//        ble_listView = (ListView)activity.findViewById(R.id.list_ble);
//        ble_listView.setAdapter(bleListAdapter);
//
//        ble_listView.setOnItemClickListener(new AdapterView.OnItemClickListener() {
//            @Override
//            public void onItemClick(AdapterView<?> parent, View view, int position, long id) {
//                Ble ble = bleListData.get(position);
////                Toast.makeText(activity.this, ble.getDeviceName() + " " + ble.getDeviceHwAddress(),
////                        Toast.LENGTH_SHORT).show();
//                Log.d(TAG, "setOnItemClickListener: " + ble.getDeviceName() + " "
//                        + ble.getDeviceHwAddress());
//
//                //Bluetooth hardware addresses must be upper case
//                BluetoothDevice bluetoothDevice = mBleAdapter.getRemoteDevice(ble.getDeviceHwAddress());
//
//                bluetoothGatt = bluetoothDevice.connectGatt(activity, false,
//                        bluetoothGattCallback);
//                if (bluetoothGatt == null){
//                    Toast.makeText(activity,  "bluetoothDevice.connectGatt fail",
//                        Toast.LENGTH_SHORT).show();
//                }
//            }
//        });
//    }

    public BluetoothGattCallback bluetoothGattCallback = new BluetoothGattCallback(){
        @Override
        public void onConnectionStateChange(BluetoothGatt gatt, int status, int newState) {
            super.onConnectionStateChange(gatt, status, newState);

            // press SW2 set beacon to config mode can be connected:
            // newState=2, meanings connected.
            if (newState == BluetoothProfile.STATE_CONNECTED) {
                Log.d(TAG, "BluetoothGattCallback onConnectionStateChange() New State: CONNECTED");
                // Attempts to discover services after successful connection.
                gatt.discoverServices();
            } else if (newState == BluetoothProfile.STATE_DISCONNECTED) {
                gatt.close();
            }
        }

        @Override
        public void onServicesDiscovered(BluetoothGatt gatt, int status) {
            super.onServicesDiscovered(gatt, status);

            Log.d(TAG, "BluetoothGattCallback onServicesDiscovered " + status);
            // Search for config service
            final BluetoothGattService configService = gatt.getService(CONFIG_SERVICE_UUID);
            if (configService == null) {
                // Config service is not present
                gatt.disconnect();
                return;
            }

            mUuidCharacteristic = configService.getCharacteristic(CONFIG_UUID_CHARACTERISTIC_UUID);
            mMajorMinorCharacteristic = configService.getCharacteristic(CONFIG_MAJOR_MINOR_CHARACTERISTIC_UUID);
            mRssiCharacteristic = configService.getCharacteristic(CONFIG_RSSI_CHARACTERISTIC_UUID);
            mManufacturerIdCharacteristic = configService.getCharacteristic(CONFIG_MANUFACTURER_ID_CHARACTERISTIC_UUID);
            mAdvIntervalCharacteristic = configService.getCharacteristic(CONFIG_ADV_INTERVAL_CHARACTERISTIC_UUID);
            mLedSettingsCharacteristic = configService.getCharacteristic(CONFIG_LED_SETTINGS_CHARACTERISTIC_UUID);
        }

        @Override
        public void onCharacteristicRead(BluetoothGatt gatt, BluetoothGattCharacteristic
                characteristic, int status) {
            super.onCharacteristicRead(gatt, characteristic, status);

            if (status != BluetoothGatt.GATT_SUCCESS) {
                Log.w(TAG, "Characteristic read error: " + status);
//                broadcastError(status);
                return;
            }

            if (CONFIG_UUID_CHARACTERISTIC_UUID.equals(characteristic.getUuid())) {
                final UUID uuid = decodeBeaconUUID(characteristic);
//                broadcastUuid(uuid);
                Log.d(TAG, "BluetoothGattCallback " + "uuid: " + uuid);
                if (mMajorMinorCharacteristic != null && mMajorMinorCharacteristic.getValue() == null)
                    gatt.readCharacteristic(mMajorMinorCharacteristic);
            } else if (CONFIG_MAJOR_MINOR_CHARACTERISTIC_UUID.equals(characteristic.getUuid())) {
                final int major = decodeUInt16(characteristic, 0);
                final int minor = decodeUInt16(characteristic, 2);
//                broadcastMajorAndMinor(major, minor);
                Log.d(TAG, "BluetoothGattCallback " + "major: " + major);
                Log.d(TAG, "BluetoothGattCallback " + "minor: " + minor);
                if (mRssiCharacteristic != null && mRssiCharacteristic.getValue() == null)
                    gatt.readCharacteristic(mRssiCharacteristic);
            } else if (CONFIG_RSSI_CHARACTERISTIC_UUID.equals(characteristic.getUuid())) {
                final int rssi = characteristic.getIntValue(BluetoothGattCharacteristic.FORMAT_SINT8, 0);
//                broadcastRssi(rssi);
                Log.d(TAG, "BluetoothGattCallback " + "rssi: " + rssi);
                if (mManufacturerIdCharacteristic != null && mManufacturerIdCharacteristic.getValue() == null)
                    gatt.readCharacteristic(mManufacturerIdCharacteristic);
            } else if (CONFIG_MANUFACTURER_ID_CHARACTERISTIC_UUID.equals(characteristic.getUuid())) {
                final int id = characteristic.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT16, 0);
//                broadcastManufacturerId(id);
                Log.d(TAG, "BluetoothGattCallback " + "id: " + id);
                if (mAdvIntervalCharacteristic != null && mAdvIntervalCharacteristic.getValue() == null)
                    gatt.readCharacteristic(mAdvIntervalCharacteristic);
            } else if (CONFIG_ADV_INTERVAL_CHARACTERISTIC_UUID.equals(characteristic.getUuid())) {
                final int interval = characteristic.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT16, 0);
//                broadcastAdvInterval(interval);
                Log.d(TAG, "BluetoothGattCallback " + "interval: " + interval);
                if (mLedSettingsCharacteristic != null && mLedSettingsCharacteristic.getValue() == null)
                    gatt.readCharacteristic(mLedSettingsCharacteristic);
            } else if (CONFIG_LED_SETTINGS_CHARACTERISTIC_UUID.equals(characteristic.getUuid())) {
                final boolean on = characteristic.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT8, 0) == 1;
//                broadcastLedStatus(on);
                Log.d(TAG, "BluetoothGattCallback " + "led status: " + on);
            }
        }

        @Override
        public void onCharacteristicWrite(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic, int status) {
            super.onCharacteristicWrite(gatt, characteristic, status);

            if (CONFIG_UUID_CHARACTERISTIC_UUID.equals(characteristic.getUuid())) {
                final UUID uuid = decodeBeaconUUID(characteristic);
//                broadcastUuid(uuid);
            } else if (CONFIG_MAJOR_MINOR_CHARACTERISTIC_UUID.equals(characteristic.getUuid())) {
                final int major = decodeUInt16(characteristic, 0);
                final int minor = decodeUInt16(characteristic, 2);
//                broadcastMajorAndMinor(major, minor);
            } else if (CONFIG_RSSI_CHARACTERISTIC_UUID.equals(characteristic.getUuid())) {
                final int rssi = characteristic.getIntValue(BluetoothGattCharacteristic.FORMAT_SINT8, 0);
//                broadcastRssi(rssi);
            } else if (CONFIG_MANUFACTURER_ID_CHARACTERISTIC_UUID.equals(characteristic.getUuid())) {
                final int id = characteristic.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT16, 0);
//                broadcastManufacturerId(id);
            } else if (CONFIG_ADV_INTERVAL_CHARACTERISTIC_UUID.equals(characteristic.getUuid())) {
                final int interval = characteristic.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT16, 0);
//                broadcastAdvInterval(interval);
            } else if (CONFIG_LED_SETTINGS_CHARACTERISTIC_UUID.equals(characteristic.getUuid())) {
                final boolean on = characteristic.getIntValue(BluetoothGattCharacteristic.FORMAT_UINT8, 0) == 1;
//                broadcastLedStatus(on);
            }
        }

        @Override
        public void onCharacteristicChanged(BluetoothGatt gatt, BluetoothGattCharacteristic characteristic) {
            super.onCharacteristicChanged(gatt, characteristic);
        }

        @Override
        public void onDescriptorRead(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status) {
            super.onDescriptorRead(gatt, descriptor, status);

            Log.d(TAG, "BluetoothGattCallback onDescriptorRead " + "characteristic.getUuid()");
        }

        @Override
        public void onDescriptorWrite(BluetoothGatt gatt, BluetoothGattDescriptor descriptor, int status) {
            super.onDescriptorWrite(gatt, descriptor, status);
        }

        @Override
        public void onReliableWriteCompleted(BluetoothGatt gatt, int status) {
            super.onReliableWriteCompleted(gatt, status);
        }

        @Override
        public void onReadRemoteRssi(BluetoothGatt gatt, int rssi, int status) {
            super.onReadRemoteRssi(gatt, rssi, status);
        }

        @Override
        public void onMtuChanged(BluetoothGatt gatt, int mtu, int status) {
            super.onMtuChanged(gatt, mtu, status);
        }
    };

    public static int decodeUInt16(final BluetoothGattCharacteristic characteristic, final int offset) {
        final byte[] data = characteristic.getValue();
        return (unsignedByteToInt(data[offset]) << 8) | unsignedByteToInt(data[offset + 1]);
    }

    public static UUID decodeBeaconUUID(final BluetoothGattCharacteristic characteristic) {
        final byte[] data = characteristic.getValue();
        final long mostSigBits = (unsignedByteToLong(data[0]) << 56) + (unsignedByteToLong(data[1]) << 48) + (unsignedByteToLong(data[2]) << 40) + (unsignedByteToLong(data[3]) << 32)
                + (unsignedByteToLong(data[4]) << 24) + (unsignedByteToLong(data[5]) << 16) + (unsignedByteToLong(data[6]) << 8) + unsignedByteToLong(data[7]);
        final long leastSigBits = (unsignedByteToLong(data[8]) << 56) + (unsignedByteToLong(data[9]) << 48) + (unsignedByteToLong(data[10]) << 40) + (unsignedByteToLong(data[11]) << 32)
                + (unsignedByteToLong(data[12]) << 24) + (unsignedByteToLong(data[13]) << 16) + (unsignedByteToLong(data[14]) << 8) + unsignedByteToLong(data[15]);
        return new UUID(mostSigBits, leastSigBits);
    }

    /**
     * Convert a signed byte to an unsigned long.
     */
    public static long unsignedByteToLong(byte b) {
        return b & 0xFF;
    }

    /**
     * Convert a signed byte to an unsigned int.
     */
    public static int unsignedByteToInt(int b) {
        return b & 0xFF;
    }

    /**
     * Reads all the values from the device, one by one.
     *
     * @return <code>true</code> if at least one required characteristic has been found on the beacon.
     */
    public boolean read() {
        if (bluetoothGatt == null)
            return false;

        if (mUuidCharacteristic != null) {
            bluetoothGatt.readCharacteristic(mUuidCharacteristic);
            return true;
        } else if (mMajorMinorCharacteristic != null) {
            bluetoothGatt.readCharacteristic(mMajorMinorCharacteristic);
            return true;
        } else if (mRssiCharacteristic != null) {
            bluetoothGatt.readCharacteristic(mRssiCharacteristic);
            return true;
        } else if (mLedSettingsCharacteristic != null) {
            bluetoothGatt.readCharacteristic(mLedSettingsCharacteristic);
            return true;
        }
        return false;
    }

    // the device will reboot when receive too fast
    public void delay() {
        for (int j = 0; j < 0xffff; j++) {
            for (int k = 0; k < 2000; k++) {
                ;
            }
        }
    }

    public boolean writeDisplayData(int[] data) {
        if (bluetoothGatt == null)
            return false;
        if (data.length == 0) {
            Log.d(TAG, "writeDisplayData no data");
            return false;
        }
        if (data.length%16 !=0) {
            Log.d(TAG, "writeDisplayData length not 16 multiple: " + data.length);
            return false;
        }

        if (mLedSettingsCharacteristic != null) {
            final byte[] packageData = new byte[20];
            //TODO 0xff will regards as -1
            for (int i=0; i<data.length/16; i++) {
                packageData[0] = (byte) (data.length/16);
                packageData[1] = (byte) i;
                packageData[2] = (byte) 1; //led enable
                packageData[3] = (byte) 0; //reserved
                for (int j = 0; j < 16; j++) {
                    packageData[j+4] = (byte) data[i*16 + j];   //usefull display data
                }

                mLedSettingsCharacteristic.setValue(packageData);
                mLedSettingsCharacteristic.setWriteType(BluetoothGattCharacteristic.WRITE_TYPE_NO_RESPONSE);
                bluetoothGatt.writeCharacteristic(mLedSettingsCharacteristic);  //16 bytes of display data
                Log.d(TAG, "writeDisplayData " + i);
                delay();
            }
            return true;
        }
        return false;
    }

    public void disconnectedBle(Activity activity) {
        if (bluetoothGatt != null) {
            bluetoothGatt.disconnect();
        }
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

                            //Log.d(TAG, "mLeScanCallback():" + deviceName + " " + deviceHardwareAddress);
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
