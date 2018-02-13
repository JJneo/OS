/*
 * Synchronization.h
 *
 *  Created on: 2018. 2. 10.
 *      Author: Administrator
 */

#ifndef __SYNCHRONIZATION_H__
#define __SYNCHRONIZATION_H__

#include "Types.h"

// ����ü
// 1����Ʈ�� ����
#pragma pack( push, 1 )

// ���ؽ� �ڷᱸ��
typedef struct kMutexStruct
{
	// �½�ũ ID, ����� ������ Ƚ��
	volatile QWORD qwTaskID;
	volatile DWORD dwLockCount;

	// ��� �÷���
	volatile BOOL bLockFlag;

	// �ڷᱸ���� ũ�⸦ 8����Ʈ ������ ���߷��� �߰��� �ʵ�
	BYTE vbPadding[3];

}MUTEX;


#pragma pack( pop )

// �Լ�
BOOL kLockForSystemData( void );
void kUnlockForSystemData( BOOL bInterruptFlag );

void kInitializeMutex( MUTEX* pstMutex );
void kLock( MUTEX* pstMutex );
void kUnlock( MUTEX* pstMutex );





#endif /* 02_KERNEL64_SOURCE_SYNCHRONIZATION_H_ */
