//
// Created by idang on 13/12/2023.
//

#include "parser.h"

NodeExpr *Parser::parseArrayBrackets() {
    this->lexer->currentAndProceedToken(); // Remove the open square bracket

    NodeExprP indexExpr = this->parseExpr();

    if (!this->checkForTokenTypeAndConsume(TokenType::closeSquare)) {
        Parser::throwSyntaxError("']' expected");
    }

    return indexExpr;
}

/*NodeExpr *Parser::parseFactor() {
    if (!this->lexer->hasNextToken()) {
        *//*std::cout << "Syntax Error: Token expected" << std::endl;
        exit(1);*//*
        Parser::throwSyntaxError("Number, identifier or parenthesis expected");
    }

    Token currentToken = this->lexer->currentAndProceedToken();

    if (currentToken.type != TokenType::immediateInteger &&
        currentToken.type != TokenType::identifier &&
        currentToken.type != TokenType::openParenthesis) {
        *//*std::cout << "Syntax Error: Number, identifier or parenthesis expected" << std::endl;
        exit(1);*//*
        Parser::throwSyntaxError("Number, identifier or parenthesis expected");
    }

    if (currentToken.type == TokenType::immediateInteger) {
        return new NodeImIntTerminal(currentToken);
    } else if (currentToken.type == TokenType::identifier) {
//        if (!this->variables.contains(currentToken.val)) {
       *//* if (!varExistsScopeStack(currentToken.val)) {
            std::cout << "Compile Error: Use of undeclared variable " << currentToken.val << std::endl;
            exit(1);
        }*//*
       Variable var = getVarScopeStack(currentToken.val);

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
}*/

std::tuple<NodeStmt *, bool> Parser::tryParseStmt() {
    if (!this->lexer->hasNextToken()) {
        return {nullptr, false};
    }

    NodeStmt *stmt = nullptr;
    bool tryAgain = true;

    Token firstToken = this->lexer->currentToken();

    if (firstToken.type == TokenType::identifier) {
        stmt = this->stmtByIdentifier(this->lexer->currentAndProceedToken());
    } else if (firstToken.type == TokenType::intKeyword) {
        stmt = stmtVariableDeclaration(VariableType::intType);
    } else if (firstToken.type == TokenType::charKeyword) {
        stmt = stmtVariableDeclaration(VariableType::charType);
    } else if (firstToken.type == TokenType::semiColon) {
        // Ignore
    } else {
        tryAgain = false;
    }

    this->stmtDelimiterTokenExists();

    return {stmt, tryAgain};
}

NodeScope *Parser::parseScope() {
    auto scope = new NodeScope();

    if (!checkForTokenTypeAndConsume(TokenType::openCurly)) {
        Parser::throwSyntaxError("'{' expected");
    }

    this->scopes.push(scope);

    std::tuple<NodeStmt *, bool> stmtTuple = this->tryParseStmt();

    while (get<0>(stmtTuple) || get<1>(stmtTuple)) {
        if (get<0>(stmtTuple)){
            scope->stmts.push_back(get<0>(stmtTuple));
        }
        stmtTuple = this->tryParseStmt();
    }

    this->scopes.pop();

    if (!checkForTokenTypeAndConsume(TokenType::closeCurly)) {
        Parser::throwSyntaxError("'}' expected");
    }

    return scope;
}

void Parser::throwError(const std::string &errorMsg) {
    std::cout << "Parser Error: " << errorMsg << std::endl;
    exit(1);
}

void Parser::throwSyntaxError(const std::string &errorMsg) {
    std::cout << "Parser Error [Syntax Error]: " << errorMsg << std::endl;
    exit(1);
}

void Parser::throwSemanticError(const std::string &errorMsg) {
    std::cout << "Parser Error [Semantic Error]: " << errorMsg << std::endl;
    exit(1);
}

bool Parser::checkForTokenType(TokenType type) {
    return this->lexer->hasNextToken() && this->lexer->currentToken().type == type;
}

bool Parser::checkForTokenTypeAndConsume(TokenType type) {
    return this->lexer->hasNextToken() && this->lexer->currentAndProceedToken().type == type;
}

void Parser::stmtDelimiterTokenExists() {
    if(!checkForTokenTypeAndConsume(TokenType::semiColon)) {
        Parser::throwSyntaxError("';' expected");
    };
}

void Parser::identifierTokenExists() {
    if (!checkForTokenType(TokenType::identifier)) {
        Parser::throwSyntaxError("Identifier expected");
    }
}

std::optional<Variable> Parser::varExistsScopeStack(const std::string &varName) {
    std::stack<NodeScopeP> scopesCopy = this->scopes;
    while (!scopesCopy.empty()) {
        /*auto it = std::find_if(scopesCopy.top()->vars.begin(), scopesCopy.top()->vars.end(),
                               [&](const Variable &v) { return v.name == varName; });

        if (it != scopesCopy.top()->vars.end()) {
            return scopesCopy.top()->vars[it - scopesCopy.top()->vars.begin()];
        }*/
        for (auto var : scopesCopy.top()->vars) {
            if (var.name == varName) return var;
        }

        scopesCopy.pop();
    }

    return {};
}

std::optional<Variable> Parser::varExistsCurrentScope(const std::string &varName) {
//    return this->scopes.top()->vars.contains(var);
    /*auto it = std::find_if(this->scopes.top()->vars.begin(), this->scopes.top()->vars.end(),
                           [&](const Variable &v) { return v.name == varName; });

    if (it != this->scopes.top()->vars.end()) {
        return this->scopes.top()->vars[it - this->scopes.top()->vars.begin()];
    }*/
    for (auto var : this->scopes.top()->vars) {
        if (var.name == varName) return var;
    }

    return {};
}

void Parser::addVarToCurrentScope(const Variable &var) {
//    this->scopes.top()->vars.insert(var);
    this->scopes.top()->vars.push_back(var);
}

Variable Parser::getVarCurrentScope(const std::string &varName) {
    std::optional<Variable> var = varExistsCurrentScope(varName);

    if (var.has_value()) return var.value();

    Parser::throwSemanticError("Use of undeclared identifier " + varName);
}

Variable Parser::getVarScopeStack(const std::string &varName) {
    std::optional<Variable> var = varExistsScopeStack(varName);

    if (var.has_value()) return var.value();

    Parser::throwSemanticError("Use of undeclared identifier " + varName);
}
