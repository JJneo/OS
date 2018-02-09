/**
 *  file    Utility.h
 *  brief   ������� ��ƿ��Ƽ �Լ����� ������ ��� ����
 */

#ifndef __ASSEMBLYUTILITY_H__
#define __ASSEMBLYUTILITY_H__

#include "Types.h"
#include "Task.h"

////////////////////////////////////////////////////////////////////////////////
//
//  �Լ�
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

//PIT ����
QWORD kReadTSC(void);

// context switching ����
void kSwitchContext( CONTEXT* pstCurrentContext, CONTEXT* pstNextContext );
void kHlt( void );


#endif /*__ASSEMBLYUTILITY_H__*/
