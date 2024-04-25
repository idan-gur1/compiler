exit:
push rbp
mov rbp, rsp
mov rax, 0x3c   ; syscall number for sys_exit
movsx rdi, DWORD [rbp + 16]  ; exit code
syscall
leave
ret 4