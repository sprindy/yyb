//
//  ff_utils.h
//  Firefly
//  一些工具类都在这里
//
//  Created by AlickWang on 14/10/31.
//  Copyright (c) 2014年 tencent. All rights reserved.
//  |       | ff | fd |  fa| fn |  fi|  fh |
//  |----   |----|----|----|----|----|-----|
//  |Device |Y   | Y  |    |    |    | Y   |
//  |Android|Y   |    | Y  |  Y |    |     |
//  | iOS   |Y   |    | Y  |    |  Y |     |

#ifndef __Firefly__ff_utils__
#define __Firefly__ff_utils__

#include "ff_inc.h"

#define SAFE_FREE(object) free(object);object=NULL


#define HEADER_LENGTH 4
#define PROTOCOL_VERSION 1
#define MAGIC_NUMBER 0xbf
#define BYTES_PER_FRAME 20


#ifdef __cplusplus
extern "C" {
#endif
    
    typedef enum{
        FF_UNSAFE_TYPE_RESULT = 0X01,
        FF_UNSAFE_TYPE_HS_WAIT = 0X02,
        FF_UNSAFE_TYPE_HS_REQ = 0X03,
        FF_UNSAFE_TYPE_HS_RSP = 0X04,
        FF_UNSAFE_TYPE_QHS_WAIT = 0X05,
        FF_UNSAFE_TYPE_PUK_REQ = 0X10,
        FF_UNSAFE_TYPE_PUK_RSP = 0X11,
        FF_SAFE_TYPE_BIND_REQ=0X82,
        FF_SAFE_TYPE_BIND_RSP=0X83,
        FF_SAFE_TYPE_PP_SET=0X90,
        FF_SAFE_TYPE_PP_ERR=0X91,
        FF_SAFE_TYPE_PP_GET=0X92,
    }ff_pkg_type_e;
    
    
    typedef struct ff_context_s{
        uint32_t rand_seed;
        uint8_t skey[16];
    }ff_context_t;
    
    uint16_t ff_encrypt_length(uint16_t in_len);
    int8_t ff_encrypt(uint8_t* in_buf, uint16_t in_len, uint16_t out_len, uint8_t* key,ff_context_t* context);
    int8_t ff_decrypt(uint8_t* in_buf, uint16_t in_len, uint16_t* out_len, uint8_t* key);
    
#ifdef __cplusplus
}
#endif

#endif /* defined(__Firefly__ff_utils__) */
