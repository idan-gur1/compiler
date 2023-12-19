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
            } else {
                this->tokens.push_back(Token(TokenType::identifier, buffer));
            }
        } else if (cCur == ';') {
            this->tokens.push_back(Token(TokenType::semiColon));
        } else if (std::isspace(cCur)) {
            // ignore
        } else {
            std::cout << "Unknown character/token " << cCur << " ." << std::endl;
            std::exit(1);
        }
    }

    return this->tokens;
}

bool Lexer::hasNextToken(int offset) {
    return nTokenIndex + offset < tokens.size();
}

Token Lexer::currentToken() {
    return this->tokens[nTokenIndex];
}

Token Lexer::currentAndProceedToken() {
    return this->tokens[nTokenIndex++];
}


std::string getTokenName(TokenType tokenType) {
    if (tokenType == TokenType::plus) return "plus";
    if (tokenType == TokenType::minus) return "minus";
    if (tokenType == TokenType::mult) return "mult";
    if (tokenType == TokenType::div) return "div";
    if (tokenType == TokenType::equal) return "equal";
    if (tokenType == TokenType::openParenthesis) return "openParenthesis";
    if (tokenType == TokenType::closeParenthesis) return "closeParenthesis";
    if (tokenType == TokenType::immediateInteger) return "immediateInteger";
    if (tokenType == TokenType::identifier) return "identifier";
    if (tokenType == TokenType::exit) return "exit";
    if (tokenType == TokenType::semiColon) return "semiColon";

    return "Unknown Token";
}
