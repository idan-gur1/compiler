//
// Created by idang on 04/02/2024.
//

#ifndef COMPILER_GENERATION_H
#define COMPILER_GENERATION_H

#include "intermediateCodeGenerator.h"

class Generator {
public:
    explicit Generator(ILGenerator *ilGenerator) {
        this->ilGenerator = ilGenerator;
    }

    ~Generator() {
        delete ilGenerator;
    }

    std::string generateProgram();
private:
    std::stringstream programOut;
    ILGenerator *ilGenerator;
};

#endif //COMPILER_GENERATION_H
