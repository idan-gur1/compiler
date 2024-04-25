section .data
overflowErrMsg db 'Stack overflow, exiting', 0xa
LEN equ $ - overflowErrMsg

section .text
global _start
_start:
mov r8, 0                ; function call stack counter
call main
movsx rdi, eax           ; exit code
mov rax, 0x3c            ; syscall number for sys_exit
syscall

_overflow:
mov rax, 1               ; syscall number for sys_write
mov rdi, 1               ; file descriptor 1 (stdout)
mov rsi, overflowErrMsg  ; pointer to the param
mov rdx, LEN             ; bytes to write
syscall
mov rax, 0x3c            ; syscall number for sys_exit
mov rdi, 1               ; exit code for error
syscall

outputChar:
push rbp
mov rbp, rsp
mov rax, 1           ; syscall number for sys_write
mov rdi, 1           ; file descriptor 1 (stdout)
lea rsi, [rbp + 16]  ; pointer to the param
mov rdx, 1           ; bytes to write
syscall
leave
ret 1

outputString:     ; FUNCTION
cmp r8, 200
jae _overflow
push rbp
mov rbp, rsp
outputStringWhile1:
jmp outputStringWhile1Condition
outputStringWhile1Body:
mov rax, QWORD [rbp + 16]
movsx rax, BYTE [rax + 1 * 0]
sub rsp, 1
mov BYTE [rbp - 1], al
inc r8
call outputChar
dec r8
mov rax, QWORD [rbp + 16]
lea rax, [rax + 1 * 1]
mov QWORD [rbp + 16], rax
outputStringWhile1Condition:
mov rax, QWORD [rbp + 16]
movsx rax, BYTE [rax + 1 * 0]
test rax, rax
jnz outputStringWhile1Body
outputStringEnd:
leave
ret 8

fib:     ; FUNCTION
cmp r8, 200
jae _overflow
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
inc r8
call fib
dec r8
movsx rax, eax
mov QWORD [rbp - 8], rax
movsx rax, DWORD [rbp + 16]
mov rbx, 2
sub rax, rbx
mov QWORD [rbp - 16], rax
mov rax, QWORD [rbp - 16]
sub rsp, 4
mov DWORD [rbp - 20], eax
inc r8
call fib
dec r8
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

getMaxPtr:     ; FUNCTION
cmp r8, 200
jae _overflow
push rbp
mov rbp, rsp
sub rsp, 8
sub rsp, 12
mov rax, 1
mov DWORD [rbp - 12], eax
mov rax, QWORD [rbp + 16]
movsx rax, DWORD [rax + 4 * 0]
mov DWORD [rbp - 16], eax
mov rax, 0
mov DWORD [rbp - 20], eax
getMaxPtrWhile1:
jmp getMaxPtrWhile1Condition
getMaxPtrWhile1Body:
getMaxPtrIf1:
mov rax, QWORD [rbp + 16]
movsx rcx, DWORD [rbp - 12]
movsx rax, DWORD [rax + 4 * rcx]
mov QWORD [rbp - 8], rax
mov rax, QWORD [rbp - 8]
movsx rbx, DWORD [rbp - 16]
cmp rax, rbx
setg al
movzx rax, al
mov QWORD [rbp - 8], rax
mov rax, QWORD [rbp - 8]
test rax, rax
jz getMaxPtrIf1End
mov rax, QWORD [rbp + 16]
movsx rcx, DWORD [rbp - 12]
movsx rax, DWORD [rax + 4 * rcx]
mov QWORD [rbp - 8], rax
mov rax, QWORD [rbp - 8]
mov DWORD [rbp - 16], eax
movsx rax, DWORD [rbp - 12]
mov DWORD [rbp - 20], eax
getMaxPtrIf1End:
movsx rax, DWORD [rbp - 12]
mov rbx, 1
add rax, rbx
mov QWORD [rbp - 8], rax
mov rax, QWORD [rbp - 8]
mov DWORD [rbp - 12], eax
getMaxPtrWhile1Condition:
movsx rax, DWORD [rbp - 12]
movsx rbx, DWORD [rbp + 24]
cmp rax, rbx
setl al
movzx rax, al
mov QWORD [rbp - 8], rax
mov rax, QWORD [rbp - 8]
test rax, rax
jnz getMaxPtrWhile1Body
mov rax, QWORD [rbp + 16]
movsx rcx, DWORD [rbp - 20]
lea rax, [rax + 4 * rcx]
jmp getMaxPtrEnd
add rsp, 12
getMaxPtrEnd:
leave
ret 12

replaceMaxWithZero:     ; FUNCTION
cmp r8, 200
jae _overflow
push rbp
mov rbp, rsp
sub rsp, 8
sub rsp, 12
mov rax, 1
mov DWORD [rbp - 12], eax
mov rax, QWORD [rbp + 16]
movsx rax, DWORD [rax + 4 * 0]
mov DWORD [rbp - 16], eax
mov rax, 0
mov DWORD [rbp - 20], eax
replaceMaxWithZeroWhile1:
jmp replaceMaxWithZeroWhile1Condition
replaceMaxWithZeroWhile1Body:
replaceMaxWithZeroIf1:
mov rax, QWORD [rbp + 16]
movsx rcx, DWORD [rbp - 12]
movsx rax, DWORD [rax + 4 * rcx]
mov QWORD [rbp - 8], rax
mov rax, QWORD [rbp - 8]
movsx rbx, DWORD [rbp - 16]
cmp rax, rbx
setg al
movzx rax, al
mov QWORD [rbp - 8], rax
mov rax, QWORD [rbp - 8]
test rax, rax
jz replaceMaxWithZeroIf1End
mov rax, QWORD [rbp + 16]
movsx rcx, DWORD [rbp - 12]
movsx rax, DWORD [rax + 4 * rcx]
mov QWORD [rbp - 8], rax
mov rax, QWORD [rbp - 8]
mov DWORD [rbp - 16], eax
movsx rax, DWORD [rbp - 12]
mov DWORD [rbp - 20], eax
replaceMaxWithZeroIf1End:
movsx rax, DWORD [rbp - 12]
mov rbx, 1
add rax, rbx
mov QWORD [rbp - 8], rax
mov rax, QWORD [rbp - 8]
mov DWORD [rbp - 12], eax
replaceMaxWithZeroWhile1Condition:
movsx rax, DWORD [rbp - 12]
movsx rbx, DWORD [rbp + 24]
cmp rax, rbx
setl al
movzx rax, al
mov QWORD [rbp - 8], rax
mov rax, QWORD [rbp - 8]
test rax, rax
jnz replaceMaxWithZeroWhile1Body
mov rax, 0
mov rbx, QWORD [rbp + 16]
movsx rcx, DWORD [rbp - 20]
mov DWORD [rbx + 4 * rcx], eax
add rsp, 12
replaceMaxWithZeroEnd:
leave
ret 12

infRec:     ; FUNCTION
cmp r8, 200
jae _overflow
push rbp
mov rbp, rsp
sub rsp, 8
movsx rax, DWORD [rbp + 16]
mov rbx, 1
sub rax, rbx
mov QWORD [rbp - 8], rax
mov rax, QWORD [rbp - 8]
sub rsp, 4
mov DWORD [rbp - 12], eax
inc r8
call infRec
dec r8
movsx rax, eax
mov QWORD [rbp - 8], rax
mov rax, QWORD [rbp - 8]
jmp infRecEnd
infRecEnd:
leave
ret 4

main:     ; FUNCTION
cmp r8, 200
jae _overflow
push rbp
mov rbp, rsp
sub rsp, 8
sub rsp, 49
mov rax, 15
mov DWORD [rbp - 40 + 4 * 0], eax
mov rax, 8
mov DWORD [rbp - 40 + 4 * 1], eax
mov rax, 8
sub rsp, 4
mov DWORD [rbp - 61], eax
inc r8
call fib
dec r8
movsx rax, eax
mov QWORD [rbp - 8], rax
mov rax, QWORD [rbp - 8]
mov DWORD [rbp - 40 + 4 * 2], eax
mov rax, 40
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
mov DWORD [rbp - 61], eax
lea rax, [rbp - 40]
sub rsp, 8
mov QWORD [rbp - 69], rax
inc r8
call replaceMaxWithZero
dec r8
mov rax, 8
sub rsp, 4
mov DWORD [rbp - 61], eax
lea rax, [rbp - 40]
sub rsp, 8
mov QWORD [rbp - 69], rax
inc r8
call getMaxPtr
dec r8
mov QWORD [rbp - 8], rax
mov rax, QWORD [rbp - 8]
mov QWORD [rbp - 48], rax
mov rax, QWORD [rbp - 48]
movsx rax, DWORD [rax + 4 * 0]
mov rbx, 2
imul rbx
mov QWORD [rbp - 8], rax
mov rax, QWORD [rbp - 8]
mov rbx, QWORD [rbp - 48]
mov DWORD [rbx + 4 * 0], eax
mov rax, 105
mov BYTE [rbp - 57 + 1 * 0], al
mov rax, 100
mov BYTE [rbp - 57 + 1 * 1], al
mov rax, 97
mov BYTE [rbp - 57 + 1 * 2], al
mov rax, 110
mov BYTE [rbp - 57 + 1 * 3], al
mov rax, 32
mov BYTE [rbp - 57 + 1 * 4], al
mov rax, 103
mov BYTE [rbp - 57 + 1 * 5], al
mov rax, 117
mov BYTE [rbp - 57 + 1 * 6], al
mov rax, 114
mov BYTE [rbp - 57 + 1 * 7], al
mov rax, 0
mov BYTE [rbp - 57 + 1 * 8], al
lea rax, [rbp - 57]
sub rsp, 8
mov QWORD [rbp - 65], rax
inc r8
call outputString
dec r8
mov rax, 10
sub rsp, 1
mov BYTE [rbp - 58], al
inc r8
call outputChar
dec r8
lea rax, [rbp - 57]
sub rsp, 8
mov QWORD [rbp - 65], rax
inc r8
call outputString
dec r8
mov rax, 10
sub rsp, 1
mov BYTE [rbp - 58], al
inc r8
call outputChar
dec r8
mov rax, QWORD [rbp - 48]
movsx rax, DWORD [rax + 4 * 0]
jmp mainEnd
add rsp, 49
mainEnd:
leave
ret 0

