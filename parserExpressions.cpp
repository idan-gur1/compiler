//
// Created by idang on 16/03/2024.
//

#include "parser.h"



NodeExpr *Parser::parseFactor() {
    if (!this->lexer->hasNextToken()) {
        this->throwSyntaxError("Expression expected");
    }

    Token currentToken = this->lexer->currentAndProceedToken();

    if (currentToken.type == TokenType::immediateInteger) { // Expr terminal
        return new NodeImIntTerminal(currentToken);
    } else if (currentToken.type == TokenType::identifier) {  // Expr terminal
        if (NodeFunctionP func = getFunction(currentToken.val)) {
            if (func->returnType == VariableType::voidType) {
                this->throwSemanticError("Function of type void does not return any value");
            }

            if (func->returnPtr) {
                this->throwSemanticError("Function of return type of pointer cant be used in expressions");
            }

            std::vector<NodeExprP> params = std::get<std::vector<NodeExprP>>(parseParenthesisExprList(false));

            validateFunctionCallParams(params, func);

            return new NodeFunctionCall(currentToken.val, params);
        }

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
    } else if (currentToken.type == TokenType::openCurly) {
        NodeExprP innerExpr = this->parseExpr();

        if (!checkForTokenTypeAndConsume(TokenType::closeParenthesis)) {
            this->throwSyntaxError("')' expected");
        }

        return new NodeParenthesisExpr(innerExpr);
    }

    this->throwSyntaxError("Expression expected");

    return nullptr;
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

NodeExpr *Parser::parseAddrExpr() {
    this->lexer->currentAndProceedToken(); // Remove the 'ampersand' lexeme

    this->identifierTokenExists();
    Variable target = getVarScopeStack(this->lexer->currentAndProceedToken().val);

    if (target.ptrType) {
        this->throwSemanticError("Cannot address pointer type");
    }

    return new AddrNodeExpr(target);
}

NodeExpr *Parser::parseExpr() {
    if (this->checkForTokenType(TokenType::ampersand)) {
        return parseAddrExpr();
    }

    return this->parseLogicalOrExpr();
}
