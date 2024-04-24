section .text
global _start
_start:
call main
movsx rdi, eax  ; exit code param
mov rax, 0x3c   ; exit code syscall code
syscall
fib:     ; FUNCTION
push rbp
mov rbp, rsp
sub rsp, 16
fibIf1:
movsx rax, DWORD [rbp + 16]
mov rbx, 1
xor rax, rbx
setz al
movzx rax, al
mov QWORD [rbp - 8], rax
mov rax, QWORD [rbp - 8]
test rax, rax
jz fibIf1End
mov rax, 1
jmp fibEnd
fibIf1End:
fibIf2:
movsx rax, DWORD [rbp + 16]
mov rbx, 1
cmp rax, rbx
setl al
movzx rax, al
mov QWORD [rbp - 8], rax
mov rax, QWORD [rbp - 8]
test rax, rax
jz fibIf2End
mov rax, 0
jmp fibEnd
fibIf2End:
movsx rax, DWORD [rbp + 16]
mov rbx, 1
sub rax, rbx
mov QWORD [rbp - 8], rax
mov rax, QWORD [rbp - 8]
sub rsp, 4
mov DWORD [rbp - 20], eax
call fib
movsx rax, eax
mov QWORD [rbp - 8], rax
movsx rax, DWORD [rbp + 16]
mov rbx, 2
sub rax, rbx
mov QWORD [rbp - 16], rax
mov rax, QWORD [rbp - 16]
sub rsp, 4
mov DWORD [rbp - 20], eax
call fib
movsx rax, eax
mov QWORD [rbp - 16], rax
mov rax, QWORD [rbp - 8]
mov rbx, QWORD [rbp - 16]
add rax, rbx
mov QWORD [rbp - 8], rax
mov rax, QWORD [rbp - 8]
jmp fibEnd
fibEnd:
leave
ret 4
findMaxIndex:     ; FUNCTION
push rbp
mov rbp, rsp
sub rsp, 8
sub rsp, 12
mov rax, 1
mov DWORD [rbp - 12], eax
mov rax, QWORD [rbp + 16]
mov rax, QWORD [rax + 8 * 0]
mov DWORD [rbp - 16], eax
mov rax, 0
mov DWORD [rbp - 20], eax
findMaxIndexWhile1:
jmp findMaxIndexWhile1Condition
findMaxIndexWhile1Body:
findMaxIndexIf1:
mov rax, QWORD [rbp + 16]
movsx rcx, DWORD [rbp - 12]
mov rax, QWORD [rax + 8 * rcx]
mov QWORD [rbp - 8], rax
mov rax, QWORD [rbp - 8]
movsx rbx, DWORD [rbp - 16]
cmp rax, rbx
setg al
movzx rax, al
mov QWORD [rbp - 8], rax
mov rax, QWORD [rbp - 8]
test rax, rax
jz findMaxIndexIf1End
mov rax, QWORD [rbp + 16]
movsx rcx, DWORD [rbp - 12]
mov rax, QWORD [rax + 8 * rcx]
mov QWORD [rbp - 8], rax
mov rax, QWORD [rbp - 8]
mov DWORD [rbp - 16], eax
movsx rax, DWORD [rbp - 12]
mov DWORD [rbp - 20], eax
findMaxIndexIf1End:
movsx rax, DWORD [rbp - 12]
mov rbx, 1
add rax, rbx
mov QWORD [rbp - 8], rax
mov rax, QWORD [rbp - 8]
mov DWORD [rbp - 12], eax
findMaxIndexWhile1Condition:
movsx rax, DWORD [rbp - 12]
movsx rbx, DWORD [rbp + 24]
cmp rax, rbx
setl al
movzx rax, al
mov QWORD [rbp - 8], rax
mov rax, QWORD [rbp - 8]
test rax, rax
jnz findMaxIndexWhile1Body
movsx rax, DWORD [rbp - 20]
jmp findMaxIndexEnd
add rsp, 12
findMaxIndexEnd:
leave
ret 12
main:     ; FUNCTION
push rbp
mov rbp, rsp
sub rsp, 8
sub rsp, 32
mov rax, 15
mov DWORD [rbp - 40 + 4 * 0], eax
mov rax, 8
mov DWORD [rbp - 40 + 4 * 1], eax
mov rax, 6
mov DWORD [rbp - 40 + 4 * 2], eax
mov rax, 20
mov DWORD [rbp - 40 + 4 * 3], eax
mov rax, 12
mov DWORD [rbp - 40 + 4 * 4], eax
mov rax, 1
mov DWORD [rbp - 40 + 4 * 5], eax
mov rax, 7
mov DWORD [rbp - 40 + 4 * 6], eax
mov rax, 3
mov DWORD [rbp - 40 + 4 * 7], eax
mov rax, 8
sub rsp, 4
mov DWORD [rbp - 44], eax
lea rax, [rbp - 40]
sub rsp, 8
mov QWORD [rbp - 52], rax
call findMaxIndex
movsx rax, eax
mov QWORD [rbp - 8], rax
mov rax, QWORD [rbp - 8]
jmp mainEnd
add rsp, 32
mainEnd:
leave
ret 0
