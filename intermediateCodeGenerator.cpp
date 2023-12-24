//
// Created by idang on 24/12/2023.
//

#include "intermediateCodeGenerator.h"

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

        if (lhs && rhs) {
            this->ilStmts.push_back(ThreeAddressStmt("t" + std::to_string(++currentTemp),
                                                     BinaryExpr(lhs->val, rhs->val, op)));
        } else if (lhs) {
            generateExprIL(binExpr->right);
            int rightTempNum = currentTemp;
            this->ilStmts.push_back(ThreeAddressStmt("t" + std::to_string(++currentTemp),
                                                     BinaryExpr(lhs->val,
                                                                Token(TokenType::identifier,
                                                                      "t" + std::to_string(rightTempNum)), op)));
        } else if (rhs) {
            generateExprIL(binExpr->left);
            int leftTempNum = currentTemp;
            this->ilStmts.push_back(ThreeAddressStmt("t" + std::to_string(++currentTemp),
                                                     BinaryExpr(Token(TokenType::identifier,
                                                                      "t" + std::to_string(leftTempNum)),
                                                                rhs->val, op)));
        } else {
            generateExprIL(binExpr->left);
            int leftTempNum = currentTemp;

            generateExprIL(binExpr->right);

            this->ilStmts.push_back(ThreeAddressStmt("t" + std::to_string(++currentTemp),
                                                     BinaryExpr(Token(TokenType::identifier,
                                                                      "t" + std::to_string(leftTempNum)),
                                                                Token(TokenType::identifier,
                                                                      "t" + std::to_string(currentTemp)), op)));
        }
    }
}
