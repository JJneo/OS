/*
 * Synchronization.c
 *
 *  Created on: 2018. 2. 10.
 *      Author: Administrator
 */

#include "Synchronization.h"
#include "Utility.h"
#include "Task.h"

// 시스탬 전역에서 사용하는 데이터를 위한 잠금 함수.
BOOL kLockForSystemData( void )
{
	return kSetInterruptFlag( FALSE );
}

void kUnlockForSystemData( BOOL bInterruptFlag )
{
	kSetInterruptFlag( bInterruptFlag );
}


// 뮤텍스를 초기화
void kInitializeMutex( MUTEX* pstMutex )
{
	// 잠김 플래그, 잠김 회수, 태스크 ID를 초기화
	pstMutex->bLockFlag = FALSE;
	pstMutex->dwLockCount = 0;
	pstMutex->qwTaskID = TASK_INVALIDID;
}

// 태스크 사이에서 사용하는 데이터를 위한 잠금 함수
void kLock( MUTEX* pstMutex )
{
	// function cmpxchg(p : pointer to int, old : int, new : int) returns bool {
	//     if *p ≠ old {
	//         return false
	//     }
	//     *p ← new
	//     return true
	// }


	// 뮤텍스 플래그를 atomic operation 하여서  상태확인 및 셋팅.
	if( kTestAndSet( &(pstMutex->bLockFlag), 0, 1 ) == FALSE )
	{
		// bLockFlag가 0이 아님으로 잠겨있다는 것, blockFlag는 원상태 유지.

		// 임계자원을 잠근 테스크가 누군지 확인.
		if( pstMutex->qwTaskID == kGetRunningTask()->stNode.qwID )
		{
			// 내가 잠궜으면? 카운터만 증가시킴.
			pstMutex->dwLockCount++;
			return ;
		}

		// 내가 잠군게 아니라면 잠굼 해제될 때까지 대기, 즉 Lock함수를 못벗어난체 콘텍스트 전환되고
		// 그렇다는 것은 해당 태스크가 임계자원에 접근하지 못했다는 것. -> 내가 아닌 태스크는 접근하지 마.
		while( kTestAndSet( &(pstMutex->bLockFlag), 0, 1) )
		{
			kSchedule();
		}
	}

	// 잠금 설정, 잠금 플래그는 위의 kTestAndSet() 함수에서 처리함
	// 임계영역에 대한 잠김이 풀려야 여기까지 올 수 있음. 잠군 플래그는 Atomic operation으로 처리해서 Lock 해놨으니 나머지 영역 처리
	pstMutex->dwLockCount = 1;
	pstMutex->qwTaskID = kGetRunningTask()->stNode.qwID;
}


// 태스크 사이에서 사용하는 데이터를 위한 잠금 해제 함수
void kUnlock( MUTEX* pstMutex )
{

	// 뮤택스를 잠근 태스크가 아니면 실패
	if( pstMutex->qwTaskID != kGetRunningTask()->stNode.qwID )
	{
		return;
	}

	// 뮤택스를 잠근 카운터를 확인해서 처리
	if( pstMutex->dwLockCount > 1 )
	{
		pstMutex->dwLockCount--;
		return;
	}

	// 여기는 뮤택스 LockCount가 1이여야 올 수 있음, 뮤택스 오브젝트 초기화.
	pstMutex->qwTaskID = TASK_INVALIDID;
	pstMutex->dwLockCount = 0;
	pstMutex->bLockFlag = FALSE;

}






















