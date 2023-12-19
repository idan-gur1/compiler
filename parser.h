// parser.h

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include "treeNodes.h"
#include "lexer.h"

class Parser {
public:
    explicit Parser(Lexer *lexer) {
        this->lexer = lexer;
    }

    NodeExpr *parseExpr(NodeExprP leftSibling = nullptr,
                        TokenType siblingOpType = TokenType::NO_TOKEN);
    NodeTerm *parseTerm();
    NodeFactor *parseFactor();

private:
    Lexer *lexer;
};

#endif //COMPILER_PARSER_H
