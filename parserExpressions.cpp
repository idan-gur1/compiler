//
// Created by idang on 16/03/2024.
//

#include "parser.h"



NodeExpr *Parser::parseFactor() {
    if (!this->lexer->hasNextToken()) {
        this->throwSyntaxError("Expression expected");
    }

    Token currentToken = this->lexer->currentAndProceedToken();

    if (currentToken.type != TokenType::immediateInteger &&
        currentToken.type != TokenType::exclamation &&
        currentToken.type != TokenType::identifier &&
        currentToken.type != TokenType::openParenthesis) {
        this->throwSyntaxError("Expression expected");
    }

    if (currentToken.type == TokenType::immediateInteger) { // Expr terminal
        return new NodeImIntTerminal(currentToken);
    } else if (currentToken.type == TokenType::identifier) {  // Expr terminal
        Variable var = this->getVarScopeStack(currentToken.val);

        if (!var.ptrType && var.arrSize == 0) {
            if (this->checkForTokenType(TokenType::openSquare)) {
                this->throwSemanticError("'" + var.name + "' is not subscriptable");
            }

            return new NodeVariableTerminal(var);
        }

        if (!this->checkForTokenType(TokenType::openSquare)) {
            this->throwSemanticError("'" + var.name + "' must be indexed in expressions");
        }

        return new NodeSubscriptableVariableTerminal(var, parseArrayBrackets());
    } else if (currentToken.type == TokenType::minus) {
        return new NodeNumericNegExpr(this->parseFactor());
    } else if (currentToken.type == TokenType::exclamation) {
        return new NodeLogicalNotExpr(this->parseFactor());
    }

    NodeExprP innerExpr = this->parseExpr();

    if (!checkForTokenTypeAndConsume(TokenType::closeParenthesis)) {
        this->throwSyntaxError("')' expected");
    }

    return new NodeParenthesisExpr(innerExpr);
}

NodeExpr *Parser::parseTerm(NodeExprP leftSibling, TokenType siblingOpType) {
    if (!this->lexer->hasNextToken()) {
        this->throwSyntaxError("Expression expected");
    }

    NodeExprP currentTerm = this->parseFactor();

    NodeExprP leftTerm = currentTerm;

    if (leftSibling != nullptr) {
        if (siblingOpType == TokenType::mult) {
            leftTerm = new NodeMultExpr(leftSibling, currentTerm);
        } else {
            leftTerm = new NodeDivExpr(leftSibling, currentTerm);
        }
    }

    if (!this->lexer->hasNextToken() ||
        (this->lexer->currentToken().type != TokenType::mult &&
         this->lexer->currentToken().type != TokenType::div)) {
        return leftTerm;
    }

    Token op = this->lexer->currentAndProceedToken();

    return parseTerm(leftTerm, op.type);
    /*NodeExprP currentTerm = parseFactor();

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

    return parseTerm(leftTerm, op.type);*/
}

NodeExpr *Parser::parseNumericExpr(NodeExprP leftSibling, TokenType siblingOpType) {
    if (!this->lexer->hasNextToken()) {
        this->throwSyntaxError("Expression expected");
    }

    NodeExprP currentExpr = this->parseTerm();

    NodeExprP leftExpr = currentExpr;

    if (leftSibling != nullptr) {
        if (siblingOpType == TokenType::plus) {
            leftExpr = new NodeAddExpr(leftSibling, currentExpr);
        } else {
            leftExpr = new NodeSubExpr(leftSibling, currentExpr);
        }
    }

    if (!this->lexer->hasNextToken() ||
        (this->lexer->currentToken().type != TokenType::plus &&
         this->lexer->currentToken().type != TokenType::minus)) {
        return leftExpr;
    }

    Token op = this->lexer->currentAndProceedToken();

    return parseNumericExpr(leftExpr, op.type);
    /*NodeExprP currentExpr = parseTerm();

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

    return parseNumericExpr(leftExpr, op.type);*/
}

NodeExpr *Parser::parseRelationalExpr(NodeExprP leftSibling, TokenType siblingOpType) {
    if (!this->lexer->hasNextToken()) {
        this->throwSyntaxError("Expression expected");
    }

    NodeExprP currentExpr = this->parseNumericExpr();

    NodeExprP leftExpr = currentExpr;

    if (leftSibling != nullptr) {
        if (siblingOpType == TokenType::relationalG) {
            leftExpr = new NodeBiggerThanExpr(leftSibling, currentExpr);
        } else if (siblingOpType == TokenType::relationalGE) {
            leftExpr = new NodeBiggerThanEqualExpr(leftSibling, currentExpr);
        } else if (siblingOpType == TokenType::relationalL) {
            leftExpr = new NodeLessThanExpr(leftSibling, currentExpr);
        } else {
            leftExpr = new NodeLessThanEqualExpr(leftSibling, currentExpr);
        }
    }

    if (!this->lexer->hasNextToken() ||
        (this->lexer->currentToken().type != TokenType::relationalG &&
        this->lexer->currentToken().type != TokenType::relationalGE &&
        this->lexer->currentToken().type != TokenType::relationalL &&
         this->lexer->currentToken().type != TokenType::relationalLE)) {
        return leftExpr;
    }

    Token op = this->lexer->currentAndProceedToken();

    return parseRelationalExpr(leftExpr, op.type);
}

NodeExpr *Parser::parseEqualityExpr(NodeExprP leftSibling, TokenType siblingOpType) {
    if (!this->lexer->hasNextToken()) {
        this->throwSyntaxError("Expression expected");
    }

    NodeExprP currentExpr = this->parseRelationalExpr();

    NodeExprP leftExpr = currentExpr;

    if (leftSibling != nullptr) {
        if (siblingOpType == TokenType::doubleEqual) {
            leftExpr = new NodeBoolEqualsExpr(leftSibling, currentExpr);
        } else {
            leftExpr = new NodeBoolNotEqualsExpr(leftSibling, currentExpr);
        }
    }

    if (!this->lexer->hasNextToken() ||
        (this->lexer->currentToken().type != TokenType::doubleEqual &&
         this->lexer->currentToken().type != TokenType::notEqual)) {
        return leftExpr;
    }

    Token op = this->lexer->currentAndProceedToken();

    return parseEqualityExpr(leftExpr, op.type);
}

NodeExpr *Parser::parseLogicalAndExpr(NodeExprP leftSibling) {
    if (!this->lexer->hasNextToken()) {
        this->throwSyntaxError("Expression expected");
    }

    NodeExprP currentExpr = this->parseEqualityExpr();

    NodeExprP leftExpr = currentExpr;

    if (leftSibling != nullptr) {
        leftExpr = new NodeLogicalAndExpr(leftSibling, currentExpr);
    }

    if (!this->checkForTokenType(TokenType::logicalAnd)) {
        return leftExpr;
    }

    this->lexer->currentAndProceedToken();

    return parseLogicalAndExpr(leftExpr);
}

NodeExpr *Parser::parseLogicalOrExpr(NodeExprP leftSibling) {
    if (!this->lexer->hasNextToken()) {
        this->throwSyntaxError("Expression expected");
    }

    NodeExprP currentExpr = this->parseLogicalAndExpr();

    NodeExprP leftExpr = currentExpr;

    if (leftSibling != nullptr) {
        leftExpr = new NodeLogicalOrExpr(leftSibling, currentExpr);
    }

    if (!this->checkForTokenType(TokenType::logicalOr)) {
        return leftExpr;
    }

    this->lexer->currentAndProceedToken();

    return parseLogicalOrExpr(leftExpr);
}

NodeExpr *Parser::parseExpr() {
    return this->parseLogicalOrExpr();
}
