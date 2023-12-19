#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "lexer.h"
#include "parser.h"

void printTree(NodeExprP expr) {

    //auto imIntFactor = dynamic_cast<NodeImIntFactorP>(expr);

    if (auto imIntFactor = dynamic_cast<NodeImIntFactorP>(expr)) {
        std::cout << "Factor: " << imIntFactor->val.val << std::endl;
        return;
    }

    if (auto divTerm = dynamic_cast<NodeMultTermP>(expr)) {
        printTree(divTerm->left);
        std::cout << "div term" << std::endl;
        printTree(divTerm->right);
    }
    else if (auto mulTerm = dynamic_cast<NodeMultTermP>(expr)) {
        printTree(mulTerm->left);
        std::cout << "mult term" << std::endl;
        printTree(mulTerm->right);
    }
    else if (auto facTerm = dynamic_cast<NodeFactorTermP>(expr)) {
        printTree(facTerm->factor);
        std::cout << "factor term" << std::endl;
    }
    else if (auto addExpr = dynamic_cast<NodeAddExprP>(expr)) {
        printTree(addExpr->left);
        std::cout << "add expr" << std::endl;
        printTree(addExpr->right);
    }
    else if (auto subExpr = dynamic_cast<NodeSubExprP>(expr)) {
        printTree(subExpr->left);
        std::cout << "sub expr" << std::endl;
        printTree(subExpr->right);
    }
    else if (auto termExpr = dynamic_cast<NodeTermExprP>(expr)) {
        printTree(termExpr->term);
        std::cout << "term expr" << std::endl;
    }
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        std::cout << "Enter the file to be compiled." << std::endl;
        std::cout << "Usage: ./compiler [filename].ig" << std::endl;

        return 1;
    }

    std::ifstream inputFile(argv[1]);

    if (inputFile.fail()) {
        std::cout << "Error with reading the input file." << std::endl;
        inputFile.close();

        return 1;
    }

    std::string fileContent;

    {
        std::stringstream fileBuffer;
        fileBuffer << inputFile.rdbuf();

        fileContent = fileBuffer.str();
    }

    inputFile.close();

//    Lexer l(fileContent);
    auto *lexer = new Lexer(fileContent);

    std::vector<Token> tokens = lexer->analyseSource();

    std::cout << std::setw(20) << std::left << "Token" << std::setw(20) << std::left << "Value" << std::endl;

    for (const Token& t : tokens) {
        std::cout << std::setw(20) << std::left << getTokenName(t.type) << std::setw(20) << std::left << t.val << std::endl;
    }

    auto parser = new Parser(lexer);

    NodeExprP expr = parser->parseExpr();

    printTree(expr);

    delete expr;
    delete parser;
    delete lexer;

    return 0;
}
