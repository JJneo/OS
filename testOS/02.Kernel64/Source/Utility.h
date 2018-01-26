/*
 * Utility.h
 *
 *  Created on: 2018. 1. 25.
 *      Author: Administrator
 */

#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "Types.h"

void kMemSet( void* pvDestination, BYTE bData, int iSize);
int kMemCpy( void* pvDestination, const void* pvSource, int iSize);
int kMemCmp( const void* pvDestination, const void* pvSource, int iSize);
BOOL kSetInterruptFlag(BOOL bEnableInterrupt);

#endif /* 02_KERNEL64_SOURCE_UTILITY_H_ */
