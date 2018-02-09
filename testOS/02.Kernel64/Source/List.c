/*
 * List.c
 *
 *  Created on: 2018. 2. 5.
 *      Author: Administrator
 */

#include "List.h"

// 리스트를 관리하는 자료구조 초기화
void kInitializeList( LIST* pstList )
{
	pstList->iItemCount = 0;
	pstList->pvHeader = NULL;
	pstList->pvTail = NULL;
}

// 리스트에 포함된 아이템 수를 반환
int kGetListCount( const LIST* pstList )
{
	return pstList->iItemCount;
}

// 리스트에 노드를 추가함
void kAddListToTail( LIST* pstList, void* pvItem)
{
	LISTNODE* pstNode;

	// 다음 데이터의 어드레스를 없음(NULL)로 설정
	pstNode = (LISTNODE*) pvItem;
	pstNode->pvNext=NULL;

	// 리스트가 빈 상태이면 header과 Tail을 추가한 데이터로 설정
	if( pstList->pvHeader == NULL )
	{
		pstList->pvHeader = pvItem;
		pstList->pvTail = pvItem;
		pstList->iItemCount = 1;

		return;
	}

	// 그게 아니라면 마지막 데이터의 LISTNODE의 위치를 구하여 다음 데이터를 추가한 데이터로 설정
	pstNode = (LISTNODE*) pstList->pvTail;
	pstNode->pvNext = pvItem;

	// 리스트의 마지막 데이터를 추가한 데이터로 변경
	pstList->pvTail = pvItem;
	pstList->iItemCount++;
}

// 리스트의 첫 부분에 데이터를 더함
void kAddListToHeader( LIST* pstList, void* pvItem )
{
	LISTNODE* pstNode;

	// 다음 데이터의 어드레스를 Header로 설정.
	pstNode = (LISTNODE*) pvItem;
	pstNode->pvNext = pstList->pvHeader;

	// 리스트가 빈 상태라면 Header과 Tail 추가한 데이터로 설정
	if( pstList->pvHeader == NULL )
	{
		pstList->pvHeader = pvItem;
		pstList->pvTail = pvItem;
		pstList->iItemCount = 1;
		return;
	}

	// 리스트의 첫 번째 데이터를 추가한 데이터로 변경
	pstList->pvHeader = pvItem;
	pstList->iItemCount++;
}

// 리스트에서 데이터를 제거한 후, 데이터의 포인터를 반환
void* kRemoveList( LIST* pstList, QWORD qwID )
{
	LISTNODE* pstNode;
	LISTNODE* pstPreviousNode;

	pstPreviousNode = (LISTNODE*) pstList->pvHeader;
	for( pstNode = pstPreviousNode; pstNode != NULL; pstNode = pstNode->pvNext )
	{
		// 일치하는 ID가 있다면 제거
		if( pstNode->qwID == qwID )
		{
			// 만약 데이터가 하나밖에 없다면 리스트 초기화
			if( (pstNode == pstList->pvHeader ) && ( pstNode == pstList->pvTail) )
			{
				pstList->pvHeader = NULL;
				pstList->pvTail = NULL;
			}
			// 만약 리스트의 첫 번째 데이터면 Header를 두번째 데이터로 변경
			else if( pstNode == pstList->pvHeader )
			{
				pstList->pvHeader = pstNode->pvNext;
			}
			// 만약 리스트의 마지막 데이터면 Tail을 마지막 이전의 데이터로 변경
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

// 리스트의 첫 번째 데이터를 제거하여 반환
void* kRemoveListFromHeader( LIST* pstList )
{
	LISTNODE* pstNode;
	if(pstList->iItemCount == 0 )
	{
		return NULL;
	}

	// 헤더를 제거하고 반환
	pstNode = (LISTNODE*) pstList->pvHeader;
	return kRemoveList( pstList, pstNode->qwID );
}

// 리스트의 마지막 데이터를 제거하여 반환
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

// 리스트에서 아이디를 찾음
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

// 리스트의 헤더를 반환
void* kGetHeaderFromList( const LIST* pstList )
{
	return pstList->pvHeader;
}

// 리스트의 테일을 반환
void* kGetTailFromList( const LIST* pstList )
{
	return pstList->pvTail;
}

// 현재 아이템의 다음 아이템을 반환
void* kGetNextFromList( const LIST* pstList, void* pstCurrent )
{
	LISTNODE* pstNode;
	pstNode = (LISTNODE*) pstCurrent;
	return pstNode->pvNext;
}














