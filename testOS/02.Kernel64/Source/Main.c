/*
 * Main.c
 *
 *  Created on: 2017. 12. 19.
 *      Author: user
 */


#include "Types.h"


void kPrintString( int iX, int iY, const char* pcString );



// main �Լ�
void Main( void )
{
	//             40ĭ  "                                         [
	kPrintString(46, 10,"PASS");
	kPrintString(0, 11, "Switch To IA-32e Mode Success~!!" );
	kPrintString(0, 12, "IA-32e C Language Kernel Start....           [PASS]" );

	
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
