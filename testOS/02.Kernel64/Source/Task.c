/*
 * Task.c
 *
 *  Created on: 2018. 1. 30.
 *      Author: Administrator
 */


#include "Task.h"
#include "Descriptor.h"

// 스케줄러 관련 자료구조
static SCHEDULER gs_stScheduler;
static TCBPOOLMANAGER gs_stTCBPoolManager;

//=============================================================================//
// 태스크 풀과 태스크 관련
//=============================================================================//

// 태스크 풀 초기화
static void kInitializeTCBPool(void)
{
	int i;

	kMemSet( &(gs_stTCBPoolManager ), 0, sizeof(gs_stTCBPoolManager) );

	// 태스크 풀의 어드레스를 지정하고 초기화
	gs_stTCBPoolManager.pstStartAddress = (TCB*) TASK_TCBPOOLADDRESS;
	kMemSet( TASK_TCBPOOLADDRESS, 0, sizeof(TCB) * TASK_MAXCOUNT );

	// TCB에 ID 할당
	for( i=0; i< TASK_MAXCOUNT; i++ )
	{
		gs_stTCBPoolManager.pstStartAddress[i].stNode.qwID = i;
	}

	// TCB의 최대 개수와 할당된 횟수를 초기화
	gs_stTCBPoolManager.iMaxCount = TASK_MAXCOUNT;
	gs_stTCBPoolManager.iAllocatedCount = 1;	// 부팅도 하나의 테스크
}

// TCB를 할당 받음
static TCB* kAllocateTCB( void )
{
	TCB* pstEmptyTCB;
	int i;

	if( gs_stTCBPoolManager.iUseCount == gs_stTCBPoolManager.iMaxCount )
	{
		return NULL;
	}

	for( i=0; i<gs_stTCBPoolManager.iMaxCount; i++)
	{
		// ID의 상위 32비트가 0이면 할당되지 않은 TCB
		if( (gs_stTCBPoolManager.pstStartAddress[i].stNode.qwID>>32) == 0 )
		{
			pstEmptyTCB = &(gs_stTCBPoolManager.pstStartAddress[i]);
			break;
		}
	}
	// 상위 32비트를 0이 아닌 값으로 설정해서 할당된 TCB로 설정
	pstEmptyTCB->stNode.qwID = ( (QWORD)gs_stTCBPoolManager.iAllocatedCount << 32) | i;
	gs_stTCBPoolManager.iUseCount++;
	gs_stTCBPoolManager.iAllocatedCount++;
	if( gs_stTCBPoolManager.iAllocatedCount == 0 )
	{
		gs_stTCBPoolManager.iAllocatedCount = 1;
	}
	return pstEmptyTCB;
}

// TCB 해제함
static void kFreeTCB( QWORD qwID )
{
	int i;

	// 태스크 ID의 하위 32비트가 인덱스 역할을 함.
	i = GETTCBOFFSET( qwID );

	// TCB를 초기화하고 ID 설정
	kMemSet( &( gs_stTCBPoolManager.pstStartAddress[i].stContext), 0, sizeof(CONTEXT) );
	gs_stTCBPoolManager.pstStartAddress[i].stNode.qwID = i;		// 상위 32비트를 지워서 등록 해제

	gs_stTCBPoolManager.iUseCount--;

}

// 태스크를 생성
// 태스크 ID에 따라서 스택 풀에서 스택 자동 할당
TCB* kCreateTask( QWORD qwFlags, QWORD qwEntryPointAddress )
{
	TCB* pstTask;
	void* pvStackAddress;
	BOOL bPreviousFlag;

	// 임계영역 시작.
	bPreviousFlag = kLockForSystemData();

	pstTask = kAllocateTCB();
	if( pstTask == NULL )
	{
		// 임계영역 끝
		kUnlockForSystemData( bPreviousFlag );
		return NULL;
	}

	// 임계영역 끝.
	kUnlockForSystemData( bPreviousFlag );


	// 태스크 ID로 스택 어드레스 계산, 하위 32비트가 스택 풀의 오프셋 역할 수행
	pvStackAddress = (void*) (TASK_STACKPOOLADDRESS + (TASK_STACKSIZE * ( GETTCBOFFSET(pstTask->stNode.qwID) ) ) );

	// TCB 설정한 후 준비 리스트에 삽입하여 스케줄링 될 수 있도록 함.
	kSetUpTask(pstTask, qwFlags, qwEntryPointAddress, pvStackAddress, TASK_STACKSIZE);

	// 임계영역 시작
	kLockForSystemData();

	kAddTaskToReadyList( pstTask );
	// 임계 영역 끝
	kUnlockForSystemData( bPreviousFlag );


	return pstTask;
}


// 파라미터를 이용해서 TCB를 설정
static void kSetUpTask( TCB* pstTCB, QWORD qwFlags, QWORD qwEntryPointAddress, void* pvStackAddress, QWORD qwStackSize)
{
	//콘택스트 초기화
	kMemSet( pstTCB->stContext.vqRegister, 0, sizeof( pstTCB->stContext.vqRegister) );

	// 스택에 관련된 RSP, RBP 레지스터 설정
	pstTCB->stContext.vqRegister[TASK_RSPOFFSET] = (QWORD) pvStackAddress+qwStackSize;
	pstTCB->stContext.vqRegister[TASK_RBPOFFSET] = (QWORD) pvStackAddress+qwStackSize;


	// 세그먼트 셀렉터 설정
	pstTCB->stContext.vqRegister[TASK_CSOFFSET] = GDT_KERNELCODESEGMENT;
	pstTCB->stContext.vqRegister[TASK_DSOFFSET] = GDT_KERNELDATASEGMENT;
	pstTCB->stContext.vqRegister[TASK_ESOFFSET] = GDT_KERNELDATASEGMENT;
	pstTCB->stContext.vqRegister[TASK_FSOFFSET] = GDT_KERNELDATASEGMENT;
	pstTCB->stContext.vqRegister[TASK_GSOFFSET] = GDT_KERNELDATASEGMENT;
	pstTCB->stContext.vqRegister[TASK_SSOFFSET] = GDT_KERNELDATASEGMENT;


	// RIP 레지스터와 인터럽트 플래그 설정
	pstTCB->stContext.vqRegister[TASK_RIPOFFSET] = qwEntryPointAddress;

	// RFLAGS 레지스터의 IF 비트(비트9)를 1로 설정하여 인터럽트 활성화
	pstTCB->stContext.vqRegister[TASK_RFLAGSOFFSET] |= 0x0200;

	//스택, 그리고 플래그 저장
	pstTCB->pvStackAddress = pvStackAddress;
	pstTCB->qwStackSize= qwStackSize;
	pstTCB->qwFlags = qwFlags;


}

//=============================================================================//
// 스케줄러 관련
//=============================================================================//

// 스케줄러 초기화
//		스케줄러를 초기화하는데 필요한 TCB풀과 init 태스크도 같이 초기화
void kInitializeScheduler( void )
{
	int i;
	// 태스크 풀 초기화
	kInitializeTCBPool();

	// 준비 리스트와 우선순위 별 실행 횟수 자료구조를 초기화
	for( i=0; i<TASK_MAXREADYLISTCOUNT; i++ )
	{
		kInitializeList( &(gs_stScheduler.vstReadyList[i]) );
		gs_stScheduler.viExecuteCount[i] = 0;
	}

	//대기 리스트 초기화
	kInitializeList( &(gs_stScheduler.stWaitList) );


	// TCB를 할당받아 실행 중인 태스크로 설정하고, 부팅을 수행한 태스크를 저장할 TCB 준비
	// (즉, 지금 실행중인 태스크는 부팅(콘솔)관련이고 TCB를 할당 받는 것은 콘솔을 태스크로 만든다는 것.)
	gs_stScheduler.pstRunningTask = kAllocateTCB();
	// 부팅(콘솔) 태스크 우선순위 설정
	gs_stScheduler.pstRunningTask->qwFlags = TASK_FLAGS_HIGHEST;

	// 프로세서 사용율을 계산하는데 사용하는 자료구조 초기화
	gs_stScheduler.qwSpendProcessorTimeInIdleTask = 0;
	gs_stScheduler.qwProcessorLoad = 0;

}

// 현재 수행 중인 태스크를 설정
void kSetRunningTask( TCB* pstTask )
{
	// 스케줄러라는 태스크들이 공용으로 사용하는 자료구조에 접근하기 때문에 동기화 필요.
	BOOL bPreviousFlag;

	// 임계영역 시작
	bPreviousFlag = kLockForSystemData();

	gs_stScheduler.pstRunningTask = pstTask;

	// 임계영역 끝.
	kUnlockForSystemData(bPreviousFlag);
}

// 현재 수행 중인 태스크를 반환
TCB* kGetRunningTask( void )
{
	BOOL bPreviousFlag;
	TCB* pstRunningTask;

	// 임계영역 설정
	bPreviousFlag = kLockForSystemData();

	pstRunningTask = gs_stScheduler.pstRunningTask;

	// 임계영역 해제
	kUnlockForSystemData(bPreviousFlag);

	return pstRunningTask;
}

// 태스크 리스트에서 다음으로 실행할 태스크를 얻음
static TCB* kGetNextTaskToRun( void )
{
	// 다음에 수행할 태스크를 저장하는 자료구조.
	TCB* pstTarget = NULL;

	// 리스트의 태스크 수.
	int iTaskCount;

	int i;
	int j;

	// 큐에 태스크가 있으나 모든 큐의 태스크가 1회씩 실행된 경우 모든 큐가 프로세서를 양보하여 태스크를 선택하지 못할 수 있으니 NULL일 경우 한번 더 수행.
	// 671p 참고.
	for( j=0; j<2; j++ )
	{
		// 높은 우선순위에서 낮은 우선순위까지 리스트를 확인하여 스케줄링을 할 태스크 선택
		for( i=0; i<TASK_MAXREADYLISTCOUNT; i++ )
		{
			// kGetListCount => gs_stScheduler.vstReadyList[i].iItemCount
			iTaskCount = kGetListCount( &(gs_stScheduler.vstReadyList[i]) );

			// 만약 각 큐의 실행한 태스크 개수보다 리스트의 태스크 수가 더 많으면  실행할 태스크가 있으니
			// 현재 우선순위 큐의 태스크를 실행(반환)함.
			if( gs_stScheduler.viExecuteCount[i] < iTaskCount )
			{
				pstTarget = (TCB*) kRemoveListFromHeader( &(gs_stScheduler.vstReadyList[i]) );
				gs_stScheduler.viExecuteCount[i]++;
				break;
			}
			// 실행 횟수가 태스크 수 보다 같거나 많다면 다실행했다는 뜻?임으로 현재 우선순위 큐의 실행횟수를 0으로 초기화 하고 다음 우선순위레벨의 큐로 양보함.
			else
			{
				gs_stScheduler.viExecuteCount[i] = 0;
			}
		}

		// 만약 수행할 태스크가 선택되었으면 반환하고 종료
		if( pstTarget != NULL )
		{
			break;
		}


	}
	return pstTarget;

}




// 태스크를 스캐줄러의 준비 리스트에 삽입
static BOOL kAddTaskToReadyList( TCB* pstTask )
{
	BYTE bPriority;

	bPriority = GETPRIORITY(pstTask->qwFlags);
	if( bPriority >= TASK_MAXREADYLISTCOUNT )
	{
		return FALSE;
	}
	kAddListToTail( &(gs_stScheduler.vstReadyList[bPriority]),pstTask );
	return TRUE;

}


// 준비 큐에서 태스크를 제거
static TCB* kRemoveTaskFromReadyList( QWORD qwTaskID )
{
	TCB* pstTarget;
	BYTE bPriority;

	if( GETTCBOFFSET(qwTaskID) >= TASK_MAXCOUNT )
	{
		return NULL;
	}

	// TCB 풀에서 해당 태스크를 찾아 실제로 ID가 일치하는지 확인
	pstTarget = &(gs_stTCBPoolManager.pstStartAddress[ GETTCBOFFSET(qwTaskID) ] );
	if( qwTaskID != pstTarget->stNode.qwID )
	{
		return NULL;
	}

	// 태스크가 존재하는 준비 리스트에서 태스크 제거.
	bPriority = GETPRIORITY(pstTarget->qwFlags);
	pstTarget = kRemoveList(&(gs_stScheduler.vstReadyList[bPriority]), qwTaskID);
	return pstTarget;
}


// 태스크의 우선순위를 변경함.
BOOL kChangePriority( QWORD qwTaskID, BYTE bPriority)
{
	TCB* pstTarget;
	BOOL bPreviousFlag;

	if( bPriority > TASK_MAXREADYLISTCOUNT )
	{
		return FALSE;
	}

	// 현재  실행 중인 태스크면 우선순위만 변경
	// PIT 컨트롤러의 인터럽트(IRQ 0)이 발생하여 태스크 전환이 수행될 때 변경된 우선순위의 리스트로 이동

	// 임계영역 시작.
	bPreviousFlag = kLockForSystemData();

	pstTarget = gs_stScheduler.pstRunningTask;
	if( pstTarget->stNode.qwID == qwTaskID )
	{
		SETPRIORITY(pstTarget->qwFlags, bPriority);
	}
	// 실행 중인 태스크가 아니면 준비 리스트에서 찾아서 해당 우선순위의 리스트로 이동.
	else
	{

		pstTarget = kRemoveTaskFromReadyList(qwTaskID);
		// 준비 리스트에서 태스크를 찾지 못하면 직접 태스크를 찾아서 우선순위 리스트로 이동
		if( pstTarget == NULL )
		{
			// 태스크 ID로 직접 찾아서 설정
			pstTarget = kGetTCBInTCBPool( GETTCBOFFSET(qwTaskID) );
			if( pstTarget != NULL )
			{
				SETPRIORITY( pstTarget->qwFlags , bPriority );
			}
		}
		else
		{
			// 우선순위를 설정하고 준비 리스트에 다시 삽입.
			SETPRIORITY(pstTarget->qwFlags, bPriority );
			kAddTaskToReadyList( pstTarget );
		}
	}

	// 임계영역 끝
	kUnlockForSystemData(bPreviousFlag);
	return TRUE;

}

// 다른 태스크를 찾아서 전환 ( 어떤 태스크에서 이 함수를 호출하면 다른 태스크로 전환됨. )
//		인터럽트나 예외가 발생했을 때 호출하면 안됨
void kSchedule(void)
{
	TCB* pstRunningTask;
	TCB* pstNextTask;
	BOOL bPreviousFlags;

	// 전환할 태스크가 있어야 함
	if( kGetReadyTaskCount() < 1 )
	{
		return;
	}

	// 전환하는 도중 인터럽트가 발생하여 태스크 전환이 또 일어나면 곤란하므로 전환하는 동안 인터럽트가 발생하지 못하도록 설정
	// 임계영역 설정
	bPreviousFlags = kLockForSystemData();

	// 실행할 다음 태스크를 얻음
	pstNextTask = kGetNextTaskToRun();
	if( pstNextTask == NULL )
	{
		kUnlockForSystemData(bPreviousFlags);
		return;
	}

	// 태스크를 전환함. 진행중인 태스크 정보를 저장하고 스캐줄러의 정보 업데이트와  태스크의 종료 플래그에 따라 콘텍스트 업데이트.
	// 현재 태스크 정보 저장.
	pstRunningTask = gs_stScheduler.pstRunningTask;
	// 스케줄러의 태스크 전환
	gs_stScheduler.pstRunningTask = pstNextTask;

	// 현재 진행하고 있었던 태스크가 유휴태스크 였다면 사용한 프로세서 시간을 증가 시킴  ( 태스크 한정 시간(5ms) - 프로세서가 사용 가능한 남은 시간 )
	if( (pstRunningTask->qwFlags & TASK_FLAGS_IDLE) == TASK_FLAGS_IDLE )
	{
		gs_stScheduler.qwSpendProcessorTimeInIdleTask += TASK_PROCESSORTIME - gs_stScheduler.iProcessorTime;
	}

	// 프로세서 남은 사용 시간을 초기화. ( 태스크 전환 되니깐 )
	gs_stScheduler.iProcessorTime = TASK_PROCESSORTIME;

	// 현재 태스크 가 종료 플래그로 설정된 경우 현재 콘텍스트를 저장할 필요가 없음으로 대기 리스트에 삽입하고 콘텍스트 전환
	if( (pstRunningTask->qwFlags & TASK_FLAGS_ENDTASK) == TASK_FLAGS_ENDTASK )
	{
		kAddListToTail( &(gs_stScheduler.stWaitList), pstRunningTask );
		kSwitchContext( NULL, &(pstNextTask->stContext) );
	}
	else
	{
		kAddTaskToReadyList( pstRunningTask );
		kSwitchContext( &(pstRunningTask->stContext), &(pstNextTask->stContext) );
	}


	kUnlockForSystemData(bPreviousFlags);

}


// 인터럽트가 발생했을 때, 다른 태스크를 찾아 전환 ( 스케줄링 시간을 보장하기 위해 5ms 이상의 태스크는 인터럽트로 강제 전환 )
//		반드시 인터럽트나 예외가 발생했을 때 호출해야 함
BOOL kScheduleInInterrupt( void )
{
	TCB* pstRunningTask;
	TCB* pstNextTask;

	char* pcContextAddress;

	BOOL bPreviousFlag;
	// 전환할 태스크가 없으면 종료
	// 임계영역 설정
	bPreviousFlag = kLockForSystemData();

	pstNextTask = kGetNextTaskToRun();

	if( pstNextTask == NULL )
	{
		// 임계영역 끝
		kUnlockForSystemData(bPreviousFlag);
		return FALSE;
	}

	//===================================================//
	// 태스크 전환 처리 ( 인터럽트 걸릴때 다른 태스크를 전환하는 것. 주기적인 스위칭을 위해 구현 )
	// 인터럽트 핸들러에서 저장한 컨텍스트를 다른 콘텍스트로 덮어쓰는 방법으로 차리
	//===================================================//

	pcContextAddress = (char*) IST_STARTADDRESS + IST_SIZE - sizeof(CONTEXT);



	// kSchedule()과 마찬가지로  현재 태스크 저장 후, 스케줄러 업데이트 및 테스크 종료 여부에 따라 콘텍스트 저장.
	pstRunningTask = gs_stScheduler.pstRunningTask;
	gs_stScheduler.pstRunningTask = pstNextTask;

	// 유휴 태스크에서 전환됬다면 사용한 프로세서의 시간을 증가시킴
	if( (pstRunningTask->qwFlags & TASK_FLAGS_IDLE) == TASK_FLAGS_IDLE )
	{
		// 5ms 이상 걸린 태스크는 강제 전환됨으로 TASK_PROCESSORTIME을 더함.
		gs_stScheduler.qwSpendProcessorTimeInIdleTask += TASK_PROCESSORTIME;
	}

	// 진행중이던 태스크가 종료 플래그가 설정된 경우 진행중이던 태스크의 콘택스트는 저장하지 않고 대기리스트에 삽입.
	if( (pstRunningTask->qwFlags & TASK_FLAGS_ENDTASK) == TASK_FLAGS_ENDTASK )
	{
		kAddListToTail( &(gs_stScheduler.stWaitList), pstRunningTask );
	}
	// 태스크가 종료 태스크가 아니면 IST에 있는 콘텍스트를 복사하고 , 현재 태스크를 준비 리스트로 옮김.
	else
	{
		kMemCpy( &(pstRunningTask->stContext), pcContextAddress, sizeof(CONTEXT));
		kAddTaskToReadyList( pstRunningTask );
	}

	// 전환해서 실행할 태스크를 Running Task로 설정하고 콘텍스트를 IST에 복사해서 자동으로 태스크 전환이 일어나도록 함.
	kMemCpy( pcContextAddress, &(pstNextTask->stContext) , sizeof(CONTEXT) );

	// 프로세서 사용 시간을 업데이트
	gs_stScheduler.iProcessorTime = TASK_PROCESSORTIME;

	// 임계영역 끝
	kUnlockForSystemData(bPreviousFlag);


	return TRUE;

}

// 프로세서를 사용할 수 있는 시간을 하나 줄임
void kDecreaseProcessorTime(void)
{
	if( gs_stScheduler.iProcessorTime > 0 )
	{
		gs_stScheduler.iProcessorTime--;
	}
}

// 프로세서를 사용할 수 있는 시간이 다 되었는지 여부를 반환
BOOL kIsProcessorTimeExpired(void)
{
	if( gs_stScheduler.iProcessorTime <= 0 )
	{
		return TRUE;
	}
	return FALSE;
}


BOOL kEndTask( QWORD qwTaskID )
{

	TCB* pstTarget;
	BYTE bPriority;
	BYTE bPreviousFlag;

	// 현재 태스크는 뭐냐
	// 임계 영역 시작
	bPreviousFlag = kLockForSystemData();

	pstTarget = gs_stScheduler.pstRunningTask;
	// 현재 실행 중인 태스크면 EndTask 비트를 설정하고 태스크를 전환
	if( pstTarget->stNode.qwID == qwTaskID )
	{
		pstTarget->qwFlags |= TASK_FLAGS_ENDTASK;
		SETPRIORITY( pstTarget->qwFlags , TASK_FLAGS_WAIT );

		// 임계 영역 끝.
		kUnlockForSystemData(bPreviousFlag);

		kSchedule();

		// 태스크가 전환 됬으면 아래코드 실행 X
		while(1);
	}
	// 실행중인 태스크가 아니면 준비 큐에서 직접 찾아서 대기 리스트에 연결.
	else
	{
		// 준비 리스트에서 태스크를 찾지 못하면 직접 태스크를 찾아서 태스크 종료 비트를 설정
		pstTarget = kRemoveTaskFromReadyList(qwTaskID);
		if( pstTarget == NULL )
		{
			pstTarget = kGetTCBInTCBPool( GETTCBOFFSET(qwTaskID) );
			if( pstTarget != NULL )
			{
				pstTarget->stNode.qwID |= TASK_FLAGS_ENDTASK;
				SETPRIORITY( pstTarget->qwFlags , TASK_FLAGS_WAIT );
			}
			// 스케줄링(kSchedule())에서 대기 큐로 넣기때문에 FALSE를 반환하는듯 함.

			// 임계영역 끝
			kUnlockForSystemData(bPreviousFlag);
			return FALSE;
		}

		// 준비 리스트에서 태스크를 찾았으면 준비리스트에서 빼서 설정하고 대기리스로 넣음.
		pstTarget->stNode.qwID |= TASK_FLAGS_ENDTASK;
		SETPRIORITY( pstTarget->qwFlags, TASK_FLAGS_WAIT );
		kAddListToTail(&(gs_stScheduler.stWaitList), pstTarget );

	}

	// 임계 영역 끝
	kUnlockForSystemData(bPreviousFlag);
	return TRUE;

}


// 태스크가 자신을 종료함 ( 지금 실행중인 태스크 종료  )
void kExitTask(void)
{
	kEndTask( gs_stScheduler.pstRunningTask->stNode.qwID );
}



// 준비 큐에 있는 모든 태스크의 수를 반환
int kGetReadyTaskCount( void )
{
	int iTotalCount = 0;
	int i;
	BOOL bPreviousFlag;

	// 임계영역 시작
	bPreviousFlag = kLockForSystemData();
	// 모든 큐를 확인하여 태스크 개수를 구함
	for( i=0; i<TASK_MAXREADYLISTCOUNT; i++)
	{
		iTotalCount += kGetListCount(&(gs_stScheduler.vstReadyList[i]) );
	}

	// 임계 영역 끝
	kUnlockForSystemData(bPreviousFlag);

	return iTotalCount;

}


// 전체 태스크 수를 반환
int kGetTaskCount(void)
{
	int iTotalCount;
	BYTE bPreviousFlag;

	//준비 큐의 태스크 수를 구한 후 대기 큐의 태스크 수와 현재 수행 중인 태스크 수를 더함.
	// 임계영역 시작
	bPreviousFlag = kLockForSystemData();

	iTotalCount = kGetReadyTaskCount();
	iTotalCount += kGetListCount( &(gs_stScheduler.stWaitList) )+1;

	// 임계 영역 끝
	kUnlockForSystemData(bPreviousFlag);

	return iTotalCount;
}

// TCB 풀에서 해당 오프셋의 TCB를 반환
TCB* kGetTCBInTCBPool( int iOffset )
{
	if( ( iOffset < -1 ) || ( iOffset> TASK_MAXCOUNT ) )
	{
		return NULL;
	}
	return &(gs_stTCBPoolManager.pstStartAddress[iOffset]);
}

// 태스크가 존재하는지 여부를 반환
BOOL kIsTaskExist( QWORD qwID )
{
	TCB* pstTCB;

	// ID로 TCB를 반환
	pstTCB = kGetTCBInTCBPool(GETTCBOFFSET(qwID));
	// TCB가 TCB풀에 없거나 ID가 일치하지 않으면 존재하지 않는 것이다.
	if( pstTCB == NULL || pstTCB->stNode.qwID != qwID )
	{
		return FALSE;
	}
	return TRUE;
}

// 프로세서의 사용률을 반환
QWORD kGetProcessorLoad( void )
{
	return gs_stScheduler.qwProcessorLoad;
}



//================================================================================
// 유휴 태스크 관련
//================================================================================
// 유휴 태스크
//		대기 큐에 삭제 대기 중인 태스크를 정리
void kIdleTask( void )
{
	TCB* pstTask;
	QWORD qwLastMeasureTickCount, qwLastSpendTickInIdleTask;
	QWORD qwCurrentMeasureTickCount, qwCurrentSpendTickInIdleTask;
	BYTE bPreviousFlag;
	QWORD qwTaskID;

	// 프로세서 사용량을 계산을 위해 기준 정보를 저장
	// gs_stScheduler.qwSpendProcessorTimeInIdleTask는 유휴 태스크가 프로세서를 다 사용하여 인터럽트가 발생할때 마다 값이 증가.
	qwLastSpendTickInIdleTask = gs_stScheduler.qwSpendProcessorTimeInIdleTask;
	qwLastMeasureTickCount = kGetTickCount();

	while(1)
	{

		qwCurrentMeasureTickCount = kGetTickCount();
		qwCurrentSpendTickInIdleTask = gs_stScheduler.qwSpendProcessorTimeInIdleTask;

		// 100-(유휴 태스크가 사용한 프로세서 시간)*100 / ( 시스템 전체에서 사용한 프로세서 시간 )
		if( qwCurrentMeasureTickCount - qwLastMeasureTickCount == 0 )
		{
			gs_stScheduler.qwProcessorLoad = 0;
		}
		else
		{
			gs_stScheduler.qwProcessorLoad = 100 - (qwCurrentSpendTickInIdleTask - qwLastSpendTickInIdleTask)*100
				/(qwCurrentMeasureTickCount-qwLastMeasureTickCount);
		}

		// 현재 상태를 이전 상태에 보관
		qwLastMeasureTickCount = qwCurrentMeasureTickCount;
		qwLastSpendTickInIdleTask = qwCurrentSpendTickInIdleTask;

		// 프로세서의 부하에 따라 쉬게 함
		kHaltProcessorByLoad();

		// 대기 큐에 대기 중인 태스크가 있으면 태스크를 종료함
		if( kGetListCount( &(gs_stScheduler.stWaitList)) >= 0 )
		{
			while(1)
			{
				// 임계 영역 시작
				bPreviousFlag = kLockForSystemData();

				pstTask = kRemoveListFromHeader( &(gs_stScheduler.stWaitList) );
				if( pstTask == NULL )
				{
					// 임계 영역 끝
					kUnlockForSystemData(bPreviousFlag);
					break;
				}
				qwTaskID = pstTask->stNode.qwID;
				kFreeTCB( qwTaskID );

				// 임계 영역 끝
				kUnlockForSystemData(bPreviousFlag);

				kPrintf("IDLE: Task ID[0x%q] is Completely ended. \n", pstTask->stNode.qwID );

			}
		}
		kSchedule();
	}
}


// 측정된 프로세서 부하에 따라 프로세서를 쉬게 함
void kHaltProcessorByLoad( void )
{
	if( gs_stScheduler.qwProcessorLoad < 40 )
	{
		kHlt();
		kHlt();
		kHlt();
	}
	else if( gs_stScheduler.qwProcessorLoad < 80 )
	{
		kHlt();
		kHlt();
	}
	else if( gs_stScheduler.qwProcessorLoad < 95 )
	{
		kHlt();
	}
}



