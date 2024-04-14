//
// Created by idang on 16/03/2024.
//

#include "parser.h"



NodeExpr *Parser::parseFactor(bool ptrNotAllowed) {
    if (!this->lexer->hasNextToken()) {
        this->throwSyntaxError("Expression expected");
    }

//    Token currentToken = this->lexer->currentAndProceedToken();
    Token currentToken = this->lexer->currentToken();

    if (currentToken.type == TokenType::immediateInteger) { // Expr terminal
        this->lexer->currentAndProceedToken();

        return new NodeImIntTerminal(currentToken);
    } else if (currentToken.type == TokenType::identifier) {  // Expr terminal
        this->lexer->currentAndProceedToken();

        if (NodeFunctionP func = getFunction(currentToken.val)) {
            if (func->returnType == VariableType::voidType) {
                this->throwSemanticError("Function of type void does not return any value");
            }

            std::vector<NodeExprP> params = parseParenthesisExprList();

            validateFunctionCallParams(params, func);

            if (func->returnPtr) {
                this->ptrUsedInExpr = true;
            }

            return new NodeFunctionCall(func, params);
        }

        Variable var = this->getVarScopeStack(currentToken.val);

        if (!var.ptrType && var.arrSize == 0) {
            if (this->checkForTokenType(TokenType::openSquare)) {
                this->throwSemanticError("'" + var.name + "' is not subscriptable");
            }

            return new NodeVariableTerminal(var);
        }

        if (!this->checkForTokenType(TokenType::openSquare)) {
            if (ptrNotAllowed) {
                this->throwSemanticError("Illegal use of '" + var.name + "'");
            }

            this->ptrUsedInExpr = true;

            return new AddrNodeExpr(new NodeVariableTerminal(var));
        }

        return new NodeSubscriptableVariableTerminal(var, parseArrayBrackets());
    } else if (currentToken.type == TokenType::minus) {
        this->lexer->currentAndProceedToken();

        return new NodeNumericNegExpr(this->parseFactor(true));
    } else if (currentToken.type == TokenType::exclamation) {
        this->lexer->currentAndProceedToken();

        return new NodeLogicalNotExpr(this->parseFactor(true));
    } else if (currentToken.type == TokenType::openParenthesis) {
        this->lexer->currentAndProceedToken();

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

    NodeExprP leftTerm = this->parseFactor();

    if (leftSibling != nullptr) {
        checkPointerUsage(leftTerm);

        if (siblingOpType == TokenType::mult) {
            leftTerm = new NodeMultExpr(leftSibling, leftTerm);
        } else {
            leftTerm = new NodeDivExpr(leftSibling, leftTerm);
        }
    }

    if (!this->lexer->hasNextToken() ||
        (this->lexer->currentToken().type != TokenType::mult &&
         this->lexer->currentToken().type != TokenType::div)) {
        return leftTerm;
    }

    checkPointerUsage(leftTerm);

    Token op = this->lexer->currentAndProceedToken();

    return parseTerm(leftTerm, op.type);
}

NodeExpr *Parser::parseNumericExpr(NodeExprP leftSibling, TokenType siblingOpType) {
    if (!this->lexer->hasNextToken()) {
        this->throwSyntaxError("Expression expected");
    }

    NodeExprP leftExpr = this->parseTerm();

    if (leftSibling != nullptr) {
        checkPointerUsage(leftExpr);

        if (siblingOpType == TokenType::plus) {
            leftExpr = new NodeAddExpr(leftSibling, leftExpr);
        } else {
            leftExpr = new NodeSubExpr(leftSibling, leftExpr);
        }
    }

    if (!this->lexer->hasNextToken() ||
        (this->lexer->currentToken().type != TokenType::plus &&
         this->lexer->currentToken().type != TokenType::minus)) {
        return leftExpr;
    }

    checkPointerUsage(leftExpr);

    Token op = this->lexer->currentAndProceedToken();

    return parseNumericExpr(leftExpr, op.type);
}

NodeExpr *Parser::parseRelationalExpr(NodeExprP leftSibling, TokenType siblingOpType) {
    if (!this->lexer->hasNextToken()) {
        this->throwSyntaxError("Expression expected");
    }

    NodeExprP leftExpr = this->parseNumericExpr();

    if (leftSibling != nullptr) {
        checkPointerUsage(leftExpr);

        if (siblingOpType == TokenType::relationalG) {
            leftExpr = new NodeBiggerThanExpr(leftSibling, leftExpr);
        } else if (siblingOpType == TokenType::relationalGE) {
            leftExpr = new NodeBiggerThanEqualExpr(leftSibling, leftExpr);
        } else if (siblingOpType == TokenType::relationalL) {
            leftExpr = new NodeLessThanExpr(leftSibling, leftExpr);
        } else {
            leftExpr = new NodeLessThanEqualExpr(leftSibling, leftExpr);
        }
    }

    if (!this->lexer->hasNextToken() ||
        (this->lexer->currentToken().type != TokenType::relationalG &&
        this->lexer->currentToken().type != TokenType::relationalGE &&
        this->lexer->currentToken().type != TokenType::relationalL &&
         this->lexer->currentToken().type != TokenType::relationalLE)) {
        return leftExpr;
    }

    checkPointerUsage(leftExpr);

    Token op = this->lexer->currentAndProceedToken();

    return parseRelationalExpr(leftExpr, op.type);
}

NodeExpr *Parser::parseEqualityExpr(NodeExprP leftSibling, TokenType siblingOpType) {
    if (!this->lexer->hasNextToken()) {
        this->throwSyntaxError("Expression expected");
    }

    NodeExprP leftExpr = this->parseRelationalExpr();

    if (leftSibling != nullptr) {
        checkPointerUsage(leftExpr);

        if (siblingOpType == TokenType::doubleEqual) {
            leftExpr = new NodeBoolEqualsExpr(leftSibling, leftExpr);
        } else {
            leftExpr = new NodeBoolNotEqualsExpr(leftSibling, leftExpr);
        }
    }

    if (!this->lexer->hasNextToken() ||
        (this->lexer->currentToken().type != TokenType::doubleEqual &&
         this->lexer->currentToken().type != TokenType::notEqual)) {
        return leftExpr;
    }

    checkPointerUsage(leftExpr);

    Token op = this->lexer->currentAndProceedToken();

    return parseEqualityExpr(leftExpr, op.type);
}

NodeExpr *Parser::parseLogicalAndExpr(NodeExprP leftSibling) {
    if (!this->lexer->hasNextToken()) {
        this->throwSyntaxError("Expression expected");
    }

    NodeExprP leftExpr = this->parseEqualityExpr();

    if (leftSibling != nullptr) {
        checkPointerUsage(leftExpr);

        leftExpr = new NodeLogicalAndExpr(leftSibling, leftExpr);
    }

    if (!this->checkForTokenType(TokenType::logicalAnd)) {
        return leftExpr;
    }

    checkPointerUsage(leftExpr);

    this->lexer->currentAndProceedToken();

    return parseLogicalAndExpr(leftExpr);
}

NodeExpr *Parser::parseLogicalOrExpr(NodeExprP leftSibling) {
    if (!this->lexer->hasNextToken()) {
        this->throwSyntaxError("Expression expected");
    }

    NodeExprP leftExpr = this->parseLogicalAndExpr();

    if (leftSibling != nullptr) {
        checkPointerUsage(leftExpr);

        leftExpr = new NodeLogicalOrExpr(leftSibling, leftExpr);
    }

    if (!this->checkForTokenType(TokenType::logicalOr)) {
        return leftExpr;
    }

    checkPointerUsage(leftExpr);

    this->lexer->currentAndProceedToken();

    return parseLogicalOrExpr(leftExpr);
}

NodeExpr *Parser::parseAddrExpr() {
    this->lexer->currentAndProceedToken(); // Remove the 'ampersand' lexeme

    this->identifierTokenExists();

    NodeExprP addressable = this->parseFactor(true);

    auto varTerminal = dynamic_cast<NodeVariableTerminal *>(addressable);

    if (!varTerminal) {
        delete addressable;
        this->throwSemanticError("Unaddressable expression");
    }

    return new AddrNodeExpr(varTerminal);
}

NodeExpr *Parser::parseExpr() {
    if (this->checkForTokenType(TokenType::ampersand)) {
        return this->parseAddrExpr();
    }

    this->ptrUsedInExpr = false;

    return this->parseLogicalOrExpr();
}

void Parser::checkPointerUsage(NodeExprP expr) {
    if (this->ptrUsedInExpr) {
        delete expr;
        this->throwSemanticError("Invalid operation on pointers");
    }
}