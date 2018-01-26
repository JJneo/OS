/*
 * Queue.h
 *
 *  Created on: 2018. 1. 26.
 *      Author: Administrator
 */

#ifndef __QUEUE_H__
#define __QUEUE_H__

#include "types.h"

// ����ü
// 1����Ʈ�� ����
#pragma pack(push,1)


//ť�� ���� ����ü
typedef struct kQueueManagerStruct
{
	// ť�� �����ϴ� ������ �ϳ��� ũ��� �ִ� ����
	int iDataSize;
	int iMaxDataCount;

	// ť ������ �����Ϳ� ����/���� �ε���
	void* pvQueueArray;
	int iPutIndex;
	int iGetIndex;

	// ť�� ����� ������ ����� ���������� ����.
	BOOL bLastOperationPut;

}QUEUE;

#pragma pack(pop)

// �Լ�
void kInitializeQueue( QUEUE* pstQueue, void* pvQueueBuffer, int iMaxDataCount, int iDataSize);
BOOL kIsQueueFull( const QUEUE* pstQueue);
BOOL kIsQueueEmpty( const QUEUE* pstQueue);
BOOL kPutQueue(QUEUE* pstQueue, const void* pvData);
BOOL kGetQueue(QUEUE* pstQueue, void* pvData);





#endif /* 02_KERNEL64_SOURCE_QUEUE_H_ */
