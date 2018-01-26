/*
 * PIC.c
 *
 *  Created on: 2018. 1. 26.
 *      Author: Administrator
 */


#include "PIC.h"

// PIC�� �ʱ�ȭ
void kInitializePIC(void)
{
	// ������ PIC ��Ʈ�ѷ��� �ʱ�ȭ
	// ICW1(��Ʈ 0x20), IC4��Ʈ(��Ʈ0) = 1;
	kOutPortByte(PIC_MASTER_PORT1, 0x11);

	// ICW2(��Ʈ 0x21), ���ͷ�Ʈ ���� (0x20)
	kOutPortByte(PIC_MASTER_PORT2, PIC_IRQSTARTVECTOR);

	// ICW3(��Ʈ 21), �����̺� PIC ��Ʈ�ѷ��� ���� ��ġ(��Ʈ�� ǥ��)
	// ������ PIC ��Ʈ�ѷ��� 2�� �ɿ� ����Ǿ� �������� 0x04(��Ʈ2)�� ����
	kOutPortByte(PIC_MASTER_PORT2, 0x04);

	// ICW4(��Ʈ0x21), uPM��Ʈ(��Ʈ0)=1;
	kOutPortByte(PIC_MASTER_PORT2, 0x01);



	// �����̺� PIC ��Ʈ�ѷ� �ʱ�ȭ
	// ICW1(��Ʈ 0xA0), IC4��Ʈ(��Ʈ0) = 1;
	kOutPortByte(PIC_SLAVE_PORT1, 0x11);

	// ICW2(��Ʈ 0xA1), ���ͷ�Ʈ ���� (0x20+8)
	kOutPortByte(PIC_SLAVE_PORT2, PIC_IRQSTARTVECTOR+8);

	// ICW3(��Ʈ 0xA1), ������ PIC ��Ʈ�ѷ��� ���� ��ġ(������ ǥ��)
	// ������ PIC ��Ʈ�ѷ��� 2�� �ɿ� ����Ǿ� �������� 0x02�� ����
	kOutPortByte(PIC_SLAVE_PORT2, 0x02);

	// ICW4(��Ʈ0xA1), uPM��Ʈ(��Ʈ0)=1;
	kOutPortByte(PIC_SLAVE_PORT2, 0x01);

}

// ���ͷ�Ʈ�� ����ũ�Ͽ� �ش� ���ͷ�Ʈ�� �߻����� �ʰ� ó��
void kMaskPICInterrupt(WORD wIRQBitmask)
{
	// ������ PIC ��Ʈ�ѷ��� IMR ����
	// OCW1(��Ʈ 0x21), IRQ 0 ~ IRQ 7
	kOutPortByte(PIC_MASTER_PORT2, (BYTE) wIRQBitmask);

	// �����̺� PIC ��Ʈ�ѷ��� IMR ����
	// OCW1(��Ʈ 0xA1), IRQ 8 ~ IRQ 15
	kOutPortByte(PIC_SLAVE_PORT2, (BYTE) (wIRQBitmask>>8));
}


// ���ͷ�Ʈ ó���� �Ϸ�Ǿ����� ����(EOI)
// ������ PIC ��Ʈ�ѷ��� ���, ������ PIC ��Ʈ�ѷ����� EOI ����
// �����̺� PIC ��Ʈ�ѷ��� ���, ������ �� �����̺� PIC ��Ʈ�ѷ� ��� ����

void kSendEOIToPIC( int iIRQNumber )
{
	// ������ PIC ��Ʈ�ѷ��� EOI ����
	// OCW2(��Ʈ 0x20), EOI ��Ʈ(��Ʈ 5) = 1
	kOutPortByte(PIC_MASTER_PORT1, 0x20);

	// �����̺� PIC ��Ʈ�ѷ��� ���ͷ�Ʈ�� ��� �����̺� PIC ��Ʈ�ѷ����Ե� EOI ����
	if( iIRQNumber >= 8)
	{
		// OCW2(��Ʈ 0xA0), EOI��Ʈ(��Ʈ 5) =1
		kOutPortByte(PIC_SLAVE_PORT1, 0x20);
	}
}












