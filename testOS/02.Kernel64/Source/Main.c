/*
 * Main.c
 *
 *  Created on: 2017. 12. 19.
 *      Author: user
 */


#include "Types.h"
#include "Keyboard.h"
#include "Descriptor.h"
#include "PIC.h"


void kPrintString( int iX, int iY, const char* pcString );



// main �Լ�
void Main( void )
{
	char vcTemp[2] = {0,};
	BYTE bFlags;
	BYTE bTemp;
	int i=0;

	KEYDATA stData;


	//             40ĭ  "                                         [
	kPrintString(46, 10,"PASS");
	kPrintString(0, 11, "Switch To IA-32e Mode Success~!!" );
	kPrintString(0, 12, "IA-32e C Language Kernel Start....           [PASS]" );

	// GDT, TSS, IDT ���� ����.
	kPrintString(0, 13, "GDT Initialize And Switch For IA-32e Mode....[    ]");
	kInitializeGDTTableAndTSS();
	kLoadGDTR(GDTR_STARTADDRESS);
	kPrintString(46, 13, "PASS");

	kPrintString(0, 14, "TSS Segment Load....                         [    ]");
	kLoadTR( GDT_TSSSEGMENT);
	kPrintString(46, 14, "PASS");

	kPrintString(0, 15, "IDT Initialize....                           [    ]");
	kInitializeIDTTables();
	kLoadIDTR(IDTR_STARTADDRESS);
	kPrintString(46, 15, "PASS");


	kPrintString(0, 16, "Keyboard Activate And Queue Initialize....   [    ]" );

	// Ű���� Ȱ��ȭ
	if( kInitializeKeyboard() == TRUE )
	{
		kPrintString(46,16,"PASS");
		kChangeKeyboardLED( FALSE, FALSE, FALSE );
	}
	else
	{
		kPrintString( 45, 16, "FAIL" );
		while(1);
	}


	// PIC�� �ʱ�ȭ�ϴ� kInitializePIC()�� ȣ���ϰ� PIC ��Ʈ�ѷ��� ����� ��� ����̽��� ���� ���ͷ�Ʈ�� �߻��� �� �ֵ���
	// kMaskPICInterrupt() �Լ��� ȣ���մϴ�. ���������� kEnableInterrupt()�� ȣ���Ͽ� ���μ����� ���ͷ�Ʈ�� ó���ϰ� �����
	kPrintString(0, 17, "PIC Controller And Interrupt Initialize....  [    ]");
	// PIC ��Ʈ�ѷ� �ʱ�ȭ �� ��� ���ͷ�Ʈ Ȱ��ȭ
	kInitializePIC();
	kMaskPICInterrupt(0);
	kEnableInterrupt();
	kPrintString(46, 17, "PASS");


	while(1)
	{
		// Ű ť�� �����Ͱ� ������ Ű�� ó��
		if( kGetKeyFromKeyQueue(&stData)==TRUE)
		{
			// Ű�� ���������� Ű�� ASCII �ڵ� ���� ȭ�鿡 ���
			if(stData.bFlags & KEY_FLAGS_DOWN)
			{
				//Ű �������� ASCII �ڵ� ���� ����
				vcTemp[0] = stData.bASCIICode;
				kPrintString( i++, 18, vcTemp );
				// 0�� �ԷµǸ� ������ 0���� ������ Divide Error ���� (���� 0��)�� �߻���Ŵ
				if( vcTemp[0] == '0')
				{
					// �Ʒ� �ڵ带 �����ϸ� Divide Error ���ܰ� �߻��Ͽ� Ŀ���� �ӽ� ��鷯�� �����
					bTemp=bTemp/0;
				}
			}
		}


	}

	
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
