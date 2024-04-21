//
// Created by idang on 15/03/2024.
//

#include "parser.h"

NodeStmt *Parser::stmtPrimitiveAssignment(const Variable &var) {
    this->lexer->currentAndProceedToken(); // Remove the 'equals' lexeme

    if (var.arrSize > 0) {
        this->throwSemanticError("'" + var.name + "' of type array is constant");
    }

    NodeExpr *innerExpr = this->parseExpr();

    auto *ptr = dynamic_cast<AddrNodeExpr *>(innerExpr);
    auto *func = dynamic_cast<NodeFunctionCall *>(innerExpr);

    if (var.ptrType != this->ptrUsedInExpr) {
        delete innerExpr;
        this->throwSemanticError("Invalid assignment to identifier '" + var.name + "'");
    }

    if ((ptr && ptr->target->variable.type != var.type) || (func && func->function->returnPtr && func->function->returnType != var.type)) {
        delete innerExpr;
        this->throwSemanticError("Incompatible pointer type assignment to '" + var.name + "'");
    }

    return new NodePrimitiveAssignmentStmt(var, innerExpr);
}

NodeStmt *Parser::stmtArrayAssignment(const Variable &var) {
    if (var.arrSize == 0 && !var.ptrType) {
        this->throwSemanticError("'" + var.name + "' is not subscriptable");
    }

    NodeExprP indexExpr = this->parseArrayBrackets();

    if (checkForTokenTypeAndConsumeIfYes(TokenType::equal)) {
        NodeExprP innerExpr = this->parseExpr();

        if (this->ptrUsedInExpr) {
            delete indexExpr;
            delete innerExpr;
            this->throwSemanticError("Invalid use of pointers");
        }

        return new NodeArrayAssignmentStmt(var, indexExpr, innerExpr);
    }

    delete indexExpr;

    return nullptr;
}

NodeStmt *Parser::stmtByIdentifier(const Token &ident) {
    if (checkForTokenType(TokenType::openParenthesis)) {
        NodeFunctionP func = getFunction(ident.val);
        if (!func) {
            this->throwSemanticError("Use of undeclared function '" + ident.val + "'");
        }

        std::vector<NodeExprP> params = parseParenthesisExprList();

        validateFunctionCallParams(params, func);

        return new NodeFunctionCall(func, params);
    }

    Variable var = this->getVarScopeStack(ident.val);

    if (checkForTokenType(TokenType::equal)) {
        return this->stmtPrimitiveAssignment(var);
    } else if (checkForTokenType(TokenType::openSquare)) {
        return this->stmtArrayAssignment(var);
    }

    return nullptr;
}

NodeStmt *Parser::stmtVariableDeclaration(VariableType type) {
    this->lexer->currentAndProceedToken(); // Remove the 'type' lexeme

    bool ptr = checkForTokenType(TokenType::mult);
    if (ptr) this->lexer->currentAndProceedToken();

    identifierTokenExists();
    std::string varName = this->lexer->currentAndProceedToken().val;

    if (varExistsCurrentScope(varName)) {
        this->throwSemanticError("Redeclaration of identifier " + varName);
    }


    if (checkForTokenType(TokenType::openSquare)) {
        if (ptr) this->throwSemanticError("Arrays can't be of type pointers");

        auto sizeExpr = dynamic_cast<NodeImIntTerminalP>(parseArrayBrackets());

        if (!sizeExpr) {
            this->throwSemanticError("Array size must be known at compile time");
        }

        this->addVarToCurrentScope(Variable(varName, type, ptr, std::stoi(sizeExpr->value)));

        return nullptr;
    }

    Variable var(varName, type, ptr);

    NodeStmtP stmt = nullptr;

    if (checkForTokenType(TokenType::equal)) {
        stmt = this->stmtPrimitiveAssignment(var);
    }

    this->addVarToCurrentScope(var);

    return stmt;
}

NodeStmt *Parser::stmtIf() {
    this->lexer->currentAndProceedToken(); // Remove the 'if' lexeme

    NodeExprP expr = parseParenthesisExpr();
    NodeScopeP ifBlock = parseScope();
    NodeScopeP elseBlock = nullptr;

    if (this->checkForTokenTypeAndConsumeIfYes(TokenType::elseKeyword)) { // Remove the 'else' lexeme
        elseBlock = parseScope();
    }

    return new NodeIf(expr, ifBlock, elseBlock);
}

NodeStmt *Parser::stmtWhile(bool isDo) {
    this->lexer->currentAndProceedToken(); // Remove the 'while' or 'do' lexeme

    NodeExprP expr;

    if (!isDo) {
        expr = parseParenthesisExpr();
    }

    NodeScopeP codeBlock = parseScope();

    if (isDo) {
        if (!this->checkForTokenTypeAndConsume(TokenType::whileKeyword)) {
            this->throwSemanticError("While keyword expected");
        }

        expr = parseParenthesisExpr();
    }

    return new NodeWhile(expr, codeBlock, isDo);
}