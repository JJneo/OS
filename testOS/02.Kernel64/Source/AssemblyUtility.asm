# file      Assembly Utility
# brief     어셈블리어 유틸리티 함수에 관련된 소스 파일

[BITS 64]           ; 이하의 코드는 64비트 코드로 설정

SECTION .text       ; text 섹션(세그먼트)을 정의

; C 언어에서 호출할 수 있도록 이름을 노출함(Export)
global kInPortByte, kOutPortByte, kLoadGDTR, kLoadTR, kLoadIDTR
global kEnableInterrupt, kDisableInterrupt, kReadRFLAGS
global kReadTSC
global kSwitchContext
global kHlt
global kTestAndSet


; 포트로부터 1바이트를 읽음
;   PARAM: 포트 번호
kInPortByte:
    push rdx        ; 함수에서 임시로 사용하는 레지스터를 스택에 저장
                    ; 함수의 마지막 부분에서 스택에 삽입된 값을 꺼내 복원

    mov rdx, rdi    ; RDX 레지스터에 파라미터 1(포트 번호)를 저장
    mov rax, 0      ; RAX 레지스터를 초기화
    in al, dx       ; DX 레지스터에 저장된 포트 어드레스에서 한 바이트를 읽어
                    ; AL 레지스터에 저장, AL 레지스터는 함수의 반환 값으로 사용됨

    pop rdx         ; 함수에서 사용이 끝난 레지스터를 복원
    ret             ; 함수를 호출한 다음 코드의 위치로 복귀
    
; 포트에 1바이트를 씀
;   PARAM: 포트 번호, 데이터
kOutPortByte:
    push rdx        ; 함수에서 임시로 사용하는 레지스터를 스택에 저장
    push rax        ; 함수의 마지막 부분에서 스택에 삽입된 값을 꺼내 복원
    
    mov rdx, rdi    ; RDX 레지스터에 파라미터 1(포트 번호)를 저장
    mov rax, rsi    ; RAX 레지스터에 파라미터 2(데이터)를 저장
    out dx, al      ; DX 레지스터에 저장된 포트 어드레스에 AL 레지스터에 저장된
                    ; 한 바이트를 씀

    pop rax         ; 함수에서 사용이 끝난 레지스터를 복원
    pop rdx
    ret             ; 함수를 호출한 다음 코드의 위치로 복귀


;GDTR 레지스터에 GDT테이블을 설정
;	PARAM: GDT 테이블 의 정보를 저장하는 자료구조의 어드레스
kLoadGDTR:
	lgdt[rdi]		;파라미터 1(GDTR의 어드레스)를 프로세서에 로드하여 GDT테이블을 설정
	ret

; TR레지스터에 TSS 세그먼트 디스크립터 설정
;	PARAM: TSS 세그먼트 디스크립터의 오프셋
kLoadTR:
	ltr di		;파라미터 1(TSS 섹먼트 디스크립터의 오프셋)을 프로세서에 설정하여 TSS 세그먼트를 로드
	ret

; IDTR 레지스터에 IDT 테이블을 설정
;	PARAM: IDT 테이블의 정보를 저장하는 자료구조의 어드레스
kLoadIDTR:
	lidt[rdi]		;파라미터 1(IDTR의 어드레스)를 프로세스에 로드하여 IDT 테이블을 설정
	ret



; 인터럽트를 활성화
;	PARAM: 없음
kEnableInterrupt:
	sti			; 인터럽트를 활성화
	ret

; 인터럽트를 비활성화
;	PARAM: 없음
kDisableInterrupt:
	cli			; 인터럽트를 비활성화
	ret


; RFLAGS 레지스터를 읽어서 되돌려줌
;	PARAM: 없음
kReadRFLAGS:
	pushfq		; RFLAGS 레지스터를 스택에 저장
	pop rax		; 스택에 저장된 RFLAGS 레지스터를 RAX 레지스터에 저장하여 함수의 반환 값으로 설정
	ret


; 타임 스탬프 카운터를 읽어서 반환
;	PARAM: 없음
kReadTSC:
	push rdx			;RDX 레지스터를 스택에 저장

	rdtsc				;타임 스탬프 카운터를 읽어서 RDX:RAX에 자장

	shl rdx, 32			; RDX 레지스터에 있는 상위 32비트 TSC 값과 RAX 레지스터에 있는
	or rax, rdx			; 하위 32비트 TSC 값을 OR 하여 RAX 레지스터에 64비트 TSC 값을 저장.

	pop rdx
	ret



;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;                  태스크 관련 어셈블리어 함수                                            ;
;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;

; 콘텍스트를 저장하고 셀렉터를 교체하는 매크로
%macro KSAVECONTEXT 0		; 파라미터를 전달 받지 않는 KSAVECONTEXT 매크로 정의
	; RBP 레지스터부터 GS 세그먼트 셀렉터 까지 모드 스택에 삽입
	; (콘텍스트 시작주소를 받아서 딱 19 번째 위치에 스택 어드레스를 위치시키고 Push를 통해 레지스터 저장.)
	push rbp
	push rax
	push rbx
	push rcx
	push rdx
	push rdi
	push rsi
	push r8
	push r9
	push r10
	push r11
	push r12
	push r13
	push r14
	push r15

	mov ax, ds		; DS 세그먼트 셀렉터와 ES 세그먼트 셀렉터는 스택에 직접 삽입할 수 없음으로
	push rax		; RAX 레지스터에 저장한 후 스택에 삽입
	mov ax, es
	push rax
	push fs
	push gs
%endmacro		; 메크로 끝

; 콘텍스트를 복원하는 매크로
%macro KLOADCONTEXT 0	; 파라미터를 전달받지 않는 KLOADCONTEXT 매크로 정의
	; GS 세그먼트 셀렉터 부터 RBP 레지스터 까지 모드 스택에서 꺼내 복원
	pop gs
	pop fs

	pop rax			; ES세그먼트 셀렉터와 DS세그먼트 셀렉터는 스택에서 직접 꺼내
	mov es, ax		; 복원할 수 없음으로 RAX 레지스터에 저장한 뒹 복원
	pop rax
	mov ds, ax

	pop r15
	pop r14
	pop r13
	pop r12
	pop r11
	pop r10
	pop r9
	pop r8
	pop rsi
	pop rdi
	pop rdx
	pop rcx
	pop rbx
	pop rax
	pop rbp
%endmacro

; current Context에 현재 콘텍스트를 저장하고 next task에서 콘택스트를 복구
;	PARAM: Current Context, Next Context
kSwitchContext:
	push rbp		; 스택에 RBP 레지스터를 저장하고 RSP 레지스터를 RBP에 저장
	mov rbp, rsp

	; Current Context가 NULL이면 콘텍스트를 저장할 필요 없음
	pushfq			; 아래의 cmp 결과로 RFLAGS 레지스터가 변하지 않도록 스택에 저장.
	cmp rdi, 0		; Current Context가 NULL이면 콘텍스트 복원으로 바로 이동
	je .LoadContext
	popfq			; 스택에 저장한 RFLAGS 레지스터 복원

	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	;    현재의 태스크 콘텍스트를 저장                                                                                      ;
	;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
	push rax            ; 콘텍스트 영역의 오프셋으로 사용할 RAX 레지스터를 스택에 저장

    ; SS, RSP, RFLAGS, CS, RIP 레지스터 순서대로 삽입
    mov ax, ss                          ; SS 레지스터 저장
    mov qword[ rdi + ( 23 * 8 ) ], rax

    mov rax, rbp                        ; RBP에 저장된 RSP 레지스터 저장
    add rax, 16                         ; RSP 레지스터는 push rbp와 Return Address를
    mov qword[ rdi + ( 22 * 8 ) ], rax  ; 제외한 값으로 저장

    pushfq                              ; RFLAGS 레지스터 저장
    pop rax
    mov qword[ rdi + ( 21 * 8 ) ], rax

    mov ax, cs                          ; CS 레지스터 저장
    mov qword[ rdi + ( 20 * 8 ) ], rax

    mov rax, qword[ rbp + 8 ]           ; RIP 레지스터를 Return Address로 설정하여
    mov qword[ rdi + ( 19 * 8 ) ], rax  ; 다음 콘텍스트 복원 시에 이 함수를 호출한
                                        ; 위치로 이동하게 함

    ; 저장한 레지스터를 복구한 후 인터럽트가 발생했을 때처럼 나머지 콘텍스트를 모두 저장
    pop rax
    pop rbp

    ; 가장 끝부분에 SS, RSP, RFLAGS, CS, RIP 레지스터를 저장했으므로, 이전 영역에
    ; push 명령어로 콘텍스트를 저장하기 위해 스택을 변경
    add rdi, ( 19 * 8 )
    mov rsp, rdi
    sub rdi, ( 19 * 8 )

    ; 나머지 레지스터를 모두 Context 자료구조에 저장
    KSAVECONTEXT

    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
    ; 다음 태스크의 콘텍스트 복원
    ;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
.LoadContext:
    mov rsp, rsi

    ; Context 자료구조에서 레지스터를 복원
    KLOADCONTEXT
    iretq


; 프로세서를 쉬게 함
;	PARAM: 없음
kHlt:
	hlt		;프로세서를 대기 상태로 진입시킴
	hlt
	ret



; 태스트와 설정을 하나의 명령으로 처리
;	 function cmpxchg(p : pointer to int, old : int, new : int) returns bool {
;	     if *p ≠ old {
;	         return false
;	     }
;	     *p ← new
;	     return true
;	 }
; 	PRAM : 값을 저장할 어드레스(Destination, rdi), 비교할 값( compare, rsi ), 설정할 값(Source, rdx)
kTestAndSet:
	mov rax, rsi		; 두번째 파라미터인 비교할 값을 RAX 레지스터에 저장.

	; RAX 레지스터에 저장된 비교할 값과 첫 번째 파라미터의 메모리 어드레스의 값을 비교하여
	; 두 값이 같다면 세 번째 파라미터의 값을 첫 번째 파라미터가 가리키는 어드레스로 삽입.
	lock cmpxchg byte [rdi], dl
	je .SUCCESS			; ZF 비트가 1이면 같다는 뜻임으로 .SUCCES로 이동

.NOTDAME:			; Destination 과 Compare가 다른 경우.
	mov rax, 0x00
	ret

.SUCCESS:			; Destination과 Compare가 같은 경우
	mov rax, 0x01
	ret









