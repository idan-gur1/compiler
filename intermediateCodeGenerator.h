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
#include <unordered_map>
#include "treeNodes.h"
#include "threeAddressExpressionsAndStatements.h"
#include "errorHandling.h"


class ILGenerator {
public:
    std::vector<ThreeAddressStmt *> ilStmts;
    std::unordered_set<std::string> builtinFunctionsUsed;

    ILGenerator(ProgramTreeP program, std::string outfileName) : outfileName(std::move(outfileName)) {
        this->program = program;
    }

    ~ILGenerator() {
        for (const auto &ilStmt: ilStmts) {
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
