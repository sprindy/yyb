/****************************************Copyright (c)**************************************************
**                                  Tencent  &    Honestar
**
**                                 http://www.tencent.com
**                                 http://www.honestar.com
**
**--------------File Info-------------------------------------------------------------------------------
** File Name:           fh_impl.h
** Last modified Date:  2015/1/22
** Last Version:        V1.0
** Description:         fh_* function implementation  in txble SDK
**
**------------------------------------------------------------------------------------------------------
** Created By:          Alan Lan
** Created date:        2015/1/22
** Version:             V1.0
** Descriptions:        Initial Version
**
**------------------------------------------------------------------------------------------------------
** Modified by:        Alick Wang
** Modified date:
** Version:
** Description:
**
********************************************************************************************************/
#ifndef  __FH_IMPL_H_
#define  __FH_IMPL_H_

#include <stdio.h>
#include <string.h>
#include "nrf.h"

#include "ble_qqs.h"

#include "ff_syshdr.h"
#include "ff_inc.h"
#include "fd_engine.h"

#include "pstorage.h"
#include "pstorage_platform.h"


/*********************************************************************************************************
** you can modify here
*********************************************************************************************************/
#define FT_SET_DEFAULT          1

// F2:AF:0C:C1:FC:FD -- reling
//#define FT_LICENSE {0x30,0x45,0x02,0x21,0x00,0xec,0xab,0xfd,0x51,0x81,0x88,0x6f,0x35,0x33,0xaf,0x3f,0x09,0x20,0x1e,0x57,0xbb,0xda,0xb8,0x86,0x0d,0xbe,0xb1,0x8c,0xe1,0x17,0x08,0xd8,0x80,0x61,0x63,0x68,0xba,0x02,0x20,0x57,0xca,0x1e,0xe1,0x0e,0x7a,0x82,0x97,0xdd,0x22,0xe9,0x22,0x56,0xb4,0x01,0xad,0xd4,0x68,0x09,0x1b,0x9e,0x10,0xe6,0xf2,0x12,0x74,0xd0,0x41,0x03,0xce,0xec,0xdb}
// DE:C5:1D:1B:80:81 -- annidy������
#define FT_LICENSE {0x30,0x45,0x02,0x20,0x35,0x23,0xbd,0xba,0xf7,0x30,0x26,0x0f,0xf8,0x72,0x91,0x3a,0x6b,0x70,0x7e,0x44,0x53,0x44,0xb2,0xb8,0x58,0xc1,0x05,0xa9,0x49,0x47,0xc8,0x30,0xcd,0x5b,0xf0,0xf9,0x02,0x21,0x00,0x91,0x72,0x9c,0xfd,0x3a,0x20,0xde,0x5f,0xca,0xd2,0xcf,0xd3,0x38,0x56,0x2d,0x0c,0xbb,0x50,0x0e,0x75,0x2e,0x1e,0xb9,0x43,0x4f,0x52,0xad,0xe2,0x7f,0x5e,0xbc,0xe9}
//E53CB28E344C0000 -- mongo������
//#define FT_LICENSE {0x30,0x45,0x02,0x21,0x00,0xA0,0x95,0x44,0x13,0x43,0x04,0x3C,0xE9,0x09,0x72,0xCE,0xB0,0x94,0xC6,0x29,0x54,0x3B,0xB1,0xCD,0xBD,0x93,0x4C,0xC6,0xF0,0xA9,0x86,0xC7,0x6C,0x7C,0x3B,0x8A,0x4E,0x02,0x20,0x7E,0x0A,0x68,0x0A,0xA1,0x0D,0x6F,0x3C,0x89,0x4D,0x1B,0x74,0xDE,0x87,0x7F,0xFD,0x31,0x51,0xE5,0x37,0x4A,0x5F,0x9A,0xCB,0x14,0x19,0x16,0xD5,0xC3,0xFC,0x87,0xF3}
/*********************************************************************************************************
** value for Demo App
*********************************************************************************************************/
#define FT_DEFAULT_PID  1000000358  

#define FT_DEFAULT_AUTH_KEY {0x12,0xc5,0xc3,0x66,0xad,0xd8,0x07,0x81,0x8e,0x13,0xc5,0x1e,0xd3,0x83,0x65,0x8e}
#define FT_DEFAULT_PUK  {0x03,0xad,0x53,0x43,0x94,0xbb,0xc9,0x02,0xac,0x40,0x0c,0xcf,0xec,0x73,0xf4,0xa3,0x42,0x0c,0xfb,0x37,0x49,0xe3,0x13,0x95,0x17}

  


/*********************************************************************************************************
** Global variable
*********************************************************************************************************/

#define mLED_1        21
#define mLED_2        22
#define mLED_3        23
#define mLED_4        24
void leds_ctrl(uint8_t lednum, uint8_t ctrl);

#define PUK_STR_LEN           25//changee,max=48
#define	COMPANY_IDENTIFIER		(FT_DEFAULT_PID&0xffff)
#define FT_LICENSE_LEN       sizeof((char[])FT_LICENSE)
#define LICENSE_BUFFER_LEN   (FT_LICENSE_LEN >> 2 + 1) << 2
#define DIN_LEN               8
#define AUTH_KEY_LEN          16
#define MAC_LEN               6
#define MAC_STR_LEN           ((MAC_LEN << 1) + 1)
#define PUK_STR_BUFFER_LEN    (((PUK_STR_LEN >> 2) + 1) << 2)
#define FT_MAX_LICENSE_LEN		80

typedef struct _QQ_SEC_INFO{
  uint8_t  license[FT_MAX_LICENSE_LEN];
  uint16_t licenselen;
  uint8_t  din[DIN_LEN];
  uint32_t pid;
  uint8_t  authkey[AUTH_KEY_LEN];
  uint8_t  pukstr[PUK_STR_BUFFER_LEN];
}QQSecInfo;

#define FDATA_BLOCK_SIZE     sizeof(QQSecInfo)
#define FDATA_BLOCK_COUNT    2

extern QQSecInfo qqSecurityInfo;


void fh_data_set_default(void);

void fh_data_load();
void fh_data_store(void);

#endif /*__FH_IMPL_H_*/

/*********************************************************************************************************
** End Of File
*********************************************************************************************************/