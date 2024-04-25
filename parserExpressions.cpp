//
// Created by idang on 16/03/2024.
//

#include "parser.h"


NodeExpr *Parser::FactorByIdentifier(const Token &ident, bool ptrNotAllowed) {
    if (checkForTokenType(TokenType::openParenthesis)) {
        return parseFunctionCall(ident, false, ptrNotAllowed);
    }

    Variable var = this->getVarScopeStack(ident.val);

    if (!var.ptrType && var.arrSize == 0) {
        if (this->checkForTokenType(TokenType::openSquare)) {
            throw SemanticAnalysisException("'" + var.name + "' is not subscriptable");
        }

        return new NodeVariableTerminal(var);
    }

    if (!this->checkForTokenType(TokenType::openSquare)) {
        if (ptrNotAllowed) {
            throw SemanticAnalysisException("Illegal use of '" + var.name + "'");
        }

        this->ptrUsedInExpr = true;

        return new AddrNodeExpr(new NodeVariableTerminal(var));
    }

    return new NodeSubscriptableVariableTerminal(var, parseArrayBrackets());
}

NodeExpr *Parser::FactorByMultToken() {
    identifierTokenExists();
    Variable var = this->getVarScopeStack(this->lexer->currentAndProceedToken().val);

    if (var.arrSize == 0 && !var.ptrType) {
        throw SemanticAnalysisException("'" + var.name + "' cannot be dereferenced");
    }

    return new NodeSubscriptableVariableTerminal(var,
                                                 new NodeImIntTerminal(Token(
                                                         TokenType::immediateInteger, "0")));
}

NodeExpr *Parser::FactorByOpenParenthesis() {
    NodeExprP innerExpr = this->parseExpr();

    this->checkPointerUsage(innerExpr);

    if (!checkForTokenTypeAndConsume(TokenType::closeParenthesis)) {
        throw SyntaxAnalysisException("')' expected");
    }

    return new NodeParenthesisExpr(innerExpr);
}

NodeExpr *Parser::parseFactor(bool ptrNotAllowed) {
    if (!this->lexer->hasNextToken()) {
        throw SyntaxAnalysisException("Expression expected");
    }

    Token currentToken = this->lexer->currentToken();

    if (this->checkForTokenTypeAndConsumeIfYes(TokenType::immediateInteger)) {
        return new NodeImIntTerminal(currentToken);
    } else if (this->checkForTokenTypeAndConsumeIfYes(TokenType::identifier)) {
        return FactorByIdentifier(currentToken, ptrNotAllowed);
    } else if (this->checkForTokenTypeAndConsumeIfYes(TokenType::mult)) {
        return FactorByMultToken();
    } else if (this->checkForTokenTypeAndConsumeIfYes(TokenType::minus)) {
        return new NodeNumericNegExpr(this->parseFactor(true));
    } else if (this->checkForTokenTypeAndConsumeIfYes(TokenType::exclamation)) {
        return new NodeLogicalNotExpr(this->parseFactor(true));
    } else if (this->checkForTokenTypeAndConsumeIfYes(TokenType::openParenthesis)) {
        return FactorByOpenParenthesis();
    }

    throw SyntaxAnalysisException("Expression expected");
}

NodeExpr *Parser::parseTerm(NodeExprP leftSibling, TokenType siblingOpType) {
    if (!this->lexer->hasNextToken()) {
        throw SyntaxAnalysisException("Expression expected");
    }

    NodeExprP leftTerm = this->parseFactor();

    if (leftSibling != nullptr) {
        checkPointerUsage(leftTerm);

        if (siblingOpType == TokenType::mult) {
            leftTerm = new NodeMultExpr(leftSibling, leftTerm);
        } else if (siblingOpType == TokenType::div) {
            leftTerm = new NodeDivExpr(leftSibling, leftTerm);
        } else {
            leftTerm = new NodeModuloExpr(leftSibling, leftTerm);
        }
    }

    if (!this->lexer->hasNextToken() ||
        (this->lexer->currentToken().type != TokenType::mult &&
         this->lexer->currentToken().type != TokenType::modulo &&
         this->lexer->currentToken().type != TokenType::div)) {
        return leftTerm;
    }

    checkPointerUsage(leftTerm);

    Token op = this->lexer->currentAndProceedToken();

    return parseTerm(leftTerm, op.type);
}

NodeExpr *Parser::parseNumericExpr(NodeExprP leftSibling, TokenType siblingOpType) {
    if (!this->lexer->hasNextToken()) {
        throw SyntaxAnalysisException("Expression expected");
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
        throw SyntaxAnalysisException("Expression expected");
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
        throw SyntaxAnalysisException("Expression expected");
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
        throw SyntaxAnalysisException("Expression expected");
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
        throw SyntaxAnalysisException("Expression expected");
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
    this->identifierTokenExists();

    NodeExprP addressable = this->parseFactor(true);

    auto varTerminal = dynamic_cast<NodeVariableTerminal *>(addressable);

    if (!varTerminal) {
        delete addressable;
        throw SemanticAnalysisException("Unaddressable expression");
    }

    this->ptrUsedInExpr = true;

    return new AddrNodeExpr(varTerminal);
}

NodeExpr *Parser::parseExpr() {
    if (this->checkForTokenTypeAndConsumeIfYes(TokenType::ampersand)) {
        return this->parseAddrExpr();
    }

    this->ptrUsedInExpr = false;

    return this->parseLogicalOrExpr();
}

void Parser::checkPointerUsage(NodeExprP expr) {
    if (this->ptrUsedInExpr) {
        delete expr;
        throw SemanticAnalysisException("Invalid use of pointers");
    }
}