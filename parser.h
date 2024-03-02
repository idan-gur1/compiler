// parser.h

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include <unordered_set>
#include "treeNodes.h"
#include "lexer.h"

class Parser {
public:
    explicit Parser(Lexer *lexer) {
        this->lexer = lexer;
    }
    NodeScope *parseScope();
    NodeAssignmentStmt *tryParseStmt();
    NodeExpr *parseExpr(NodeExprP leftSibling = nullptr,
                        TokenType siblingOpType = TokenType::NO_TOKEN);
    NodeExpr *parseTerm(NodeExprP leftSibling = nullptr,
                        TokenType siblingOpType = TokenType::NO_TOKEN);
    NodeExpr *parseFactor();

private:
    Lexer *lexer;
    std::unordered_set<std::string> variables;
};

#endif //COMPILER_PARSER_H
