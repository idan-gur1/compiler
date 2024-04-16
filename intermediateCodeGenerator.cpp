//
// Created by idang on 24/12/2023.
//

#include <fstream>
#include "intermediateCodeGenerator.h"
#include "lexer.h"

std::string uniExprToStr(UniExpr *uniExpr) {
//    if (auto val = dynamic_cast<UniValP>(uniExpr)) {
//        return val->val.val;
//    } else if (auto temp = dynamic_cast<UniTempP>(uniExpr)) {
//        return "temp" + std::to_string(temp->id);
//    }
    return "";
}

std::string ilStmtToStr(ThreeAddressStmt *taStmt) {
    std::stringstream strStream;


    return strStream.str();
}

UniExpr *ILGenerator::convertTerminalToUniExpr(TerminalNodeExprP terminalExpr) {
    if (auto imInt = dynamic_cast<NodeImIntTerminalP>(terminalExpr)) {
        return new ImIntVal(imInt->value);
    } else if (auto subVar = dynamic_cast<NodeSubscriptableVariableTerminalP>(terminalExpr)) {
        UniExpr *index;

        if (auto innerNum = dynamic_cast<NodeImIntTerminalP>(subVar->index)) {
            return new SubscriptableVariableVal(subVar->variable, new ImIntVal(innerNum->value));
        }

        generateExprIL(subVar);

        return new UniTemp(this->currentTemp);
    } else if (auto var = dynamic_cast<NodeVariableTerminalP>(terminalExpr)) {
        return new VariableVal(var->variable);
    }

    return nullptr;
}

TempAssignmentTAStmt *ILGenerator::generateTempAssignmentIL(UniExprP uniLhs, UniExprP uniRhs, ExprOperator op) {
    auto uniTempLhs = dynamic_cast<UniTempP>(uniLhs);
    auto uniTempRhs = dynamic_cast<UniTempP>(uniRhs);

    if (uniTempLhs && uniTempRhs) {
        currentTemp--;
        return new TempAssignmentTAStmt(uniTempLhs->id, new BinaryExpr(uniTempLhs, uniTempRhs, op));
    } else if (uniTempLhs) {
        return new TempAssignmentTAStmt(uniTempLhs->id, new BinaryExpr(uniTempLhs, uniRhs, op));
    } else if (uniTempRhs) {
        return new TempAssignmentTAStmt(uniTempRhs->id, new BinaryExpr(uniLhs, uniTempRhs, op));
    }

    if (++currentTemp > this->maxTemp) this->maxTemp = currentTemp;

    return new TempAssignmentTAStmt(currentTemp, new BinaryExpr(uniLhs, uniRhs, op));
}

void ILGenerator::generateBinaryExprIL(BinaryNodeExprP binExpr) {
    ExprOperator op = this->exprOperatorMap[typeid(*binExpr)];

    auto lhs = dynamic_cast<TerminalNodeExprP>(binExpr->left);
    auto rhs = dynamic_cast<TerminalNodeExprP>(binExpr->right);

    if (lhs && rhs) {
        UniExprP uniLhs = this->convertTerminalToUniExpr(lhs);
        UniExprP uniRhs = this->convertTerminalToUniExpr(rhs);

        this->ilStmts.push_back(generateTempAssignmentIL(uniLhs, uniRhs, op));
    } else if (lhs) {
        UniExprP uniLhs = this->convertTerminalToUniExpr(lhs);

        generateExprIL(binExpr->right);

        this->ilStmts.push_back(generateTempAssignmentIL(uniLhs, new UniTemp(currentTemp), op));
    } else if (rhs) {
        generateExprIL(binExpr->left);
        auto uniTempLhs = new UniTemp(currentTemp);

        UniExprP uniRhs = this->convertTerminalToUniExpr(rhs);

        this->ilStmts.push_back(generateTempAssignmentIL(uniTempLhs, uniRhs, op));
    } else {
        generateExprIL(binExpr->left);
        auto uniTempLhs = new UniTemp(currentTemp);

        generateExprIL(binExpr->right);

        this->ilStmts.push_back(generateTempAssignmentIL(uniTempLhs, new UniTemp(currentTemp), op));
    }
}

void ILGenerator::generateExprIL(NodeExprP expr) {
    if (auto binExpr = dynamic_cast<BinaryNodeExprP>(expr)) {
        generateBinaryExprIL(binExpr);
    } else if (auto unaryExpr = dynamic_cast<ParenthesisNodeExprP>(expr)) {

    } else if (auto subVar = dynamic_cast<NodeSubscriptableVariableTerminalP>(expr)) {
        auto innerVar = dynamic_cast<NodeVariableTerminalP>(subVar->index);

        if (auto innerNum = dynamic_cast<NodeImIntTerminalP>(subVar->index)) {
            this->ilStmts.push_back(new TempAssignmentTAStmt(++currentTemp,
                                                             new SubscriptableVariableVal(subVar->variable,
                                                                                          new ImIntVal(
                                                                                                  innerNum->value))));

        } else if (innerVar && !dynamic_cast<NodeSubscriptableVariableTerminalP>(subVar->index)) {
            this->ilStmts.push_back(new TempAssignmentTAStmt(++currentTemp,
                                                             new SubscriptableVariableVal(subVar->variable,
                                                                                          new VariableVal(
                                                                                                 innerVar->variable))));
        } else {
            generateExprIL(subVar->index);

            this->ilStmts.push_back(new TempAssignmentTAStmt(currentTemp,
                                                             new SubscriptableVariableVal(subVar->variable,
                                                                                          new UniTemp(
                                                                                                  this->currentTemp))));
        }

    } else if (auto var = dynamic_cast<NodeVariableTerminalP>(expr)) {

    } else if (auto imInt = dynamic_cast<NodeImIntTerminalP>(expr)) {

    }
}

void ILGenerator::generateStmtIL(NodeStmtP stmt) {
//    if (auto assignmentStmt = dynamic_cast<NodeAssignmentStmtP>(stmt)) {
//        this->currentTemp = 0;
//        this->generateExprIL(assignmentStmt->expr);
//
//        VarAssignmentTAStmt *tac = nullptr;
//        this->declarations++;
//
//        if (auto terminalExpr = dynamic_cast<TerminalNodeExprP>(assignmentStmt->expr)) {
//            tac = new VarAssignmentTAStmt(assignmentStmt->ident.val, new UniVal(terminalExpr->val));
//        } else {
//            tac = new VarAssignmentTAStmt(assignmentStmt->ident.val, new UniTemp(currentTemp));
//        }
//
//        this->ilStmts.push_back(tac);
//    }

}

void ILGenerator::generateScopeIL(NodeScopeP scope) {
//    int scopeId = this->currentScopeId++;
//
//    this->ilStmts.push_back(new ScopeDeclarationStmt(scopeId, scope->vars));
//
//    for (NodeStmtP stmt : scope->stmts) {
//        this->generateStmtIL(stmt);
//    }
//
//    this->ilStmts.push_back(new ScopeExitStmt(scopeId));
}

void ILGenerator::generateFunctionIL(NodeFunctionP function) {
//    auto funcDecStmt = new FunctionDeclarationStmt(function->name, function->params);
//    this->maxTemp = 0;
//    this->currentFunctionName = function->name;
//
//    this->ilStmts.push_back(funcDecStmt);
//    this->generateScopeIL(function->scope);
//    this->ilStmts.push_back(new FunctionExitStmt(function->name));
//
//    funcDecStmt->maxTemp = this->maxTemp;

}

void ILGenerator::generateProgramIL() {
    for (auto funcPtr: this->program->functions) {
        this->generateFunctionIL(funcPtr);
    }

    std::ofstream outFile(this->outfileName);

    if (outFile.fail()) {
        std::cout << "File Error: Error while opening the file " << this->outfileName << std::endl;
        outFile.close();
        exit(1);
    }

    for (ThreeAddressStmt *tasP: this->ilStmts) {
        outFile << ilStmtToStr(tasP);
    }

    outFile.close();

}
