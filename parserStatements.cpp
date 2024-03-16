//
// Created by idang on 15/03/2024.
//

#include "parser.h"

NodeStmt *Parser::stmtPrimitiveAssignment(const Variable& var) {
    this->lexer->currentAndProceedToken(); // Remove the equals
//    if (this->lexer->hasNextToken() && this->lexer->currentToken().type == TokenType::ampersand) {
    if (!checkForTokenType(TokenType::ampersand)) {
        if (var.ptrType || var.arrSize > 0) {
            Parser::throwSemanticError("Invalid assignment to identifier '" + var.name + "'");
        }

        return new NodePrimitiveAssignmentStmt(var, this->parseExpr());
    }
    this->lexer->currentAndProceedToken();

    if (!var.ptrType) {
        Parser::throwSemanticError("Invalid assignment to identifier '" + var.name + "'");
    }

    identifierTokenExists();
    Variable targetVar = this->getVarScopeStack(this->lexer->currentAndProceedToken().val);

    return new NodePointerAddrAssignmentStmt(var, targetVar);
}

NodeStmt *Parser::stmtArrayAssignment(const Variable& var) {
    /*this->lexer->currentAndProceedToken(); // Remove the open square bracket

    NodeExprP indexExpr = this->parseExpr();

    if (!this->checkForTokenTypeAndConsume(TokenType::closeSquare)) {
        Parser::throwError("[Syntax Error] ']' expected");
    }*/
    if (var.arrSize == 0 && !var.ptrType) {
        Parser::throwSemanticError("'" + var.name + "' is not subscriptable");
    }

    NodeExprP indexExpr = this->parseArrayBrackets();

    if (checkForTokenType(TokenType::equal)) {
        this->lexer->currentAndProceedToken();
        return new NodeArrayAssignmentStmt(var, indexExpr, this->parseExpr());
    }

    delete indexExpr;

    return nullptr;
}

NodeStmt *Parser::stmtByIdentifier(const Token& ident) {
    Variable var = this->getVarScopeStack(ident.val);

    if (checkForTokenType(TokenType::equal)) {
        return this->stmtPrimitiveAssignment(var);
    } else if (checkForTokenType(TokenType::openSquare)) {
        return this->stmtArrayAssignment(var);
    }

    return nullptr;
}

NodeStmt *Parser::stmtVariableDeclaration(VariableType type) {
    this->lexer->currentAndProceedToken(); // Remove the type lexeme

    bool ptr = checkForTokenType(TokenType::mult);
    if (ptr) this->lexer->currentAndProceedToken();

    identifierTokenExists();
    std::string varName = this->lexer->currentAndProceedToken().val;

    if (varExistsCurrentScope(varName)) {
        Parser::throwSemanticError("Redeclaration of identifier " + varName);
    }


    if (checkForTokenType(TokenType::openSquare)) {
        if (ptr) Parser::throwSemanticError("Arrays can only be of primitive types");

        auto sizeExpr = dynamic_cast<NodeImIntTerminalP>(parseArrayBrackets());

        if (!sizeExpr) {
            Parser::throwSemanticError("Array size must be known at compile time");
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