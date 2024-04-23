//
// Created by idang on 04/02/2024.
//

#include "generation.h"

void Generator::generateProgram() {
    this->programOut << "section .text\n"
                        "global _start\n"
                        "_start:\n"
                        "call main\n"
                        "movsx rdi, eax  ; exit code param\n"
                        "mov rax, 0x3c   ; exit code syscall code\n"
                        "mov rdi, 0\n"
                        "syscall\n";

    for (auto ilStmt: this->ilGenerator->ilStmts) {
        convertTAStmtToAsm(ilStmt);
    }
}

std::string Generator::movTo64BitReg(const std::string &reg, const std::string &val, int size) {
    std::string movType = "mov ";

    if (size < bit64RegSize) {
        movType = "movsx ";
    }

    return movType + reg + ", " + val;
}

std::string Generator::getStackAddr(const VariableStackData &var) {
    if (var.stackPos > 0) {
        return "bp + " + std::to_string(var.stackPos);
    }

    return "bp - " + std::to_string(std::abs(var.stackPos));
}

std::string Generator::getSubscriptableStackPosition(SubscriptableVariableValP subVar, const std::string &freeReg) {
    VariableStackData varData = this->variableStack[subVar->var.name].top();

    std::string varBaseAddr = getStackAddr(varData);

    if (subVar->var.ptrType) {
        this->programOut << "mov " << freeReg << ", QWORD [" << varBaseAddr << "]\n";
        varBaseAddr = freeReg;
    }

    std::string offset = "rcx";

    if (auto imIntIndex = dynamic_cast<ImIntValP>(subVar->index)) {
        offset = imIntIndex->value;
    } else {
        convertUniExprToRegister(subVar->index, "rcx");
    }

    return "[" + varBaseAddr + " + " + std::to_string(varData.varSize) + " * " + offset + "]";
}

void Generator::convertUniExprToRegister(UniExprP expr, const std::string &reg) {
    if (auto imInt = dynamic_cast<ImIntValP>(expr)) {
        this->programOut << "mov " << reg << ", " << imInt->value << "\n";
    } else if (auto temp = dynamic_cast<UniTempP>(expr)) {
        this->programOut << "mov " << reg << ", QWORD [bp - " << temp->id << "]\n";
    } else if (auto subVar = dynamic_cast<SubscriptableVariableValP>(expr)) {
        VariableStackData varData = this->variableStack[subVar->var.name].top();

        std::string varAddr = this->sizeIdentifiers[varData.varSize] + " " +
                              getSubscriptableStackPosition(subVar, reg);

        this->programOut << movTo64BitReg(reg, varAddr, varData.varSize) << "\n";
    } else if (auto var = dynamic_cast<VariableValP>(expr)) {
        VariableStackData varData = this->variableStack[var->var.name].top();

        std::string varAddr = this->sizeIdentifiers[varData.varSize] +
                              " [" + getStackAddr(varData) + "]";

        this->programOut << movTo64BitReg(reg, varAddr, varData.varSize) << "\n";
    } else if (auto logicalNot = dynamic_cast<LogicalNotExprP>(expr)) {
        convertUniExprToRegister(logicalNot->expr, reg);
        this->programOut << "test " << reg << ", " << reg << "\n";
        this->programOut << "setz dl\n";
        this->programOut << "movzx " << reg << " dl\n";
    } else if (auto numericNeg = dynamic_cast<NumericNegExprP>(expr)) {
        convertUniExprToRegister(logicalNot->expr, reg);
        this->programOut << "neg " << reg << "\n";
    } else if (auto functionCall = dynamic_cast<FunctionCallExprP>(expr)) {
        // WON'T REACH HERE
    }
}

void Generator::convertAddrExprToRegister(AddrExprP expr, const std::string &reg) {
    if (auto subVar = dynamic_cast<SubscriptableVariableValP>(expr->addressable)) {
        this->programOut << "lea " << reg << ", " << getSubscriptableStackPosition(subVar, reg) << "\n";
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

void Generator::convertBinaryExprToRegister(BinaryExprP expr) {
    convertUniExprToRegister(expr->left, "rax");
    convertUniExprToRegister(expr->left, "rbx");

    if (this->BinaryExprToAsmStrSteps.contains(expr->op)) {
        this->programOut << this->BinaryExprToAsmStrSteps[expr->op];
    } else if (expr->op == ExprOperator::logicalOr) {
        std::string orTrue = "orTrue" + std::to_string(++this->labelCount);
        std::string orFalse = "orFalse" + std::to_string(++this->labelCount);
        std::string orEnd = "orEnd" + std::to_string(++this->labelCount);

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
        std::string andEnd = "andEnd" + std::to_string(++this->labelCount);

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

    // REPLACED BY UNORDERED MAP
    /*if (expr->op == ExprOperator::add) {
        this->programOut << "add rax, rbx\n";
    } else if (expr->op == ExprOperator::sub) {
        this->programOut << "sub rax, rbx\n";
    } else if (expr->op == ExprOperator::mult) {
        this->programOut << "imul rbx\n";
    } else if (expr->op == ExprOperator::div) {
        this->programOut << "cqo\n"
                            "idiv rbx\n";
    } else if (expr->op == ExprOperator::mod) {
        this->programOut << "cqo\n"
                            "idiv rbx\n"
                            "mov rax, rdx\n";
    } else if (expr->op == ExprOperator::equals) {
        this->programOut << "xor rax, rbx\n"
                            "setz al\n"
                            "movzx rax, al\n";
    } else if (expr->op == ExprOperator::notEquals) {
        this->programOut << "xor rax, rbx\n"
                            "setnz al\n"
                            "movzx rax, al\n";
    } else if (expr->op == ExprOperator::biggerThan) {
        this->programOut << "cmp rax, rbx\n"
                            "setg al\n"
                            "movzx rax, al\n";
    } else if (expr->op == ExprOperator::biggerThanEquals) {
        this->programOut << "cmp rax, rbx\n"
                            "setge al\n"
                            "movzx rax, al\n";
    } else if (expr->op == ExprOperator::lessThan) {
        this->programOut << "cmp rax, rbx\n"
                            "setl al\n"
                            "movzx rax, al\n";
    } else if (expr->op == ExprOperator::lessThanEquals) {
        this->programOut << "cmp rax, rbx\n"
                            "setle al\n"
                            "movzx rax, al\n";
    }*/
}

void Generator::convertTAExprToRegister(ThreeAddressExprP expr) {
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
        convertScopeExitToAsm(scopeExit);
    } else if (auto functionDeclaration = dynamic_cast<FunctionDeclarationStmtP>(taStmt)) {
        convertFunctionDeclarationToAsm(functionDeclaration);
    } else if (auto functionExit = dynamic_cast<FunctionExitStmtP>(taStmt)) {
        convertFunctionExitToAsm(functionExit);
    }
}

void Generator::convertTempAssignmentToAsm(TempAssignmentTAStmtP stmt) {
    convertTAExprToRegister(stmt->expr);
}

void Generator::convertVarAssignmentToAsm(VarAssignmentTAStmtP stmt) {

}

void Generator::convertFunctionParamPushToAsm(FunctionParamPushStmtP stmt) {

}

void Generator::convertFunctionCallToAsm(FunctionCallExprP stmt) {

}

void Generator::convertLabelToAsm(LabelStmtP stmt) {

}

void Generator::convertGotoToAsm(GotoStmtP stmt) {

}

void Generator::convertGotoIfZeroToAsm(GotoIfZeroStmtP stmt) {

}

void Generator::convertGotoIfNotZeroToAsm(GotoIfNotZeroStmtP stmt) {

}

void Generator::convertSetReturnValueToAsm(SetReturnValueStmtP stmt) {

}

void Generator::convertScopeEnterToAsm(ScopeEnterStmtP stmt) {

}

void Generator::convertScopeExitToAsm(ScopeExitStmtP stmt) {

}

void Generator::convertFunctionDeclarationToAsm(FunctionDeclarationStmtP stmt) {

}

void Generator::convertFunctionExitToAsm(FunctionExitStmtP stmt) {

}

std::string Generator::getRegisterBySize(int size, const std::string &reg) {
    std::stringstream stream;

    if (size == 1) {
        stream << reg << "l";
    } else if (size == 2) {
        stream << reg << "x";
    } else if (size == 4) {
        stream << "e" << reg << "x";
    } else if (size == 8) {
        stream << "r" << reg << "x";
    }

    return stream.str();
}
