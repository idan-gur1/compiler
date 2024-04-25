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

class ScopeFrame {
public:
    int frameSize;
    std::unordered_set<std::string> frameVars;

    ScopeFrame(int frameSize, std::unordered_set<std::string> frameVars) : frameSize(frameSize),
                                                                           frameVars(std::move(frameVars)) {

    }
};

class Generator {
public:
    Generator(ILGenerator *ilGenerator, std::string outFileName) : outFileName(std::move(outFileName)) {
        this->ilGenerator = ilGenerator;
    }

    void generateProgram();

private:
    inline static const std::string PATH_TO_BUILTIN_FUNCTIONS_FOLDER = "../builtins/";
    static const int STACK_OVERFLOW_LIMIT = 200;

    static const int TEMP_SIZE = 8;
    static const int BIT_64_REG_SIZE = 8;
    static const int PTR_SIZE = 8;

    static std::unordered_map<VariableType, int> typeSizes;
    static std::unordered_map<int, std::string> sizeIdentifiers;
    static std::unordered_map<ExprOperator, std::string> BinaryExprToAsmStrSteps;

    std::unordered_map<std::string, std::stack<VariableStackData>> variableStack;
    std::stack<ScopeFrame> scopeFrameStack;

    ILGenerator *ilGenerator;
    std::string outFileName;
    std::stringstream programOut;
    int labelCount = 0;
    int currentRelativeSP = 0;
    int FuncParamsOffsetSP = 0;
    int paramsSize = 0;

    static std::string getStackAddr(const VariableStackData &);

    static std::string movTo64BitReg(const std::string &, const std::string &, int);

    std::string getSubscriptableStackPosition(SubscriptableVariableValP subVar, const std::string &reg);

    void convertUniExprToRegister(UniExprP, const std::string &);

    void convertAddrExprToRegister(AddrExprP, const std::string &);

    void convertBinaryExprToRegister(BinaryExprP);

    void convertTAExprToRaxRegister(ThreeAddressExprP);

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

    void convertScopeExitToAsm();

    void convertFunctionDeclarationToAsm(FunctionDeclarationStmtP);

    void convertFunctionExitToAsm();

    void generateAsmFunctionCall(const std::string& funcName);

    static std::string getAxRegisterBySize(int);

    static int sizeByTypeAndPtr(VariableType, bool, int);
};

inline std::unordered_map<VariableType, int> Generator::typeSizes = {
        {VariableType::intType,  4},
        {VariableType::charType, 1},
};

inline std::unordered_map<int, std::string> Generator::sizeIdentifiers = {
        {8, "QWORD"},
        {4, "DWORD"},
        {2, "WORD"},
        {1, "BYTE"},
};

#endif //COMPILER_GENERATION_H
