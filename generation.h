//
// Created by idang on 04/02/2024.
//

#ifndef COMPILER_GENERATION_H
#define COMPILER_GENERATION_H

#include <utility>

#include "intermediateCodeGenerator.h"

class Generator {
public:
    explicit Generator(ILGenerator *ilGenerator, std::string outFileName) : outFileName(std::move(outFileName)) {
        this->ilGenerator = ilGenerator;
    }

    void generateProgram();
private:
    void convertTAStmtToAsm(ThreeAddressStmtP);

    ILGenerator *ilGenerator;
    std::string outFileName;
    std::stringstream programOut;
};

#endif //COMPILER_GENERATION_H
