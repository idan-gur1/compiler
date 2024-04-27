outputString:
push rbp
mov rbp, rsp
mov rax, 1                   ; syscall number for sys_write
mov rdi, 1                   ; file descriptor 1 (stdout)
mov rsi, QWORD [rbp + 16]    ; pointer to the param
movsx rdx, DWORD [rbp + 24]  ; bytes to write
syscall
leave
ret 12