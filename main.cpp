#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "lexer.h"
#include "parser.h"
#include "intermediateCodeGenerator.h"
#include "errorHandling.h"

void printExprTree(NodeExprP expr) {

    //auto imIntFactor = dynamic_cast<NodeImIntFactorP>(expr);

    if (auto terminal = dynamic_cast<TerminalNodeExprP>(expr)) {

        return;
    }

    if (auto binaryExpr = dynamic_cast<BinaryNodeExprP>(expr)) {
        printExprTree(binaryExpr->left);
        printExprTree(binaryExpr->right);

        if (auto divExpr = dynamic_cast<NodeDivExprP>(binaryExpr)) {
            std::cout << "div term" << std::endl;
        }
        else if (auto mulExpr = dynamic_cast<NodeMultExprP>(binaryExpr)) {
            std::cout << "mult term" << std::endl;
        }
        else if (auto addExpr = dynamic_cast<NodeAddExprP>(binaryExpr)) {
            std::cout << "add expr" << std::endl;
        }
        else if (auto subExpr = dynamic_cast<NodeSubExprP>(binaryExpr)) {
            std::cout << "sub expr" << std::endl;
        }

    } else if (auto parenthesisExpr = dynamic_cast<NodeParenthesisExprP>(expr)) {
        std::cout << "entered parenthesis node" << std::endl;
        printExprTree(parenthesisExpr->expr);
        std::cout << "exited parenthesis node" << std::endl;
    }
}

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
