package com.sprindy.handdraw;

/**
 * Created by sprindy on 4/27/17.
 */

public class Ble {
    private String deviceName;
    private String deviceHwAddress;

    public Ble(String deviceName, String deviceHwAddress) {
        this.deviceName = deviceName;
        this.deviceHwAddress = deviceHwAddress;
    }

    public String getDeviceName() {
        return deviceName;
    }

    public String getDeviceHwAddress() {
        return deviceHwAddress;
    }
}
