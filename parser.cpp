//
// Created by idang on 13/12/2023.
//

#include "parser.h"

NodeFactor *Parser::parseFactor() {
    return nullptr;
}

NodeTerm *Parser::parseTerm() {
    return nullptr;
}

NodeExpr *Parser::parseExpr(NodeExprP leftSibling, TokenType siblingOpType) {
    if (!this->lexer->hasNextToken()) {
        std::cout << "Syntax Error: Token expected" << std::endl;
        exit(1);
    }

    NodeTermP current = parseTerm(); // need to be
    Token cur = this->lexer->currentAndProceedToken();
    current = new NodeFactorTerm(new NodeImIntFactor(cur));

    auto currentExpr = new NodeTermExpr(current);

    if (!this->lexer->hasNextToken() ||
    this->lexer->currentToken().type == TokenType::closeParenthesis) {
        if (leftSibling == nullptr) {
            return currentExpr;
        }
        if (siblingOpType == TokenType::plus) {
            return new NodeAddExpr(nullptr, nullptr);
        }

        return new NodeSubExpr(nullptr, nullptr);
    }

    Token op = this->lexer->currentAndProceedToken();
    //std::cout << "Token type:" << getTokenName(op.type) << "Value:" << op.val << std::endl;

    if (op.type != TokenType::plus && op.type != TokenType::minus) {
        std::cout << "Syntax Error: plus or minus expected. ";
        std::cout << "Received: " << getTokenName(op.type) << std::endl;
        exit(1);
    }

    NodeExprP leftExpr = currentExpr;

    if (leftSibling != nullptr) {
        if (siblingOpType == TokenType::plus) {
            leftExpr = new NodeAddExpr(nullptr, nullptr);
        } else {
            leftExpr = new NodeSubExpr(nullptr, nullptr);
        }
    }

    return parseExpr(leftExpr, op.type);

    /*NodeExprP rightExpr;
    NodeExprP leftExpr = nullptr;

    if (leftSibling != nullptr) {
        if (siblingOpType == TokenType::plus) {
            leftExpr = new NodeAddExpr(leftSibling, currentExpr);
        } else {
            leftExpr = new NodeSubExpr(leftSibling, currentExpr);
            std::cout << "test" << std::endl;
        }
        rightExpr = parseExpr(leftExpr);
        std::cout << "test1" << std::endl;
    } else {
        rightExpr = parseExpr(currentExpr);
    }

    if (leftExpr == nullptr) {
        return rightExpr;
    }

    if (op.type == TokenType::plus) {
        return new NodeAddExpr(leftSibling, rightExpr);
    }

    return new NodeSubExpr(leftSibling, rightExpr);*/
}
