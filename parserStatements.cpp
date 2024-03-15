//
// Created by idang on 15/03/2024.
//

#include "parser.h"

NodeStmt *Parser::stmtPrimitiveAssignment(Variable var) {
    this->lexer->currentAndProceedToken(); // Remove the open equals
    if (this->lexer->hasNextToken() && this->lexer->currentToken().type == TokenType::ampersand) {
        this->lexer->currentAndProceedToken();
        if (!this->lexer->hasNextToken() || this->lexer->currentToken().type != TokenType::identifier) {
            Parser::throwError("[Syntax Error] ']' identifier expected");
        }
        std::optional<Variable> targetVar = this->varExistsScopeStack(this->lexer->currentAndProceedToken().val);

        if (!targetVar.has_value()) {
            Parser::throwError("Use of undeclared identifier " + targetVar->name);
        }
        return new NodePointerAddrAssignmentStmt(var, targetVar.value());
    } else {
        return new NodePrimitiveAssignmentStmt(var, this->parseExpr());
    }
}

NodeStmt *Parser::stmtArrayAssignment(Variable var) {
    this->lexer->currentAndProceedToken(); // Remove the open square bracket

    NodeExprP indexExpr = this->parseExpr();

    if (!this->lexer->hasNextToken() || this->lexer->currentAndProceedToken().type != TokenType::closeSquare) {
        Parser::throwError("[Syntax Error] ']' expected");
    }

    if (this->lexer->hasNextToken() && this->lexer->currentToken().type == TokenType::equal) {
        this->lexer->currentAndProceedToken();
        return new NodeArrayAssignmentStmt(var, indexExpr, this->parseExpr());
    }

    delete indexExpr;

    return nullptr;
}

NodeStmt *Parser::stmtByIdentifier(const Token& ident) {
    std::optional<Variable> var = this->varExistsScopeStack(ident.val);

    if (!var.has_value()) {
        Parser::throwError("Use of undeclared identifier " + ident.val);
    }

    if (this->lexer->hasNextToken() && this->lexer->currentToken().type == TokenType::equal) {
        return this->stmtPrimitiveAssignment(var.value());
    } else if (this->lexer->hasNextToken() && this->lexer->currentToken().type == TokenType::openSquare) {
        return this->stmtArrayAssignment(var.value());
    }

    return nullptr;
}

NodeStmt *Parser::stmtVariableDeclaration(VariableType type) {
    // TODO continue here
    return nullptr;
}