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
#include "Console.h"
#include "ConsoleShell.h"

#include "Task.h"
#include "PIT.h"



// main 함수
void Main( void )
{
	int iCursorX, iCursorY;

	// 콘솔으 먼저 초기화한 후  다음 작업을 수행, P-47째...
	kInitializeConsole(0,10);
	kPrintf("-----------------------------------------------------\n");
	kPrintf("----------  Switch To IA-32e Mode Success  ----------\n");
	kPrintf("-----------------------------------------------------\n");
	kPrintf("IA-32e C Language Kernel Start....             [PASS]\n");
	kPrintf("Initialize Console....                         [PASS]\n");

	// 부팅 상황을 화면에 출력
	kGetCursor(&iCursorX, &iCursorY);
	kPrintf("GDT Initialize And Switch For IA-32e Mode....  [    ]\n");
	kInitializeGDTTableAndTSS();
	kLoadGDTR(GDTR_STARTADDRESS);
	kSetCursor(48, iCursorY++);
	kPrintf("PASS\n");

	kPrintf("TSS Segment Load....                           [    ]\n");
	kLoadTR( GDT_TSSSEGMENT);
	kSetCursor(48, iCursorY++);
	kPrintf("PASS\n");

	kPrintf("IDT Initialize....                             [    ]\n");
	kInitializeIDTTables();
	kLoadIDTR(IDTR_STARTADDRESS);
	kSetCursor(48, iCursorY++);
	kPrintf("PASS\n");


	kPrintf("Total RAM Size Check....                       [    ]\n");
	kCheckTotalRAMSize();
	kSetCursor(48, iCursorY++);
	kPrintf("PASS], Size=%dMB\n",kGetTotalRAMSize());




	kPrintf("TCB Pool And Scheduler Initialize....          [PASS]\n");
	iCursorY++;
	kInitializeScheduler();
	// 1ms당 한번씩 인터럽트가 발생하도록 설정
	kInitializePIT( MSTOCOUNT(1), 1 );



	kPrintf("Keyboard Activate And Queue Initialize....     [    ]\n");

	// 키보드 활성화
	if( kInitializeKeyboard() == TRUE )
	{
		kSetCursor(48, iCursorY++);
		kPrintf("PASS\n");
		kChangeKeyboardLED( FALSE, FALSE, FALSE );
	}
	else
	{
		kSetCursor(48, iCursorY++);
		kPrintf("FAIL\n");
		while(1);
	}


	// PIC를 초기화하는 kInitializePIC()를 호출하고 PIC 컨트롤러에 연결된 모든 디바이스에 대해 인터럽트를 발생할 수 있도록
	// kMaskPICInterrupt() 함수를 호출합니다. 마지막으로 kEnableInterrupt()를 호출하여 프로세스가 인터럽트를 처리하게 만든다
	kPrintf("PIC Controller And Interrupt Initialize....    [    ]\n");

	// PIC 컨트롤러 초기화 및 모든 인터럽트 활성화
	kInitializePIC();
	kMaskPICInterrupt(0);
	kEnableInterrupt();
	kSetCursor(48, iCursorY++);
	kPrintf("PASS\n");


	// 셸을 시작
	kCreateTask(TASK_FLAGS_LOWEST|TASK_FLAGS_IDLE,  (QWORD) kIdleTask);
	kStartConsoleShell();

}


