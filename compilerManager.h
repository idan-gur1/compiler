//
// Created by idang on 25/04/2024.
//

#ifndef COMPILER_COMPILERMANAGER_H
#define COMPILER_COMPILERMANAGER_H

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
            std::cout << usageErrMsg << std::endl;

            exit(1);
        }

        sourceFileName = argv[1];
        intermediateLanguageFileName = argv[2];
        targetFileName = argv[3];

        checkExtension(sourceFileName, "ig");
        checkExtension(intermediateLanguageFileName, "il");
        checkExtension(targetFileName, "asm");
    }

    ~Compiler() {
        std::cout << "Compilation finished - cleaning memory" << std::endl;

        delete this->codeGenerator;
        delete this->ilProgram;
        delete this->ilGenerator;
        delete this->programTree;
        delete this->parser;
        delete this->lexer;
    }

    /**
     * @brief Compile the program by performing lexical analysis, parsing,
     * intermediate language generation, and code generation.
     *
     * This function manages the compilation process by reading the source code from the specified file,
     * performing lexical analysis using a Lexer, parsing the token stream into a program tree using a Parser,
     * generating intermediate language (IL) from the program tree using an ILGenerator, and finally generating
     * machine code using a Generator. Any compilation errors are caught and handled by outputting the error message.
     *
     * @return 0 if compilation succeeds, 1 if there are compilation errors.
     */
    int compileProgram();

private:
    inline static const std::string usageErrMsg = "Usage: ./compiler [filename].ig [filename].il [filename].asm";

    std::string sourceFileName;
    std::string intermediateLanguageFileName;
    std::string targetFileName;

    Lexer *lexer = nullptr;
    Parser *parser = nullptr;
    ProgramTreeP programTree = nullptr;
    ILGenerator *ilGenerator = nullptr;
    ThreeAddressProgramP ilProgram = nullptr;
    Generator *codeGenerator = nullptr;

    /**
     * @brief Retrieve the source code from the specified source file.
     *
     * @return The contents of the source file as a string.
     * @throws FileOpenException if the source file cannot be opened.
     */
    std::string getSourceCode();

    void checkExtension(std::string filename, std::string ext);
};


#endif //COMPILER_COMPILERMANAGER_H
