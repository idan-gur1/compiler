//
// Created by idang on 16/03/2024.
//

#include "parser.h"



NodeExpr *Parser::parseFactor() {
    if (!this->lexer->hasNextToken()) {
        /*std::cout << "Syntax Error: Token expected" << std::endl;
        exit(1);*/
        this->throwSyntaxError("Number, identifier or parenthesis expected");
    }

    Token currentToken = this->lexer->currentAndProceedToken();

    if (currentToken.type != TokenType::immediateInteger &&
        currentToken.type != TokenType::identifier &&
        currentToken.type != TokenType::openParenthesis) {
        this->throwSyntaxError("Number, identifier or parenthesis expected");
    }

    if (currentToken.type == TokenType::immediateInteger) {
        return new NodeImIntTerminal(currentToken);
    } else if (currentToken.type == TokenType::identifier) {
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
    }

    NodeExprP innerExpr = parseExpr();

    if (!checkForTokenTypeAndConsume(TokenType::closeParenthesis)) {
        this->throwSyntaxError("')' expected");
    }

    return new ParenthesisNodeExpr(innerExpr);
}

NodeExpr *Parser::parseTerm(NodeExprP leftSibling, TokenType siblingOpType) {
    if (!this->lexer->hasNextToken()) {
        this->throwSyntaxError("Token expected");
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
        this->throwSyntaxError("Token expected");
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

NodeExpr *Parser::parseRelationalExpr(NodeExprP leftSibling) {
    return nullptr;
}

NodeExpr *Parser::parseEqualityExpr(NodeExprP leftSibling) {
    return nullptr;
}

NodeExpr *Parser::parseLogicalAndExpr(NodeExprP leftSibling) {
    return nullptr;
}

NodeExpr *Parser::parseLogicalOrExpr(NodeExprP leftSibling) {
    return nullptr;
}