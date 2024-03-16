// parser.h

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include <unordered_set>
#include <stack>
#include <optional>
#include <variant>
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

private:
//    bool varExistsCurrentScope(const std::string&);
    NodeExpr *parseTerm(NodeExprP leftSibling = nullptr,
                        TokenType siblingOpType = TokenType::NO_TOKEN);
    NodeExpr *parseFactor();
    NodeExpr *parseArrayBrackets();

    std::optional<Variable> varExistsCurrentScope(const std::string&);
    std::optional<Variable> varExistsScopeStack(const std::string&);
    Variable getVarCurrentScope(const std::string&);
    Variable getVarScopeStack(const std::string&);
    void addVarToCurrentScope(const Variable&);

    bool checkForTokenType(TokenType type);
    bool checkForTokenTypeAndConsume(TokenType type);
    void stmtDelimiterTokenExists();
    void identifierTokenExists();

    static void throwError(const std::string& errorMsg);
    static void throwSyntaxError(const std::string& errorMsg);
    static void throwSemanticError(const std::string& errorMsg);

    NodeStmt *stmtByIdentifier(const Token& ident);
    NodeStmt *stmtPrimitiveAssignment(const Variable& var);
    NodeStmt *stmtArrayAssignment(const Variable& var);

    NodeStmt *stmtVariableDeclaration(VariableType type);

    Lexer *lexer;
//    std::unordered_set<std::string> variables;
    std::stack<NodeScopeP> scopes;
};

#endif //COMPILER_PARSER_H
