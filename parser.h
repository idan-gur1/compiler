// parser.h

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include <unordered_set>
#include <stack>
#include <optional>
#include "treeNodes.h"
#include "lexer.h"

class Parser {
public:
    explicit Parser(Lexer *lexer) {
        this->lexer = lexer;
    }
    NodeScope *parseScope();
    std::tuple<NodeStmt *, bool> tryParseStmt();
    NodeExpr *parseExpr(NodeExprP leftSibling = nullptr,
                        TokenType siblingOpType = TokenType::NO_TOKEN);
    NodeExpr *parseTerm(NodeExprP leftSibling = nullptr,
                        TokenType siblingOpType = TokenType::NO_TOKEN);
    NodeExpr *parseFactor();

private:
//    bool varExistsCurrentScope(const std::string&);
    std::optional<Variable> varExistsCurrentScope(const std::string&);
    std::optional<Variable> varExistsScopeStack(const std::string&);
    void addVarToCurrentScope(const Variable&);
    bool stmtDelimiterExists();
    static void throwError(const std::string& errorMsg);

    NodeStmt *stmtByIdentifier(const Token& ident);
    NodeStmt *stmtPrimitiveAssignment(Variable var);
    NodeStmt *stmtArrayAssignment(Variable var);

    NodeStmt *stmtVariableDeclaration(VariableType type);

    Lexer *lexer;
//    std::unordered_set<std::string> variables;
    std::stack<NodeScopeP> scopes;
};

#endif //COMPILER_PARSER_H
