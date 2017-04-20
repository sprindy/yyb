//
//  ff_inc.h
//  Firefly
//
//  @brief public header for all platform
//
//  Created by AlickWang on 14/11/14.
//  Copyright (c) 2014年 tencent. All rights reserved.
//

#ifndef Firefly_ff_inc_h
#define Firefly_ff_inc_h

#include "ff_syshdr.h"

//if enable, it will log with printf(fmt,...)
#define FF_ENABLE_LOG 0

#ifdef __cplusplus
extern "C" {
#endif
    //return code of function
    typedef enum{
        FF_FUNC_RET_OK=0,
        FF_FUNC_RET_ERR=-1, //common error
        FF_FUNC_RET_TODO=-2, //the function was not finish ,need to implement
        FF_FUNC_RET_NOT_SAFE=-20, //safe channel not establed, could not send the data
        FF_FUNC_RET_NOT_LOGIN=-21, //need to login before set prop or get prop
        FF_FUNC_RET_RX_RUN=-22, //is receiving data ,could not send now
        FF_FUNC_RET_TX_RUN=-23, //is sending data ,could not receive now
        FF_FUNC_RET_DATA_UNMATCH=-30, //data is not wanted
        FF_FUNC_RET_DATA_NULL=-31, //data is null
        FF_FUNC_RET_DATA_LESS=-32, //data is less than wanted
        FF_FUNC_RET_DATA_MORE=-33, //data is more than wanted
        FF_FUNC_RET_DATA_UNKNOWN_TYPE=-34, //recieved data type is unknown
        FF_FUNC_RET_DEVICE_UNSUPPORT_BIND=-40,//the device not support tap to determin binding
    }ff_func_ret_e;
    
    //result code of bussiness or server internal error
    typedef enum{
        FF_RESULT_CODE_OK = 0X00,       //all right
        FF_RESULT_CODE_PROPERTY = 0X01, //bussiness property error, see error field for detail
        FF_RESULT_CODE_ACK = 0X02,      //acknowdge for receive the data from QQ
        FF_RESULT_CODE_FAIL = 0X10,     //common fail
        FF_RESULT_CODE_SERVER_TIMEOUT = 0X11,  //could not connect to QQ server
        FF_RESULT_CODE_APP_VERSION_LOW = 0X16, //QQ app version is lower ,not corresponse to bussiness
        FF_RESULT_CODE_ERR_TYPE = 0x20,        //received a message type unknown
        FF_RESULT_CODE_ERR_DATA = 0x21,        //received a err data, could not unpack
        FF_RESULT_CODE_ERR_BIND = 0x22,        //the hardware product could not response to bind
        FF_RESULT_CODE_ERR_TX_RUN = 0x23,      //the hardware product is sending data ,could not handle the incoming data
    }ff_result_code_e;

    
    typedef struct{
        uint16_t seq_no;
        uint16_t len;
        uint32_t prop_id;
        uint8_t* value;
    }ff_property_t;
    
    typedef struct{
        uint16_t seq_no;
        uint32_t prop_id;
        uint32_t err_code;
        uint8_t* msg;
        uint16_t len;
    }ff_property_error_t;
    
    
    typedef struct{
        uint8_t ret_code;
        ff_property_error_t error;
    }ff_result_t;
#ifdef __cplusplus
}
#endif
#endif
