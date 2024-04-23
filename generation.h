//
// Created by idang on 04/02/2024.
//

#ifndef COMPILER_GENERATION_H
#define COMPILER_GENERATION_H

#include <utility>
#include <stack>

#include "intermediateCodeGenerator.h"


class VariableStackData {
public:
    int stackPos;
    int varSize;

    VariableStackData(int stackPos, int varSize) : stackPos(stackPos), varSize(varSize) {

    }
};

class Generator {
public:
    Generator(ILGenerator *ilGenerator, std::string outFileName) : outFileName(std::move(outFileName)) {
        this->ilGenerator = ilGenerator;

        typeSizes = {
                {VariableType::intType,  4},
                {VariableType::charType, 1},
        };

        sizeIdentifiers = {
                {8, "QWORD"},
                {4, "DWORD"},
                {2, "WORD"},
                {1, "BYTE"},
        };

        BinaryExprToAsmStrSteps = {
                {ExprOperator::add,              "add rax, rbx\n"},

                {ExprOperator::sub,              "sub rax, rbx\n"},

                {ExprOperator::mult,             "imul rbx\n"},

                {ExprOperator::div,              "cqo\n"
                                                 "idiv rbx\n"},

                {ExprOperator::mod,              "cqo\n"
                                                 "idiv rbx\n"
                                                 "mov rax, rdx\n"},

                {ExprOperator::equals,           "xor rax, rbx\n"
                                                 "setz al\n"
                                                 "movzx rax, al\n"},

                {ExprOperator::notEquals,        "xor rax, rbx\n"
                                                 "setnz al\n"
                                                 "movzx rax, al\n"},

                {ExprOperator::biggerThan,       "cmp rax, rbx\n"
                                                 "setg al\n"
                                                 "movzx rax, al\n"},

                {ExprOperator::biggerThanEquals, "cmp rax, rbx\n"
                                                 "setge al\n"
                                                 "movzx rax, al\n"},

                {ExprOperator::lessThan,         "cmp rax, rbx\n"
                                                 "setl al\n"
                                                 "movzx rax, al\n"},

                {ExprOperator::lessThanEquals,   "cmp rax, rbx\n"
                                                 "setle al\n"
                                                 "movzx rax, al\n"}
        };
    }

    void generateProgram();

private:
    static const int bit64RegSize = 8;
    static const int ptrSize = 8;

    static std::string getStackAddr(const VariableStackData &);

    static std::string movTo64BitReg(const std::string &, const std::string &, int);

    std::string getSubscriptableStackPosition(SubscriptableVariableValP subVar, const std::string &reg);

    void convertUniExprToRegister(UniExprP, const std::string &);

    void convertAddrExprToRegister(AddrExprP, const std::string &);

    void convertBinaryExprToRegister(BinaryExprP);

    void convertTAExprToRegister(ThreeAddressExprP);

    void convertTAStmtToAsm(ThreeAddressStmtP);

    void convertTempAssignmentToAsm(TempAssignmentTAStmtP);

    void convertVarAssignmentToAsm(VarAssignmentTAStmtP);

    void convertFunctionParamPushToAsm(FunctionParamPushStmtP);

    void convertFunctionCallToAsm(FunctionCallExprP);

    void convertLabelToAsm(LabelStmtP);

    void convertGotoToAsm(GotoStmtP);

    void convertGotoIfZeroToAsm(GotoIfZeroStmtP);

    void convertGotoIfNotZeroToAsm(GotoIfNotZeroStmtP);

    void convertSetReturnValueToAsm(SetReturnValueStmtP);

    void convertScopeEnterToAsm(ScopeEnterStmtP);

    void convertScopeExitToAsm(ScopeExitStmtP);

    void convertFunctionDeclarationToAsm(FunctionDeclarationStmtP);

    void convertFunctionExitToAsm(FunctionExitStmtP);

    static std::string getRegisterBySize(int, const std::string &);

    std::unordered_map<VariableType, int> typeSizes;
    std::unordered_map<int, std::string> sizeIdentifiers;
    std::unordered_map<ExprOperator, std::string> BinaryExprToAsmStrSteps;
    std::unordered_map<std::string, std::stack<VariableStackData>> variableStack;

    ILGenerator *ilGenerator;
    std::string outFileName;
    std::stringstream programOut;
    int labelCount = 0;
};

#endif //COMPILER_GENERATION_H
