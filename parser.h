// parser.h

#ifndef COMPILER_PARSER_H
#define COMPILER_PARSER_H

#include <unordered_set>
#include <stack>
#include <optional>
#include <variant>
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

    ~Parser() {

    }

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
    NodeExpr *parseFactor();
    NodeExpr *parseArrayBrackets();
    NodeExpr *parseParenthesisExpr();
    std::variant<std::vector<NodeExpr *>, std::vector<Variable>> parseParenthesisExprList(bool vars);

    std::optional<Variable> varExistsCurrentScope(const std::string&);
    std::optional<Variable> varExistsScopeStack(const std::string&);
    Variable getVarCurrentScope(const std::string&);
    Variable getVarScopeStack(const std::string&);
    void addVarToCurrentScope(const Variable&);
    NodeFunction *getFunction(const std::string&);

    bool checkForTokenType(TokenType type);
    bool checkForTokenTypeAndConsume(TokenType type);
    void stmtDelimiterTokenExists();
    void identifierTokenExists();

    void throwSyntaxError(const std::string& errorMsg);
    void throwSemanticError(const std::string& errorMsg);

    NodeStmt *stmtByIdentifier(const Token& ident);
    NodeStmt *stmtPrimitiveAssignment(const Variable& var);
    NodeStmt *stmtArrayAssignment(const Variable& var);

    NodeStmt *stmtVariableDeclaration(VariableType type);

    NodeStmt *stmtIf();
    NodeStmt *stmtWhile(bool isDo = false);

    Lexer *lexer;
//    std::unordered_set<std::string> variables;
    std::stack<NodeScopeP> scopes;
    ProgramTreeP programTree;

    std::unordered_map<TokenType, VariableType> typeMap;
};

#endif //COMPILER_PARSER_H
