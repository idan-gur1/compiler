//
// Created by idang on 25/04/2024.
//

#ifndef COMPILER_COMPILER_H
#define COMPILER_COMPILER_H

#include <iostream>
#include <fstream>
#include "lexer.h"
#include "parser.h"
#include "treeNodes.h"
#include "intermediateCodeGenerator.h"
#include "generation.h"

class Compiler {
public:
    Compiler(int argc, char *argv[]) {
        if (argc < 4) {
            std::cout << "Enter the file to be compiled, IL file and target file." << std::endl;
            std::cout << "Usage: ./compiler [filename].ig [filename].il [filename].asm" << std::endl;

            exit(1);
        }

        sourceFileName = argv[1];
        intermediateLanguageFileName = argv[2];
        targetFileName = argv[3];
    }

    ~Compiler() {
        std::cout << "Compilation finished - cleaning memory" << std::endl;

        delete this->codeGenerator;
        delete this->ilGenerator;
        delete this->programTree;
        delete this->parser;
        delete this->lexer;
    }

    int compileProgram();

private:
    std::string sourceFileName;
    std::string intermediateLanguageFileName;
    std::string targetFileName;

    Lexer *lexer = nullptr;
    Parser *parser = nullptr;
    ProgramTreeP programTree = nullptr;
    ILGenerator *ilGenerator = nullptr;
    Generator *codeGenerator = nullptr;

    std::string getSourceCode();
};


#endif //COMPILER_COMPILER_H
