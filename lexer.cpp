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
        } else if (cCur == '=') {
            this->tokens.push_back(Token(TokenType::equal));
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
        } else if (cCur == '&') {
            this->tokens.push_back(Token(TokenType::ampersand));
        } else if (std::isdigit(cCur)) {
            std::string buffer(1, cCur);

            while (this->hasBuffer() && std::isdigit(this->current())) {
                buffer.push_back(this->currentAndProceed());
            }

            this->tokens.push_back(Token(TokenType::immediateInteger, buffer));
        } else if (std::isalpha(cCur)) {
            std::string buffer(1, cCur);

            while (this->hasBuffer() && std::isalnum(this->current())) {
                buffer.push_back(this->currentAndProceed());
            }

            if (buffer == "exit") {
                this->tokens.push_back(Token(TokenType::exit, buffer));
            } else if (buffer == "int") {
                this->tokens.push_back(Token(TokenType::intKeyword, buffer));
            } else if (buffer == "char") {
                this->tokens.push_back(Token(TokenType::charKeyword, buffer));
            } else {
                this->tokens.push_back(Token(TokenType::identifier, buffer));
            }
        } else if (cCur == ';') {
            this->tokens.push_back(Token(TokenType::semiColon));
        } else if (std::isspace(cCur)) {
            // ignore
        } else {
            std::cout << "Lexer Error: Unknown character/token " << cCur << " ." << std::endl;
            std::exit(1);
        }
    }

    return this->tokens;
}

bool Lexer::hasNextToken(int offset) {
    return nTokenIndex + offset < tokens.size();
}

Token Lexer::currentToken(int offset) {
    return this->tokens[nTokenIndex + offset];
}

Token Lexer::currentAndProceedToken() {
    return this->tokens[nTokenIndex++];
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
    if (tokenType == TokenType::exit) return "exit";
    if (tokenType == TokenType::semiColon) return "semiColon";

    return "Unknown Token";
}
