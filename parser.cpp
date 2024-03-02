//
// Created by idang on 13/12/2023.
//

#include "parser.h"

NodeExpr *Parser::parseFactor() {
    if (!this->lexer->hasNextToken()) {
        std::cout << "Syntax Error: Token expected" << std::endl;
        exit(1);
    }

    Token currentToken = this->lexer->currentAndProceedToken();

    if (currentToken.type != TokenType::immediateInteger &&
        currentToken.type != TokenType::identifier &&
        currentToken.type != TokenType::openParenthesis) {
        std::cout << "Syntax Error: Number, identifier or parenthesis expected" << std::endl;
        exit(1);
    }

    if (currentToken.type == TokenType::immediateInteger) {
        return new NodeImIntTerminal(currentToken);
    } else if (currentToken.type == TokenType::identifier) {
        if (!this->variables.contains(currentToken.val)) {
            std::cout << "Compile Error: Undeclared variable " << currentToken.val << std::endl;
            exit(1);
        }
        return new NodeIdentTerminal(currentToken);
    }

    NodeExprP innerExpr = parseExpr();

    if (!this->lexer->hasNextToken() ||
        this->lexer->currentAndProceedToken().type != TokenType::closeParenthesis) {
        std::cout << "Syntax Error: ) expected" << std::endl;
        exit(1);
    }

    return new ParenthesisNodeExpr(innerExpr);
}

NodeExpr *Parser::parseTerm(NodeExprP leftSibling, TokenType siblingOpType) {
    if (!this->lexer->hasNextToken()) {
        std::cout << "Syntax Error: Token expected" << std::endl;
        exit(1);
    }

    NodeExprP currentTerm = parseFactor();

    if (!this->lexer->hasNextToken() ||
        (this->lexer->currentToken().type != TokenType::mult &&
         this->lexer->currentToken().type != TokenType::div)) {
        if (leftSibling == nullptr) {
            return currentTerm;
        }
        if (siblingOpType == TokenType::mult) {
            return new NodeMultExpr(leftSibling, currentTerm);
        }

        return new NodeDivExpr(leftSibling, currentTerm);
    }

    Token op = this->lexer->currentAndProceedToken();

    NodeExprP leftTerm = currentTerm;

    if (leftSibling != nullptr) {
        if (siblingOpType == TokenType::mult) {
            leftTerm = new NodeMultExpr(leftSibling, currentTerm);
        } else {
            leftTerm = new NodeDivExpr(leftSibling, currentTerm);
        }
    }

    return parseTerm(leftTerm, op.type);
}

NodeExpr *Parser::parseExpr(NodeExprP leftSibling, TokenType siblingOpType) {
    if (!this->lexer->hasNextToken()) {
        std::cout << "Syntax Error: Token expected" << std::endl;
        exit(1);
    }

    NodeExprP currentExpr = parseTerm();

    if (!this->lexer->hasNextToken() ||
        (this->lexer->currentToken().type != TokenType::plus &&
         this->lexer->currentToken().type != TokenType::minus)) {
        if (leftSibling == nullptr) {
            return currentExpr;
        }
        if (siblingOpType == TokenType::plus) {
            return new NodeAddExpr(leftSibling, currentExpr);
        }

        return new NodeSubExpr(leftSibling, currentExpr);
    }

    Token op = this->lexer->currentAndProceedToken();

    NodeExprP leftExpr = currentExpr;

    if (leftSibling != nullptr) {
        if (siblingOpType == TokenType::plus) {
            leftExpr = new NodeAddExpr(leftSibling, currentExpr);
        } else {
            leftExpr = new NodeSubExpr(leftSibling, currentExpr);
        }
    }

    return parseExpr(leftExpr, op.type);
}

NodeAssignmentStmt *Parser::tryParseStmt() {
//    if (!this->lexer->hasNextToken()) {
//        std::cout << "Syntax Error: Token expected" << std::endl;
//        exit(1);
//    }
    if (!this->lexer->hasNextToken()) {
        return nullptr;
    }

    Token ident = this->lexer->currentToken();

    if (ident.type != TokenType::identifier) {
        return nullptr;
//        std::cout << "Syntax Error: L-type expression expected; identifier." << std::endl;
//        exit(1);
    }

    this->lexer->currentAndProceedToken();

    if (!this->lexer->hasNextToken()) {
        std::cout << "Syntax Error: = expected" << std::endl;
        exit(1);
    }

    if (this->lexer->currentAndProceedToken().type != TokenType::equal) {
        std::cout << "Syntax Error: Unexpected token" << std::endl;
        exit(1);
    }

    auto stmt = new NodeAssignmentStmt(ident, parseExpr());
    if (this->variables.contains(ident.val)) {
        std::cout << "Compile Error: Redeclaration of the variable " << ident.val << std::endl;
        exit(1);
    }
    this->variables.insert(ident.val);

    if (!this->lexer->hasNextToken() ||
        this->lexer->currentAndProceedToken().type != TokenType::semiColon) {
        std::cout << "Syntax Error: ; expected" << std::endl;
        exit(1);
    }

//    if (this->lexer->currentAndProceedToken().type != TokenType::semiColon) {
//        std::cout << "Syntax Error: Unexpected token" << std::endl;
//        exit(1);
//    }

    return stmt;
}

NodeScope *Parser::parseScope() {
    auto scope = new NodeScope();

    if (!this->lexer->hasNextToken() ||
        this->lexer->currentAndProceedToken().type != TokenType::openCurly) {
        std::cout << "Syntax Error: { expected" << std::endl;
        exit(1);
    }

//    while (this->lexer->hasNextToken()) {
//        scope->stmts.push_back(tryParseStmt());
//    }
    while (auto stmt = tryParseStmt()) {
        scope->stmts.push_back(stmt);
    }

    if (!this->lexer->hasNextToken() ||
        this->lexer->currentAndProceedToken().type != TokenType::closeCurly) {
        std::cout << "Syntax Error: } expected" << std::endl;
        exit(1);
    }

    return scope;
}
