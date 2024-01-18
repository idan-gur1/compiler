//
// Created by idang on 24/12/2023.
//

#include <fstream>
#include "intermediateCodeGenerator.h"
#include "lexer.h"

std::string ilToStr(ThreeAddressStmt *taStmt) {
    std::stringstream strStream;

    strStream << taStmt->targetIdent;

    if (taStmt->temp) {
        strStream << " = ";
    } else {
        strStream << " := ";
    }

    if (auto binExpr = dynamic_cast<BinaryExprP>(taStmt->expr)) {
        strStream << binExpr->left.val << " " << getTokenName(binExpr->op) << " " << binExpr->right.val << std::endl;
    } else if (auto uniExpr = dynamic_cast<UniExprP>(taStmt->expr)) {
        strStream << uniExpr->val.val << std::endl;
    }

    return strStream.str();
}

void ILGenerator::generateExprIL(NodeExprP expr) {
    if (auto binExpr = dynamic_cast<BinaryNodeExprP>(expr)) {
        TokenType op = TokenType::NO_TOKEN;
        if (auto divExpr = dynamic_cast<NodeDivExprP>(binExpr)) {
            op = TokenType::div;
        } else if (auto mulExpr = dynamic_cast<NodeMultExprP>(binExpr)) {
            op = TokenType::mult;
        } else if (auto addExpr = dynamic_cast<NodeAddExprP>(binExpr)) {
            op = TokenType::plus;
        } else if (auto subExpr = dynamic_cast<NodeSubExprP>(binExpr)) {
            op = TokenType::minus;
        }

        auto lhs = dynamic_cast<TerminalNodeExprP>(binExpr->left);
        auto rhs = dynamic_cast<TerminalNodeExprP>(binExpr->right);
//        std::cout << "test  " << getTokenName(op) << std::endl;

        if (lhs && rhs) {
//            std::cout << "1" << std::endl;
            this->ilStmts.push_back(new ThreeAddressStmt("t" + std::to_string(++currentTemp),
                                                     new BinaryExpr(lhs->val, rhs->val, op)));
//            std::cout << "1after" << std::endl;
        } else if (lhs) {
//            std::cout << "2" << std::endl;
            generateExprIL(binExpr->right);
            int rightTempNum = currentTemp;
            this->ilStmts.push_back(new ThreeAddressStmt("t" + std::to_string(++currentTemp),
                                                     new BinaryExpr(lhs->val,
                                                                Token(TokenType::identifier,
                                                                      "t" + std::to_string(rightTempNum)), op)));
        } else if (rhs) {
//            std::cout << "3" << std::endl;
            generateExprIL(binExpr->left);
            int leftTempNum = currentTemp;
            this->ilStmts.push_back(new ThreeAddressStmt("t" + std::to_string(++currentTemp),
                                                     new BinaryExpr(Token(TokenType::identifier,
                                                                      "t" + std::to_string(leftTempNum)),
                                                                rhs->val, op)));
        } else {
//            std::cout << "4" << std::endl;
            generateExprIL(binExpr->left);
            int leftTempNum = currentTemp;

            generateExprIL(binExpr->right);
            int rightTempNum = currentTemp;

            this->ilStmts.push_back(new ThreeAddressStmt("t" + std::to_string(++currentTemp),
                                                     new BinaryExpr(Token(TokenType::identifier,
                                                                      "t" + std::to_string(leftTempNum)),
                                                                Token(TokenType::identifier,
                                                                      "t" + std::to_string(rightTempNum)), op)));
        }
    } else if (auto parenExpr = dynamic_cast<ParenthesisNodeExprP>(expr)) {
        generateExprIL(parenExpr->expr);
    }
}

void ILGenerator::generateStmtIL(NodeAssignmentStmtP stmt) {
    this->generateExprIL(stmt->expr);
    auto tac = new ThreeAddressStmt(stmt->ident.val,
                                                 new UniExpr(Token(TokenType::identifier,
                                                                   "t" + std::to_string(currentTemp))));
    tac->temp = false;
    this->ilStmts.push_back(tac);
}

void ILGenerator::generateProgramIL() {
    for (NodeAssignmentStmtP naStmt : this->program->stmts) {
        generateStmtIL(naStmt);
    }

    std::ofstream outFile(this->outfileName);

    if (outFile.fail()) {
        std::cout << "File Error: Error while opening the file " << this->outfileName << std::endl;
        outFile.close();
        exit(1);
    }

    for (ThreeAddressStmt *taeP:this->ilStmts) {
        outFile << ilToStr(taeP);
    }

    outFile.close();

}
