//
// Created by idang on 24/12/2023.
//

#ifndef COMPILER_INTERMEDIATECODEGENERATOR_H
#define COMPILER_INTERMEDIATECODEGENERATOR_H

#include <string>
#include <utility>
#include "lexer.h"
#include "treeNodes.h"

class ThreeAddressExpr {
public:
    virtual ~ThreeAddressExpr() = default;
};

class UniExpr : public ThreeAddressExpr{
public:
    Token val;

    explicit UniExpr(Token val) {
        this->val = std::move(val);
    }
};

class BinaryExpr : public ThreeAddressExpr{
public:
    Token left;
    Token right;
    TokenType op;

    BinaryExpr(Token left, Token right, TokenType op) {
        this->left = std::move(left);
        this->right = std::move(right);
        this->op = op;
    }
};

class ThreeAddressStmt {
public:
    std::string targetIdent;
    ThreeAddressExpr expr;

    ThreeAddressStmt(std::string targetIdent, const ThreeAddressExpr& expr) {
        this->targetIdent = std::move(targetIdent);
        this->expr = expr;
    }
};


class ILGenerator {
public:
    ILGenerator(NodeScopeP program, std::string outfileName) {
        this->program = program;
        this->outfileName= std::move(outfileName);
    }

    void generateProgramIL();
    void generateStmtIL();
    void generateExprIL(NodeExprP expr);

private:
    std::vector<ThreeAddressStmt> ilStmts;
    NodeScopeP program;
    std::string outfileName;
    int currentTemp = 0;
};

#endif //COMPILER_INTERMEDIATECODEGENERATOR_H
