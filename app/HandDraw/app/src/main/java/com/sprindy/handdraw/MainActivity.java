package com.sprindy.handdraw;

import android.app.Activity;
import android.bluetooth.BluetoothAdapter;
import android.bluetooth.BluetoothDevice;
import android.content.Context;
import android.content.Intent;
import android.content.IntentFilter;
import android.graphics.Bitmap;
import android.graphics.BlurMaskFilter;
import android.graphics.Color;
import android.graphics.EmbossMaskFilter;
import android.os.Handler;
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
import android.widget.FrameLayout;
import android.widget.ImageView;
import android.widget.LinearLayout;
import android.widget.ListAdapter;
import android.widget.ListView;
import android.widget.TextView;
import android.widget.Toast;

import java.util.ArrayList;
import java.util.List;


public class MainActivity extends AppCompatActivity implements View.OnClickListener{

    public static final int REQUEST_ENABLE_BT = 1;
    public String TAG = "sprindy_yyb";
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

    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        emboss = new EmbossMaskFilter(new float[]{1.5f,1.5f,1.5f},0.6f,6,4.2f);
        blur = new BlurMaskFilter(8,BlurMaskFilter.Blur.NORMAL);

        // Register buttons on the same ClickListener.
        Button btn_ble_scan = (Button)findViewById(R.id.btn_ble_scan);
        Button btn_ble_read = (Button)findViewById(R.id.btn_ble_read);
        Button btn_ble_send = (Button)findViewById(R.id.btn_ble_send);
        btn_ble_scan.setOnClickListener(this);
        btn_ble_read.setOnClickListener(this);
        btn_ble_send.setOnClickListener(this);


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
            mPaintView.setBleController(mainActivity.getmBleController());
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

        mBleController.bleListInit(this);

    }

    public BleController getmBleController() {
        return mBleController;
    }

    @Override
    public void onClick(View v) {
        switch (v.getId()){
            case R.id.btn_ble_scan:
                mBleController.turnOnBluetooth(this, REQUEST_ENABLE_BT);
                mBleController.startScanBle(this, true);
                break;
            case R.id.btn_ble_read:
                mBleController.read();
                break;
            case R.id.btn_ble_send:
                //one word = 4*32, send one words need about 0.46s.
                mBleController.writeDisplayData(mBleController.getFontArray());
                break;
            default:break;
        }
    }

    @Override
    protected void onDestroy() {
        super.onDestroy();

        //stop scan ble.
        mBleController.startScanBle(this, false);
        mBleController.disconnectedBle(this);

        // Don't forget to unregister the ACTION_FOUND receiver.
        //call it when use bt not ble.
        //unregisterReceiver(mReceiver);
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
}