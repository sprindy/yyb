//
//  fd_engine.h
//  Firefly
//
// @brief public header for device
//
//  Created by AlickWang on 14/11/16.
//  Copyright (c) 2014年 tencent. All rights reserved.
//

#ifndef Firefly_fd_engine_h
#define Firefly_fd_engine_h

#include "ff_inc.h"

#ifdef __cplusplus
extern "C" {
#endif
    struct fd_engine_s;
    //SDK init. the param 'zone',is the memory alloced for SDK
    struct fd_engine_s* fd_init(uint8_t* zone,uint16_t size);
    //when Device received data from BLE rx charactic
    int8_t fd_received(struct fd_engine_s* engine, uint8_t* datum,uint16_t length);
    // SDK will reset internal buffer
    void fd_flush(struct fd_engine_s* engine);
    //when ble connect ,call this
    int8_t fd_connect(struct fd_engine_s* engine);
    //when ble disconnet, call this
    int8_t fd_disconnet(struct fd_engine_s* engine);
    
    //get the property value
    int8_t fd_get_property(struct fd_engine_s* engine, ff_property_t* prop);
    //if the value is little ,use fd_set_property
    //set the property, direct copy the value to transport stream
    int8_t fd_set_property(struct fd_engine_s* engine, ff_property_t* prop);
    //if the value is huge ,use fd_fill_property
    //set the property, use function callback ,to fill the value zone in transport stream
    int8_t fd_fill_property(struct fd_engine_s* engine, ff_property_t* prop,void(*fill)(uint8_t* zone,ff_property_t* prop));
    //post result
    int8_t fd_post_result(struct fd_engine_s* engine, ff_result_t* result);

    
    //this should implement by hardware bussiness
    int8_t fh_send(uint8_t* datum,uint16_t length);
    int8_t fh_disconnect(void);
    //if could not interface for human, just set it NULL
    //when User agree to bind,for example ,click twice;the call bind
    int8_t fh_on_wait_bind(int8_t bind(struct fd_engine_s* engine));
    void fh_on_binded(int8_t ret_code);
    void fh_on_login(int8_t ret_code, uint32_t timestamp);
    //app get property value from device
    int8_t fh_get_property(ff_property_t* prop);
    //app set property value to device
    int8_t fh_set_property(ff_property_t* prop);
    //app post result to device
    int8_t fh_post_result(ff_result_t* result);
    
    //data save and load
    //license is less than 80 bytes
    int16_t fh_get_license_length(void);
    void fh_load_license(uint8_t* zone);
    uint32_t fh_get_pid(void);
    //auth_key has 16 bytes
    void fh_load_auth_key(uint8_t* zone);
    //mac string has 12 chars, format as AABBCCDDEEFF, upper case
    void fh_load_mac(uint8_t* zone);
    //puk has 25 bytes
    void fh_load_puk(uint8_t* zone);

    //din always 8 bytes; if din exist ,copy to zone; or else, set first 8 bytes of zone to 0
    void fh_load_din(uint8_t* zone);
    //save the din(8 bytes) , when the device relauch, the token would be exist
    void fh_save_din(uint8_t* din);
#ifdef __cplusplus
}
#endif
#endif
