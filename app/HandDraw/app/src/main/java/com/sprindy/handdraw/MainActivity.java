package com.sprindy.handdraw;

import android.graphics.BlurMaskFilter;
import android.graphics.Color;
import android.graphics.EmbossMaskFilter;
import android.support.v7.app.AppCompatActivity;
import android.os.Bundle;
import android.view.Menu;
import android.view.MenuInflater;
import android.view.MenuItem;

public class MainActivity extends AppCompatActivity {

    /** Called when the activity is first created. */
    EmbossMaskFilter emboss;
    BlurMaskFilter blur;
    @Override
    public void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);
        setContentView(R.layout.activity_main);
        emboss = new EmbossMaskFilter(new float[]{1.5f,1.5f,1.5f},0.6f,6,4.2f);
        blur = new BlurMaskFilter(8,BlurMaskFilter.Blur.NORMAL);
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