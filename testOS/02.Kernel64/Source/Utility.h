/*
 * Utility.h
 *
 *  Created on: 2018. 1. 25.
 *      Author: Administrator
 */

#ifndef __UTILITY_H__
#define __UTILITY_H__

#include <stdarg.h>
#include "Types.h"



void kMemSet( void* pvDestination, BYTE bData, int iSize);
int kMemCpy( void* pvDestination, const void* pvSource, int iSize);
int kMemCmp( const void* pvDestination, const void* pvSource, int iSize);
BOOL kSetInterruptFlag(BOOL bEnableInterrupt);

//셸관련
void kCheckTotalRAMSize( void );
QWORD kGetTotalRAMSize( void );
void kReverseString( char* pcBuffer );
long kAToI( const char* pcBuffer, int iRadix );
QWORD kHexStringToQword( const char* pcBuffer );
long kDecimalStringToLong( const char* pcBuffer );
int kIToA( long lValue, char* pcBuffer, int iRadix );
int kHexToString( QWORD qwValue, char* pcBuffer );
int kDecimalToString( long lValue, char* pcBuffer );
int kSPrintf( char* pcBuffer, const char* pcFormatString, ... );
int kVSPrintf( char* pcBuffer, const char* pcFormatString, va_list ap );

// 함수
QWORD kGetTickCount(void);

// 기타
extern volatile QWORD g_qwTickCount;

#endif /* 02_KERNEL64_SOURCE_UTILITY_H_ */
