//
// Created by idang on 24/12/2023.
//

#ifndef COMPILER_INTERMEDIATECODEGENERATOR_H
#define COMPILER_INTERMEDIATECODEGENERATOR_H

#include <string>
#include <utility>
#include <sstream>
#include <typeindex>
#include <fstream>
#include <ranges>
#include "lexer.h"
#include "treeNodes.h"

class ThreeAddressExpr {
public:
    virtual ~ThreeAddressExpr() = default;
};

class UniExpr : public ThreeAddressExpr {
public:
    ~UniExpr() override = default;
};

class ImIntVal : public UniExpr {
public:
    std::string value;

    explicit ImIntVal(std::string value) {
        this->value = std::move(value);
    }

    ~ImIntVal() override = default;
};

class UniTemp : public UniExpr {
public:
    int id;

    explicit UniTemp(int id) {
        this->id = id;
    }
};

class VariableVal : public UniExpr {
public:
    Variable var;

    explicit VariableVal(Variable var) : var(std::move(var)) {
    }

    ~VariableVal() override = default;
};

class SubscriptableVariableVal : public VariableVal {
public:
    UniExpr *index;

    SubscriptableVariableVal(Variable var, UniExpr *index) : VariableVal(std::move(var)) {
        this->index = index;
    }

    ~SubscriptableVariableVal() override {
        delete index;
    }
};

class LogicalNotExpr : public UniExpr {
public:
    UniExpr *expr;

    explicit LogicalNotExpr(UniExpr *expr) {
        this->expr = expr;
    }

    ~LogicalNotExpr() override {
        delete expr;
    }
};

class NumericNegExpr : public UniExpr {
public:
    UniExpr *expr;

    explicit NumericNegExpr(UniExpr *expr) {
        this->expr = expr;
    }

    ~NumericNegExpr() override {
        delete expr;
    }
};

class AddrExpr : public ThreeAddressExpr {
public:
    VariableVal *addressable;

    explicit AddrExpr(VariableVal *addressable) {
        this->addressable = addressable;
    }

    ~AddrExpr() override {
        delete addressable;
    }
};

enum class ExprOperator {
    add,
    sub,
    mult,
    div,
    mod,
    logicalOr,
    logicalAnd,
    equals,
    notEquals,
    biggerThan,
    biggerThanEquals,
    lessThan,
    lessThanEquals,
};

class BinaryExpr : public ThreeAddressExpr {
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

class FunctionParamPushStmt : public ThreeAddressStmt {
public:
    VariableType varType;
    bool isPtr;
    ThreeAddressExpr *expr;

    FunctionParamPushStmt(VariableType varType, bool isPtr, ThreeAddressExpr *expr) : varType(varType) {
        this->isPtr = isPtr;
        this->expr = expr;
    }

    ~FunctionParamPushStmt() override {
        delete expr;
    }
};

class FunctionCallExpr : public UniExpr, public ThreeAddressStmt {
public:
    std::string functionName;
    VariableType retType;
    bool retPtr;

    FunctionCallExpr(std::string functionName, VariableType retType, bool retPtr)
            : functionName(std::move(functionName)),
              retType(retType),
              retPtr(retPtr) {
    }

    ~FunctionCallExpr() override = default;
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
    UniExpr *expr;

    GotoIfZeroStmt(std::string labelName, UniExpr *expr) : labelName(std::move(labelName)) {
        this->expr = expr;
    }

    ~GotoIfZeroStmt() override {
        delete expr;
    }
};

class GotoIfNotZeroStmt : public ThreeAddressStmt {
public:
    std::string labelName;
    UniExpr *expr;

    GotoIfNotZeroStmt(std::string labelName, UniExpr *expr) : labelName(std::move(labelName)) {
        this->expr = expr;
    }

    ~GotoIfNotZeroStmt() override {
        delete expr;
    }
};

class SetReturnValueStmt : public ThreeAddressStmt {
public:
    ThreeAddressExpr *expr;

    explicit SetReturnValueStmt(ThreeAddressExpr *expr) {
        this->expr = expr;
    }

    ~SetReturnValueStmt() override {
        delete expr;
    }
};

class ScopeEnterStmt : public ThreeAddressStmt {
public:
    std::vector<Variable> vars;

    explicit ScopeEnterStmt(std::vector<Variable> vars) : vars(std::move(vars)) {
    }
};

class ScopeExitStmt : public ThreeAddressStmt {
public:
    ScopeExitStmt() = default;
};

class FunctionDeclarationStmt : public ThreeAddressStmt {
public:
    std::string name;
    std::vector<Variable> params;
    int maxTemp = 0;

    FunctionDeclarationStmt(std::string name, std::vector<Variable> params) : name(std::move(name)),
                                                                              params(std::move(params)) {
    }
};

class FunctionExitStmt : public ThreeAddressStmt {
public:
    FunctionExitStmt() = default;
};

class ILGenerator {
public:
    std::vector<ThreeAddressStmt *> ilStmts;

    ILGenerator(ProgramTreeP program, std::string outfileName) : outfileName(std::move(outfileName)) {
        this->program = program;
    }

    ~ILGenerator() {
        for (auto ilStmt: ilStmts) {
            delete ilStmt;
        }
    }

    void generateProgramIL();

    void generateFunctionIL(NodeFunctionP function);

    void generateScopeIL(NodeScopeP scope);

    void generateStmtIL(NodeStmtP stmt);

    ThreeAddressExpr *generateExprIL(NodeExprP expr);

private:
    static std::unordered_map<std::type_index, ExprOperator> NodeExprToExprOperator;
    static std::unordered_map<ExprOperator, std::string> exprOperatorToStr;

    ProgramTreeP program;
    std::string outfileName;

    std::string currentFunctionName;
    int currentIfId = 0;
    int currentWhileId = 0;
    int currentTemp = 0;
    int maxTemp = 0;

    UniExpr *generateNumericExprIL(NodeExprP expr);

    TempAssignmentTAStmt *generateBinaryTempAssignmentIL(UniExpr *uniLhs, UniExpr *uniRhs, ExprOperator op);

    void generateBinaryExprIL(BinaryNodeExprP);

    UniExpr *generateUnaryExprIL(UnaryNodeExprP);

    UniExpr *convertTerminalToUniExpr(TerminalNodeExprP terminalExpr);

    FunctionCallExpr *generateFunctionCallExprIL(NodeFunctionCallP);

    int incCurrentTemp();

    static std::string ilStmtToStr(ThreeAddressStmt *);

    static std::string ilExprToStr(ThreeAddressExpr *);
};

typedef ThreeAddressExpr *ThreeAddressExprP;
typedef UniExpr *UniExprP;
typedef UniTemp *UniTempP;
typedef ImIntVal *ImIntValP;
typedef VariableVal *VariableValP;
typedef SubscriptableVariableVal *SubscriptableVariableValP;
typedef LogicalNotExpr *LogicalNotExprP;
typedef NumericNegExpr *NumericNegExprP;
typedef AddrExpr *AddrExprP;
typedef BinaryExpr *BinaryExprP;
typedef ThreeAddressStmt *ThreeAddressStmtP;
typedef TempAssignmentTAStmt *TempAssignmentTAStmtP;
typedef VarAssignmentTAStmt *VarAssignmentTAStmtP;
typedef FunctionParamPushStmt *FunctionParamPushStmtP;
typedef FunctionCallExpr *FunctionCallExprP;
typedef LabelStmt *LabelStmtP;
typedef GotoStmt *GotoStmtP;
typedef GotoIfZeroStmt *GotoIfZeroStmtP;
typedef GotoIfNotZeroStmt *GotoIfNotZeroStmtP;
typedef SetReturnValueStmt *SetReturnValueStmtP;
typedef ScopeEnterStmt *ScopeEnterStmtP;
typedef ScopeExitStmt *ScopeExitStmtP;
typedef FunctionDeclarationStmt *FunctionDeclarationStmtP;
typedef FunctionExitStmt *FunctionExitStmtP;

#endif //COMPILER_INTERMEDIATECODEGENERATOR_H
