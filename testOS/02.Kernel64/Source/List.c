/*
 * List.c
 *
 *  Created on: 2018. 2. 5.
 *      Author: Administrator
 */

#include "List.h"

// ����Ʈ�� �����ϴ� �ڷᱸ�� �ʱ�ȭ
void kInitializeList( LIST* pstList )
{
	pstList->iItemCount = 0;
	pstList->pvHeader = NULL;
	pstList->pvTail = NULL;
}

// ����Ʈ�� ���Ե� ������ ���� ��ȯ
int kGetListCount( const LIST* pstList )
{
	return pstList->iItemCount;
}

// ����Ʈ�� ��带 �߰���
void kAddListToTail( LIST* pstList, void* pvItem)
{
	LISTNODE* pstNode;

	// ���� �������� ��巹���� ����(NULL)�� ����
	pstNode = (LISTNODE*) pvItem;
	pstNode->pvNext=NULL;

	// ����Ʈ�� �� �����̸� header�� Tail�� �߰��� �����ͷ� ����
	if( pstList->pvHeader == NULL )
	{
		pstList->pvHeader = pvItem;
		pstList->pvTail = pvItem;
		pstList->iItemCount = 1;

		return;
	}

	// �װ� �ƴ϶�� ������ �������� LISTNODE�� ��ġ�� ���Ͽ� ���� �����͸� �߰��� �����ͷ� ����
	pstNode = (LISTNODE*) pstList->pvTail;
	pstNode->pvNext = pvItem;

	// ����Ʈ�� ������ �����͸� �߰��� �����ͷ� ����
	pstList->pvTail = pvItem;
	pstList->iItemCount++;
}

// ����Ʈ�� ù �κп� �����͸� ����
void kAddListToHeader( LIST* pstList, void* pvItem )
{
	LISTNODE* pstNode;

	// ���� �������� ��巹���� Header�� ����.
	pstNode = (LISTNODE*) pvItem;
	pstNode->pvNext = pstList->pvHeader;

	// ����Ʈ�� �� ���¶�� Header�� Tail �߰��� �����ͷ� ����
	if( pstList->pvHeader == NULL )
	{
		pstList->pvHeader = pvItem;
		pstList->pvTail = pvItem;
		pstList->iItemCount = 1;
		return;
	}

	// ����Ʈ�� ù ��° �����͸� �߰��� �����ͷ� ����
	pstList->pvHeader = pvItem;
	pstList->iItemCount++;
}

// ����Ʈ���� �����͸� ������ ��, �������� �����͸� ��ȯ
void* kRemoveList( LIST* pstList, QWORD qwID )
{
	LISTNODE* pstNode;
	LISTNODE* pstPreviousNode;

	pstPreviousNode = (LISTNODE*) pstList->pvHeader;
	for( pstNode = pstPreviousNode; pstNode != NULL; pstNode = pstNode->pvNext )
	{
		// ��ġ�ϴ� ID�� �ִٸ� ����
		if( pstNode->qwID == qwID )
		{
			// ���� �����Ͱ� �ϳ��ۿ� ���ٸ� ����Ʈ �ʱ�ȭ
			if( (pstNode == pstList->pvHeader ) && ( pstNode == pstList->pvTail) )
			{
				pstList->pvHeader = NULL;
				pstList->pvTail = NULL;
			}
			// ���� ����Ʈ�� ù ��° �����͸� Header�� �ι�° �����ͷ� ����
			else if( pstNode == pstList->pvHeader )
			{
				pstList->pvHeader = pstNode->pvNext;
			}
			// ���� ����Ʈ�� ������ �����͸� Tail�� ������ ������ �����ͷ� ����
			else if( pstNode == pstList->pvTail )
			{
				pstList->pvTail = pstPreviousNode;
			}
			else
			{
				pstPreviousNode->pvNext = pstNode->pvNext;
			}
			pstList->iItemCount--;
			return pstNode;
		}
		pstPreviousNode = pstNode;
	}
	return NULL;
}

// ����Ʈ�� ù ��° �����͸� �����Ͽ� ��ȯ
void* kRemoveListFromHeader( LIST* pstList )
{
	LISTNODE* pstNode;
	if(pstList->iItemCount == 0 )
	{
		return NULL;
	}

	// ����� �����ϰ� ��ȯ
	pstNode = (LISTNODE*) pstList->pvHeader;
	return kRemoveList( pstList, pstNode->qwID );
}

// ����Ʈ�� ������ �����͸� �����Ͽ� ��ȯ
void* kRemoveListFromTail( LIST* pstList )
{
	LISTNODE* pstNode;
	if( pstList->iItemCount == 0 )
	{
		return 0;
	}

	pstNode = (LISTNODE*) pstList->pvTail;
	return kRemoveList( pstList, pstNode->qwID );
}

// ����Ʈ���� ���̵� ã��
void* kFindList( const LIST* pstList, QWORD qwID )
{
	LISTNODE* pstNode;

	for( pstNode = (LISTNODE*) pstList->pvHeader; pstNode != NULL; pstNode= pstNode->pvNext )
	{
		if( pstNode->qwID == qwID )
		{
			return pstNode;
		}
	}
	return NULL;
}

// ����Ʈ�� ����� ��ȯ
void* kGetHeaderFromList( const LIST* pstList )
{
	return pstList->pvHeader;
}

// ����Ʈ�� ������ ��ȯ
void* kGetTailFromList( const LIST* pstList )
{
	return pstList->pvTail;
}

// ���� �������� ���� �������� ��ȯ
void* kGetNextFromList( const LIST* pstList, void* pstCurrent )
{
	LISTNODE* pstNode;
	pstNode = (LISTNODE*) pstCurrent;
	return pstNode->pvNext;
}














