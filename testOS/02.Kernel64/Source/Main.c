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



// main 함수
void Main( void )
{
	char vcTemp[2] = {0,};
	BYTE bFlags;
	BYTE bTemp;
	int i=0;


	//             40칸  "                                         [
	kPrintString(46, 10,"PASS");
	kPrintString(0, 11, "Switch To IA-32e Mode Success~!!" );
	kPrintString(0, 12, "IA-32e C Language Kernel Start....           [PASS]" );

	// GDT, TSS, IDT 관련 설정.
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


	kPrintString(0, 16, "Keyboard Activate....                        [    ]" );

	// 키보드 활성화
	if( kActivateKeyboard() == TRUE )
	{
		kPrintString(46,16,"PASS");
		kChangeKeyboardLED( FALSE, FALSE, FALSE );
	}
	else
	{
		kPrintString( 45, 16, "FAIL" );
		while(1);
	}


	// PIC를 초기화하는 kInitializePIC()를 호출하고 PIC 컨트롤러에 연결된 모든 디바이스에 대해 인터럽트를 발생할 수 있도록
	// kMaskPICInterrupt() 함수를 호출합니다. 마지막으로 kEnableInterrupt()를 호출하여 프로세스가 인터럽트를 처리하게 만든다
	kPrintString(0, 17, "PIC Controller And Interrupt Initialize....  [    ]");
	// PIC 컨트롤러 초기화 및 모든 인터럽트 활성화
	kInitializePIC();
	kMaskPICInterrupt(0);
	kEnableInterrupt();
	kPrintString(46, 17, "PASS");


	while(1)
	{
		// 출력 버퍼( 포트0x60 )이 차 있으면 스캔 코드를 읽을 수 있음
		if( kIsOutputBufferFull() ==TRUE )
		{
			// 출력 버퍼(포트 0x60)에서 스캔 코드를 읽어서 저장
			bTemp = kGetKeyboardScanCode();

			// 스캔 코드를 ASCII 코드로 변환하는 함수를 호출하여 ASCII 코드와 눌림 또는 떨어짐 정보를 반환
			if( kConvertScanCodeToASCIICode( bTemp, &(vcTemp[0]),&bFlags)==TRUE)
			{
				// 키가 눌려졌으면 키의 ASCII코드 값을 화면에 출력
				if(bFlags & KEY_FLAGS_DOWN )
				{
					kPrintString( i++, 18, vcTemp );
				}

			}

		}

	}

	
}


// 문자열 출력 함수

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
