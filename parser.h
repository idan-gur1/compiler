// parser.h

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include <unordered_set>
#include <stack>
#include "treeNodes.h"
#include "lexer.h"

class Parser {
public:
    explicit Parser(Lexer *lexer) {
        this->lexer = lexer;
    }
    NodeScope *parseScope();
    NodeStmt *tryParseStmt();
    NodeExpr *parseExpr(NodeExprP leftSibling = nullptr,
                        TokenType siblingOpType = TokenType::NO_TOKEN);
    NodeExpr *parseTerm(NodeExprP leftSibling = nullptr,
                        TokenType siblingOpType = TokenType::NO_TOKEN);
    NodeExpr *parseFactor();

private:
    bool varExists(const std::string&);

    Lexer *lexer;
//    std::unordered_set<std::string> variables;
    std::stack<NodeScopeP> scopes;
};

#endif //COMPILER_PARSER_H
