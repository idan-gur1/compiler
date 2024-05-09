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

    for (const auto &builtinName: this->ilProgram->builtinFunctionsUsed) {
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

    for (auto ilStmt: this->ilProgram->ilStmts) {
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
    // Determine the appropriate move instruction based on the size of the value
    std::string movType = (size < BIT_64_REG_SIZE) ? "movsx " : "mov ";

    // Construct the assembly instruction string
    return movType + reg + ", " + val;
}

std::string Generator::getStackAddr(const VariableStackData &var) {
    // If positive stack position: "rbp + stackPos"
    if (var.stackPos > 0) {
        return "rbp + " + std::to_string(var.stackPos);
    }

    // Negative stack position: "rbp - |stackPos|"
    return "rbp - " + std::to_string(std::abs(var.stackPos));
}

std::string Generator::getSubscriptableStackPosition(SubscriptableVariableValP subVar,
                                                     const std::string &freeReg) {
    // Retrieve the current VariableStackData for the variable name
    VariableStackData varData = this->variableStack[subVar->var.name].top();
    // Initialize typeSize with the size of the variable's type, if the
    // var is a pointer then it would be changed to the side of the type
    int typeSize = varData.varSize;
    std::string varBaseAddr = getStackAddr(varData);

    // Load the address pointed to by the pointer into the freeReg register if the variable is a pointer
    if (subVar->var.ptrType) {
        this->programOut << "mov " << freeReg << ", QWORD [" << varBaseAddr << "]\n";
        varBaseAddr = freeReg;
        typeSize = typeSizes[subVar->var.type];
    }

    // Determine the offset used for subscripting
    std::string offset = "rcx";

    if (auto imIntIndex = dynamic_cast<ImIntValP>(subVar->index)) {
        offset = imIntIndex->value;
    } else {
        convertUniExprToRegister(subVar->index, "rcx");
    }

    // Return the memory location of the subscripted variable
    return "[" + varBaseAddr + " + " + std::to_string(typeSize) + " * " + offset + "]";
}

void Generator::convertUniExprToRegister(UniExprP expr, const std::string &reg) {
    if (auto imInt = dynamic_cast<ImIntValP>(expr)) {
        // If the expression is an immediate integer value (ImIntVal), move the value to the register.
        this->programOut << "mov " << reg << ", " << imInt->value << "\n";
    } else if (auto temp = dynamic_cast<UniTempP>(expr)) {
        // If the expression is a temporary (UniTemp), load its value from the stack into the register.
        this->programOut << "mov " << reg << ", QWORD [rbp - " << (temp->id * TEMP_SIZE) << "]\n";
    } else if (auto subVar = dynamic_cast<SubscriptableVariableValP>(expr)) {
        // If the expression is a subscriptable variable, calculate its address and move the value to the register
        VariableStackData varData = this->variableStack[subVar->var.name].top();

        int typeSize = varData.varSize;

        if (subVar->var.ptrType) {
            typeSize = typeSizes[subVar->var.type];
        }

        std::string varAddr = sizeIdentifiers[typeSize] + " " +
                              getSubscriptableStackPosition(subVar, reg);

        this->programOut << movTo64BitReg(reg, varAddr, typeSize) << "\n";
    } else if (auto var = dynamic_cast<VariableValP>(expr)) {
        // If the expression is a variable, load its value from the stack into the register
        VariableStackData varData = this->variableStack[var->var.name].top();

        std::string varAddr = sizeIdentifiers[varData.varSize] +
                              " [" + getStackAddr(varData) + "]";

        this->programOut << movTo64BitReg(reg, varAddr, varData.varSize) << "\n";
    } else if (auto logicalNot = dynamic_cast<LogicalNotExprP>(expr)) {
        // If the expression is a logical negation, evaluate the expression and set the register based on the result
        convertUniExprToRegister(logicalNot->expr, reg);
        this->programOut << "test " << reg << ", " << reg << "\n";
        this->programOut << "setz dl\n";
        this->programOut << "movzx " << reg << " dl\n";
    } else if (auto numericNeg = dynamic_cast<NumericNegExprP>(expr)) {
        // If the expression is a numeric negation, negate the value and store it in the register
        convertUniExprToRegister(numericNeg->expr, reg);
        this->programOut << "neg " << reg << "\n";
    }
}

void Generator::convertAddrExprToRegister(AddrExprP expr, const std::string &reg) {
    if (auto subVar = dynamic_cast<SubscriptableVariableValP>(expr->addressable)) {
        // If subscriptable then used the 'getSubscriptableStackPosition' to get the address
        std::string stackPos = getSubscriptableStackPosition(subVar, reg);
        this->programOut << "lea " << reg << ", " << stackPos << "\n";
    } else {
        // Otherwise load as a regular variable
        Variable var = expr->addressable->var;
        VariableStackData varData = this->variableStack[var.name].top();

        std::string varBaseAddr = getStackAddr(varData);

        if (var.ptrType) {
            // If the variable is a pointer then the address is its value
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
    // Convert the left and right operands of the binary expression to registers
    convertUniExprToRegister(expr->left, "rax");
    convertUniExprToRegister(expr->right, "rbx");

    // Determine the assembly code corresponding to the binary operation
    if (BinaryExprToAsmStrSteps.contains(expr->op)) {
        this->programOut << BinaryExprToAsmStrSteps[expr->op];
    } else if (expr->op == ExprOperator::logicalOr) {
        // Generate assembly code for logical OR operation with unique labels
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
        // Generate assembly code for logical AND operation with unique labels
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
        // Convert the function call to assembly
        generateAsmFunctionCall(funcCall->functionName);

        int retSize = sizeByTypeAndPtr(funcCall->retType, funcCall->retPtr, 0);

        if (retSize < BIT_64_REG_SIZE) {
            this->programOut << "movsx rax, " << getAxRegisterBySize(retSize) << "\n";
        }

        // Restore the Stack Pointer counter to the state before the
        // function call, changed by the function push params
        currentRelativeSP -= FuncParamsOffsetSP;
        FuncParamsOffsetSP = 0;
    } else {
        // Convert the expression to the 'rax' register
        convertTAExprToRaxRegister(tempAssignment->expr);
    }

    this->programOut << "mov QWORD [rbp - " << (tempAssignment->id * TEMP_SIZE) << "], rax\n";
}

void Generator::convertVarAssignmentToAsm(VarAssignmentTAStmtP varAssignmentStmt) {
    // Convert the expression to the 'rax' register
    convertTAExprToRaxRegister(varAssignmentStmt->expr);

    // Retrieve the current VariableStackData for the variable name
    VariableStackData varData = this->variableStack[varAssignmentStmt->var->var.name].top();

    // Determine the address and size of the assigned variable on the stack
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
    // Convert the expression to the 'rax' register
    convertTAExprToRaxRegister(funcParamPushStmt->expr);

    // Determine the size to push onto the stack based on the variable type and pointer status
    int sizeToPush = sizeByTypeAndPtr(funcParamPushStmt->varType, funcParamPushStmt->isPtr, 0);

    // Update the current relative stack pointer
    currentRelativeSP += sizeToPush;
    // Update the size of the offset of the params
    FuncParamsOffsetSP += sizeToPush;

    // Generate assembly code to adjust the stack pointer and move the value onto the stack
    this->programOut << "sub rsp, " << sizeToPush << "\n";
    this->programOut << "mov " << sizeIdentifiers[sizeToPush]
                     << " [rbp - " << currentRelativeSP << "], "
                     << getAxRegisterBySize(sizeToPush) << "\n";
}

void Generator::convertFunctionCallToAsm(FunctionCallExprP functionCallStmt) {
    // Convert the function call to assembly
    generateAsmFunctionCall(functionCallStmt->functionName);

    // Restore the Stack Pointer counter to the state before the
    // function call, changed by the function push params
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
    // Convert the expression to the 'rax' register
    convertTAExprToRaxRegister(gotoIfZeroStmt->expr);

    this->programOut << "test rax, rax\n"
                        "jz " << gotoIfZeroStmt->labelName << "\n";
}

void Generator::convertGotoIfNotZeroToAsm(GotoIfNotZeroStmtP gotoIfNotZeroStmt) {
    // Convert the expression to the 'rax' register
    convertTAExprToRaxRegister(gotoIfNotZeroStmt->expr);

    this->programOut << "test rax, rax\n"
                        "jnz " << gotoIfNotZeroStmt->labelName << "\n";
}

void Generator::convertSetReturnValueToAsm(SetReturnValueStmtP setReturnValueStmt) {
    // Convert the expression to the 'rax' register
    convertTAExprToRaxRegister(setReturnValueStmt->expr);
}

void Generator::convertScopeEnterToAsm(ScopeEnterStmtP scopeEnterStmt) {
    // Initialize variables to track frame size and frame variables
    std::vector<std::string> frameVars;
    int frameSize = 0;

    // Process each variable in the scope
    for (auto &var: scopeEnterStmt->vars) {
        int varSize = sizeByTypeAndPtr(var.type, var.ptrType, var.arrSize);

        // Add variable to frame variables and update frame size
        frameVars.push_back(var.name);
        frameSize += varSize;

        // Update the stack pointer to allocate space for the variable
        currentRelativeSP += varSize;
        // Push variable information to the variable stack
        variableStack[var.name].push(VariableStackData(-currentRelativeSP,
                                                       sizeByTypeAndPtr(var.type,
                                                                        var.ptrType, 0)));
    }

    // Create a new scope frame and push it onto the scope frame stack
    scopeFrameStack.push(ScopeFrame(frameSize, frameVars));

    // If frame size is greater than zero, adjust the stack pointer (rsp)
    if (frameSize > 0) {
        this->programOut << "sub rsp, " << frameSize << "\n";
    }
}

void Generator::convertScopeExitToAsm() {
    // Retrieve the top scope frame from the scope frame stack
    ScopeFrame scopeFrame = scopeFrameStack.top();

    // Remove variables of the exiting scope from the variable stack
    for (auto &frameVar: scopeFrame.frameVars) {
        variableStack[frameVar].pop();
    }

    // If the frame size is greater than zero, adjust the stack pointer (rsp) to deallocate stack space
    if (scopeFrame.frameSize > 0) {
        currentRelativeSP -= scopeFrame.frameSize;
        this->programOut << "add rsp, " << scopeFrame.frameSize << "\n";
    }

    scopeFrameStack.pop();
}

void Generator::convertFunctionDeclarationToAsm(FunctionDeclarationStmtP functionDeclarationStmt) {
    // Clear the variable stack to prepare for the new function
    variableStack.clear();

    // Generate assembly code for function prologue
    this->programOut << functionDeclarationStmt->name << ":     ; FUNCTION\n"
                     << "cmp r8, " << STACK_OVERFLOW_LIMIT << "\n"
                                                              "jae _overflow\n"
                                                              "push rbp\n"
                                                              "mov rbp, rsp\n";

    // Calculate the size of local temporaries needed for the function
    currentRelativeSP = functionDeclarationStmt->maxTemp * TEMP_SIZE;

    // Allocate stack space for local temporaries if needed
    if (currentRelativeSP > 0) {
        this->programOut << "sub rsp, " << currentRelativeSP << "\n";
    }

    // Initialize parameters on the variables stack with the correct offset
    int paramOffset = BIT_64_REG_SIZE * 2;

    for (const auto &param: functionDeclarationStmt->params) {
        int paramSize = sizeByTypeAndPtr(param.type, param.ptrType, 0);

        variableStack[param.name].push(VariableStackData(paramOffset, paramSize));
        paramOffset += paramSize;
    }

    // Calculate and store the total size of parameters
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
