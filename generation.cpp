//
// Created by idang on 04/02/2024.
//

#include "generation.h"

std::string Generator::generateProgram() {
    this->programOut << "section .text\n"
                        "global _start\n"
                        "_start:\n"
                        "push rbp\n"
                        "mov rbp, sp\n"
                        "sub rsp, " << ((this->ilGenerator->maxTemp * 8) + (this->ilGenerator->declarations * 8)) << "\n";



    for (ThreeAddressStmt *taStmt: this->ilGenerator->ilStmts) {
        if (auto tempTAS = dynamic_cast<TempAssignmentTAStmtP>(taStmt)) {

        } else if (auto varTAS = dynamic_cast<VarAssignmentTAStmtP>(taStmt)) {

        }
    }

    this->programOut << "    mov rax, 0x3c\n"
                        "    mov rdi, 0\n"
                        "    syscall";

    return this->programOut.str();
}
