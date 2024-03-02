//
// Created by idang on 24/12/2023.
//

#include <fstream>
#include "intermediateCodeGenerator.h"
#include "lexer.h"

std::string uniExprToStr(UniExpr *uniExpr) {
    if (auto val = dynamic_cast<UniValP>(uniExpr)) {
        return val->val.val;
    } else if (auto temp = dynamic_cast<UniTempP>(uniExpr)) {
        return "temp" + std::to_string(temp->id);
    }
    return "";
}

std::string ilStmtToStr(ThreeAddressStmt *taStmt) {
    std::stringstream strStream;

//    strStream << taStmt->targetIdent;
//
//    if (taStmt->temp) {
//        strStream << " = ";
//    } else {
//        strStream << " := ";
//    }
//
//    if (auto binExpr = dynamic_cast<BinaryExprP>(taStmt->expr)) {
//        strStream << binExpr->left.val << " " << getTokenName(binExpr->op) << " " << binExpr->right.val << std::endl;
//    } else if (auto uniExpr = dynamic_cast<UniExprP>(taStmt->expr)) {
//        strStream << uniExpr->val.val << std::endl;
//    }

    if (auto tempTAS = dynamic_cast<TempAssignmentTAStmtP>(taStmt)) {
        strStream << "temp" << tempTAS->id << " := ";
    } else if (auto varTAS = dynamic_cast<VarAssignmentTAStmtP>(taStmt)) {
        strStream << varTAS->targetIdent << " = ";
    }

    if (auto binExpr = dynamic_cast<BinaryExprP>(taStmt->expr)) {
        strStream << uniExprToStr(binExpr->left)
                  << " " << getTokenName(binExpr->op) << " "
                  << uniExprToStr(binExpr->right);
    } else if (auto uniExpr = dynamic_cast<UniExprP>(taStmt->expr)) {
        strStream << uniExprToStr(uniExpr);
    }

    strStream << std::endl;

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
//            this->ilStmts.push_back(new ThreeAddressStmt("t" + std::to_string(++currentTemp),
//                                                     new BinaryExpr(lhs->val, rhs->val, op)));
            this->ilStmts.push_back(new TempAssignmentTAStmt(++currentTemp,
                                                             new BinaryExpr(new UniVal(lhs->val),
                                                                            new UniVal(rhs->val), op)));
        } else if (lhs) {
//            generateExprIL(binExpr->right);
//            int rightTempNum = currentTemp;
//            this->ilStmts.push_back(new ThreeAddressStmt("t" + std::to_string(++currentTemp),
//                                                         new BinaryExpr(lhs->val,
//                                                                        Token(TokenType::tempIdentifier,
//                                                                              "t" + std::to_string(rightTempNum)),
//                                                                        op)));
            generateExprIL(binExpr->right);
//            int rightTempNum = currentTemp;
            this->ilStmts.push_back(new TempAssignmentTAStmt(currentTemp,
                                                             new BinaryExpr(new UniVal(lhs->val),
                                                                            new UniTemp(currentTemp), op)));
        } else if (rhs) {
//            generateExprIL(binExpr->left);
//            int leftTempNum = currentTemp;
//            this->ilStmts.push_back(new ThreeAddressStmt("t" + std::to_string(++currentTemp),
//                                                         new BinaryExpr(Token(TokenType::tempIdentifier,
//                                                                              "t" + std::to_string(leftTempNum)),
//                                                                        rhs->val, op)));
            generateExprIL(binExpr->left);
//            int leftTempNum = currentTemp;
            this->ilStmts.push_back(new TempAssignmentTAStmt(currentTemp,
                                                             new BinaryExpr(new UniTemp(currentTemp),
                                                                            new UniVal(rhs->val), op)));
        } else {
//            generateExprIL(binExpr->left);
//            int leftTempNum = currentTemp;
//
//            generateExprIL(binExpr->right);
//            int rightTempNum = currentTemp;
//
//            this->ilStmts.push_back(new ThreeAddressStmt("t" + std::to_string(++currentTemp),
//                                                         new BinaryExpr(Token(TokenType::tempIdentifier,
//                                                                              "t" + std::to_string(leftTempNum)),
//                                                                        Token(TokenType::tempIdentifier,
//                                                                              "t" + std::to_string(rightTempNum)),
//                                                                        op)));
            generateExprIL(binExpr->left);
//            int leftTempNum = currentTemp;

            generateExprIL(binExpr->right);
//            int rightTempNum = currentTemp;
            currentTemp--;
            this->ilStmts.push_back(new TempAssignmentTAStmt(currentTemp,
                                                             new BinaryExpr(new UniTemp(currentTemp),
                                                                            new UniTemp(currentTemp + 1), op)));
        }
    } else if (auto parenExpr = dynamic_cast<ParenthesisNodeExprP>(expr)) {
        generateExprIL(parenExpr->expr);
    } else if (auto terminalExpr = dynamic_cast<TerminalNodeExprP>(expr)) {
//        this->ilStmts.push_back(new ThreeAddressStmt("t" + std::to_string(++currentTemp),
//                                                     new UniVal(terminalExpr->val)));
    }
}

void ILGenerator::generateStmtIL(NodeAssignmentStmtP stmt) {
    this->currentTemp = 0;
    this->generateExprIL(stmt->expr);
//    auto tac = new ThreeAddressStmt(stmt->ident.val,
//                                    new UniExpr(Token(TokenType::tempIdentifier,
//                                                      "t" + std::to_string(currentTemp))));
    VarAssignmentTAStmt *tac = nullptr;
    if (auto terminalExpr = dynamic_cast<TerminalNodeExprP>(stmt->expr)) {
        tac = new VarAssignmentTAStmt(stmt->ident.val, new UniVal(terminalExpr->val));
    } else {
        tac = new VarAssignmentTAStmt(stmt->ident.val, new UniTemp(currentTemp));
    }

//    tac->temp = false;
    this->ilStmts.push_back(tac);
}

void ILGenerator::generateProgramIL() {
    for (NodeAssignmentStmtP naStmt: this->program->stmts) {
        generateStmtIL(naStmt);
    }

    std::ofstream outFile(this->outfileName);

    if (outFile.fail()) {
        std::cout << "File Error: Error while opening the file " << this->outfileName << std::endl;
        outFile.close();
        exit(1);
    }

    for (ThreeAddressStmt *tasP: this->ilStmts) {
        outFile << ilStmtToStr(tasP);
    }

    outFile.close();

}
