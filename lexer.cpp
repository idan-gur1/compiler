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

void Lexer::analyseSource() {
    // std::vector<Token> tokens;

    while (this->hasBuffer()) {
        char cCur = this->currentAndProceed();

        if (this->singleTypes.contains(cCur)) {
            this->tokens.push_back(Token(this->singleTypes[cCur]));
        } else if (this->doubleTypes.contains(cCur)) {
            if (this->hasBuffer() && this->current() == cCur) {
                this->currentAndProceed();
                this->tokens.push_back(Token(get<1>(this->doubleTypes[cCur])));
            } else {
                this->tokens.push_back(Token(get<0>(this->doubleTypes[cCur])));
            }
        } else if (this->equalTypes.contains(cCur)) {
            if (this->hasBuffer() && this->current() == '=') {
                this->currentAndProceed();
                this->tokens.push_back(Token(get<1>(this->equalTypes[cCur])));
            } else {
                this->tokens.push_back(Token(get<0>(this->equalTypes[cCur])));
            }
        } else if (cCur == '\'') {
            char innerVal = this->currentAndProceed();

            if (this->currentAndProceed() != '\'') {
                this->throwLexerError("Expected ' At the end of char declaration");
            }

            this->tokens.push_back(Token(TokenType::immediateInteger, std::to_string(innerVal)));
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

            if (this->keywords.contains(buffer)) {
                this->tokens.push_back(Token(this->keywords[buffer]));
            } else {
                this->tokens.push_back(Token(TokenType::identifier, buffer));
            }
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

    // return this->tokens;
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
