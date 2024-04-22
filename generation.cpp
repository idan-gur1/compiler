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

std::string Generator::convertUniExprToRegister(UniExprP expr, std::string reg) {
    if (auto imInt = dynamic_cast<ImIntValP>(expr)) {
        this->programOut << "mov " << Generator::getRegisterBySize(8, reg) << ", " << imInt->value;
    } else if (auto temp = dynamic_cast<UniTempP>(expr)) {
        this->programOut << "mov " << Generator::getRegisterBySize(this->tempSizes[temp->id], reg) << ", "
                         << this->sizeIdentifiers[this->tempSizes[temp->id]] << "[bp - " << temp->id << "]";
    } else if (auto subVar = dynamic_cast<SubscriptableVariableValP>(expr)) {

    } else if (auto var = dynamic_cast<VariableValP>(expr)) {

    } else if (auto logicalNot = dynamic_cast<LogicalNotExprP>(expr)) {

    } else if (auto numericNeg = dynamic_cast<NumericNegExprP>(expr)) {

    } else if (auto functionCall = dynamic_cast<FunctionCallExprP>(expr)) {

    }
}

std::string Generator::convertAddrExprToRegister(AddrExprP expr, std::string reg) {
    return std::string();
}

std::string Generator::convertBinaryExprToRegister(BinaryExprP expr, std::string reg) {
    return std::string();
}

std::string Generator::convertTAExprToRegister(ThreeAddressExprP expr, const std::string &reg) {
    if (auto uni = dynamic_cast<UniExprP>(expr)) {
        return convertUniExprToRegister(uni, reg);
    } else if (auto addr = dynamic_cast<AddrExprP>(expr)) {
        return convertAddrExprToRegister(addr, reg);
    } else if (auto binary = dynamic_cast<BinaryExprP>(expr)) {
        return convertBinaryExprToRegister(binary, reg);
    }

    return "";
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
