/*
 * Types.h
 *
 *  Created on: 2017. 12. 19.
 *      Author: user
 */

#ifndef TYPES_H_
#define TYPES_H_

#define BYTE	unsigned char
#define WORD	unsigned short
#define DWORD	unsigned int
#define QWORD	unsigned long
#define BOOL	unsigned char

#define TRUE	1
#define FALSE	0
#define	NULL	0

#pragma pack( push, 1 )

// ���� ��� �� �ؽ�Ʈ ��� ȭ���� �����ϴ� �ڷᱸ��
typedef struct kCharactorStruct
{
	BYTE bCharactor;
	BYTE bAttribute;
}CHARACTER;

#pragma pack(pop)

#endif /* TYPES_H_ */