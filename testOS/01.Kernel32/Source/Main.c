/*
 * Main.c
 *
 *  Created on: 2017. 12. 19.
 *      Author: user
 */


#include "Types.h"

void kPrintString( int iX, int iY, const char* pcString );
BOOL kInitializeKernel64Area( void );
BOOL kIsMemoryEnough( void );


// main �Լ�
void Main( void )
{
	DWORD i;
	// ���ۺ��� [ ������ 40ĭ
	kPrintString(0, 3, "C Language Kernel loading....           [PASS]" );

	//IA-32e ����� Ŀ�� ������ �ʱ�ȭ �ϱ� ���� �ּ� �޸� ũ�⸦ �����ϴ��� �˻�
	kPrintString(0, 4, "Minimum Memory Size Check....           [    ]" );
	if( kIsMemoryEnough() == FALSE )
	{
		kPrintString( 41, 4, "Fail" );
		kPrintString( 0, 5, "Not Enough Memory. testOS Requires Over 64MB Memory");
		while(1);
	}
	else
	{
		kPrintString( 41, 4, "PASS");

	}

	//IA-32e ����� Ŀ�� ������ �ʱ�ȭ
	kInitializeKernel64Area();
	kPrintString(0, 5, "IA_43e Kernel Area Initialization....   [    ]" );
	if( kInitializeKernel64Area() == FALSE )
	{
		kPrintString(41, 5,"FAIL");
		kPrintString(0, 6, "Kernel Area Initialization Fail...");
		while(1);
	}
	kPrintString(41,5,"PASS");

	while(1);
}

// ���ڿ� ��� �Լ�

void kPrintString( int iX, int iY, const char* pcString )
{
	CHARACTER* pstScreen = ( CHARACTER* ) 0xB8000;
	int i;

	pstScreen += ( iY*80 ) + iX;
	for(i=0; pcString[i] != 0; i++)
	{
		pstScreen[i].bCharactor = pcString[i];
	}

}

// IA-32e ���� Ŀ�� ������ 0���� �ʱ�ȭ
BOOL kInitializeKernel64Area( void )
{
	DWORD* pdwCurrentAddress;

	//�ʱ�ȭ�� ������ ��巹���� 0x100000(1MB)�� ����
	pdwCurrentAddress = ( DWORD* ) 0x100000;

	//������ ��巹���� 0x600000(6M)���� ������ ���鼭 4����Ʈ�� 0���� ä��
	while( ( DWORD ) pdwCurrentAddress < 0x600000 )
	{
		*pdwCurrentAddress = 0x00;

		// 0���� ������ �� �ٽ� �о��� �� 0�� ������ ������ �ش� ��巹���� ����ϴµ� ������ ���� �������� ���̻� �������� �ʰ� ����
		if( *pdwCurrentAddress != 0 )
		{
			return FALSE;
		}

		// ���� ��巹���� �̵�
		pdwCurrentAddress++;
	}

	return TRUE;


}

//testOS�� �����ϱ⿡ ����� �޸𸮸� ������ �ִ��� üũ
BOOL kIsMemoryEnough(void)
{
	DWORD* pdwCurrentAddress;

	// 0x100000(1M)���� �˻� ����
	pdwCurrentAddress = ( DWORD* ) 0x100000;
	// 0x4000000(64MB)���� ������ ���鼭 Ȯ��.
	while( (DWORD) pdwCurrentAddress < 0x4000000 )
	{
		*pdwCurrentAddress = 0x12345678;

		//0x12345678�� ������ �� �ٽ� �о��� �� 0x12345678�� �ƴϸ� �ش� ��巹���� ����ϴµ� ������ ����Ŵ� ����
		if( *pdwCurrentAddress != 0x12345678 )
		{
			return FALSE;
		}

		// 1MB�� �̵��ϸ鼭 �ݺ�
		pdwCurrentAddress += ( 0x100000 / 4);

	}

	return TRUE;
}