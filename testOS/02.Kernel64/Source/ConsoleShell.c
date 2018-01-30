/*
 * ConsoleShell.c
 *
 *  Created on: 2018. 1. 27.
 *      Author: Administrator
 */

#include "ConsoleShell.h"
#include "Console.h"
#include "Keyboard.h"
#include "Utility.h"
#include "PIT.h"
#include "RTC.h"
#include "AssemblyUtility.h"
#include "Task.h"

// Ŀ�ǵ� ���̺� ����
SHELLCOMMANDENTRY gs_vstCommandTable[] =
{
		{"help", "Show Help", kHelp },
		{"cls", "Clear Screen", kCls },
		{"totalram", "Show Total RAM Size", kShowTotalRAMSize},
		{"strtod","String to Decimal/Hex Convert", kStringToDecimalHexTest },
		{"shutdown", "Shutdown And ReBoot OS", kShutdown},
		{ "settimer", "Set PIT Controller Counter0, ex)settimer 10(ms) 1(periodic)",
		                kSetTimer },
		{ "wait", "Wait ms Using PIT, ex)wait 100(ms)", kWaitUsingPIT },
	    { "rdtsc", "Read Time Stamp Counter", kReadTimeStampCounter },
		{ "cpuspeed", "Measure Processor Speed", kMeasureProcessorSpeed },
		{ "date", "Show Date And Time", kShowDateAndTime },
		{ "createtask", "Create Task", kCreateTestTask },
};

//===============================================================
// ���� ���� �����ϴ� �ڵ�
//===============================================================
// ���� ���� ����
void kStartConsoleShell(void)
{
	char vcCommandBuffer[ CONSOLESHELL_MAXCOMMANDBUFFERCOUNT ];
	int iCommandBufferIndex=0;
	BYTE bKey;
	int iCursorX, iCursorY;

	//������Ʈ ���
	kPrintf( CONSOLESHELL_PROMPTMESSAGE );

	while( 1 )
	{
		bKey = kGetCh();
		// Backspace Ű ó��
		if( bKey == KEY_BACKSPACE )
		{
			if( iCommandBufferIndex > 0 )
			{
				// ���� Ŀ�� ��ġ�� �� �� ���� ������ �̵��� ���� ������ ����ϰ� Ŀ�ǵ� ���ۿ��� ������ ���� ����
				kGetCursor( &iCursorX, &iCursorY );
				kPrintStringXY(iCursorX-1, iCursorY, " ");
				kSetCursor( iCursorX-1,iCursorY);
				iCommandBufferIndex--;
			}
		}
		//����Ű ó��
		else if( bKey == KEY_ENTER )
		{
			kPrintf("\n");
			if( iCommandBufferIndex > 0 )
			{
				// Ŀ�ǵ� ���ۿ� �ִ� ������ ����
				vcCommandBuffer[ iCommandBufferIndex ] = '\0';		// Ŀ�ǵ尡 ���ڿ��� ����Ǵ� �ι��ڸ� ��������.
				kExecuteCommand(vcCommandBuffer);

			}

			// ������Ʈ ��� �� Ŀ�ǵ� ���� �ʱ�ȭ
			kPrintf( "%s", CONSOLESHELL_PROMPTMESSAGE);
			kMemSet( vcCommandBuffer,'\0',CONSOLESHELL_MAXCOMMANDBUFFERCOUNT);
			iCommandBufferIndex=0;
		}
		// shift, Caps Lock, Num Lock, Scroll Lock�� ����
		else if( (bKey == KEY_LSHIFT) || (bKey == KEY_RSHIFT) || (bKey == KEY_CAPSLOCK) ||
			     (bKey == KEY_NUMLOCK) || (bKey == KEY_SCROLLLOCK) )
		{
			;
		}
		else
		{
			//TAB�� �������� ��ȯ
			if( bKey == KEY_TAB )
			{
				bKey=' ';
			}
			// ���ۿ� ������ ���� ���� ���� ���� ( �׿� ���ڴ� Ŀ�ǵ� ���ۿ� ���ų��� )
			if( iCommandBufferIndex < CONSOLESHELL_MAXCOMMANDBUFFERCOUNT )
			{
				vcCommandBuffer[iCommandBufferIndex++] = bKey;
				kPrintf("%c",bKey);
			}
		}
	}
}


// Ŀ�ǵ� ���ۿ� �ִ� Ŀ�ǵ带 ���Ͽ� �ش� Ŀ�ǵ带 ó���ϴ� �Լ��� ����
void kExecuteCommand( const char* pcCommandBuffer )
{
	int i;
	int iSpaceIndex;
	int iCommandBufferLength; 	// ó���� Ŀ�ǵ� ���� ����.
	int iCommandLength;			// ��ϵ� Ŀ�ǵ� ���� ����.
	int iCount;					// ��ϵ� Ŀ�ǵ� ����.

	// �������� ���е� Ŀ�ǵ带 ����
	iCommandBufferLength = kStrLen( pcCommandBuffer );
	for( iSpaceIndex = 0; iSpaceIndex < iCommandBufferLength; iSpaceIndex++)
	{
		if( pcCommandBuffer[iSpaceIndex] == ' ')
		{
			break;
		}
	}

	//Ŀ�ǵ� ���̺��� �˻��ؼ� ���� �̸��� Ŀ�ǵ尡 �ִ��� Ȯ��.
	iCount = sizeof(gs_vstCommandTable) / sizeof(SHELLCOMMANDENTRY);
	for( i=0; i<iCount; i++ )
	{
		iCommandLength = kStrLen(gs_vstCommandTable[i].pcCommand);
		// Ŀ�ǵ� ���̿� ������ ������ ��ġ�ϴ��� �˻� ( ���� �������� iSpaceIndex�� �Է� Ŀ�ǵ� ���̸� �����س��� )
		if( (iCommandLength == iSpaceIndex) &&
				(kMemCmp(gs_vstCommandTable[i].pcCommand, pcCommandBuffer, iSpaceIndex) == 0) )
		{
			// (pcCommandBuffer + iSpaceindex) ������ Ŀ�ǵ忡 ���� ��ġ +1�ؼ�  �� ���� ��ġ�� �Լ��� ���ڰ� ���Եȴ�. �� ���ڸ� �б� ���� ��ġ.
			gs_vstCommandTable[i].pfFunction( pcCommandBuffer + iSpaceIndex +1 );
			break;
		}
	}
	// ����Ʈ���� ã�� �� ���ٸ� �ַ� ���.
	if( i >= iCount )
	{
		kPrintf("'%s' is not found.\n",pcCommandBuffer);
	}

}

// �Ķ���� �ڷᱸ���� �ʱ�ȭ
void kInitializeParameter( PARAMETERLIST* pstList, const char* pcParameter )
{
	pstList->pcBuffer = pcParameter;			// �Ķ������ �����ּ�.
	pstList->iLength = kStrLen(pcParameter);	// ��ü �Ķ���� ����
	pstList->iCurrentPosition = 0;				// �Ķ���� ���� ��ġ�� ��Ÿ�� (Offset)
}

// �������� ���е� �Ķ������ ����� ���̸� ��ȯ
int kGetNextParameter( PARAMETERLIST* pstList, char* pcParameter )
{
	int i;
	int iLength;

	// ó����  �Ķ���Ͱ� ������ ����
	if( pstList->iLength <= pstList->iCurrentPosition )
	{
		return 0;
	}
	// ó���� �Ķ���Ͱ� ������ ������ ���̸�ŭ �̵��ϸ鼭 ������ �˻�( ������ ���� �Ķ���� ���� )
	for( i=pstList->iCurrentPosition; i<pstList->iLength; i++)
	{
		if(pstList->pcBuffer[i] == ' ')
		{
			break;
		}
	}

	// �Ķ���͸� �����ϰ� ���̸� ��ȯ
	kMemCpy( pcParameter, pstList->pcBuffer+pstList->iCurrentPosition, i );
	iLength = i - pstList->iCurrentPosition;
	pcParameter[iLength ] = '\0';

	// �Ķ���� ��ġ ������Ʈ
	pstList->iCurrentPosition += iLength+1;
	return iLength;

}



// ===========================================================
// Ŀ�ǵ带 ó���ϴ� �ڵ�
// ===========================================================
// �� ������ ���
void kHelp( const char* pcCommandBuffer )
{
	int i;
	int iCount;
	int iCursorX, iCursorY;
	int iLength, iMaxCommandLength = 0;

	kPrintf("===============================================================\n");
	kPrintf("                       TestOS Shell Help                       \n");
	kPrintf("===============================================================\n");

	iCount = sizeof( gs_vstCommandTable) / sizeof(SHELLCOMMANDENTRY);

	// ���� �� Ŀ�ǵ��� ���̸� ���  ( [Ŀ�ǵ�    - ���� ] �̷� ������ ����� �ǵ� Ŀ�ǵ� �ִ� ���̸� �˾ƾ� - ��ġ�� ���� �� ��������
	for( i=0; i<iCount; i++)
	{
		iLength = kStrLen(gs_vstCommandTable[i].pcCommand );
		if( iLength > iMaxCommandLength )
		{
			iMaxCommandLength = iLength;
		}
	}

	// ���� ���
	for(i=0; i<iCount; i++)
	{
		kPrintf( "%s", gs_vstCommandTable[i].pcCommand );
		kGetCursor(&iCursorX, &iCursorY );
		kSetCursor( iMaxCommandLength, iCursorY );
		kPrintf(" - %s\n",gs_vstCommandTable[i].pcHelp );
	}

}


// ȭ���� ����
void kCls( const char* pcParmeterBuffer )
{
	// �� ������ ����� ������ ����ϹǷ� ȭ���� ���� �� ���� 1�� Ŀ�� �̵�
	kClearScreen();
	kSetCursor(0, 1);
}


// �� �޸� ũ�⸦ ���
void kShowTotalRAMSize( const char* pcParameterBuffer )
{
	kPrintf("Total RAM Size = %d MB\n",kGetTotalRAMSize() );
}


// ���ڿ��� �� ���ڸ� ���ڷ� ��ȯ�Ͽ� ȭ�鿡 ���
void kStringToDecimalHexTest( const char* pcParameterBuffer )
{
	// pcParameterBuffer�� �Ķ������ �����ּҰ� �Ѿ�´�.

	char vcParameter[100];	// ���� �Ķ������ ���۰���.
	int iLength;			// �Ķ������ ����
	PARAMETERLIST stList;	// Ŀ�ǵ忡 ���� �Ķ���͸� �����ϴ� ����ü
	int iCount = 0;
	long lValue;

	// �Ķ���� �����ּ�, ����, ���� �Ķ���� ��ġ�� stList ����ü�� �ʱ�ȭ.
	kInitializeParameter( &stList,  pcParameterBuffer );

	while(1)
	{
		// ���� �Ķ���͸� ����, �Ķ������ ���̰� 0�̸� �Ķ���Ͱ� ���� �������� ����
		iLength = kGetNextParameter(&stList, vcParameter);

		// �Ķ���� ���̰�  0�̸�  �Ķ���Ͱ� �����ϱ� ����
		if(iLength == 0)
		{
			break;
		}

		// �Ķ���Ͱ� ������  �Ķ���Ϳ� ���� ������ ����ϰ� 16�������� 10�������� �Ǵ��Ͽ� ��ȯ �� ����� kPrintf�� ���
		kPrintf("Param %d = '%s', Length = %d, ", iCount+1, vcParameter, iLength );
		// 0x�� �����ϸ� 16����, �׿ܴ� 10������...
		if( kMemCmp(vcParameter, "0x", 2) == 0 )
		{
			lValue = kAToI(vcParameter+2, 16);
			kPrintf("HEX value = %q\n",lValue);
		}
		else
		{
			lValue = kAToI(vcParameter, 10);
			kPrintf("Decimal Value = %d\n",lValue);
		}
		iCount++;
	}
}

// PC�� �����
void kShutdown( const char* pcParameterBuffer )
{
	kPrintf("System Shutdown start...\n");

	// Ű���� ��Ʈ�ѷ��� ���� pc�� �����
	kPrintf("Press Any Key To Reboot PC...\n");
	kGetCh();
	kReboot();
}

// PIT ��Ʈ�ѷ��� ī���� 0 ����
void kSetTimer( const char* pcParameterBuffer )
{
	char vcParameter[ 100 ];
	PARAMETERLIST stList;
	long lValue;
	BOOL bPeriodic;

	// �Ķ���� �ʱ�ȭ
	kInitializeParameter(&stList, pcParameterBuffer);

	// milisecond ����
	if( kGetNextParameter(&stList, vcParameter) == 0 )
	{
		kPrintf(" ex)settimer 10(ms) 1(periodic)\n");
		return ;
	}
	lValue = kAToI( vcParameter,10);

	// Periodic ����
	if( kGetNextParameter(&stList, vcParameter) == 0)
	{
		kPrintf("ex)settimer 10(ms) 1(periodic)\n");
		return;
	}
	bPeriodic = kAToI( vcParameter, 10);

	kInitializePIT(MSTOCOUNT(lValue), bPeriodic);
	kPrintf("Time = %d ms, Periodic = %d Change Complete\n", lValue, bPeriodic);
}


// PIT ��Ʈ�ѷ��� ���� ����Ͽ� ms ���� ���
void kWaitUsingPIT( const char* pcParameterBuffer)
{
	char vcParameter[ 100 ];
	int iLength;
	PARAMETERLIST stList;
	long lMilisecond;
	int i;

	// �Ķ���� �ʱ�ȭ
	kInitializeParameter(&stList, pcParameterBuffer);

	if( kGetNextParameter(&stList, vcParameter) == 0 )
	{
		kPrintf("ex)wait 100(ms)\n");
		return;
	}
	lMilisecond = kAToI(pcParameterBuffer, 10);
	kPrintf("%d ms Sleep Start...\n",lMilisecond);

	// ���ͷ�Ʈ�� ��Ȱ��ȭ �ϰ� PIT ��Ʈ�ѷ��� ���� ���� �ð��� ����
	kDisableInterrupt();
	for( i=0; i<lMilisecond/30; i++)
	{
		kWaitUsingDirectPIT(MSTOCOUNT(30));
	}
	kWaitUsingDirectPIT(MSTOCOUNT(lMilisecond%30));
	kEnableInterrupt();
	kPrintf("%d ms Sleep Complete\n", lMilisecond );

	// Ÿ�̸� ����
	kInitializePIT(MSTOCOUNT(1), TRUE);

}

// Ÿ�� ������ ī���͸� ����
void kReadTimeStampCounter( const char* pcParameterBuffer)
{
	QWORD qwTSC;
	qwTSC = kReadTSC();
	kPrintf("time stamp Counter = %q\n",qwTSC);
}


// ���μ����� �ӵ��� ����
void kMeasureProcessorSpeed( const char* pcParameterBuffer)
{
	int i;
	QWORD qwLastTSC, qwTotalTSC = 0;

	kPrintf("Now Measuring.");

	// 10�� ���� ��ȭ�� Ÿ�� ������ ī���͸� �̿��Ͽ� ���μ����� �ӵ��� ���������� ����
	kDisableInterrupt();
	for( i=0; i<200; i++)
	{
		qwLastTSC =kReadTSC();
		kWaitUsingDirectPIT(MSTOCOUNT(50));
		qwTotalTSC += kReadTSC() - qwLastTSC;

		kPrintf(".");
	}
	// Ÿ�̸� ����
	kInitializePIT(MSTOCOUNT(1),TRUE);
	kEnableInterrupt();

	kPrintf("\nCPU Speed = %d MHz\n",qwTotalTSC/10/1000/1000 );
}

// RTC ��Ʈ�ѷ��� ����� ���� �� �ð� ������ ǥ��
void kShowDateAndTime( const char* pcParameterBuffer)
{
	BYTE bSecond, bMinute, bHour;
	BYTE bDayOfWeek, bDayOfMonth, bMonth;
	WORD wYear;

	// RTC ��Ʈ�ѷ����� �ð� �� ���ڸ� ����
	kReadRTCTime(&bHour, &bMinute, &bSecond);
	kReadRTCDate(&wYear, &bMonth, &bDayOfMonth, &bDayOfWeek);

	kPrintf(" Date: %d/%d/%d %s, ",wYear,bMonth,bDayOfMonth,kConvertDayOfWeekToString(bDayOfWeek));
	kPrintf(" Time: %d:%d:%d \n",bHour,bMinute,bSecond);
}


// TCB �ڷᱸ���� ���� ����
static TCB gs_vstTask[2] = {0, };
static QWORD gs_vstStack[ 1024 ] = {0, };

// �׽�ũ ��ȯ�� �׽�Ʈ �ϴ� �׽�ũ
void kTestTask(void)
{
	int i = 0;
	while(1)
	{
		 // �޽����� ����ϰ� Ű �Է��� ���
		 kPrintf( "[%d] This message is from kTestTask. Press any key to switch "
		          "kConsoleShell~!!\n", i++ );
		 kGetCh();

		 // ������ Ű�� �ԷµǸ� �½�ũ�� ��ȯ
		 kSwitchContext( &( gs_vstTask[ 1 ].stContext ), &( gs_vstTask[ 0 ].stContext ) );
	}
}


// �½�ũ�� �����ؼ� ��Ƽ �½�ŷ ����
void kCreateTestTask( const char* pcParameterBuffer )
{
    KEYDATA stData;
    int i = 0;

    // �½�ũ ����
    kSetUpTask( &( gs_vstTask[ 1 ] ), 1, 0, ( QWORD ) kTestTask, &( gs_vstStack ),
                sizeof( gs_vstStack ) );

    // 'q' Ű�� �Էµ��� ���� ������ ����
    while( 1 )
    {
        // �޽����� ����ϰ� Ű �Է��� ���
        kPrintf( "[%d] This message is from kConsoleShell. Press any key to "
                 "switch TestTask~!!\n", i++ );
        if( kGetCh() == 'q' )
        {
            break;
        }
        // ������ Ű�� �ԷµǸ� �½�ũ�� ��ȯ
        kSwitchContext( &( gs_vstTask[ 0 ].stContext ), &( gs_vstTask[ 1 ].stContext ) );
    }
}











