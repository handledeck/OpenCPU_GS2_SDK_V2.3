/*==========================================================================
 |               Quectel OpenCPU --  User API
 |
 |              Copyright (c) 2011 Quectel Ltd.
 |
 |--------------------------------------------------------------------------
 | File Description
 | ----------------
 |      Definitions for Utility
 |
 |--------------------------------------------------------------------------
 |
 |  Designed by   :     Stanley YONG
 |  Coded by      :     Stanley YONG
 |  Tested by     :     Stanley YONG
 |
 \=========================================================================*/
#ifndef __QL_UTILITY_H__
#define __QL_UTILITY_H__
#include "ql_type.h"

typedef enum{
    LANG_ENG = 0,
    LANG_CHZ = 1,
    LANG_END
}QlLanguage;
typedef enum{
    CHARSET_UTF8 = 0,
    CHARSET_GB2132,
    CHARSET_GBK,
    CHARSET_END
}QlCharSet;
typedef struct{
    double longitude;
    double latitude;
}QlCoordinate;


/*
*  Codec/dstCode:
*   one value of QuectelCodecType_e.
***********************************/
s32 Ql_ConvertCodec(u8 srcCode, u8* src_p, u8 dstCode, u8* dst_p);

#endif  // End-of __QL_UTILITY_H__

