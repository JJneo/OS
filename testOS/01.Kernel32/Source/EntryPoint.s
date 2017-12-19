[ORG 0x00]
[BITS 16]

SECTION .text

;;;;;;;;;;;;;;;;;;;;;;;;;;
;	코드 영역
;;;;;;;;;;;;;;;;;;;;;;;;;;


START:
	mov ax, 0x1000
	mov ds, ax
	mov es, ax

	;;;;;;;;;;;;;;;;;;;;;;;
	; A20 게이트 활성화
	; BIOS를 이용한 전환이 실패 했을 때 시스템 컨트롤 포트로 전환 시도.
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	; BIOS 서비스를 사용해서 A20 게이트를 활성화
	mov ax, 0x2401			; A20 게이트 활성화 서비스 설정
	int 0x15				; BIOS인터럽트 서비스 호출

	jc .A20GATEERROR			;A20 게이트 에러 발생시 시스템 컨트롤 포트로 전환시도
	jmp .A20GATESUCCESS

.A20GATEERROR:
	in al, 0x92				; 시스템 컨트롤 포트(0x92)에서 1바이트를 읽어 AL 레지스터에 저장
	or al, 0x02				; 읽은 값에 A20 활성화 비트인(비트1)을 1로 설정
	and al, 0xFE			; 시스템 리셋 방지를 위해 0xFE와 AND연산하여 비트 0을 0으로 설정
	out 0x92, al			; 시스템 컨트롤 포트(0x92)에 변경된 값을 1바이트 설정

.A20GATESUCCESS:

	cli
	lgdt [ GDTR ]

	;;;;;;;;;;;;;;;;;;;;;;;
	; 보호모드로 진입
	; Disable Paging, Disable Cache, Internal FPU, Disalbe Align Check,
	; Enable ProtectedMode
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

	mov eax, 0x4000003B		; PG=0, CD=1, NW=0, AM=0, WP=0, NE=1, ET=1, TS=1, EM=0, MP=1, PE=1
	mov cr0, eax

	jmp dword 0x08: ( PROTECTEDMODE - $$ + 0x10000 )

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;
	; 보호모드로 진입
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

	jmp dword 0x08: 0x10200 ; C언어 커널이 존재하는 0x10200 어드레스로 이동하여 C 언어 커널 수행


	;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	; 함수 코드 영역
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
	; X, Y좌표로 비디오 메모리의 어드레스를 계산함
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
	; 데이터 영역
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




