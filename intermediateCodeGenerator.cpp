//
// Created by idang on 24/12/2023.
//

#include "intermediateCodeGenerator.h"


FunctionCallExpr *ILGenerator::generateFunctionCallExprIL(NodeFunctionCallP funcCall) {
    int tempCurrentTemp = this->currentTemp;

    if (funcCall->function->scope == nullptr) {
        builtinFunctionsUsed.insert(funcCall->function->name);
    }

    for (int i = funcCall->function->params.size() - 1; i >= 0; --i) {
        this->ilStmts.push_back(new FunctionParamPushStmt(funcCall->function->params[i].type,
                                                          funcCall->function->params[i].ptrType,
                                                          this->generateExprIL(funcCall->params[i])));

        this->currentTemp = tempCurrentTemp;
    }

    return new FunctionCallExpr(funcCall->function->name, funcCall->function->returnType,
                                funcCall->function->returnPtr);
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

std::unordered_map<std::type_index, ExprOperator> ILGenerator::NodeExprToExprOperator = {
        {typeid(NodeAddExpr),             ExprOperator::add},
        {typeid(NodeSubExpr),             ExprOperator::sub},
        {typeid(NodeMultExpr),            ExprOperator::mult},
        {typeid(NodeDivExpr),             ExprOperator::div},
        {typeid(NodeModuloExpr),          ExprOperator::mod},
        {typeid(NodeLogicalOrExpr),       ExprOperator::logicalOr},
        {typeid(NodeLogicalAndExpr),      ExprOperator::logicalAnd},
        {typeid(NodeBoolEqualsExpr),      ExprOperator::equals},
        {typeid(NodeBoolNotEqualsExpr),   ExprOperator::notEquals},
        {typeid(NodeBiggerThanExpr),      ExprOperator::biggerThan},
        {typeid(NodeBiggerThanEqualExpr), ExprOperator::biggerThanEquals},
        {typeid(NodeLessThanExpr),        ExprOperator::lessThan},
        {typeid(NodeLessThanEqualExpr),   ExprOperator::lessThanEquals},
};

void ILGenerator::generateBinaryExprIL(BinaryNodeExprP binExpr) {
    ExprOperator op = NodeExprToExprOperator[typeid(*binExpr)];

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

UniExpr *ILGenerator::generateUnaryExprIL(UnaryNodeExprP unaryExpr) {
    UniExpr *innerUni = generateNumericExprIL(unaryExpr->expr);

    if (auto logicalNot = dynamic_cast<NodeLogicalNotExprP>(unaryExpr)) {
        return new LogicalNotExpr(innerUni);
    } else if (auto numericNeg = dynamic_cast<NodeNumericNegExprP>(unaryExpr)) {
        return new NumericNegExpr(innerUni);
    }

    return nullptr;
}

UniExpr *ILGenerator::generateNumericExprIL(NodeExprP expr) {
    if (auto binExpr = dynamic_cast<BinaryNodeExprP>(expr)) {
        this->generateBinaryExprIL(binExpr);
        return new UniTemp(this->currentTemp);
    } else if (auto paren = dynamic_cast<NodeParenthesisExprP>(expr)) {
        return generateNumericExprIL(paren->expr);
    } else if (auto unary = dynamic_cast<UnaryNodeExprP>(expr)) {
        return this->generateUnaryExprIL(unary);
//        return new UniTemp(this->currentTemp);
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
    currentTemp = 0;

    if (auto primitiveAssignmentStmt = dynamic_cast<NodePrimitiveAssignmentStmtP>(stmt)) {
        this->ilStmts.push_back(new VarAssignmentTAStmt(new VariableVal(primitiveAssignmentStmt->variable),
                                                        generateExprIL(primitiveAssignmentStmt->expr)));
    } else if (auto arrayAssignmentStmt = dynamic_cast<NodeArrayAssignmentStmtP>(stmt)) {
        this->ilStmts.push_back(new VarAssignmentTAStmt(new SubscriptableVariableVal(arrayAssignmentStmt->array,
                                                                                     generateNumericExprIL(
                                                                                             arrayAssignmentStmt->index)),
                                                        generateExprIL(arrayAssignmentStmt->expr)));
    } else if (auto functionCallStmt = dynamic_cast<NodeFunctionCallP>(stmt)) {
        this->ilStmts.push_back(generateFunctionCallExprIL(functionCallStmt));
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

        currentTemp = 0;

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
    this->ilStmts.push_back(new ScopeEnterStmt(scope->vars));

    for (NodeStmtP stmt: scope->stmts) {
        this->generateStmtIL(stmt);
    }

    this->ilStmts.push_back(new ScopeExitStmt());
}

void ILGenerator::generateFunctionIL(NodeFunctionP function) {
    auto funcDecStmt = new FunctionDeclarationStmt(function->name, function->params);
    this->maxTemp = 0;
    this->currentIfId = 0;
    this->currentWhileId = 0;
    this->currentFunctionName = function->name;

    this->ilStmts.push_back(funcDecStmt);
    this->generateScopeIL(function->scope);
    this->ilStmts.push_back(new LabelStmt(function->name + "End"));
    this->ilStmts.push_back(new FunctionExitStmt());

    funcDecStmt->maxTemp = this->maxTemp;

}

void ILGenerator::generateProgramIL() {
    for (auto funcPtr: this->program->functions) {
        if (funcPtr->scope != nullptr) {
            this->generateFunctionIL(funcPtr);
        }
    }

    std::ofstream outFile(this->outfileName);

    if (outFile.fail()) {
        outFile.close();
        throw CompilationException("[IL Generation - File Error] Error while opening the file " + this->outfileName);
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

std::unordered_map<ExprOperator, std::string> ILGenerator::exprOperatorToStr = {
        {ExprOperator::add,              " + "},
        {ExprOperator::sub,              " - "},
        {ExprOperator::mult,             " * "},
        {ExprOperator::div,              " / "},
        {ExprOperator::mod,              " % "},
        {ExprOperator::logicalOr,        " || "},
        {ExprOperator::logicalAnd,       " && "},
        {ExprOperator::equals,           " == "},
        {ExprOperator::notEquals,        " != "},
        {ExprOperator::biggerThan,       " > "},
        {ExprOperator::biggerThanEquals, " >= "},
        {ExprOperator::lessThan,         " < "},
        {ExprOperator::lessThanEquals,   " <= "},
};

std::string ILGenerator::ilExprToStr(ThreeAddressExprP taExpr) {
    std::stringstream strStream;

    if (auto imInt = dynamic_cast<ImIntValP>(taExpr)) {
        strStream << imInt->value;
    } else if (auto temp = dynamic_cast<UniTempP>(taExpr)) {
        strStream << "temp" << temp->id;
    } else if (auto subVar = dynamic_cast<SubscriptableVariableValP>(taExpr)) {
        strStream << subVar->var.name << "[" << ilExprToStr(subVar->index) << "]";
    } else if (auto var = dynamic_cast<VariableValP>(taExpr)) {
        strStream << var->var.name;
    } else if (auto logicalNot = dynamic_cast<LogicalNotExprP>(taExpr)) {
        strStream << "!" << ilExprToStr(logicalNot->expr);
    } else if (auto numericNeg = dynamic_cast<NumericNegExprP>(taExpr)) {
        strStream << "-" << ilExprToStr(numericNeg->expr);
    } else if (auto addr = dynamic_cast<AddrExprP>(taExpr)) {
        strStream << "&" << ilExprToStr(addr->addressable);
    } else if (auto binary = dynamic_cast<BinaryExprP>(taExpr)) {
        strStream << ilExprToStr(binary->left) << ILGenerator::exprOperatorToStr[binary->op]
                  << ilExprToStr(binary->right);
    } else if (auto functionCall = dynamic_cast<FunctionCallExprP>(taExpr)) {
        strStream << "RetValOf Call " << functionCall->functionName;
    }

    return strStream.str();
}

std::string ILGenerator::ilStmtToStr(ThreeAddressStmtP taStmt) {
    std::stringstream strStream;

    if (auto tempAssignment = dynamic_cast<TempAssignmentTAStmtP>(taStmt)) {
        strStream << "temp" << std::to_string(tempAssignment->id) << " := " << ilExprToStr(tempAssignment->expr);
    } else if (auto varAssignment = dynamic_cast<VarAssignmentTAStmtP>(taStmt)) {
        strStream << ilExprToStr(varAssignment->var) << " = " << ilExprToStr(varAssignment->expr);
    } else if (auto functionParamPush = dynamic_cast<FunctionParamPushStmtP>(taStmt)) {
        strStream << "PushParam " << ilExprToStr(functionParamPush->expr);
    } else if (auto functionCall = dynamic_cast<FunctionCallExprP>(taStmt)) {
        strStream << "Call " << functionCall->functionName;
    } else if (auto labelStmt = dynamic_cast<LabelStmtP>(taStmt)) {
        strStream << labelStmt->labelName << ":";
    } else if (auto gotoStmt = dynamic_cast<GotoStmtP>(taStmt)) {
        strStream << "Goto " << gotoStmt->labelName;
    } else if (auto gotoIfZeroStmt = dynamic_cast<GotoIfZeroStmtP>(taStmt)) {
        strStream << "GotoIfZero " << ilExprToStr(gotoIfZeroStmt->expr) << " " << gotoIfZeroStmt->labelName;
    } else if (auto gotoIfNotZeroStmt = dynamic_cast<GotoIfNotZeroStmtP>(taStmt)) {
        strStream << "GotoIfNotZero " << ilExprToStr(gotoIfNotZeroStmt->expr) << " " << gotoIfNotZeroStmt->labelName;
    } else if (auto setReturnValue = dynamic_cast<SetReturnValueStmtP>(taStmt)) {
        strStream << "SetReturnValue " << ilExprToStr(setReturnValue->expr);
    } else if (auto scopeEnter = dynamic_cast<ScopeEnterStmtP>(taStmt)) {
        strStream << "ScopeEnter Vals: ";

        for (const auto &var: scopeEnter->vars) {
            strStream << var.name << " ";
        }

        if (scopeEnter->vars.empty()) strStream << "none";
    } else if (auto scopeExit = dynamic_cast<ScopeExitStmtP>(taStmt)) {
        strStream << "ScopeExit";
    } else if (auto functionDeclaration = dynamic_cast<FunctionDeclarationStmtP>(taStmt)) {
        strStream << "Function " << functionDeclaration->name << "  MaxTemp: "
                  << std::to_string(functionDeclaration->maxTemp) << " Params: ";

        for (const auto &params: functionDeclaration->params) {
            strStream << params.name << " ";
        }

        if (functionDeclaration->params.empty()) strStream << "none";
    } else if (auto functionExit = dynamic_cast<FunctionExitStmtP>(taStmt)) {
        strStream << "EndFunction";
    }

    strStream << std::endl;

    return strStream.str();
}
