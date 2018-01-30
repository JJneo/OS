/*
 * Task.c
 *
 *  Created on: 2018. 1. 30.
 *      Author: Administrator
 */


#include "Task.h"
#include "Descriptor.h"

// �Ķ���͸� �̿��ؼ� TCB�� ����
void kSetUpTask( TCB* pstTCB, QWORD qwID, QWORD qwFlags, QWORD qwEntryPointAddress,
				void* pvStackAddress, QWORD qwStackSize)
{
	//���ý�Ʈ �ʱ�ȭ
	kMemSet( pstTCB->stContext.vqRegister, 0, sizeof( pstTCB->stContext.vqRegister) );

	// ���ÿ� ���õ� RSP, RBP �������� ����
	pstTCB->stContext.vqRegister[TASK_RSPOFFSET] = (QWORD) pvStackAddress+qwStackSize;
	pstTCB->stContext.vqRegister[TASK_RBPOFFSET] = (QWORD) pvStackAddress+qwStackSize;


	// ���׸�Ʈ ������ ����
	pstTCB->stContext.vqRegister[TASK_CSOFFSET] = GDT_KERNELCODESEGMENT;
	pstTCB->stContext.vqRegister[TASK_DSOFFSET] = GDT_KERNELDATASEGMENT;
	pstTCB->stContext.vqRegister[TASK_ESOFFSET] = GDT_KERNELDATASEGMENT;
	pstTCB->stContext.vqRegister[TASK_FSOFFSET] = GDT_KERNELDATASEGMENT;
	pstTCB->stContext.vqRegister[TASK_GSOFFSET] = GDT_KERNELDATASEGMENT;
	pstTCB->stContext.vqRegister[TASK_SSOFFSET] = GDT_KERNELDATASEGMENT;


	// RIP �������Ϳ� ���ͷ�Ʈ �÷��� ����
	pstTCB->stContext.vqRegister[TASK_RIPOFFSET] = qwEntryPointAddress;

	// RFLAGS ���������� IF ��Ʈ(��Ʈ9)�� 1�� �����Ͽ� ���ͷ�Ʈ Ȱ��ȭ
	pstTCB->stContext.vqRegister[TASK_RFLAGSOFFSET] |= 0x0200;

	//ID �� ����, �׸��� �÷��� ����
	pstTCB->qwID = qwID;
	pstTCB->pvStackAddress = pvStackAddress;
	pstTCB->qwStackSize= qwStackSize;
	pstTCB->qwFlags = qwFlags;


}