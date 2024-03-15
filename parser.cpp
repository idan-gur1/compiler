//
// Created by idang on 13/12/2023.
//

#include "parser.h"

std::optional<Variable> Parser::varExistsScopeStack(const std::string &varName) {
    std::stack<NodeScopeP> scopesCopy = this->scopes;
    while (!scopesCopy.empty()) {
        auto it = std::find_if(scopesCopy.top()->vars.begin(), scopesCopy.top()->vars.end(),
                               [&](const Variable &v) { return v.name == varName; });

        if (it != scopesCopy.top()->vars.end()) {
            return scopesCopy.top()->vars[it - scopesCopy.top()->vars.begin()];
        }

        scopesCopy.pop();
    }

    return {};
}

std::optional<Variable> Parser::varExistsCurrentScope(const std::string &varName) {
//    return this->scopes.top()->vars.contains(var);
    auto it = std::find_if(this->scopes.top()->vars.begin(), this->scopes.top()->vars.end(),
                           [&](const Variable &v) { return v.name == varName; });

    if (it != this->scopes.top()->vars.end()) {
        return this->scopes.top()->vars[it - this->scopes.top()->vars.begin()];
    }
    return {};
}

void Parser::addVarToCurrentScope(const Variable &var) {
//    this->scopes.top()->vars.insert(var);
    this->scopes.top()->vars.push_back(var);
}

bool Parser::stmtDelimiterExists() {
    return this->lexer->hasNextToken() && this->lexer->currentAndProceedToken().type == TokenType::semiColon;
}

NodeExpr *Parser::parseFactor() {
    if (!this->lexer->hasNextToken()) {
        std::cout << "Syntax Error: Token expected" << std::endl;
        exit(1);
    }

    Token currentToken = this->lexer->currentAndProceedToken();

    if (currentToken.type != TokenType::immediateInteger &&
        currentToken.type != TokenType::identifier &&
        currentToken.type != TokenType::openParenthesis) {
        std::cout << "Syntax Error: Number, identifier or parenthesis expected" << std::endl;
        exit(1);
    }

    if (currentToken.type == TokenType::immediateInteger) {
        return new NodeImIntTerminal(currentToken);
    } else if (currentToken.type == TokenType::identifier) {
//        if (!this->variables.contains(currentToken.val)) {
        if (!varExistsScopeStack(currentToken.val)) {
            std::cout << "Compile Error: Use of undeclared variable " << currentToken.val << std::endl;
            exit(1);
        }
        return new NodeIdentTerminal(currentToken);
    }

    NodeExprP innerExpr = parseExpr();

    if (!this->lexer->hasNextToken() ||
        this->lexer->currentAndProceedToken().type != TokenType::closeParenthesis) {
        std::cout << "Syntax Error: ) expected" << std::endl;
        exit(1);
    }

    return new ParenthesisNodeExpr(innerExpr);
}

NodeExpr *Parser::parseTerm(NodeExprP leftSibling, TokenType siblingOpType) {
    if (!this->lexer->hasNextToken()) {
        std::cout << "Syntax Error: Token expected" << std::endl;
        exit(1);
    }

    NodeExprP currentTerm = parseFactor();

    if (!this->lexer->hasNextToken() ||
        (this->lexer->currentToken().type != TokenType::mult &&
         this->lexer->currentToken().type != TokenType::div)) {
        if (leftSibling == nullptr) {
            return currentTerm;
        }
        if (siblingOpType == TokenType::mult) {
            return new NodeMultExpr(leftSibling, currentTerm);
        }

        return new NodeDivExpr(leftSibling, currentTerm);
    }

    Token op = this->lexer->currentAndProceedToken();

    NodeExprP leftTerm = currentTerm;

    if (leftSibling != nullptr) {
        if (siblingOpType == TokenType::mult) {
            leftTerm = new NodeMultExpr(leftSibling, currentTerm);
        } else {
            leftTerm = new NodeDivExpr(leftSibling, currentTerm);
        }
    }

    return parseTerm(leftTerm, op.type);
}

NodeExpr *Parser::parseExpr(NodeExprP leftSibling, TokenType siblingOpType) {
    if (!this->lexer->hasNextToken()) {
        std::cout << "Syntax Error: Token expected" << std::endl;
        exit(1);
    }

    NodeExprP currentExpr = parseTerm();

    if (!this->lexer->hasNextToken() ||
        (this->lexer->currentToken().type != TokenType::plus &&
         this->lexer->currentToken().type != TokenType::minus)) {
        if (leftSibling == nullptr) {
            return currentExpr;
        }
        if (siblingOpType == TokenType::plus) {
            return new NodeAddExpr(leftSibling, currentExpr);
        }

        return new NodeSubExpr(leftSibling, currentExpr);
    }

    Token op = this->lexer->currentAndProceedToken();

    NodeExprP leftExpr = currentExpr;

    if (leftSibling != nullptr) {
        if (siblingOpType == TokenType::plus) {
            leftExpr = new NodeAddExpr(leftSibling, currentExpr);
        } else {
            leftExpr = new NodeSubExpr(leftSibling, currentExpr);
        }
    }

    return parseExpr(leftExpr, op.type);
}

std::tuple<NodeStmt *, bool> Parser::tryParseStmt() {
    if (!this->lexer->hasNextToken()) {
        return {nullptr, false};
    }

    NodeStmt *stmt = nullptr;

    Token firstToken = this->lexer->currentToken();

    if (firstToken.type == TokenType::identifier) {
        /*this->lexer->currentAndProceedToken();

        std::optional<Variable> var = this->varExistsScopeStack(firstToken.val);

        if (!var.has_value()) {
            Parser::throwError("Use of undeclared identifier " + firstToken.val);
        }

        if (this->lexer->hasNextToken() && this->lexer->currentToken().type == TokenType::equal) {
            this->lexer->currentAndProceedToken();
            if (this->lexer->hasNextToken() && this->lexer->currentToken().type == TokenType::ampersand) {
                this->lexer->currentAndProceedToken();
                if (!this->lexer->hasNextToken() || this->lexer->currentToken().type != TokenType::identifier) {
                    Parser::throwError("[Syntax Error] ']' identifier expected");
                }
                std::optional<Variable> targetVar = this->varExistsScopeStack(this->lexer->currentAndProceedToken().val);

                if (!targetVar.has_value()) {
                    Parser::throwError("Use of undeclared identifier " + targetVar->name);
                }
                stmt = new NodePointerAddrAssignmentStmt(var.value(), targetVar.value());
            } else {
                stmt = new NodePrimitiveAssignmentStmt(var.value(), this->parseExpr());
            }
            stmt = this->stmtPrimitiveAssignment(var.value());
        } else if (this->lexer->hasNextToken() && this->lexer->currentToken().type == TokenType::openSquare) {
            this->lexer->currentAndProceedToken();

            NodeExprP indexExpr = this->parseExpr();

            if (!this->lexer->hasNextToken() || this->lexer->currentAndProceedToken().type != TokenType::closeSquare) {
                Parser::throwError("[Syntax Error] ']' expected");
            }

            if (this->lexer->hasNextToken() && this->lexer->currentToken().type == TokenType::equal) {
                this->lexer->currentAndProceedToken();
                stmt = new NodeArrayAssignmentStmt(var.value(), indexExpr, this->parseExpr());
            } else {
                delete indexExpr;
            }
            stmt = this->stmtArrayAssignment(var.value());
        }*/
        stmt = this->stmtByIdentifier(this->lexer->currentAndProceedToken());

    } else if (firstToken.type == TokenType::intKeyword) {
        stmt = stmtVariableDeclaration(VariableType::intType);
        if (stmt == nullptr) {
            return {nullptr, true};
        }
    }

    if (!this->stmtDelimiterExists()) {
        Parser::throwError("[Syntax Error] ';' expected");
    }

    return {stmt, false};
}

NodeScope *Parser::parseScope() {
    auto scope = new NodeScope();

    if (!this->lexer->hasNextToken() ||
        this->lexer->currentAndProceedToken().type != TokenType::openCurly) {
        std::cout << "Syntax Error: { expected" << std::endl;
        exit(1);
    }

//    while (this->lexer->hasNextToken()) {
//        scope->stmts.push_back(tryParseStmt());
//    }
    this->scopes.push(scope);

    /*while (auto stmt = tryParseStmt()) {
        scope->stmts.push_back(stmt);
    }*/
    std::tuple<NodeStmt *, bool> stmtTuple = this->tryParseStmt();

    while (get<0>(stmtTuple) || get<1>(stmtTuple)) {
        scope->stmts.push_back(get<0>(stmtTuple));
        stmtTuple = this->tryParseStmt();
    }

    this->scopes.pop();

    if (!this->lexer->hasNextToken() ||
        this->lexer->currentAndProceedToken().type != TokenType::closeCurly) {
        std::cout << "Syntax Error: } expected" << std::endl;
        exit(1);
    }

    return scope;
}

void Parser::throwError(const std::string &errorMsg) {
    std::cout << "Parser Error: " << errorMsg << std::endl;
    exit(1);
}

