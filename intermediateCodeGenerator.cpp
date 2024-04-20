//
// Created by idang on 24/12/2023.
//

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

FunctionCallExpr *ILGenerator::generateFunctionCallExprIL(NodeFunctionCallP funcCall) {
    int tempCurrentTemp = this->currentTemp;

    for (int i = funcCall->function->params.size() - 1; i >= 0; --i) {
        this->ilStmts.push_back(new FunctionParamPushStmt(funcCall->function->params[i].type,
                                                          funcCall->function->params[i].ptrType,
                                                          this->generateExprIL(funcCall->params[i])));

        this->currentTemp = tempCurrentTemp;
    }

    return new FunctionCallExpr(funcCall->function->name);
}

UniExpr *ILGenerator::convertTerminalToUniExpr(TerminalNodeExprP terminalExpr) {
    if (auto imInt = dynamic_cast<NodeImIntTerminalP>(terminalExpr)) {
        return new ImIntVal(imInt->value);
    } else if (auto subVar = dynamic_cast<NodeSubscriptableVariableTerminalP>(terminalExpr)) {
        if (auto innerNum = dynamic_cast<NodeImIntTerminalP>(subVar->index)) {
            return new SubscriptableVariableVal(subVar->variable, new ImIntVal(innerNum->value));
        }

        UniExprP innerUni = generateNumericExprIL(subVar->index);

        if (!dynamic_cast<UniTempP>(innerUni)) incCurrentTemp();

        this->ilStmts.push_back(new TempAssignmentTAStmt(currentTemp,
                                                         new SubscriptableVariableVal(subVar->variable, innerUni)));

        return new UniTemp(this->currentTemp);
    } else if (auto var = dynamic_cast<NodeVariableTerminalP>(terminalExpr)) {
        return new VariableVal(var->variable);
    } else if (auto funcCall = dynamic_cast<NodeFunctionCallP>(terminalExpr)) {
        auto funcCallIL = generateFunctionCallExprIL(funcCall);

        this->ilStmts.push_back(new TempAssignmentTAStmt(incCurrentTemp(), funcCallIL));

        return new UniTemp(this->currentTemp);
    }

    return nullptr;
}

TempAssignmentTAStmt *ILGenerator::generateBinaryTempAssignmentIL(UniExprP uniLhs, UniExprP uniRhs, ExprOperator op) {
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

    return new TempAssignmentTAStmt(incCurrentTemp(), new BinaryExpr(uniLhs, uniRhs, op));
}

void ILGenerator::generateBinaryExprIL(BinaryNodeExprP binExpr) {
    ExprOperator op = this->exprOperatorMap[typeid(*binExpr)];

    auto lhs = dynamic_cast<TerminalNodeExprP>(binExpr->left);
    auto rhs = dynamic_cast<TerminalNodeExprP>(binExpr->right);

    UniExprP uniLhs, uniRhs;

    if (lhs && rhs) {
        uniLhs = this->convertTerminalToUniExpr(lhs);
        uniRhs = this->convertTerminalToUniExpr(rhs);
    } else if (lhs) {
        uniLhs = this->convertTerminalToUniExpr(lhs);
        uniRhs = generateNumericExprIL(binExpr->right);
    } else if (rhs) {
        uniLhs = generateNumericExprIL(binExpr->left);
        uniRhs = this->convertTerminalToUniExpr(rhs);
    } else {
        uniLhs = generateNumericExprIL(binExpr->left);
        uniRhs = generateNumericExprIL(binExpr->right);
    }

    this->ilStmts.push_back(generateBinaryTempAssignmentIL(uniLhs, uniRhs, op));
}

void ILGenerator::generateUnaryExprIL(UnaryNodeExprP unaryExpr) {
    UniExpr *innerUni = generateNumericExprIL(unaryExpr->expr);

    if (!dynamic_cast<UniTempP>(innerUni)) incCurrentTemp();

    if (auto logicalNot = dynamic_cast<NodeLogicalNotExprP>(unaryExpr)) {
        this->ilStmts.push_back(new TempAssignmentTAStmt(currentTemp, new LogicalNotExpr(innerUni)));
    } else if (auto numericNeg = dynamic_cast<NodeNumericNegExprP>(unaryExpr)) {
        this->ilStmts.push_back(new TempAssignmentTAStmt(currentTemp, new NumericNegExpr(innerUni)));
    }
}

UniExpr *ILGenerator::generateNumericExprIL(NodeExprP expr) {
    if (auto binExpr = dynamic_cast<BinaryNodeExprP>(expr)) {
        this->generateBinaryExprIL(binExpr);
        return new UniTemp(this->currentTemp);
    } else if (auto paren = dynamic_cast<NodeParenthesisExprP>(expr)) {
        return generateNumericExprIL(paren->expr);
    } else if (auto unary = dynamic_cast<UnaryNodeExprP>(expr)) {
        this->generateUnaryExprIL(unary);
        return new UniTemp(this->currentTemp);
    } else if (auto terminal = dynamic_cast<TerminalNodeExprP>(expr)) {
        return this->convertTerminalToUniExpr(terminal);
    }

    return nullptr;
}

ThreeAddressExpr *ILGenerator::generateExprIL(NodeExprP expr) {
    if (auto addr = dynamic_cast<AddrNodeExprP>(expr)) {
        VariableVal *ret;

        if (auto sub = dynamic_cast<NodeSubscriptableVariableTerminalP>(addr->target)) {
            ret = new SubscriptableVariableVal(sub->variable, generateNumericExprIL(sub->index));
        } else {
            ret = new VariableVal(addr->target->variable);
        }

        return new AddrExpr(ret);
    }

    return generateNumericExprIL(expr);
}

void ILGenerator::generateStmtIL(NodeStmtP stmt) {
    if (auto primitiveAssignmentStmt = dynamic_cast<NodePrimitiveAssignmentStmtP>(stmt)) {
        this->ilStmts.push_back(new VarAssignmentTAStmt(new VariableVal(primitiveAssignmentStmt->variable),
                                                        generateExprIL(primitiveAssignmentStmt->expr)));
    } else if (auto arrayAssignmentStmt = dynamic_cast<NodeArrayAssignmentStmtP>(stmt)) {
        this->ilStmts.push_back(new VarAssignmentTAStmt(new SubscriptableVariableVal(arrayAssignmentStmt->array,
                                                                                     generateNumericExprIL(
                                                                                             arrayAssignmentStmt->index)),
                                                        generateExprIL(arrayAssignmentStmt->expr)));
    } else if (auto ifStmt = dynamic_cast<NodeIfP>(stmt)) {
        std::string ifName = currentFunctionName + "If" + std::to_string(++currentIfId);
        std::string endLabel = ifName + "End";
        std::string elseLabel = ifStmt->elseBlock ? ifName + "Else" : endLabel;

        this->ilStmts.push_back(new LabelStmt(ifName));
        this->ilStmts.push_back(new GotoIfZeroStmt(elseLabel, generateNumericExprIL(ifStmt->expr)));
        generateScopeIL(ifStmt->ifBlock);

        if (ifStmt->elseBlock) {
            this->ilStmts.push_back(new GotoStmt(endLabel));
            this->ilStmts.push_back(new LabelStmt(elseLabel));
            generateScopeIL(ifStmt->elseBlock);
        }

        this->ilStmts.push_back(new LabelStmt(endLabel));
    } else if (auto whileStmt = dynamic_cast<NodeWhileP>(stmt)) {
        std::string whileName = currentFunctionName + "While" + std::to_string(++currentWhileId);
        std::string conditionLabel = whileName + "Condition";
        std::string bodyLabel = whileName + "Body";

        this->ilStmts.push_back(new LabelStmt(whileName));
        if (!whileStmt->isDoWhile) this->ilStmts.push_back(new GotoStmt(conditionLabel));
        this->ilStmts.push_back(new LabelStmt(bodyLabel));
        generateScopeIL(whileStmt->codeBlock);
        this->ilStmts.push_back(new LabelStmt(conditionLabel));
        this->ilStmts.push_back(new GotoIfNotZeroStmt(bodyLabel, generateNumericExprIL(whileStmt->expr)));
    } else if (auto returnStmt = dynamic_cast<NodeReturnStmtP>(stmt)) {
        if (returnStmt->expr) {
            this->ilStmts.push_back(new SetReturnValueStmt(generateExprIL(returnStmt->expr)));
        }

        this->ilStmts.push_back(new GotoStmt(currentFunctionName + "End"));
    } else if (auto scope = dynamic_cast<NodeScopeP>(stmt)) {
        generateScopeIL(scope);
    }

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

int ILGenerator::incCurrentTemp() {
    if (++currentTemp > this->maxTemp) this->maxTemp = currentTemp;
    return currentTemp;
}
