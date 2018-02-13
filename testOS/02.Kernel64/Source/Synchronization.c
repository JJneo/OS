/*
 * Synchronization.c
 *
 *  Created on: 2018. 2. 10.
 *      Author: Administrator
 */

#include "Synchronization.h"
#include "Utility.h"
#include "Task.h"

// �ý��� �������� ����ϴ� �����͸� ���� ��� �Լ�.
BOOL kLockForSystemData( void )
{
	return kSetInterruptFlag( FALSE );
}

void kUnlockForSystemData( BOOL bInterruptFlag )
{
	kSetInterruptFlag( bInterruptFlag );
}


// ���ؽ��� �ʱ�ȭ
void kInitializeMutex( MUTEX* pstMutex )
{
	// ��� �÷���, ��� ȸ��, �½�ũ ID�� �ʱ�ȭ
	pstMutex->bLockFlag = FALSE;
	pstMutex->dwLockCount = 0;
	pstMutex->qwTaskID = TASK_INVALIDID;
}

// �½�ũ ���̿��� ����ϴ� �����͸� ���� ��� �Լ�
void kLock( MUTEX* pstMutex )
{
	// function cmpxchg(p : pointer to int, old : int, new : int) returns bool {
	//     if *p �� old {
	//         return false
	//     }
	//     *p �� new
	//     return true
	// }


	// ���ؽ� �÷��׸� atomic operation �Ͽ���  ����Ȯ�� �� ����.
	if( kTestAndSet( &(pstMutex->bLockFlag), 0, 1 ) == FALSE )
	{
		// bLockFlag�� 0�� �ƴ����� ����ִٴ� ��, blockFlag�� ������ ����.

		// �Ӱ��ڿ��� ��� �׽�ũ�� ������ Ȯ��.
		if( pstMutex->qwTaskID == kGetRunningTask()->stNode.qwID )
		{
			// ���� �������? ī���͸� ������Ŵ.
			pstMutex->dwLockCount++;
			return ;
		}

		// ���� �ᱺ�� �ƴ϶�� ��� ������ ������ ���, �� Lock�Լ��� �����ü ���ؽ�Ʈ ��ȯ�ǰ�
		// �׷��ٴ� ���� �ش� �½�ũ�� �Ӱ��ڿ��� �������� ���ߴٴ� ��. -> ���� �ƴ� �½�ũ�� �������� ��.
		while( kTestAndSet( &(pstMutex->bLockFlag), 0, 1) )
		{
			kSchedule();
		}
	}

	// ��� ����, ��� �÷��״� ���� kTestAndSet() �Լ����� ó����
	// �Ӱ迵���� ���� ����� Ǯ���� ������� �� �� ����. �ᱺ �÷��״� Atomic operation���� ó���ؼ� Lock �س����� ������ ���� ó��
	pstMutex->dwLockCount = 1;
	pstMutex->qwTaskID = kGetRunningTask()->stNode.qwID;
}


// �½�ũ ���̿��� ����ϴ� �����͸� ���� ��� ���� �Լ�
void kUnlock( MUTEX* pstMutex )
{

	// ���ý��� ��� �½�ũ�� �ƴϸ� ����
	if( pstMutex->qwTaskID != kGetRunningTask()->stNode.qwID )
	{
		return;
	}

	// ���ý��� ��� ī���͸� Ȯ���ؼ� ó��
	if( pstMutex->dwLockCount > 1 )
	{
		pstMutex->dwLockCount--;
		return;
	}

	// ����� ���ý� LockCount�� 1�̿��� �� �� ����, ���ý� ������Ʈ �ʱ�ȭ.
	pstMutex->qwTaskID = TASK_INVALIDID;
	pstMutex->dwLockCount = 0;
	pstMutex->bLockFlag = FALSE;

}






















