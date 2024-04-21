// parser.h

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include <unordered_set>
#include <stack>
#include <optional>
#include <variant>
#include <vector>
#include <unordered_map>
#include "treeNodes.h"
#include "lexer.h"

class Parser {
public:
    explicit Parser(Lexer *lexer) {
        this->lexer = lexer;

        this->typeMap[TokenType::intKeyword] = VariableType::intType;
        this->typeMap[TokenType::charKeyword] = VariableType::charType;
        this->typeMap[TokenType::voidKeyword] = VariableType::voidType;
    }

    ~Parser() = default;

    ProgramTree *parseProgram();
    NodeFunction *tryParseFunction();
    NodeScope *parseScope();
    std::tuple<NodeStmt *, bool> tryParseStmt();
    NodeExpr *parseExpr();

private:
    NodeExpr *parseAddrExpr();
    NodeExpr *parseLogicalOrExpr(NodeExprP leftSibling = nullptr);
    NodeExpr *parseLogicalAndExpr(NodeExprP leftSibling = nullptr);
    NodeExpr *parseEqualityExpr(NodeExprP leftSibling = nullptr,
                        TokenType siblingOpType = TokenType::NO_TOKEN);
    NodeExpr *parseRelationalExpr(NodeExprP leftSibling = nullptr,
                        TokenType siblingOpType = TokenType::NO_TOKEN);
    NodeExpr *parseNumericExpr(NodeExprP leftSibling = nullptr,
                        TokenType siblingOpType = TokenType::NO_TOKEN);
    NodeExpr *parseTerm(NodeExprP leftSibling = nullptr,
                        TokenType siblingOpType = TokenType::NO_TOKEN);
    NodeExpr *parseFactor(bool ptrNotAllowed = false);
    NodeExpr *parseArrayBrackets();
    NodeExpr *parseParenthesisExpr();
    std::vector<Variable> parseParenthesisVariableList();
    std::vector<NodeExpr *> parseParenthesisExprList();
    void checkPointerUsage(NodeExprP expr);

    std::optional<Variable> varExistsCurrentScope(const std::string&);
    std::optional<Variable> varExistsScopeStack(const std::string&);
    Variable getVarCurrentScope(const std::string&);
    Variable getVarScopeStack(const std::string&);
    void addVarToCurrentScope(const Variable&);
    NodeFunction *getFunction(const std::string&);

    bool checkForTokenType(TokenType type);
    bool checkForTokenTypeAndConsume(TokenType type);
    bool checkForTokenTypeAndConsumeIfYes(TokenType type);
    void identifierTokenExists();
    void validateFunctionCallParams(std::vector<NodeExprP> params, NodeFunctionP func);

    void throwSyntaxError(const std::string& errorMsg);
    void throwSemanticError(const std::string& errorMsg);

    NodeStmt *stmtByIdentifier(const Token& ident);
    NodeStmt *stmtPrimitiveAssignment(const Variable& var);
    NodeStmt *stmtArrayAssignment(const Variable& var);

    NodeStmt *stmtVariableDeclaration(VariableType type);

    NodeStmt *stmtIf();
    NodeStmt *stmtWhile(bool isDo = false);

    Lexer *lexer;
    std::stack<NodeScopeP> scopes;
    ProgramTreeP programTree = nullptr;

    std::unordered_map<TokenType, VariableType> typeMap;

    bool ptrUsedInExpr = false;
    bool mainFunctionExists = false;
};

#endif //COMPILER_PARSER_H
