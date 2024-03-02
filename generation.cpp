//
// Created by idang on 04/02/2024.
//

#include "generation.h"

std::string Generator::generateProgram() {
    this->programOut << "section .text\n"
                        "global _start\n"
                        "_start:\n";



    this->programOut << "    mov rax, 0x3c\n"
                        "    mov rdi, 0\n"
                        "    syscall";

    return this->programOut.str();
}
