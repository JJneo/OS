/*
 * List.h
 *
 *  Created on: 2018. 2. 5.
 *      Author: Administrator
 */

#ifndef __LIST_H__
#define __LIST_H__


#include "Types.h"

////////////////////////////////////////////////////////////////////////////////
//
// ����ü
//
////////////////////////////////////////////////////////////////////////////////
// 1����Ʈ�� ����
#pragma pack( push, 1 )

// �����͸� �����ϴ� �ڷᱸ��
// �ݵ�� �������� ���� �պκп� ��ġ�ؾ� ��
typedef struct kListLinkStruct
{
    // ���� �������� ��巹���� �����͸� �����ϱ� ���� ID
    void* pvNext;
    QWORD qwID;
} LISTNODE;


/*
// ����Ʈ�� ����� �����͸� �����ϴ� ��
// �ݵ�� ���� �պκ��� LISTLINK�� �����ؾ� ��
struct kListItemExampleStruct
{
    // ����Ʈ�� �����ϴ� �ڷᱸ��
    LISTNODE stNode;

    // �����͵�
    int iData1;
    char cData2;
};
*/


// ����Ʈ�� �����ϴ� �ڷᱸ��
typedef struct kListManagerStruct
{
    // ����Ʈ �������� ��
    int iItemCount;

    // ����Ʈ�� ù ��°�� ������ �������� ��巹��
    void* pvHeader;
    void* pvTail;
} LIST;

#pragma pack( pop )


////////////////////////////////////////////////////////////////////////////////
//
// �Լ�
//
////////////////////////////////////////////////////////////////////////////////
void kInitializeList( LIST* pstList );
int kGetListCount( const LIST* pstList );
void kAddListToTail( LIST* pstList, void* pvItem );
void kAddListToHeader( LIST* pstList, void* pvItem );
void* kRemoveList( LIST* pstList, QWORD qwID );
void* kRemoveListFromHeader( LIST* pstList );
void* kRemoveListFromTail( LIST* pstList );
void* kFindList( const LIST* pstList, QWORD qwID );
void* kGetHeaderFromList( const LIST* pstList );
void* kGetTailFromList( const LIST* pstList );
void* kGetNextFromList( const LIST* pstList, void* pstCurrent );



#endif /* 02_KERNEL64_SOURCE_LIST_H_ */