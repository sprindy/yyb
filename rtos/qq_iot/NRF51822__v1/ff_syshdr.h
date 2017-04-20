//
//  ff_syshdr.h
//  Firefly
//
// @brief system header , specailly for Device ,which has no operation system, and necessary lib
//
//  Created by AlickWang on 14/10/30.
//  Copyright (c) 2014年 tencent. All rights reserved.
//

#ifndef _FF_SYSHDR_H_
#define _FF_SYSHDR_H_


#ifdef __cplusplus
extern "C" {
#endif
    
	
    /* stdint.h subset */
#ifndef FF_MISS_STDINT_H
    #include <stdint.h>
#else
    /* You will need to modify these to match the word size of your platform. */
    typedef signed char int8_t;
    typedef unsigned char uint8_t;
    typedef signed short int16_t;
    typedef unsigned short uint16_t;
  #ifdef FF_INT_32
    typedef signed int int32_t;
    typedef unsigned int uint32_t;
  #else
    typedef signed long int32_t;
    typedef unsigned long uint32_t;
  #endif
#endif
    
    /* stddef.h subset */
#ifndef FF_MISS_STDDEF_H
#include <stddef.h>
#else
#ifndef NULL
#define NULL 0
#endif
#endif
    
    /* stdbool.h subset */
#ifndef FF_MISS_STDBOOL_H
#include <stdbool.h>
#else
#ifndef __cplusplus
#ifdef FF_ONLY_CSR
#include <types.h>
#else
typedef int bool;
#endif
#define false 0
#define true 1
#endif
#endif
    
    /* stdlib.h subset */
#ifndef FF_MISS_STDLIB_H
#include <stdlib.h>
#endif
    
    /* string.h subset */
#ifndef FF_MISS_STRING_H
#include <string.h>
//for Broadcom build
#ifdef FF_ONLY_BCM
#include "spar_utils.h"
#endif
//for CSR build
#ifdef FF_ONLY_CSR
#include <mem.h>
#define strlen StrLen    
#define memcpy MemCopy 
#define memset MemSet
#define memcmp MemCmp
#endif
#else
    
    /* Implementations are from the Public Domain C Library (PDCLib). */
    static size_t strlen( const char * s )
    {
        size_t rc = 0;
        while ( s[rc] )
        {
            ++rc;
        }
        return rc;
    }
    
    static void * memcpy( void *s1, const void *s2, size_t n )
    {
        char * dest = (char *) s1;
        const char * src = (const char *) s2;
        while ( n-- )
        {
            *dest++ = *src++;
        }
        return s1;
    }
    
    static void * memset( void * s, int c, size_t n )
    {
        unsigned char * p = (unsigned char *) s;
        while ( n-- )
        {
            *p++ = (unsigned char) c;
        }
        return s;
    }
    
    static  int memcmp (const void *s1, const void *s2, size_t n){
        if (!n)
            return(0);
        while ( --n && *(char *)s1 == *(char *)s2)
        {
            s1 = (char *)s1 + 1;
            s2 = (char *)s2 + 1;
        }
        return( *((unsigned char *)s1) - *((unsigned char *)s2) );
    }
#endif

#ifndef FF_MISS_LIMITS_H
#include <limits.h>
#endif    
    
#ifdef __cplusplus
}
#endif
#endif
