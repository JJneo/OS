/*
 * PIC.c
 *
 *  Created on: 2018. 1. 26.
 *      Author: Administrator
 */


#include "PIC.h"

// PIC를 초기화
void kInitializePIC(void)
{
	// 마스터 PIC 컨트롤러를 초기화
	// ICW1(포트 0x20), IC4비트(비트0) = 1;
	kOutPortByte(PIC_MASTER_PORT1, 0x11);

	// ICW2(포트 0x21), 인터럽트 벡터 (0x20)
	kOutPortByte(PIC_MASTER_PORT2, PIC_IRQSTARTVECTOR);

	// ICW3(포트 21), 슬레이브 PIC 컨트롤러가 연결 위치(비트로 표현)
	// 마스터 PIC 컨트롤러의 2번 핀에 연결되어 있음으로 0x04(비트2)로 설정
	kOutPortByte(PIC_MASTER_PORT2, 0x04);

	// ICW4(포트0x21), uPM비트(비트0)=1;
	kOutPortByte(PIC_MASTER_PORT2, 0x01);



	// 슬레이브 PIC 컨트롤러 초기화
	// ICW1(포트 0xA0), IC4비트(비트0) = 1;
	kOutPortByte(PIC_SLAVE_PORT1, 0x11);

	// ICW2(포트 0xA1), 인터럽트 벡터 (0x20+8)
	kOutPortByte(PIC_SLAVE_PORT2, PIC_IRQSTARTVECTOR+8);

	// ICW3(포트 0xA1), 마스터 PIC 컨트롤러가 연결 위치(정수로 표현)
	// 마스터 PIC 컨트롤러의 2번 핀에 연결되어 있음으로 0x02로 설정
	kOutPortByte(PIC_SLAVE_PORT2, 0x02);

	// ICW4(포트0xA1), uPM비트(비트0)=1;
	kOutPortByte(PIC_SLAVE_PORT2, 0x01);

}

// 인터럽트를 마스크하여 해당 인터럽트가 발생하지 않게 처리
void kMaskPICInterrupt(WORD wIRQBitmask)
{
	// 마스터 PIC 컨트롤러에 IMR 설정
	// OCW1(포트 0x21), IRQ 0 ~ IRQ 7
	kOutPortByte(PIC_MASTER_PORT2, (BYTE) wIRQBitmask);

	// 슬레이브 PIC 컨트롤러에 IMR 설정
	// OCW1(포트 0xA1), IRQ 8 ~ IRQ 15
	kOutPortByte(PIC_SLAVE_PORT2, (BYTE) (wIRQBitmask>>8));
}


// 인터럽트 처리가 완료되었음을 전송(EOI)
// 마스터 PIC 컨트롤러의 경우, 마스터 PIC 컨트롤러에만 EOI 전송
// 슬레이브 PIC 컨트롤러의 경우, 마스터 및 슬레이브 PIC 컨트롤러 모두 전송

void kSendEOIToPIC( int iIRQNumber )
{
	// 마스터 PIC 컨트롤러에 EOI 전송
	// OCW2(포트 0x20), EOI 비트(비트 5) = 1
	kOutPortByte(PIC_MASTER_PORT1, 0x20);

	// 슬레이브 PIC 컨트롤러의 인터럽트인 경우 슬레이브 PIC 컨트롤러에게도 EOI 전송
	if( iIRQNumber >= 8)
	{
		// OCW2(포트 0xA0), EOI비트(비트 5) =1
		kOutPortByte(PIC_SLAVE_PORT1, 0x20);
	}
}













