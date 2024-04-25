//
// Created by idang on 04/02/2024.
//

#include "generation.h"

void Generator::generateProgram() {
    this->programOut << "section .data\n"
                        "overflowErrMsg db 'Stack overflow, exiting', 0xa\n"
                        "LEN equ $ - overflowErrMsg\n\n"
                        "section .text\n"
                        "global _start\n"
                        "_start:\n"
                        "mov r8, 0                ; function call stack counter\n"
                        "call main\n"
                        "movsx rdi, eax           ; exit code\n"
                        "mov rax, 0x3c            ; syscall number for sys_exit\n"
                        "syscall\n"
                        "\n_overflow:\n"
                        "mov rax, 1               ; syscall number for sys_write\n"
                        "mov rdi, 1               ; file descriptor 1 (stdout)\n"
                        "mov rsi, overflowErrMsg  ; pointer to the param\n"
                        "mov rdx, LEN             ; bytes to write\n"
                        "syscall\n"
                        "mov rax, 0x3c            ; syscall number for sys_exit\n"
                        "mov rdi, 1               ; exit code for error\n"
                        "syscall\n\n";

    for (const auto &builtinName: this->ilGenerator->builtinFunctionsUsed) {
        std::ifstream inputFile(PATH_TO_BUILTIN_FUNCTIONS_FOLDER + builtinName + ".asm");

        if (inputFile.fail()) {
            inputFile.close();
            throw CompilationException("Can't read builtin function " + builtinName);
        }

        std::stringstream fileBuffer;
        fileBuffer << inputFile.rdbuf();

        this->programOut << fileBuffer.str() << "\n\n";

        inputFile.close();
    }

    for (auto ilStmt: this->ilGenerator->ilStmts) {
        convertTAStmtToAsm(ilStmt);
    }

    std::ofstream outFile(this->outFileName);

    if (outFile.fail()) {
        outFile.close();
        throw FileOpenException(this->outFileName);
    }

    outFile << this->programOut.str();

    outFile.close();
}

std::string Generator::movTo64BitReg(const std::string &reg, const std::string &val, int size) {
    std::string movType = "mov ";

    if (size < BIT_64_REG_SIZE) {
        movType = "movsx ";
    }

    return movType + reg + ", " + val;
}

std::string Generator::getStackAddr(const VariableStackData &var) {
    if (var.stackPos > 0) {
        return "rbp + " + std::to_string(var.stackPos);
    }

    return "rbp - " + std::to_string(std::abs(var.stackPos));
}

std::string Generator::getSubscriptableStackPosition(SubscriptableVariableValP subVar,
                                                     const std::string &freeReg) {
    VariableStackData varData = this->variableStack[subVar->var.name].top();
    int typeSize = varData.varSize;
    std::string varBaseAddr = getStackAddr(varData);

    if (subVar->var.ptrType) {
        this->programOut << "mov " << freeReg << ", QWORD [" << varBaseAddr << "]\n";
        varBaseAddr = freeReg;
        typeSize = typeSizes[subVar->var.type];
    }

    std::string offset = "rcx";

    if (auto imIntIndex = dynamic_cast<ImIntValP>(subVar->index)) {
        offset = imIntIndex->value;
    } else {
        convertUniExprToRegister(subVar->index, "rcx");
    }

    return "[" + varBaseAddr + " + " + std::to_string(typeSize) + " * " + offset + "]";
}

void Generator::convertUniExprToRegister(UniExprP expr, const std::string &reg) {
    if (auto imInt = dynamic_cast<ImIntValP>(expr)) {
        this->programOut << "mov " << reg << ", " << imInt->value << "\n";
    } else if (auto temp = dynamic_cast<UniTempP>(expr)) {
        this->programOut << "mov " << reg << ", QWORD [rbp - " << (temp->id * TEMP_SIZE) << "]\n";
    } else if (auto subVar = dynamic_cast<SubscriptableVariableValP>(expr)) {
        VariableStackData varData = this->variableStack[subVar->var.name].top();

        int typeSize = varData.varSize;

        if (subVar->var.ptrType) {
            typeSize = typeSizes[subVar->var.type];
        }

        std::string varAddr = sizeIdentifiers[typeSize] + " " +
                              getSubscriptableStackPosition(subVar, reg);

        this->programOut << movTo64BitReg(reg, varAddr, typeSize) << "\n";
    } else if (auto var = dynamic_cast<VariableValP>(expr)) {
        VariableStackData varData = this->variableStack[var->var.name].top();

        std::string varAddr = sizeIdentifiers[varData.varSize] +
                              " [" + getStackAddr(varData) + "]";

        this->programOut << movTo64BitReg(reg, varAddr, varData.varSize) << "\n";
    } else if (auto logicalNot = dynamic_cast<LogicalNotExprP>(expr)) {
        convertUniExprToRegister(logicalNot->expr, reg);
        this->programOut << "test " << reg << ", " << reg << "\n";
        this->programOut << "setz dl\n";
        this->programOut << "movzx " << reg << " dl\n";
    } else if (auto numericNeg = dynamic_cast<NumericNegExprP>(expr)) {
        convertUniExprToRegister(numericNeg->expr, reg);
        this->programOut << "neg " << reg << "\n";
    }
}

void Generator::convertAddrExprToRegister(AddrExprP expr, const std::string &reg) {
    if (auto subVar = dynamic_cast<SubscriptableVariableValP>(expr->addressable)) {
        std::string stackPos = getSubscriptableStackPosition(subVar, reg);
        this->programOut << "lea " << reg << ", " << stackPos << "\n";
    } else {
        Variable var = expr->addressable->var;
        VariableStackData varData = this->variableStack[var.name].top();

        std::string varBaseAddr = getStackAddr(varData);

        if (var.ptrType) {
            this->programOut << "mov " << reg << ", QWORD [" << varBaseAddr << "]\n";
        } else {
            this->programOut << "lea " << reg << ", [" << varBaseAddr << "]\n";
        }
    }
}

std::unordered_map<ExprOperator, std::string> Generator::BinaryExprToAsmStrSteps = {
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

void Generator::convertBinaryExprToRegister(BinaryExprP expr) {
    convertUniExprToRegister(expr->left, "rax");
    convertUniExprToRegister(expr->right, "rbx");

    if (BinaryExprToAsmStrSteps.contains(expr->op)) {
        this->programOut << BinaryExprToAsmStrSteps[expr->op];
    } else if (expr->op == ExprOperator::logicalOr) {
        std::string orTrue = "orTrue" + std::to_string(++this->labelCount);
        std::string orFalse = "orFalse" + std::to_string(this->labelCount);
        std::string orEnd = "orEnd" + std::to_string(this->labelCount);

        this->programOut << "test rax, rax\n" <<
                         "jnz " << orTrue << "\n" <<
                         "test rbx, rbx\n" <<
                         "jz " << orFalse << "\n" <<
                         orTrue << ":\n" <<
                         "mov rax, 1\n"
                         "jmp " << orEnd << "\n" <<
                         orFalse << ":\n" <<
                         "mov rax, 0\n" <<
                         orEnd << ":\n";
    } else if (expr->op == ExprOperator::logicalAnd) {
        std::string andFalse = "andFalse" + std::to_string(++this->labelCount);
        std::string andEnd = "andEnd" + std::to_string(this->labelCount);

        this->programOut << "test rax, rax\n" <<
                         "jz " << andFalse << "\n" <<
                         "test rbx, rbx\n" <<
                         "jz " << andFalse << "\n" <<
                         "mov rax, 1\n"
                         "jmp " << andEnd << "\n" <<
                         andFalse << ":\n" <<
                         "mov rax, 0\n" <<
                         andEnd << ":\n";
    }
}

void Generator::convertTAExprToRaxRegister(ThreeAddressExprP expr) {
    if (auto uni = dynamic_cast<UniExprP>(expr)) {
        convertUniExprToRegister(uni, "rax");
    } else if (auto addr = dynamic_cast<AddrExprP>(expr)) {
        convertAddrExprToRegister(addr, "rax");
    } else if (auto binary = dynamic_cast<BinaryExprP>(expr)) {
        convertBinaryExprToRegister(binary);
    }

    //return "";
}

void Generator::convertTAStmtToAsm(ThreeAddressStmtP taStmt) {

    if (auto tempAssignment = dynamic_cast<TempAssignmentTAStmtP>(taStmt)) {
        convertTempAssignmentToAsm(tempAssignment);
    } else if (auto varAssignment = dynamic_cast<VarAssignmentTAStmtP>(taStmt)) {
        convertVarAssignmentToAsm(varAssignment);
    } else if (auto functionParamPush = dynamic_cast<FunctionParamPushStmtP>(taStmt)) {
        convertFunctionParamPushToAsm(functionParamPush);
    } else if (auto functionCall = dynamic_cast<FunctionCallExprP>(taStmt)) {
        convertFunctionCallToAsm(functionCall);
    } else if (auto labelStmt = dynamic_cast<LabelStmtP>(taStmt)) {
        convertLabelToAsm(labelStmt);
    } else if (auto gotoStmt = dynamic_cast<GotoStmtP>(taStmt)) {
        convertGotoToAsm(gotoStmt);
    } else if (auto gotoIfZeroStmt = dynamic_cast<GotoIfZeroStmtP>(taStmt)) {
        convertGotoIfZeroToAsm(gotoIfZeroStmt);
    } else if (auto gotoIfNotZeroStmt = dynamic_cast<GotoIfNotZeroStmtP>(taStmt)) {
        convertGotoIfNotZeroToAsm(gotoIfNotZeroStmt);
    } else if (auto setReturnValue = dynamic_cast<SetReturnValueStmtP>(taStmt)) {
        convertSetReturnValueToAsm(setReturnValue);
    } else if (auto scopeEnter = dynamic_cast<ScopeEnterStmtP>(taStmt)) {
        convertScopeEnterToAsm(scopeEnter);
    } else if (auto scopeExit = dynamic_cast<ScopeExitStmtP>(taStmt)) {
        convertScopeExitToAsm();
    } else if (auto functionDeclaration = dynamic_cast<FunctionDeclarationStmtP>(taStmt)) {
        convertFunctionDeclarationToAsm(functionDeclaration);
    } else if (auto functionExit = dynamic_cast<FunctionExitStmtP>(taStmt)) {
        convertFunctionExitToAsm();
    }
}

void Generator::convertTempAssignmentToAsm(TempAssignmentTAStmtP tempAssignment) {
    if (auto funcCall = dynamic_cast<FunctionCallExprP>(tempAssignment->expr)) {
        generateAsmFunctionCall(funcCall->functionName);

        int retSize = sizeByTypeAndPtr(funcCall->retType, funcCall->retPtr, 0);

        if (retSize < BIT_64_REG_SIZE) {
            this->programOut << "movsx rax, " << getAxRegisterBySize(retSize) << "\n";
        }

        currentRelativeSP -= FuncParamsOffsetSP;
        FuncParamsOffsetSP = 0;
    } else {
        convertTAExprToRaxRegister(tempAssignment->expr);
    }

    this->programOut << "mov QWORD [rbp - " << (tempAssignment->id * TEMP_SIZE) << "], rax\n";
}

void Generator::convertVarAssignmentToAsm(VarAssignmentTAStmtP varAssignmentStmt) {
    convertTAExprToRaxRegister(varAssignmentStmt->expr);

    VariableStackData varData = this->variableStack[varAssignmentStmt->var->var.name].top();

    std::string varStackAddr = "[" + getStackAddr(varData) + "]";

    int typeSize = varData.varSize;

    if (auto subVar = dynamic_cast<SubscriptableVariableValP>(varAssignmentStmt->var)) {
        varStackAddr = getSubscriptableStackPosition(subVar, "rbx");

        if (subVar->var.ptrType) {
            typeSize = typeSizes[subVar->var.type];
        }
    }

    this->programOut << "mov " << sizeIdentifiers[typeSize] <<
                     " " << varStackAddr << ", " << getAxRegisterBySize(typeSize) << "\n";
}

void Generator::convertFunctionParamPushToAsm(FunctionParamPushStmtP funcParamPushStmt) {
    convertTAExprToRaxRegister(funcParamPushStmt->expr);

    int sizeToPush = sizeByTypeAndPtr(funcParamPushStmt->varType, funcParamPushStmt->isPtr, 0);

    currentRelativeSP += sizeToPush;
    FuncParamsOffsetSP += sizeToPush;

    this->programOut << "sub rsp, " << sizeToPush << "\n";

    this->programOut << "mov " << sizeIdentifiers[sizeToPush]
                     << " [rbp - " << currentRelativeSP << "], "
                     << getAxRegisterBySize(sizeToPush) << "\n";
}

void Generator::convertFunctionCallToAsm(FunctionCallExprP functionCallStmt) {
    generateAsmFunctionCall(functionCallStmt->functionName);
    currentRelativeSP -= FuncParamsOffsetSP;
    FuncParamsOffsetSP = 0;
}

void Generator::convertLabelToAsm(LabelStmtP labelStmt) {
    this->programOut << labelStmt->labelName << ":\n";
}

void Generator::convertGotoToAsm(GotoStmtP gotoStmt) {
    this->programOut << "jmp " << gotoStmt->labelName << "\n";
}

void Generator::convertGotoIfZeroToAsm(GotoIfZeroStmtP gotoIfZeroStmt) {
    convertTAExprToRaxRegister(gotoIfZeroStmt->expr);

    this->programOut << "test rax, rax\n"
                        "jz " << gotoIfZeroStmt->labelName << "\n";
}

void Generator::convertGotoIfNotZeroToAsm(GotoIfNotZeroStmtP gotoIfNotZeroStmt) {
    convertTAExprToRaxRegister(gotoIfNotZeroStmt->expr);

    this->programOut << "test rax, rax\n"
                        "jnz " << gotoIfNotZeroStmt->labelName << "\n";
}

void Generator::convertSetReturnValueToAsm(SetReturnValueStmtP setReturnValueStmt) {
    convertTAExprToRaxRegister(setReturnValueStmt->expr);
}

void Generator::convertScopeEnterToAsm(ScopeEnterStmtP scopeEnterStmt) {
    std::unordered_set<std::string> frameVars;
    int frameSize = 0;

    for (auto &var: scopeEnterStmt->vars) {
        int varSize = sizeByTypeAndPtr(var.type, var.ptrType, var.arrSize);

        frameVars.insert(var.name);
        frameSize += varSize;

        currentRelativeSP += varSize;
        variableStack[var.name].push(VariableStackData(-currentRelativeSP,
                                                       sizeByTypeAndPtr(var.type,
                                                                        var.ptrType, 0)));
    }

    scopeFrameStack.push(ScopeFrame(frameSize, frameVars));

    if (frameSize > 0) {
        this->programOut << "sub rsp, " << frameSize << "\n";
    }
}

void Generator::convertScopeExitToAsm() {
    ScopeFrame scopeFrame = scopeFrameStack.top();

    for (auto &frameVar: scopeFrame.frameVars) {
        variableStack[frameVar].pop();
    }

    if (scopeFrame.frameSize > 0) {
        currentRelativeSP -= scopeFrame.frameSize;
        this->programOut << "add rsp, " << scopeFrame.frameSize << "\n";
    }

    scopeFrameStack.pop();
}

void Generator::convertFunctionDeclarationToAsm(FunctionDeclarationStmtP functionDeclarationStmt) {
    variableStack.clear(); // from last function

    this->programOut << functionDeclarationStmt->name << ":     ; FUNCTION\n"
                     << "cmp r8, " << STACK_OVERFLOW_LIMIT << "\n"
                                                              "jae _overflow\n"
                                                              "push rbp\n"
                                                              "mov rbp, rsp\n";

    currentRelativeSP = functionDeclarationStmt->maxTemp * TEMP_SIZE;

    if (currentRelativeSP > 0) {
        this->programOut << "sub rsp, " << currentRelativeSP << "\n";
    }

    int paramOffset = BIT_64_REG_SIZE * 2;

    for (const auto &param: functionDeclarationStmt->params) {
        int paramSize = sizeByTypeAndPtr(param.type, param.ptrType, 0);

        variableStack[param.name].push(VariableStackData(paramOffset, paramSize));
        paramOffset += paramSize;
    }

    this->paramsSize = paramOffset - (BIT_64_REG_SIZE * 2);
}

void Generator::convertFunctionExitToAsm() {
    this->programOut << "leave\n"
                        "ret " << this->paramsSize << "\n\n";
}

void Generator::generateAsmFunctionCall(const std::string &funcName) {
    this->programOut << "inc r8\n"
                        "call " << funcName << "\n"
                     << "dec r8\n";
}

std::string Generator::getAxRegisterBySize(int size) {
    std::stringstream stream;

    if (size == 1) {
        return "al";
    } else if (size == 2) {
        return "ax";
    } else if (size == 4) {
        return "eax";
    } else if (size == 8) {
        return "rax";
    }

    return "";
}

int Generator::sizeByTypeAndPtr(VariableType type, bool isPtr, int arrSize) {
    if (isPtr) {
        return PTR_SIZE;
    }

    if (arrSize > 0) {
        return typeSizes[type] * arrSize;
    }

    return typeSizes[type];
}
