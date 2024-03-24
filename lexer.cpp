// lexer.cpp

#include "lexer.h"


//Lexer::Lexer(std::string src) : srcCode(std::move(src)) {
//
//}

char Lexer::current() {
    return srcCode.at(this->nCurIndex);
}

char Lexer::currentAndProceed() {
    return srcCode.at(this->nCurIndex++);
}

bool Lexer::hasBuffer() {
    return nCurIndex < srcCode.size();
}

std::vector<Token> Lexer::analyseSource() {
    // std::vector<Token> tokens;

    while (this->hasBuffer()) {
        char cCur = this->currentAndProceed();

        if (cCur == '+') {
            this->tokens.push_back(Token(TokenType::plus));
        } else if (cCur == '-') {
            this->tokens.push_back(Token(TokenType::minus));
        } else if (cCur == '*') {
            this->tokens.push_back(Token(TokenType::mult));
        } else if (cCur == '/') {
            this->tokens.push_back(Token(TokenType::div));
        } else if (cCur == '(') {
            this->tokens.push_back(Token(TokenType::openParenthesis));
        } else if (cCur == ')') {
            this->tokens.push_back(Token(TokenType::closeParenthesis));
        } else if (cCur == '{') {
            this->tokens.push_back(Token(TokenType::openCurly));
        } else if (cCur == '}') {
            this->tokens.push_back(Token(TokenType::closeCurly));
        } else if (cCur == '[') {
            this->tokens.push_back(Token(TokenType::openSquare));
        } else if (cCur == ']') {
            this->tokens.push_back(Token(TokenType::closeSquare));
        } else if (cCur == ',') {
            this->tokens.push_back(Token(TokenType::coma));
        } else if (cCur == '\'') {
            char innerVal = this->currentAndProceed();

            if (this->currentAndProceed() != '\'') {
                this->throwLexerError("Expected ' At the end of char declaration");
            }

            this->tokens.push_back(Token(TokenType::immediateInteger, std::to_string(innerVal)));
        } else if (cCur == '=') {
            if (this->hasBuffer() && this->current() == '=') {
                this->currentAndProceed();
                this->tokens.push_back(Token(TokenType::doubleEqual));
            } else {
                this->tokens.push_back(Token(TokenType::equal));
            }
        } else if (cCur == '&') {
            if (this->hasBuffer() && this->current() == '&') {
                this->currentAndProceed();
                this->tokens.push_back(Token(TokenType::logicalAnd));
            } else {
                this->tokens.push_back(Token(TokenType::ampersand));
            }
        } else if (cCur == '|') {
            if (this->hasBuffer() && this->current() == '|') {
                this->currentAndProceed();
                this->tokens.push_back(Token(TokenType::logicalOr));
            } else {
                this->tokens.push_back(Token(TokenType::pipe));
            }
        } else if (cCur == '!') {
            if (this->hasBuffer() && this->current() == '=') {
                this->currentAndProceed();
                this->tokens.push_back(Token(TokenType::notEqual));
            } else {
                this->tokens.push_back(Token(TokenType::exclamation));
            }
        } else if (cCur == '>') {
            if (this->hasBuffer() && this->current() == '=') {
                this->currentAndProceed();
                this->tokens.push_back(Token(TokenType::relationalGE));
            } else {
                this->tokens.push_back(Token(TokenType::relationalG));
            }
        } else if (cCur == '<') {
            if (this->hasBuffer() && this->current() == '=') {
                this->currentAndProceed();
                this->tokens.push_back(Token(TokenType::relationalLE));
            } else {
                this->tokens.push_back(Token(TokenType::relationalL));
            }
        } else if (std::isdigit(cCur)) {
            std::string buffer(1, cCur);

            while (this->hasBuffer() && std::isdigit(this->current())) {
                buffer.push_back(this->currentAndProceed());
            }
            if (this->hasBuffer() && std::isalpha(this->current())) {
                this->throwLexerError("Invalid character '" + std::string(1, cCur) + "'");
            }

            this->tokens.push_back(Token(TokenType::immediateInteger, buffer));
        } else if (std::isalpha(cCur)) {
            std::string buffer(1, cCur);

            while (this->hasBuffer() && std::isalnum(this->current())) {
                buffer.push_back(this->currentAndProceed());
            }

            if (buffer == "exit") {
                this->tokens.push_back(Token(TokenType::exit));
            } else if (buffer == "int") {
                this->tokens.push_back(Token(TokenType::intKeyword));
            } else if (buffer == "char") {
                this->tokens.push_back(Token(TokenType::charKeyword));
            } else if (buffer == "void") {
                this->tokens.push_back(Token(TokenType::voidKeyword));
            } else if (buffer == "if") {
                this->tokens.push_back(Token(TokenType::ifKeyword));
            } else if (buffer == "else") {
                this->tokens.push_back(Token(TokenType::elseKeyword));
            } else if (buffer == "while") {
                this->tokens.push_back(Token(TokenType::whileKeyword));
            } else if (buffer == "do") {
                this->tokens.push_back(Token(TokenType::doKeyword));
            } else if (buffer == "return") {
                this->tokens.push_back(Token(TokenType::returnKeyword));
            } else {
                this->tokens.push_back(Token(TokenType::identifier, buffer));
            }
        } else if (cCur == ';') {
            this->tokens.push_back(Token(TokenType::semiColon));
        } else if (cCur == '\n') {
            this->currentLine++;
            this->tokens.push_back(Token(TokenType::NEW_LINE));
        } else if (std::isspace(cCur)) {
            // ignore
        } else {
//            throw CompilationException("[Lexer] Unknown character/token " + std::string(1, cCur));
            this->throwLexerError("[Lexer] Unknown character/token " + std::string(1, cCur));
        }
    }

    this->currentLine = 1;

    this->countLines();

    return this->tokens;
}

bool Lexer::hasNextToken() {
    return nTokenIndex < tokens.size();
}

Token Lexer::currentToken() {
    return this->tokens[nTokenIndex];
}

Token Lexer::currentAndProceedToken() {
    Token ret =  this->tokens[nTokenIndex++];

    this->countLines();

    return ret;
}

void Lexer::countLines() {
    while (this->hasNextToken() && this->tokens[nTokenIndex].type == TokenType::NEW_LINE) {
        this->currentLine++;
        nTokenIndex++;
    }
}

void Lexer::throwLexerError(const std::string &errorMsg) const {
    throw CompilationException("[Lexer] " + errorMsg + " On line " + std::to_string(this->currentLine));
}

std::string getTokenName(TokenType tokenType) {
    if (tokenType == TokenType::plus) return "+";
    if (tokenType == TokenType::minus) return "-";
    if (tokenType == TokenType::mult) return "*";
    if (tokenType == TokenType::div) return "/";
    if (tokenType == TokenType::equal) return "=";
    if (tokenType == TokenType::openParenthesis) return "(";
    if (tokenType == TokenType::closeParenthesis) return ")";
    if (tokenType == TokenType::openCurly) return "{";
    if (tokenType == TokenType::closeCurly) return "}";
    if (tokenType == TokenType::immediateInteger) return "immediateInteger";
    if (tokenType == TokenType::identifier) return "identifier";
    if (tokenType == TokenType::intKeyword) return "int";
    if (tokenType == TokenType::charKeyword) return "char";
    if (tokenType == TokenType::voidKeyword) return "void";
    if (tokenType == TokenType::ifKeyword) return "if";
    if (tokenType == TokenType::elseKeyword) return "else";
    if (tokenType == TokenType::whileKeyword) return "while";
    if (tokenType == TokenType::doKeyword) return "do";
    if (tokenType == TokenType::exit) return "exit";
    if (tokenType == TokenType::semiColon) return ";";
    if (tokenType == TokenType::coma) return ",";
    if (tokenType == TokenType::openSquare) return "[";
    if (tokenType == TokenType::closeSquare) return "]";
    if (tokenType == TokenType::exclamation) return "!";
    if (tokenType == TokenType::ampersand) return "&";
    if (tokenType == TokenType::pipe) return "|";
    if (tokenType == TokenType::logicalOr) return "||";
    if (tokenType == TokenType::logicalAnd) return "&&";
    if (tokenType == TokenType::doubleEqual) return "==";
    if (tokenType == TokenType::notEqual) return "!=";
    if (tokenType == TokenType::relationalG) return ">";
    if (tokenType == TokenType::relationalL) return "<";
    if (tokenType == TokenType::relationalLE) return "<=";

    return "Unknown Token";
}
