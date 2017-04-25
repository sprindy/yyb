package com.sprindy.handdraw;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.BroadcastReceiver;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.BlurMaskFilter;
import android.graphics.Color;
import android.graphics.EmbossMaskFilter;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.util.Log;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;
import android.view.View;
import android.widget.ArrayAdapter;
import android.widget.Button;
import android.widget.ListAdapter;
import android.widget.ListView;
import android.widget.SimpleAdapter;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;


public class MainActivity extends AppCompatActivity implements View.OnClickListener{

    public static final int REQUEST_ENABLE_BT = 1;
    public String TAG = "sprindy_yyb";
    private BluetoothController mController = new BluetoothController();
    private ListView ble_listView;
    private ArrayAdapter<String> bleListAdapter;
    private List<String> bleListData = new ArrayList<String>();

    /** Called when the activity is first created. */
    EmbossMaskFilter emboss;
    BlurMaskFilter blur;

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        emboss = new EmbossMaskFilter(new float[]{1.5f,1.5f,1.5f},0.6f,6,4.2f);
        blur = new BlurMaskFilter(8,BlurMaskFilter.Blur.NORMAL);

        // Register buttons on the same ClickListener.
        Button btn_ble_support = (Button)findViewById(R.id.btn_ble_enable);
        Button btn_ble_scan = (Button)findViewById(R.id.btn_ble_scan);
        btn_ble_support.setOnClickListener(this);
        btn_ble_scan.setOnClickListener(this);

        // the sample offer by AndroidDevelop: "android.R.layout.simple_list_item_1"
        // do not change to "R.id.list_ble", or will be kill process when List.add
        bleListAdapter = new ArrayAdapter<String>(MainActivity.this,
                android.R.layout.simple_list_item_1, bleListData);
        ble_listView = (ListView)findViewById(R.id.list_ble);
        ble_listView.setAdapter(bleListAdapter);

        // Register for broadcasts when a device is discovered.
        IntentFilter filter = new IntentFilter(BluetoothDevice.ACTION_FOUND);
        registerReceiver(mReceiver, filter);
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.btn_ble_enable:
                mController.turnOnBluetooth(this, REQUEST_ENABLE_BT);
                break;
            case R.id.btn_ble_scan:
                mController.startScanBluetooth(this);
                break;
        }

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
                bleListData.add(deviceName + "\n" + deviceHardwareAddress);
                bleListAdapter.notifyDataSetChanged();
            }
        }
    };

    @Override
    protected void onDestroy() {
        super.onDestroy();

        // Don't forget to unregister the ACTION_FOUND receiver.
        unregisterReceiver(mReceiver);
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
    //菜单项被单击后的回调方法
    @Override
    public boolean onOptionsItemSelected(MenuItem mi)
    {
        DrawView dv = (DrawView)findViewById(R.id.my_draw);
        //判断单击的是哪个菜单项,并有针对性地做出响应
        switch(mi.getItemId())
        {
            case R.id.red:
                dv.paint.setColor(Color.RED);
                mi.setChecked(true);
                break;
            case R.id.green:
                dv.paint.setColor(Color.GREEN);
                mi.setChecked(true);
                break;
            case R.id.blue:
                dv.paint.setColor(Color.BLUE);
                mi.setChecked(true);
                break;
            case R.id.width_1:
                dv.paint.setStrokeWidth(1);
                mi.setChecked(true);
                break;
            case R.id.width_3:
                dv.paint.setStrokeWidth(3);
                mi.setChecked(true);
                break;
            case R.id.width_5:
                dv.paint.setStrokeWidth(5);
                mi.setChecked(true);
                break;
            case R.id.blur:
                dv.paint.setMaskFilter(blur);
                mi.setChecked(true);
                break;
            case R.id.emboss:
                dv.paint.setMaskFilter(emboss);
                mi.setChecked(true);
                break;
        }
        return true;
    }
}