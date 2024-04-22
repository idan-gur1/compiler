//
// Created by idang on 04/02/2024.
//

#ifndef COMPILER_GENERATION_H
#define COMPILER_GENERATION_H

#include <utility>

#include "intermediateCodeGenerator.h"

class Generator {
public:
    explicit Generator(ILGenerator *ilGenerator, std::string outFileName) : outFileName(std::move(outFileName)) {
        this->ilGenerator = ilGenerator;
    }

    void generateProgram();

private:
    const int ptrSize = 8;
    std::unordered_map<VariableType, int> typeSizes = {
            {VariableType::intType, 4},
            {VariableType::charType, 1},
    };
    std::unordered_map<int, std::string> sizeIdentifiers = {
            {8, "QWORD"},
            {4, "DWORD"},
            {2, "WORD"},
            {1, "BYTE"},
    };
    int *tempSizes;

    std::string convertUniExprToRegister(UniExprP, std::string);

    std::string convertAddrExprToRegister(AddrExprP, std::string);

    std::string convertBinaryExprToRegister(BinaryExprP, std::string);

    std::string convertTAExprToRegister(ThreeAddressExprP, const std::string &);

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

    ILGenerator *ilGenerator;
    std::string outFileName;
    std::stringstream programOut;
};

#endif //COMPILER_GENERATION_H
