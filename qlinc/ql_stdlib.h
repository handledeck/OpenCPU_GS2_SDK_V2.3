/*==========================================================================
 |               Quectel OpenCPU --  User API
 |
 |              Copyright (c) 2010 Quectel Ltd.
 |
 |--------------------------------------------------------------------------
 | File Description
 | ----------------
 |      Definitions for common function
 |
 |--------------------------------------------------------------------------
 |
 |  Designed by    :    Jay XIN
 |  Coded by       :    Jay XIN
 |  Tested by      :    Stanley YONG
 |
 \=========================================================================*/

#ifndef __QL_STDLIB_H__
#define __QL_STDLIB_H__

#include "ql_type.h"

s32    Ql_atoi(const char* s);
double Ql_atof(const char* s);
char* Ql_strcpy(char* dest, const char* src);
char* Ql_strncpy(char* dest, const char* src, u16 size);
char* Ql_strcat(char* s1, const char* s2);
char* Ql_strncat(char* s1, const char* s2, u16 size);
s32   Ql_strcmp(const char*s1, const char*s2);
void* Ql_memcpy(void* dest, const void* src, u16 size);
s32   Ql_memcmp(const void* dest, const void*src, u16 size);
void* Ql_memmove(void* dest, const void* src, u16 size);
char* Ql_strchr(const char* s1, u16 ch);
u16   Ql_strlen(const char* str);
s32   Ql_strncmp(const char* s1, const char* s2, u16 size);
void* Ql_memset(void* dest, u8 value, u32 size);
char* Ql_strstr(const char* s1, const char* s2);
extern s32 (*Ql_sprintf)(char *, const char *, ...);

#endif  // End-of __QL_STDLIB_H__

