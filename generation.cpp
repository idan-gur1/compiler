//
// Created by idang on 04/02/2024.
//

#include "generation.h"

void Generator::generateProgram() {
    this->programOut << "section .text\n"
                        "global _start\n"
                        "_start:\n"
                        "call main\n"
                        "movsx rdi, eax  ; exit code param\n"
                        "mov rax, 0x3c   ; exit code syscall code\n"
                        "mov rdi, 0\n"
                        "syscall";

    for (auto ilStmt: this->ilGenerator->ilStmts) {
        convertTAStmtToAsm(ilStmt);
    }
}

void Generator::convertTAStmtToAsm(ThreeAddressStmtP) {

}
