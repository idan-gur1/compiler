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
//    Token val;
//
//    explicit UniExpr(Token val) {
//        this->val = std::move(val);
//    }
    ~UniExpr() override = default;
};

class UniVal : public UniExpr{
public:
    Token val;

    explicit UniVal(Token val) {
        this->val = std::move(val);
    }
};

class UniTemp : public UniExpr{
public:
    int id;

    explicit UniTemp(int id) {
        this->id = id;
    }
};

class BinaryExpr : public ThreeAddressExpr{
public:
    UniExpr *left;
    UniExpr *right;
    TokenType op;

    BinaryExpr(UniExpr *left,UniExpr *right, TokenType op) {
        this->left = left;
        this->right = right;
        this->op = op;
    }

    ~BinaryExpr() override {
        delete left;
        delete right;
    }
};

class ThreeAddressStmt {
public:
    /*std::string targetIdent;
    ThreeAddressExpr *expr;
    bool temp;
    int id;

    ThreeAddressStmt(std::string targetIdent, ThreeAddressExpr *expr) {
        this->targetIdent = std::move(targetIdent);
        this->expr = expr;
        this->temp = false;
        this->id = 0;
    }

    explicit ThreeAddressStmt(int tId, ThreeAddressExpr *expr) {
        this->id = tId;
        this->expr = expr;
        this->temp = true;
        this->targetIdent = "";
    }*/
    ThreeAddressExpr *expr;

    explicit ThreeAddressStmt(ThreeAddressExpr *expr) {
        this->expr = expr;
    }

    virtual ~ThreeAddressStmt() {
        delete expr;
    }
};

class TempAssignmentTAStmt : public ThreeAddressStmt {
public:
    int id;
    TempAssignmentTAStmt(int id, ThreeAddressExpr *expr) : ThreeAddressStmt(expr) {
        this->id = id;
    }
};

class VarAssignmentTAStmt : public ThreeAddressStmt {
public:
    std::string targetIdent;
    VarAssignmentTAStmt(std::string targetIdent, ThreeAddressExpr *expr) : ThreeAddressStmt(expr) {
        this->targetIdent = std::move(targetIdent);
    }
};

typedef UniExpr *UniExprP;
typedef UniTemp *UniTempP;
typedef UniVal *UniValP;
typedef BinaryExpr *BinaryExprP;
typedef ThreeAddressExpr *ThreeAddressExprP;
typedef ThreeAddressStmt *ThreeAddressStmtP;
typedef TempAssignmentTAStmt *TempAssignmentTAStmtP;
typedef VarAssignmentTAStmt *VarAssignmentTAStmtP;

std::string ilStmtToStr(ThreeAddressStmt *taStmt);

class ILGenerator {
public:
    std::vector<ThreeAddressStmt *> ilStmts;

    ILGenerator(NodeScopeP program, std::string outfileName) {
        this->program = program;
        this->outfileName= std::move(outfileName);
    }

    ~ILGenerator() {
        for (int i = 0; i < ilStmts.size(); ++i) {
            delete ilStmts[i];
        }
        delete program;
    }

    void generateProgramIL();
    void generateScopeIL(NodeScopeP scope);
    void generateStmtIL(NodeAssignmentStmtP stmt);
    void generateExprIL(NodeExprP expr);

private:
    NodeScopeP program;
    std::string outfileName;
    int currentTemp = 0;
    int maxTemp = 0;
};

#endif //COMPILER_INTERMEDIATECODEGENERATOR_H
