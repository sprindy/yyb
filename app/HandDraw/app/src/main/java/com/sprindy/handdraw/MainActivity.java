package com.sprindy.handdraw;

import android.Manifest;
import android.app.Activity;
import android.app.AlertDialog;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.ComponentName;
import android.content.Context;
import android.content.DialogInterface;
import android.content.Intent;
import android.content.IntentFilter;
import android.content.ServiceConnection;
import android.content.pm.PackageManager;
import android.graphics.Bitmap;
import android.graphics.BlurMaskFilter;
import android.graphics.Color;
import android.graphics.EmbossMaskFilter;
import android.os.Build;
import android.os.Handler;
import android.os.IBinder;
import android.os.Message;
import android.support.v4.content.LocalBroadcastManager;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.DisplayMetrics;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.EditText;
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListAdapter;
import android.widget.ListView;
import android.widget.RadioGroup;
import android.widget.TextView;
import android.widget.Toast;

import java.io.UnsupportedEncodingException;
import java.text.DateFormat;
import java.util.ArrayList;
import java.util.Date;
import java.util.List;


public class MainActivity extends Activity {

    private static final int PERMISSION_REQUEST_COARSE_LOCATION = 1;
    private static final int REQUEST_SELECT_DEVICE = 1;
    private static final int REQUEST_ENABLE_BT = 2;
    private static final int UART_PROFILE_READY = 10;
    private static final int UART_PROFILE_CONNECTED = 20;
    private static final int UART_PROFILE_DISCONNECTED = 21;
    public String TAG = "sprindy_yyb";
//    private String SEND_DATA_INTENT = "com.sprindy.handdraw.sent_data_broadcast";
//    private IntentFilter intentFilter;
//    private LocalReceiver localReceiver;
//    private LocalBroadcastManager localBroadcastManager;


    private BleController mBleController = new BleController();
    private ImageView mIVSign;
    private TextView mTVSign;
    private Bitmap mSignBitmap;
    //use real data to replace
    public int[] wangDataArray = {
            0x00,0x00,0x00,0x00,0x01,0xff,0xFF,0xC0,0x0E,0x00,0x80,0x00,0x00,0x00,0x80,0x00,
            0x00,0x00,0x80,0x00,0x00,0x01,0x00,0x00,0x00,0xFF,0xFC,0x00,0x00,0x01,0x00,0x00,
            0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0xB7,0xC0,0x00,
            0x3E,0x00,0x1D,0xE0,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
            0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
    };

    /** Called when the activity is first created. */
    EmbossMaskFilter emboss;
    BlurMaskFilter blur;

    private PaintView mPaintView;
    private FrameLayout mFrameLayout;
    private Button mBtnOK, mBtnClear, mBtnCancel;
    private Button btn_ble_send, btn_ble_connect;
    private int mState = UART_PROFILE_DISCONNECTED;
    private UartService mService = null;
    private BluetoothDevice mDevice = null;
    private BluetoothAdapter mBtAdapter = null;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        mBtAdapter = BluetoothAdapter.getDefaultAdapter();
        emboss = new EmbossMaskFilter(new float[]{1.5f,1.5f,1.5f},0.6f,6,4.2f);
        blur = new BlurMaskFilter(8,BlurMaskFilter.Blur.NORMAL);

//        localBroadcastManager = LocalBroadcastManager.getInstance(this);

//        mIVSign = (ImageView) findViewById(R.id.iv_sign);
//        mTVSign = (TextView) findViewById(R.id.tv_sign);
//
//        mTVSign.setOnClickListener(new View.OnClickListener() {
//
//            @Override
//            public void onClick(View view) {
//                WritePadDialog mWritePadDialog = new WritePadDialog(
//                        MainActivity.this, new WriteDilogListener() {
//
//                    @Override
//                    public void onPaintDone(Object object) {
//                        mSignBitmap = (Bitmap) object;
//                        mIVSign.setImageBitmap(mSignBitmap);
//                        //mTVSign.setVisibility(View.GONE);
//                    }
//                });
//
//                mWritePadDialog.show();
//            }
//        });
        DisplayMetrics mDisplayMetrics = new DisplayMetrics();
        getWindow().getWindowManager().getDefaultDisplay().getMetrics(mDisplayMetrics);
        int screenWidth = mDisplayMetrics.widthPixels;
        int screenHeight = mDisplayMetrics.heightPixels;
        final float scale = this.getResources().getDisplayMetrics().density;

        mIVSign = (ImageView) findViewById(R.id.iv_sign);
        mFrameLayout = (FrameLayout) findViewById(R.id.tablet_view);

        LinearLayout.LayoutParams ff=new LinearLayout.LayoutParams(LinearLayout.LayoutParams.WRAP_CONTENT, screenWidth);
//        FrameLayout.LayoutParams frameLayoutParams =(FrameLayout.LayoutParams) mFrameLayout.getLayoutParams();
//        frameLayoutParams.height = screenWidth;
        mFrameLayout.setLayoutParams(ff);

        mPaintView = new PaintView(MainActivity.this, screenWidth, screenWidth);
        if(MainActivity.this instanceof MainActivity){
            MainActivity mainActivity = (MainActivity) MainActivity.this;
//            mPaintView.setBleController(mainActivity.getmBleController());
        }
        mFrameLayout.addView(mPaintView);
        mPaintView.requestFocus();

        mBtnOK = (Button) findViewById(R.id.write_pad_ok);
        mBtnOK.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                if (mPaintView.getPath().isEmpty()) {
                    Toast.makeText(MainActivity.this, "请写下你的大名", Toast.LENGTH_SHORT).show();
                    return;
                }
                mIVSign.setImageBitmap(mPaintView.getPaintBitmap());

            }
        });

        mBtnClear = (Button) findViewById(R.id.write_pad_clear);
        mBtnClear.setOnClickListener(new View.OnClickListener() {

            @Override
            public void onClick(View v) {
                mPaintView.clear();
            }
        });

        service_init();

        btn_ble_connect = (Button)findViewById(R.id.btn_ble_connect);
        // Handle Disconnect & Connect button
        btn_ble_connect.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
                if (!mBtAdapter.isEnabled()) {
                    Log.i(TAG, "onClick - BT not enabled yet");
                    Intent enableIntent = new Intent(BluetoothAdapter.ACTION_REQUEST_ENABLE);
                    startActivityForResult(enableIntent, REQUEST_ENABLE_BT);
                }
                else {
                    if (btn_ble_connect.getText().equals("Connect")){

                        //Connect button pressed, open DeviceListActivity class, with popup windows that scan for devices

                        Intent newIntent = new Intent(MainActivity.this, DeviceListActivity.class);
                        startActivityForResult(newIntent, REQUEST_SELECT_DEVICE);
                    } else {
                        //Disconnect button pressed
                        if (mDevice!=null)
                        {
                            mService.disconnect();
                        }
                    }
                }
            }
        });

        // Handle Send button
        btn_ble_send = (Button)findViewById(R.id.btn_ble_send);
        btn_ble_send.setOnClickListener(new View.OnClickListener() {
            @Override
            public void onClick(View v) {
//                Intent intent = new Intent(SEND_DATA_INTENT);
//                localBroadcastManager.sendBroadcast(intent);

                int[] dataArray = mPaintView.getFontArray();
                if (mBtAdapter == null)
                    return;
                if (dataArray.length == 0) {
                    Log.e(TAG, "writeDisplayData no data");
                    return;
                }
                if (dataArray.length%16 !=0) {
                    Log.e(TAG, "writeDisplayData length not 16 multiple: " + dataArray.length);
                    return;
                }
                SendBleDataThread(dataArray);

            }
        });

//        intentFilter = new IntentFilter();
//        intentFilter.addAction(SEND_DATA_INTENT);
//        localReceiver = new LocalReceiver();
//        localBroadcastManager.registerReceiver(localReceiver, intentFilter);

        if (Build.VERSION.SDK_INT >= Build.VERSION_CODES.M) {
            // Android M Permission check
            if (this.checkSelfPermission(Manifest.permission.ACCESS_COARSE_LOCATION) != PackageManager.PERMISSION_GRANTED) {
                requestPermissions(new String[]{Manifest.permission.ACCESS_COARSE_LOCATION}, PERMISSION_REQUEST_COARSE_LOCATION);
            }
        }
    }

    /*
    Create a new thread to send ble data
     */
    private void SendBleDataThread(final int[] data) {
        new Thread(new Runnable() {
            @Override
            public void run() {

                final byte[] packageData = new byte[20];
                //TODO 0xff will regards as -1
                for (int i = 0; i < data.length / 16; i++) {
                    packageData[0] = (byte) (data.length / 16);
                    packageData[1] = (byte) i;
                    packageData[2] = (byte) 1; //led enable
                    packageData[3] = (byte) 0; //reserved
                    for (int j = 0; j < 16; j++) {
                        packageData[j + 4] = (byte) data[i * 16 + j];   //usefull display data
                    }

                    mService.writeRXCharacteristic(packageData);

                    //wait for YYB receive ok
                    try {
                        Thread.sleep(10);
                    } catch (InterruptedException e) {
                        e.printStackTrace();
                    }
                }

            }
        }).start();
    }

//    class LocalReceiver extends BroadcastReceiver {
//
//        @Override
//        public void onReceive(Context context, Intent intent) {
//
////            writeDisplayData(mPaintView.getFontArray());
//
//            Toast.makeText(context, "sprindy received local broadcast", Toast.LENGTH_SHORT).show();
//
//        }
//
//    }
    @Override
    public void onRequestPermissionsResult(int requestCode, String permissions[], int[] grantResults) {
        switch (requestCode) {
            case PERMISSION_REQUEST_COARSE_LOCATION:
                if (grantResults[0] == PackageManager.PERMISSION_GRANTED) {
                    // TODO request success
                }
                break;
        }
    }

    //UART service connected/disconnected
    private ServiceConnection mServiceConnection = new ServiceConnection() {
        public void onServiceConnected(ComponentName className, IBinder rawBinder) {
            mService = ((UartService.LocalBinder) rawBinder).getService();
            Log.d(TAG, "onServiceConnected mService= " + mService);
            if (!mService.initialize()) {
                Log.e(TAG, "Unable to initialize Bluetooth");
                finish();
            }

        }

        public void onServiceDisconnected(ComponentName classname) {
            ////     mService.disconnect(mDevice);
            mService = null;
        }
    };

    private Handler mHandler = new Handler() {
        @Override

        //Handler events that received from UART service
        public void handleMessage(Message msg) {

        }
    };

    private final BroadcastReceiver UARTStatusChangeReceiver = new BroadcastReceiver() {

        public void onReceive(Context context, Intent intent) {
            String action = intent.getAction();

            final Intent mIntent = intent;
            //*********************//
            if (action.equals(UartService.ACTION_GATT_CONNECTED)) {
                runOnUiThread(new Runnable() {
                    public void run() {
                        String currentDateTimeString = DateFormat.getTimeInstance().format(new Date());
                        Log.d(TAG, "UART_CONNECT_MSG");
                        btn_ble_connect.setText("Disconnect");
//                        edtMessage.setEnabled(true);
                        btn_ble_send.setEnabled(true);
//                        ((TextView) findViewById(R.id.deviceName)).setText(mDevice.getName()+ " - ready");
//                        listAdapter.add("["+currentDateTimeString+"] Connected to: "+ mDevice.getName());
//                        messageListView.smoothScrollToPosition(listAdapter.getCount() - 1);
                        mState = UART_PROFILE_CONNECTED;
                    }
                });
            }

            //*********************//
            if (action.equals(UartService.ACTION_GATT_DISCONNECTED)) {
                runOnUiThread(new Runnable() {
                    public void run() {
                        String currentDateTimeString = DateFormat.getTimeInstance().format(new Date());
                        Log.d(TAG, "UART_DISCONNECT_MSG");
                        btn_ble_connect.setText("Connect");
//                        edtMessage.setEnabled(false);
                        btn_ble_send.setEnabled(false);
//                        ((TextView) findViewById(R.id.deviceName)).setText("Not Connected");
//                        listAdapter.add("["+currentDateTimeString+"] Disconnected to: "+ mDevice.getName());
                        mState = UART_PROFILE_DISCONNECTED;
                        mService.close();
                        //setUiState();

                    }
                });
            }


            //*********************//
            if (action.equals(UartService.ACTION_GATT_SERVICES_DISCOVERED)) {
                mService.enableTXNotification();
            }
            //*********************//
            if (action.equals(UartService.ACTION_DATA_AVAILABLE)) {

                final byte[] txValue = intent.getByteArrayExtra(UartService.EXTRA_DATA);
                runOnUiThread(new Runnable() {
                    public void run() {
                        try {
                            String text = new String(txValue, "UTF-8");
                            String currentDateTimeString = DateFormat.getTimeInstance().format(new Date());
//                            listAdapter.add("["+currentDateTimeString+"] RX: "+text);
//                            messageListView.smoothScrollToPosition(listAdapter.getCount() - 1);

                        } catch (Exception e) {
                            Log.e(TAG, e.toString());
                        }
                    }
                });
            }
            //*********************//
            if (action.equals(UartService.DEVICE_DOES_NOT_SUPPORT_UART)){
                showMessage("Device doesn't support UART. Disconnecting");
                mService.disconnect();
            }


        }
    };

    private void service_init() {
        Intent bindIntent = new Intent(this, UartService.class);
        bindService(bindIntent, mServiceConnection, Context.BIND_AUTO_CREATE);

        LocalBroadcastManager.getInstance(this).registerReceiver(UARTStatusChangeReceiver, makeGattUpdateIntentFilter());
    }
    private static IntentFilter makeGattUpdateIntentFilter() {
        final IntentFilter intentFilter = new IntentFilter();
        intentFilter.addAction(UartService.ACTION_GATT_CONNECTED);
        intentFilter.addAction(UartService.ACTION_GATT_DISCONNECTED);
        intentFilter.addAction(UartService.ACTION_GATT_SERVICES_DISCOVERED);
        intentFilter.addAction(UartService.ACTION_DATA_AVAILABLE);
        intentFilter.addAction(UartService.DEVICE_DOES_NOT_SUPPORT_UART);
        return intentFilter;
    }


    public BleController getmBleController() {
        return mBleController;
    }

//    @Override
//    public void onClick(View v) {
//        switch (v.getId()){
//            case R.id.btn_ble_connect:
//                mBleController.read();
//                break;
//            case R.id.btn_ble_send:
//                //one word = 4*32, send one words need about 0.46s.
//                mBleController.writeDisplayData(mBleController.getFontArray());
//                break;
//            default:break;
//        }
//    }

    @Override
    public void onDestroy() {
        super.onDestroy();
        Log.d(TAG, "onDestroy()");

        try {
            LocalBroadcastManager.getInstance(this).unregisterReceiver(UARTStatusChangeReceiver);
        } catch (Exception ignore) {
            Log.e(TAG, ignore.toString());
        }
        unbindService(mServiceConnection);
        mService.stopSelf();
        mService= null;

    }

    //负责创建选项菜单
    @Override
    public boolean onCreateOptionsMenu(Menu menu)
    {
        MenuInflater inflator = new MenuInflater(this);
        //状态R.menu.context对应菜单,并添加到中
        inflator.inflate(R.menu.my_menu,menu);
        return super.onCreateOptionsMenu(menu);
    }
//    //菜单项被单击后的回调方法
//    @Override
//    public boolean onOptionsItemSelected(MenuItem mi)
//    {
//        DrawView dv = (DrawView)findViewById(R.id.my_draw);
//        //判断单击的是哪个菜单项,并有针对性地做出响应
//        switch(mi.getItemId())
//        {
//            case R.id.red:
//                dv.paint.setColor(Color.RED);
//                mi.setChecked(true);
//                break;
//            case R.id.green:
//                dv.paint.setColor(Color.GREEN);
//                mi.setChecked(true);
//                break;
//            case R.id.blue:
//                dv.paint.setColor(Color.BLUE);
//                mi.setChecked(true);
//                break;
//            case R.id.width_1:
//                dv.paint.setStrokeWidth(1);
//                mi.setChecked(true);
//                break;
//            case R.id.width_3:
//                dv.paint.setStrokeWidth(3);
//                mi.setChecked(true);
//                break;
//            case R.id.width_5:
//                dv.paint.setStrokeWidth(5);
//                mi.setChecked(true);
//                break;
//            case R.id.blur:
//                dv.paint.setMaskFilter(blur);
//                mi.setChecked(true);
//                break;
//            case R.id.emboss:
//                dv.paint.setMaskFilter(emboss);
//                mi.setChecked(true);
//                break;
//        }
//        return true;
//    }

    @Override
    public void onActivityResult(int requestCode, int resultCode, Intent data) {
        switch (requestCode) {

            case REQUEST_SELECT_DEVICE:
                //When the DeviceListActivity return, with the selected device address
                if (resultCode == Activity.RESULT_OK && data != null) {
                    String deviceAddress = data.getStringExtra(BluetoothDevice.EXTRA_DEVICE);
                    mDevice = BluetoothAdapter.getDefaultAdapter().getRemoteDevice(deviceAddress);

                    Log.d(TAG, "... onActivityResultdevice.address==" + mDevice + " mserviceValue " + mService);
//                    ((TextView) findViewById(R.id.deviceName)).setText(mDevice.getName()+ " - connecting");
                    mService.connect(deviceAddress);

                }
                break;
            case REQUEST_ENABLE_BT:
                // When the request to enable Bluetooth returns
                if (resultCode == Activity.RESULT_OK) {
                    Toast.makeText(this, "Bluetooth has turned on ", Toast.LENGTH_SHORT).show();

                } else {
                    // User did not enable Bluetooth or an error occurred
                    Log.d(TAG, "BT not enabled");
                    Toast.makeText(this, "Problem in BT Turning ON ", Toast.LENGTH_SHORT).show();
                    finish();
                }
                break;
            default:
                Log.e(TAG, "wrong request code");
                break;
        }
    }

//    @Override
//    public void onCheckedChanged(RadioGroup group, int checkedId) {
//
//    }


    private void showMessage(String msg) {
        Toast.makeText(this, msg, Toast.LENGTH_SHORT).show();

    }

    @Override
    public void onBackPressed() {
        if (mState == UART_PROFILE_CONNECTED) {
            Intent startMain = new Intent(Intent.ACTION_MAIN);
            startMain.addCategory(Intent.CATEGORY_HOME);
            startMain.setFlags(Intent.FLAG_ACTIVITY_NEW_TASK);
            startActivity(startMain);
            showMessage("nRFUART's running in background.\n             Disconnect to exit");
        }
        else {
            new AlertDialog.Builder(this)
                    .setIcon(android.R.drawable.ic_dialog_alert)
                    .setTitle(R.string.popup_title)
                    .setMessage(R.string.popup_message)
                    .setPositiveButton(R.string.popup_yes, new DialogInterface.OnClickListener()
                    {
                        @Override
                        public void onClick(DialogInterface dialog, int which) {
                            finish();
                        }
                    })
                    .setNegativeButton(R.string.popup_no, null)
                    .show();
        }
    }

}