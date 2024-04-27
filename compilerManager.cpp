//
// Created by idang on 25/04/2024.
//

#include "compilerManager.h"

int Compiler::compileProgram() {
    try {
        std::string fileContent = getSourceCode();

        // Perform lexical analysis
        lexer = new Lexer(fileContent);
        lexer->analyseSource();

        // Parse token stream into program tree
        this->parser = new Parser(lexer);
        this->programTree = this->parser->parseProgram();

        // Generate intermediate language (IL) from program tree
        this->ilGenerator = new ILGenerator(this->programTree, this->intermediateLanguageFileName);
        this->ilGenerator->generateProgramIL();

        // Generate machine code from intermediate language
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

void Compiler::checkExtension(std::string filename, std::string ext) {
    if (filename.substr(filename.find_last_of('.') + 1) != ext) {
        std::cout << "Unknown extension for file " << filename << std::endl;
        std::cout << usageErrMsg << std::endl;

        exit(1);
    }
}
