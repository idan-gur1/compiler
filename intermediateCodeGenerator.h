//
// Created by idang on 24/12/2023.
//

#ifndef COMPILER_INTERMEDIATECODEGENERATOR_H
#define COMPILER_INTERMEDIATECODEGENERATOR_H

#include <string>
#include <utility>
#include <sstream>
#include <typeindex>
#include "lexer.h"
#include "treeNodes.h"

class ThreeAddressExpr {
public:
    virtual ~ThreeAddressExpr() = default;
};

class UniExpr : public ThreeAddressExpr{
public:
    ~UniExpr() override = default;
};

class ImIntVal : public UniExpr{
public:
    std::string value;

    explicit ImIntVal(std::string value) {
        this->value = std::move(value);
    }

    ~ImIntVal() override = default;
};

class UniTemp : public UniExpr{
public:
    int id;

    explicit UniTemp(int id) {
        this->id = id;
    }
};

class VariableVal : public UniExpr{
public:
    Variable var;

    explicit VariableVal(Variable var) : var(std::move(var)) {
    }

    ~VariableVal() override = default;
};

class SubscriptableVariableVal : public VariableVal{
public:
    UniExpr *index;

    SubscriptableVariableVal(Variable var, UniExpr *index) : VariableVal(std::move(var)) {
        this->index = index;
    }

    ~SubscriptableVariableVal() override {
        delete index;
    }
};

enum class ExprOperator {
    add,
    sub,
    mult,
    div,
    logicalOr,
    logicalAnd,
    equals,
    notEquals,
    biggerThan,
    biggerThanEquals,
    lessThan,
    lessThanEquals,
};

class BinaryExpr : public ThreeAddressExpr{
public:
    UniExpr *left;
    UniExpr *right;
    ExprOperator op;

    BinaryExpr(UniExpr *left, UniExpr *right, ExprOperator op) : op(op) {
        this->left = left;
        this->right = right;
    }

    ~BinaryExpr() override {
        delete left;
        delete right;
    }
};

class ThreeAddressStmt {
public:
    virtual ~ThreeAddressStmt() = default;
};

class TempAssignmentTAStmt : public ThreeAddressStmt {
public:
    int id;
    ThreeAddressExpr *expr;

    TempAssignmentTAStmt(int id, ThreeAddressExpr *expr) {
        this->id = id;
        this->expr = expr;
    }

    ~TempAssignmentTAStmt() override {
        delete expr;
    }
};

class VarAssignmentTAStmt : public ThreeAddressStmt {
public:
    VariableVal *var;
    ThreeAddressExpr *expr;

    VarAssignmentTAStmt(VariableVal *var, ThreeAddressExpr *expr) {
        this->var = var;
        this->expr = expr;
    }

    ~VarAssignmentTAStmt() override {
        delete var;
        delete expr;
    }
};

/*class FunctionDeclarationStmt : public ThreeAddressStmt {
public:
    std::string funcName;
    std::vector<Variable> params;
    int maxTemp = 0;

    FunctionDeclarationStmt(std::string funcName, std::vector<Variable> params) : funcName(std::move(funcName)) {
        this->params = std::move(params);
    }
};

class SetFunctionReturnValStmt : public ThreeAddressStmt {
public:
    UniExpr expr;

    explicit SetFunctionReturnValStmt(const UniExpr& expr) : expr(expr) {
    }
};

class FunctionExitStmt : public ThreeAddressStmt {
public:
    std::string funcName;

    explicit FunctionExitStmt(std::string funcName) : funcName(std::move(funcName)) {
    }
};

class ScopeDeclarationStmt : public ThreeAddressStmt {
public:
    int scopeId;
    std::vector<Variable> vars;

    ScopeDeclarationStmt(int scopeId, std::vector<Variable> vars) {
        this->scopeId = scopeId;
        this->vars = std::move(vars);
    }
};

class ScopeExitStmt : public ThreeAddressStmt {
public:
    int scopeId;

    explicit ScopeExitStmt(int scopeId) {
        this->scopeId = scopeId;
    }
};

class LabelStmt : public ThreeAddressStmt {
public:
    std::string labelName;

    explicit LabelStmt(std::string labelName) : labelName(std::move(labelName)) {
    }
};

class GotoStmt : public ThreeAddressStmt {
public:
    std::string labelName;

    explicit GotoStmt(std::string labelName) : labelName(std::move(labelName)) {
    }
};

class GotoIfZeroStmt : public ThreeAddressStmt {
public:
    std::string labelName;
    UniExpr expr;

    GotoIfZeroStmt(std::string labelName, const UniExpr& expr) : labelName(std::move(labelName)), expr(expr) {
    }
};

class GotoIfNotZeroStmt : public ThreeAddressStmt {
public:
    std::string labelName;
    UniExpr expr;

    GotoIfNotZeroStmt(std::string labelName, const UniExpr& expr) : labelName(std::move(labelName)), expr(expr) {
    }
};*/

typedef UniExpr *UniExprP;
typedef UniTemp *UniTempP;
typedef BinaryExpr *BinaryExprP;
typedef ThreeAddressExpr *ThreeAddressExprP;
typedef ThreeAddressStmt *ThreeAddressStmtP;
typedef TempAssignmentTAStmt *TempAssignmentTAStmtP;
typedef VarAssignmentTAStmt *VarAssignmentTAStmtP;

std::string ilStmtToStr(ThreeAddressStmt *taStmt);

class ILGenerator {
public:
    std::vector<ThreeAddressStmt *> ilStmts;

    ILGenerator(ProgramTreeP program, std::string outfileName) : outfileName(std::move(outfileName)) {
        this->program = program;

        exprOperatorMap = {
                {typeid(NodeAddExpr), ExprOperator::add},
                {typeid(NodeSubExpr), ExprOperator::sub},
                {typeid(NodeMultExpr), ExprOperator::mult},
                {typeid(NodeDivExpr), ExprOperator::div},
                {typeid(NodeLogicalOrExpr), ExprOperator::logicalOr},
                {typeid(NodeLogicalAndExpr), ExprOperator::logicalAnd},
                {typeid(NodeBoolEqualsExpr), ExprOperator::equals},
                {typeid(NodeBoolNotEqualsExpr), ExprOperator::notEquals},
                {typeid(NodeBiggerThanExpr), ExprOperator::biggerThan},
                {typeid(NodeBiggerThanEqualExpr), ExprOperator::biggerThanEquals},
                {typeid(NodeLessThanExpr), ExprOperator::lessThan},
                {typeid(NodeLessThanEqualExpr), ExprOperator::lessThanEquals},
        };
    }

    ~ILGenerator() {
        for (int i = 0; i < ilStmts.size(); ++i) {
            delete ilStmts[i];
        }
        delete program;
    }

    void generateProgramIL();
    void generateFunctionIL(NodeFunctionP function);
    void generateScopeIL(NodeScopeP scope);
    void generateStmtIL(NodeStmtP stmt);
    void generateExprIL(NodeExprP expr);

private:
    TempAssignmentTAStmt *generateTempAssignmentIL(UniExprP uniLhs, UniExprP uniRhs, ExprOperator op);
    void generateBinaryExprIL(BinaryNodeExprP);
    UniExpr *convertTerminalToUniExpr(TerminalNodeExprP terminalExpr);

    ProgramTreeP program;
    std::string outfileName;

    std::unordered_map<std::type_index, ExprOperator> exprOperatorMap;
    std::string currentFunctionName;
    int currentScopeId = 0;
    int currentTemp = 0;
    int maxTemp = 0;
};

#endif //COMPILER_INTERMEDIATECODEGENERATOR_H
