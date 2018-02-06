/*
 * InterruptHandler.h
 *
 *  Created on: 2018. 1. 26.
 *      Author: Administrator
 */

#ifndef __INTERRUPTHANDLER_H__
#define __INTERRUPTHANDLER_H__

#include "Types.h"

// ÇÔ¼ö
void kCommonExceptionHandler( int iVectorNumber, QWORD qwErrorCode);
void kCommonInterruptHandler( int iVectorNumber);
void kKeyboardHandler( int iVectorNumber);
void kTimerHandler( int iVectorNumber );



#endif /* 02_KERNEL64_SOURCE_INTERRUPTHANDLER_H_ */
