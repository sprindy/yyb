package com.sprindy.handdraw;

import android.content.Context;
import android.view.LayoutInflater;
import android.view.ViewGroup;
import android.widget.ArrayAdapter;
import android.widget.TextView;
import android.view.View;

import java.util.List;

/**
 * Created by sprindy on 4/27/17.
 */

public class BleAdapter extends ArrayAdapter<Ble>{
    private int resourceId;

    public BleAdapter(Context context, int textViewResourceId, List<Ble> objects) {
        super(context, textViewResourceId, objects);
        resourceId = textViewResourceId;
    }

    @Override
    public View getView(int position, View convertView, ViewGroup parent) {
        Ble ble = getItem(position); // 获取当前项的Ble实例
        View view;
        ViewHolder viewHolder;
        if (convertView == null) {
            view = LayoutInflater.from(getContext()).inflate(resourceId, parent, false);
            viewHolder = new ViewHolder();
            viewHolder.bleDeviceName = (TextView) view.findViewById (R.id.ble_device_name);
            viewHolder.bleHwAddress = (TextView) view.findViewById (R.id.ble_hw_address);
            view.setTag(viewHolder); // 将ViewHolder存储在View中
        } else {
            view = convertView;
            viewHolder = (ViewHolder) view.getTag(); // 重新获取ViewHolder
        }
        viewHolder.bleDeviceName.setText(ble.getDeviceName());
        viewHolder.bleHwAddress.setText(ble.getDeviceHwAddress());
        return view;
    }

    class ViewHolder {

        TextView bleDeviceName;

        TextView bleHwAddress;

    }
}
