//
// Created by idang on 15/03/2024.
//

#include "parser.h"

NodeStmt *Parser::stmtPrimitiveAssignment(const Variable &var) {
    this->lexer->currentAndProceedToken(); // Remove the 'equals' lexeme

    if (var.arrSize > 0) {
        this->throwSemanticError("'" + var.name + "' of type array is constant");
    }

    NodeExpr *innerExpr = this->parseExpr();

    auto *ptr = dynamic_cast<AddrNodeExpr *>(innerExpr);

    if (!var.ptrType) {
        if (ptr) {
            delete innerExpr;

            this->throwSemanticError("Invalid assignment to identifier '" + var.name + "'");
        }
        return new NodePrimitiveAssignmentStmt(var, innerExpr);
    }

    if (!ptr) this->throwSemanticError("Invalid assignment to identifier '" + var.name + "'");

    Variable targetVar = ptr->target;

    delete innerExpr;

    return new NodePointerAddrAssignmentStmt(var, targetVar);
}

NodeStmt *Parser::stmtArrayAssignment(const Variable &var) {
    /*this->lexer->currentAndProceedToken(); // Remove the open square bracket lexeme

    NodeExprP indexExpr = this->parseExpr();

    if (!this->checkForTokenTypeAndConsume(TokenType::closeSquare)) {
        this->throwError("[Syntax Error] ']' expected");
    }*/
    if (var.arrSize == 0 && !var.ptrType) {
        this->throwSemanticError("'" + var.name + "' is not subscriptable");
    }

    NodeExprP indexExpr = this->parseArrayBrackets();

    if (checkForTokenType(TokenType::equal)) {
        this->lexer->currentAndProceedToken();
        return new NodeArrayAssignmentStmt(var, indexExpr, this->parseExpr());
    }

    delete indexExpr;

    return nullptr;
}

NodeStmt *Parser::stmtByIdentifier(const Token &ident) {
    if (checkForTokenType(TokenType::openParenthesis)) {
        NodeFunctionP func = getFunction(ident.val);
        if (!func) {
            this->throwSemanticError("Use of undeclared function '" + ident.val + "'");
        }

        std::vector<NodeExprP> params = std::get<std::vector<NodeExprP>>(parseParenthesisExprList(false));

        if (params.size() != func->params.size()) {
            for (auto &expr: params) {
                delete expr;
            }

            this->throwSemanticError(
                    "Function '" + ident.val + "' expected " + std::to_string(func->params.size()) + " parameters");
        }

        for (int i = 0; i < params.size(); ++i) {
            auto exprAddr = dynamic_cast<AddrNodeExpr *>(params[i]);

            if (func->params[i].ptrType && !exprAddr ||
                !func->params[i].ptrType && exprAddr ||
                (exprAddr && func->params[i].type != exprAddr->target.type)) {
                for (auto &expr: params) {
                    delete expr;
                }

                this->throwSemanticError("Function call with incompatible type");
            }
        }

        return new NodeFunctionCall(ident.val, params);
    }

    Variable var = this->getVarScopeStack(ident.val);

    if (checkForTokenType(TokenType::equal)) {
        return this->stmtPrimitiveAssignment(var);
    } else if (checkForTokenType(TokenType::openSquare)) {
        return this->stmtArrayAssignment(var);
    }

    return nullptr;
}

NodeStmt *Parser::stmtVariableDeclaration(VariableType type) {
    this->lexer->currentAndProceedToken(); // Remove the 'type' lexeme

    bool ptr = checkForTokenType(TokenType::mult);
    if (ptr) this->lexer->currentAndProceedToken();

    identifierTokenExists();
    std::string varName = this->lexer->currentAndProceedToken().val;

    if (varExistsCurrentScope(varName)) {
        this->throwSemanticError("Redeclaration of identifier " + varName);
    }


    if (checkForTokenType(TokenType::openSquare)) {
        if (ptr) this->throwSemanticError("Arrays can only be of primitive types");

        auto sizeExpr = dynamic_cast<NodeImIntTerminalP>(parseArrayBrackets());

        if (!sizeExpr) {
            this->throwSemanticError("Array size must be known at compile time");
        }

        this->addVarToCurrentScope(Variable(varName, type, ptr, std::stoi(sizeExpr->value)));

        return nullptr;
    }

    Variable var(varName, type, ptr);

    NodeStmtP stmt = nullptr;

    if (checkForTokenType(TokenType::equal)) {
        stmt = this->stmtPrimitiveAssignment(var);
    }

    this->addVarToCurrentScope(var);

    return stmt;
}

NodeStmt *Parser::stmtIf() {
    this->lexer->currentAndProceedToken(); // Remove the 'if' lexeme

    NodeExprP expr = parseParenthesisExpr();
    NodeScopeP ifBlock = parseScope();
    NodeScopeP elseBlock = nullptr;

    if (this->checkForTokenType(TokenType::elseKeyword)) {
        this->lexer->currentAndProceedToken(); // Remove the 'else' lexeme

        elseBlock = parseScope();
    }

    return new NodeIf(expr, ifBlock, elseBlock);
}

NodeStmt *Parser::stmtWhile(bool isDo) {
    this->lexer->currentAndProceedToken(); // Remove the 'while' or 'do' lexeme

    NodeExprP expr = nullptr;

    if (!isDo) {
        expr = parseParenthesisExpr();
    }

    NodeScopeP codeBlock = parseScope();

    if (isDo) {
        expr = parseParenthesisExpr();
    }

    return new NodeWhile(expr, codeBlock, isDo);
}