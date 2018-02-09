/**
 *  file    Utility.h
 *  brief   어셈블리어 유틸리티 함수들을 정의한 헤더 파일
 */

#ifndef __ASSEMBLYUTILITY_H__
#define __ASSEMBLYUTILITY_H__

#include "Types.h"
#include "Task.h"

////////////////////////////////////////////////////////////////////////////////
//
//  함수
//
////////////////////////////////////////////////////////////////////////////////
BYTE kInPortByte( WORD wPort );
void kOutPortByte( WORD wPort, BYTE bData );
void kLoadGDTR( QWORD qwGDTRAddress );
void kLoadTR( WORD wTSSSegmentOffest );
void kLoadIDTR( QWORD qwIDTRAddress );

void kEnableInterrupt( void );
void kDisableInterrupt( void );
QWORD kReadRFLAGS( void );

//PIT 관련
QWORD kReadTSC(void);

// context switching 관련
void kSwitchContext( CONTEXT* pstCurrentContext, CONTEXT* pstNextContext );
void kHlt( void );


#endif /*__ASSEMBLYUTILITY_H__*/
