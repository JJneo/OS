[ORG 0x00]
[BITS 16]

SECTION .text

;;;;;;;;;;;;;;;;;;;;;;;;;;
;	�ڵ� ����
;;;;;;;;;;;;;;;;;;;;;;;;;;


START:
	mov ax, 0x1000
	mov ds, ax
	mov es, ax

	;;;;;;;;;;;;;;;;;;;;;;;
	; A20 ����Ʈ Ȱ��ȭ
	; BIOS�� �̿��� ��ȯ�� ���� ���� �� �ý��� ��Ʈ�� ��Ʈ�� ��ȯ �õ�.
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; BIOS ���񽺸� ����ؼ� A20 ����Ʈ�� Ȱ��ȭ
	mov ax, 0x2401			; A20 ����Ʈ Ȱ��ȭ ���� ����
	int 0x15				; BIOS���ͷ�Ʈ ���� ȣ��

	jc .A20GATEERROR			;A20 ����Ʈ ���� �߻��� �ý��� ��Ʈ�� ��Ʈ�� ��ȯ�õ�
	jmp .A20GATESUCCESS

.A20GATEERROR:
	in al, 0x92				; �ý��� ��Ʈ�� ��Ʈ(0x92)���� 1����Ʈ�� �о� AL �������Ϳ� ����
	or al, 0x02				; ���� ���� A20 Ȱ��ȭ ��Ʈ��(��Ʈ1)�� 1�� ����
	and al, 0xFE			; �ý��� ���� ������ ���� 0xFE�� AND�����Ͽ� ��Ʈ 0�� 0���� ����
	out 0x92, al			; �ý��� ��Ʈ�� ��Ʈ(0x92)�� ����� ���� 1����Ʈ ����

.A20GATESUCCESS:

	cli
	lgdt [ GDTR ]

	;;;;;;;;;;;;;;;;;;;;;;;
	; ��ȣ���� ����
	; Disable Paging, Disable Cache, Internal FPU, Disalbe Align Check,
	; Enable ProtectedMode
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	mov eax, 0x4000003B		; PG=0, CD=1, NW=0, AM=0, WP=0, NE=1, ET=1, TS=1, EM=0, MP=1, PE=1
	mov cr0, eax

	jmp dword 0x08: ( PROTECTEDMODE - $$ + 0x10000 )

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;
	; ��ȣ���� ����
	;
	;;;;;;;;;;;;;;;;;;;;;;;;;;

[BITS 32]

PROTECTEDMODE:
	mov ax, 0x10
	mov ds, ax
	mov es, ax
	mov fs, ax
	mov gs, ax

	mov ss, ax
	mov esp, 0xFFFE
	mov ebp, 0xFFFE

	push ( SWITCHSUCCESSMESSAGE - $$ + 0x10000 )
	push 2
	push 0
	call PRINTMESSAGE
	add esp, 12

	jmp dword 0x08: 0x10200 ; C��� Ŀ���� �����ϴ� 0x10200 ��巹���� �̵��Ͽ� C ��� Ŀ�� ����


	;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; �Լ� �ڵ� ����
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;

PRINTMESSAGE:
	push ebp
	mov ebp, esp
	push esi
	push edi
	push eax
	push ecx
	push edx


	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; X, Y��ǥ�� ���� �޸��� ��巹���� �����
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	mov eax, dword [ ebp + 12 ]
	mov esi, 160
	mul esi
	mov edi, eax

	mov eax, dword [ ebp + 8 ]
	mov esi, 2
	mul esi
	add edi, eax

	mov esi, dword [ ebp + 16 ]

.MESSAGELOOP:
	mov cl, byte [ esi ]

	cmp cl, 0
	je .MESSAGEEND

	mov byte [ edi + 0xB8000 ], cl

	add esi, 1
	add edi, 2

	jmp .MESSAGELOOP


.MESSAGEEND:
	pop edx
	pop ecx
	pop eax
	pop edi
	pop esi
	pop ebp
	ret


	;;;;;;;;;;;;;;;;;;;;;;;;
	; ������ ����
	;;;;;;;;;;;;;;;;;;;;;;;;


	align 8, db 0
	dw 0x0000
GDTR:
	dw GDTEND -GDT -1
	dd ( GDT - $$ + 0x10000 )

GDT:
	NULLDescriptor:
		dw 0x0000
		dw 0x0000
		db 0x00
		db 0x00
		db 0x00
		db 0x00

	CODEDESCRIPTOR:
		dw 0xFFFF
		dw 0x0000
		db 0x00
		db 0x9A
		db 0xCF
		db 0x00

	DATADESCRIPTOR:
		dw 0xFFFF
		dw 0x0000
		db 0x00
		db 0x92
		db 0xCF
		db 0x00
GDTEND:

SWITCHSUCCESSMESSAGE: db 'Switch To protected Mode loading....    [PASS]', 0


times 512 - ( $ - $$ ) db 0x00




