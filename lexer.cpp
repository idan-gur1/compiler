// lexer.cpp

#include "lexer.h"


char Lexer::current() {
    return srcCode.at(this->nCurIndex);
}

char Lexer::currentAndProceed() {
    return srcCode.at(this->nCurIndex++);
}

bool Lexer::hasBuffer() {
    return nCurIndex < srcCode.size();
}

void Lexer::analyseSource() {
    while (this->hasBuffer()) {
        char cCur = this->currentAndProceed();

        if (this->singleTypes.contains(cCur)) {
            this->tokens.push_back(Token(this->singleTypes[cCur]));
        } else if (this->doubleTypes.contains(cCur)) {
            generateTokenFromDoubleType(cCur);
        } else if (this->equalTypes.contains(cCur)) {
            generateTokenFromEqualType(cCur);
        } else if (cCur == '\'') {
            generateNumTokenFromCharDec();
        } else if (std::isdigit(cCur)) {
            generateTokenByDigit(cCur);
        } else if (std::isalpha(cCur)) {
            generateTokenByChar(cCur);
        } else if (cCur == '\n') {
            this->currentLine++;
            this->tokens.push_back(Token(TokenType::NEW_LINE));
        } else if (std::isspace(cCur)) {
            // ignore
        } else {
            this->throwLexerError("[Lexer] Unknown character/token " + std::string(1, cCur));
        }
    }

    this->currentLine = 1;

    this->countLines();
}

void Lexer::generateTokenByDigit(char cCur) {
    std::string buffer(1, cCur);

    while (this->hasBuffer() && std::isdigit(this->current())) {
        buffer.push_back(this->currentAndProceed());
    }
    if (this->hasBuffer() && std::isalpha(this->current())) {
        this->throwLexerError("Invalid character '" + std::string(1, cCur) + "'");
    }

    this->tokens.push_back(Token(TokenType::immediateInteger, buffer));
}

void Lexer::generateTokenByChar(char cCur) {
    std::string buffer(1, cCur);

    while (this->hasBuffer() && std::isalnum(this->current())) {
        buffer.push_back(this->currentAndProceed());
    }

    if (this->keywords.contains(buffer)) {
        this->tokens.push_back(Token(this->keywords[buffer]));
    } else {
        this->tokens.push_back(Token(TokenType::identifier, buffer));
    }
}

void Lexer::generateTokenFromDoubleType(char cCur) {
    if (this->hasBuffer() && this->current() == cCur) {
        this->currentAndProceed();
        this->tokens.push_back(Token(get<1>(this->doubleTypes[cCur])));
    } else {
        this->tokens.push_back(Token(get<0>(this->doubleTypes[cCur])));
    }
}

void Lexer::generateTokenFromEqualType(char cCur) {
    if (this->hasBuffer() && this->current() == '=') {
        this->currentAndProceed();
        this->tokens.push_back(Token(get<1>(this->equalTypes[cCur])));
    } else {
        this->tokens.push_back(Token(get<0>(this->equalTypes[cCur])));
    }
}

void Lexer::generateNumTokenFromCharDec() {
    char innerVal = this->currentAndProceed();

    if (this->currentAndProceed() != '\'') {
        this->throwLexerError("Expected ' At the end of char declaration");
    }

    this->tokens.push_back(Token(TokenType::immediateInteger, std::to_string(innerVal)));
}

bool Lexer::hasNextToken() {
//    return nTokenIndex < tokens.size();
    return !this->tokens.empty();
}

Token Lexer::currentToken() {
//    return this->tokens[nTokenIndex];
    return this->tokens.front();
}

Token Lexer::currentAndProceedToken() {
//    Token ret =  this->tokens[nTokenIndex++];
    Token ret = this->tokens.front();
    this->tokens.pop_front();

    this->countLines();

    return ret;
}

void Lexer::countLines() {
//    while (this->hasNextToken() && this->tokens[nTokenIndex].type == TokenType::NEW_LINE) {
    while (this->hasNextToken() && this->tokens.front().type == TokenType::NEW_LINE) {
        this->currentLine++;
//        nTokenIndex++;
        this->tokens.pop_front();
    }
}

void Lexer::throwLexerError(const std::string &errorMsg) const {
    throw CompilationException("[Lexer] " + errorMsg + " On line " + std::to_string(this->currentLine));
}
