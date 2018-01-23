/*
 * Page.c
 *
 *  Created on: 2017. 12. 20.
 *      Author: user
 */

#include "Page.h"
// IA-32e ��� Ŀ���� ���� ������ ���̺� ����
void kInitializePageTables(void)
{

	PML4TENTRY* pstPML4TEntry;
	PDPTENTRY* pstPDPTEntry;
	PDENTRY* pstPDEntry;
	DWORD dwMappingAddress;
	int i;

	// PML4 ���̺� ����
	// ù ��° ��Ʈ�� �ܿ� �������� ��� 0���� �ʱ�ȭ
	pstPML4TEntry = (PML4TENTRY*) 0x100000;
	kSetPageEntryData( &(pstPML4TEntry[0]), 0x00, 0x101000, PAGE_FLAGS_DEFAULT, 0 );
	for( i=1; i<PAGE_MAXENTRYCOUNT; i++ )
	{
		kSetPageEntryData( &(pstPML4TEntry[i] ), 0, 0, 0, 0 );
	}

	// ������ ���͸� ������ ���̺� ����
	// �ϳ��� PDPT�� 512G������ ���� �����ϹǷ� �����
	// 64���� ��Ʈ���� �����Ͽ� 64G���� ������
	pstPDPTEntry = ( PDPTENTRY* ) 0x101000;
	for( i=0; i<64; i++)
	{
		kSetPageEntryData( &(pstPDPTEntry[i] ),0,0x102000+ (i*PAGE_TABLESIZE), PAGE_FLAGS_DEFAULT, 0);
	}
	for( i=64; i<PAGE_MAXENTRYCOUNT; i++ )
	{
		kSetPageEntryData( &(pstPDPTEntry[i]),0, 0, 0, 0);
	}


	// ������ ���͸� ���̺� ����
	// �ϳ��� ������ ���͸��� 1GB���� ���� ����
	// �����ְ� 64���� ������ ���͸��� �����Ͽ� �� 64G���� ����
	pstPDEntry = ( PDENTRY*) 0x102000;
	dwMappingAddress = 0;
	for( i=0; i<PAGE_MAXENTRYCOUNT*64; i++)
	{
		// 32��Ʈ�δ� ���� ��巹���� ǥ���� �� �������� MB������ ����� ����
		// ���� ����� 4KB�� ������ 32��Ʈ �̻��� ��巹���� �����
		// �ᱹ�� i�� ���° ���͸� ���̺� ���° ��Ʈ���̰� �װ� �����ּҷ� ���̳� �ε�
		// 32��Ʈ���� ������ i�� 2M�� ���ϰ� 4G�� ������ �װ� ���� �� �ּ��� ���� ��巹���̴�. �̰� ���� �ٲ㼭 ����ϸ� ��~
		kSetPageEntryData( &(pstPDEntry[i] ), (i*(PAGE_DEFAULTSIZE >>20 ) ) >>12, dwMappingAddress, PAGE_FLAGS_DEFAULT | PAGE_FLAGS_PS, 0 );
		dwMappingAddress += PAGE_DEFAULTSIZE;
	}




}

// ������ ��Ʈ���� ���� �ּҿ� �Ӽ� �÷��׸� ����
void kSetPageEntryData( PTENTRY* pstEntry, DWORD dwUpperBaseAddress, DWORD dwLowerBaseAddress, DWORD dwLowerFlags, DWORD dwUpperFlags )
{
	pstEntry->dwAttributeAndLowerBaseAddress = dwLowerBaseAddress | dwLowerFlags;
	pstEntry->dwUpperBaseAddressAndEXB = (dwUpperBaseAddress & 0xFF ) | dwUpperFlags;
}
