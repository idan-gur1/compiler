#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "lexer.h"
#include "parser.h"
#include "intermediateCodeGenerator.h"
#include "errorHandling.h"


int main(int argc, char *argv[]) {
    // TODO: BETTER ERROR MANAGEMENT, ADD ERROR MANAGEMENT IN IL
    // TODO REMOVE COMMENT: FOR TESTING PURPOSES
//    if (argc < 2) {
//        std::cout << "Enter the file to be compiled." << std::endl;
//        std::cout << "Usage: ./compiler [filename].ig" << std::endl;
//
//        return 1;
//    }

//    std::ifstream inputFile(argv[1]);
//    std::ifstream inputFile("../testExprGeneration.ig");  // TODO REMOVE: JUST FOR TEST
    std::ifstream inputFile("../test.ig");  // TODO REMOVE: JUST FOR TEST

    if (inputFile.fail()) {
        std::cout << "Error with reading the input file." << std::endl;
        inputFile.close();

        return 1;
    }

    std::string fileContent;

    auto *fileBuffer = new std::stringstream();
    *fileBuffer << inputFile.rdbuf();

    fileContent = fileBuffer->str();

    delete fileBuffer;

    inputFile.close();

    Lexer *lexer = nullptr;
    Parser *parser = nullptr;
    ProgramTreeP program = nullptr;
    ILGenerator *generator = nullptr;

    try {
        lexer = new Lexer(fileContent);

        lexer->analyseSource();

        parser = new Parser(lexer);

        program = parser->parseProgram();

        generator = new ILGenerator(program, "../test.il");
        generator->generateProgramIL();

    } catch (const CompilationException &e) {
        std::cout << e.what() << std::endl;
    }

    std::cout << "Compilation finished - cleaning memory" << std::endl;

    delete generator;
    delete program;
    delete parser;
    delete lexer;

    return 0;
}
