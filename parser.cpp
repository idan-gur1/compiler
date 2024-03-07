//
// Created by idang on 13/12/2023.
//

#include "parser.h"

bool Parser::varExistsScopeStack(const std::string &var) {
    std::stack<NodeScopeP> scopesCopy = this->scopes;
    while (!scopesCopy.empty()) {
        if (scopesCopy.top()->vars.contains(var)) return true;
        scopesCopy.pop();
    }

    return false;
}

bool Parser::varExistsCurrentScope(const std::string &var) {
    return this->scopes.top()->vars.contains(var);
}

void Parser::addVarToCurrentScope(const std::string& var) {
    this->scopes.top()->vars.insert(var);
}

bool Parser::stmtDelimiterExists() {
    return this->lexer->hasNextToken() && this->lexer->currentAndProceedToken().type == TokenType::semiColon;
}

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
//        if (!this->variables.contains(currentToken.val)) {
        if (!varExistsScopeStack(currentToken.val)) {
            std::cout << "Compile Error: Use of undeclared variable " << currentToken.val << std::endl;
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

NodeStmt *Parser::tryParseStmt() {
    if (!this->lexer->hasNextToken()) {
        return nullptr;
    }

    NodeStmt *stmt = nullptr;

    Token firstToken = this->lexer->currentToken();

    if (firstToken.type == TokenType::identifier) {
        this->lexer->currentAndProceedToken();

        if (this->lexer->hasNextToken()) {
            Token nextToken = this->lexer->currentToken();

            if (nextToken.type == TokenType::equal) {
                if (!varExistsScopeStack(firstToken.val)) {
                    std::cout << "Compile Error: Use of undeclared identifier " << firstToken.val << std::endl;
                    exit(1);
                }

                stmt = new NodeAssignmentStmt(firstToken, parseExpr());

                if (!stmtDelimiterExists()) {
                    std::cout << "Syntax Error: ; expected" << std::endl;
                    exit(1);
                }
            }
        }

    } else if (firstToken.type == TokenType::intKeyword) {
        this->lexer->currentAndProceedToken();

        if (!this->lexer->hasNextToken() || this->lexer->currentToken().type != TokenType::identifier) {
            std::cout << "Syntax Error: Identifier expected" << std::endl;
            exit(1);
        }

        Token ident = this->lexer->currentAndProceedToken();

        if (varExistsCurrentScope(ident.val)) {
            std::cout << "Compile Error: redeclaration of identifier " << ident.val << std::endl;
            exit(1);
        }

        this->addVarToCurrentScope(ident.val);

        if (this->lexer->hasNextToken() && this->lexer->currentToken().type == TokenType::equal) {
            this->lexer->currentAndProceedToken();
            stmt = new NodeAssignmentStmt(ident, parseExpr());
        }

        if (!stmtDelimiterExists()) {
            std::cout << "Syntax Error: ; expected" << std::endl;
            exit(1);
        }
    }
    // TODO continue here
    // from here - old code

//    Token ident = this->lexer->currentToken();
//
//    if (ident.type != TokenType::identifier) {
//        return nullptr;
//    }
//
//    this->lexer->currentAndProceedToken();
//
//    if (!this->lexer->hasNextToken()) {
//        std::cout << "Syntax Error: = expected" << std::endl;
//        exit(1);
//    }
//
//    if (this->lexer->currentAndProceedToken().type != TokenType::equal) {
//        std::cout << "Syntax Error: Unexpected token" << std::endl;
//        exit(1);
//    }
//
//    auto stmt = new NodeAssignmentStmt(ident, parseExpr());
//
//    if (this->scopes.top()->vars.contains(ident.val)) {
//        std::cout << "Compile Error: Redeclaration of the variable " << ident.val << std::endl;
//        exit(1);
//    }
//
//    this->scopes.top()->vars.insert(ident.val);
//
//    if (!this->lexer->hasNextToken() ||
//        this->lexer->currentAndProceedToken().type != TokenType::semiColon) {
//        std::cout << "Syntax Error: ; expected" << std::endl;
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
    this->scopes.push(scope);

    while (auto stmt = tryParseStmt()) {
        scope->stmts.push_back(stmt);
    }

    this->scopes.pop();

    if (!this->lexer->hasNextToken() ||
        this->lexer->currentAndProceedToken().type != TokenType::closeCurly) {
        std::cout << "Syntax Error: } expected" << std::endl;
        exit(1);
    }

    return scope;
}
