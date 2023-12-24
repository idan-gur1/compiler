#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include "lexer.h"
#include "parser.h"

void printExprTree(NodeExprP expr) {

    //auto imIntFactor = dynamic_cast<NodeImIntFactorP>(expr);

    if (auto terminal = dynamic_cast<TerminalNodeExprP>(expr)) {
        std::cout << "Terminal -> type: " << getTokenName(terminal->val.type) << "Terminal -> type: " << terminal->val.val << std::endl;
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

    } else if (auto parenthesisExpr = dynamic_cast<ParenthesisNodeExprP>(expr)) {
        std::cout << "entered parenthesis node" << std::endl;
        printExprTree(parenthesisExpr->expr);
        std::cout << "exited parenthesis node" << std::endl;
    }

    // old printing
    /*if (auto divExpr = dynamic_cast<NodeDivExprP>(expr)) {
        printTree(divExpr->left);
        std::cout << "div term" << std::endl;
        printTree(divExpr->right);
    }
    else if (auto mulExpr = dynamic_cast<NodeMultExprP>(expr)) {
        printTree(mulExpr->left);
        std::cout << "mult term" << std::endl;
        printTree(mulExpr->right);
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
    }*/
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

    auto *fileBuffer = new std::stringstream();
    *fileBuffer << inputFile.rdbuf();

    fileContent = fileBuffer->str();

    delete fileBuffer;

    inputFile.close();

    auto *lexer = new Lexer(fileContent);

    std::vector<Token> tokens = lexer->analyseSource();

    /*std::cout << std::setw(20) << std::left << "Token" << std::setw(20) << std::left << "Value" << std::endl;

    for (const Token& t : tokens) {
        std::cout << std::setw(20) << std::left << getTokenName(t.type) << std::setw(20) << std::left << t.val << std::endl;
    }*/

    auto parser = new Parser(lexer);

    //NodeExprP expr = parser->parseExpr();

    //printExprTree(expr);

    auto stmt = parser->parseStmt();

    printExprTree(stmt->expr);

    //delete expr;
    delete stmt;
    delete parser;
    delete lexer;

    return 0;
}
