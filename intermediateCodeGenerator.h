//
// Created by idang on 24/12/2023.
//

#ifndef COMPILER_INTERMEDIATECODEGENERATOR_H
#define COMPILER_INTERMEDIATECODEGENERATOR_H

#include <string>
#include <utility>
#include <sstream>
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
    ThreeAddressExpr *expr;
    bool temp = true;

    ThreeAddressStmt(std::string targetIdent, ThreeAddressExpr *expr) {
        this->targetIdent = std::move(targetIdent);
        this->expr = expr;
    }

    ~ThreeAddressStmt() {
        std::cout << "dead";
        delete expr;
    }
};

typedef UniExpr *UniExprP;
typedef BinaryExpr *BinaryExprP;
typedef ThreeAddressExpr *ThreeAddressExprP;

std::string ilToStr(ThreeAddressStmt *taStmt);

class ILGenerator {
public:
    std::vector<ThreeAddressStmt *> ilStmts;

    ILGenerator(NodeScopeP program, std::string outfileName) {
        this->program = program;
        this->outfileName= std::move(outfileName);
    }

    void generateProgramIL();
    void generateStmtIL(NodeAssignmentStmtP stmt);
    void generateExprIL(NodeExprP expr);

private:
    NodeScopeP program;
    std::string outfileName;
    int currentTemp = 0;
};

#endif //COMPILER_INTERMEDIATECODEGENERATOR_H
