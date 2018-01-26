/*
 * PIC.h
 *
 *  Created on: 2018. 1. 26.
 *      Author: Administrator
 */

#ifndef __PIC_H__
#define __PIC_H__

#include "Types.h"

// ��ũ��
// I/O ��Ʈ ����
#define PIC_MASTER_PORT1	0x20
#define PIC_MASTER_PORT2	0x21
#define PIC_SLAVE_PORT1		0xA0
#define PIC_SLAVE_PORT2		0xA1

// IDT ���̺��� ���ͷ�Ʈ ���Ͱ� ���۵Ǵ� ��ġ, 0x20(32��)
// IRQ(Interrupt Request Q...)
#define PIC_IRQSTARTVECTOR 0x20

// �Լ�
void kInitializePIC(void);
void kMaskPICInterrupt( WORD wIRQBitmask);
void kSendEOIToPIC(int iIRQNumber);


#endif /* 02_KERNEL64_SOURCE_PIC_H_ */
