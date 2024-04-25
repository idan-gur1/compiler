//
// Created by idang on 25/04/2024.
//

#include "compiler.h"

int Compiler::compileProgram() {
    try {
        std::string fileContent = getSourceCode();

        lexer = new Lexer(fileContent);

        lexer->analyseSource();

        this->parser = new Parser(lexer);

        this->programTree = this->parser->parseProgram();

        this->ilGenerator = new ILGenerator(this->programTree, this->intermediateLanguageFileName);
        this->ilGenerator->generateProgramIL();

        this->codeGenerator = new Generator(ilGenerator, this->targetFileName);
        this->codeGenerator->generateProgram();

    } catch (const CompilationException &e) {
        std::cout << e.what() << std::endl;
        return 1;
    }

    return 0;
}

std::string Compiler::getSourceCode() {
    std::ifstream inputFile(this->sourceFileName);

    if (inputFile.fail()) {
        inputFile.close();
        throw FileOpenException(this->sourceFileName);
    }

    std::stringstream fileBuffer;
    fileBuffer << inputFile.rdbuf();

    inputFile.close();

    return fileBuffer.str();
}
