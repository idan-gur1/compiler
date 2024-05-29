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

        if (singleTypes.contains(cCur)) {
            this->tokens.push_back(Token(singleTypes[cCur]));
        } else if (doubleTypes.contains(cCur)) {
            generateTokenFromDoubleType(cCur);
        } else if (equalTypes.contains(cCur)) {
            generateTokenFromEqualType(cCur);
        } else if (cCur == charLiteralDefinition) {
            generateNumTokenFromCharDec();
        } else if (cCur == stringLiteralDefinition) {
            generateStringLiteral();
        } else if (std::isdigit(cCur)) {
            generateTokenByDigit(cCur);
        } else if (std::isalpha(cCur)) {
            generateTokenByChar(cCur);
        } else if (cCur == '\n') {
            this->currentLine++;
            updateErrorLineNumber();
            this->tokens.push_back(Token(TokenType::NEW_LINE));
        } else if (std::isspace(cCur)) {
            // ignore
        } else {
            throw LexicalAnalysisException("[Lexer] Unknown character/token " + std::string(1, cCur));
        }
    }

    this->currentLine = 1;
    updateErrorLineNumber();

    this->countLines();
}

void Lexer::generateTokenByDigit(char cCur) {
    std::string buffer(1, cCur);

    while (this->hasBuffer() && std::isdigit(this->current())) {
        buffer.push_back(this->currentAndProceed());
    }
    if (this->hasBuffer() && std::isalpha(this->current())) {
        throw LexicalAnalysisException("Invalid character '" + std::string(1, cCur) + "' in immediate integer");
    }

    this->tokens.push_back(Token(TokenType::immediateInteger, buffer));
}

void Lexer::generateTokenByChar(char cCur) {
    std::string buffer(1, cCur);

    while (this->hasBuffer() && std::isalnum(this->current())) {
        buffer.push_back(this->currentAndProceed());
    }

    if (keywords.contains(buffer)) {
        this->tokens.push_back(Token(keywords[buffer]));
    } else {
        this->tokens.push_back(Token(TokenType::identifier, buffer));
    }
}

void Lexer::generateTokenFromDoubleType(char cCur) {
    if (this->hasBuffer() && this->current() == cCur) {
        this->currentAndProceed();
        this->tokens.push_back(Token(get<1>(doubleTypes[cCur])));
    } else {
        this->tokens.push_back(Token(get<0>(doubleTypes[cCur])));
    }
}

void Lexer::generateTokenFromEqualType(char cCur) {
    if (this->hasBuffer() && this->current() == '=') {
        this->currentAndProceed();
        this->tokens.push_back(Token(get<1>(equalTypes[cCur])));
    } else {
        this->tokens.push_back(Token(get<0>(equalTypes[cCur])));
    }
}

void Lexer::generateNumTokenFromCharDec() {
    if (!this->hasBuffer()) {
        throw LexicalAnalysisException("Unexpected EOF");
    }

    char innerVal = this->currentAndProceed();

    if (innerVal == EscapedCharLiteralDefinition) {
        if (!this->hasBuffer()) {
            throw LexicalAnalysisException("Unexpected EOF");
        }

        char escapable = this->currentAndProceed();

        if (!escapeChars.contains(escapable)) {
            throw LexicalAnalysisException("'" + std::string(escapable, 1) + "' cannot be escaped");
        }

        innerVal = escapeChars[escapable];
    }

    if (!this->hasBuffer() || this->currentAndProceed() != charLiteralDefinition) {
        throw LexicalAnalysisException("Expected ' At the end of char declaration");
    }

    this->tokens.push_back(Token(TokenType::immediateInteger, std::to_string(innerVal)));
}

void Lexer::generateStringLiteral() {
    std::string buffer;

    while (this->hasBuffer() && this->current() != '\n' && this->current() != stringLiteralDefinition) {
        buffer.push_back(this->currentAndProceed());
    }

    if (!this->hasBuffer() || this->currentAndProceed() != stringLiteralDefinition) {
        throw LexicalAnalysisException("Expected \" At the end of string literal declaration");
    }

    this->tokens.push_back(Token(TokenType::stringLiteral, buffer));
}

bool Lexer::hasNextToken() {
    return !this->tokens.empty();
}

Token Lexer::currentToken() {
    return this->tokens.front();
}

Token Lexer::currentAndProceedToken() {
    Token ret = this->tokens.front();
    this->tokens.pop_front();

    this->countLines();

    return ret;
}

void Lexer::countLines() {
    while (this->hasNextToken() && this->tokens.front().type == TokenType::NEW_LINE) {
        this->currentLine++;
        updateErrorLineNumber();

        this->tokens.pop_front();
    }
}

void Lexer::updateErrorLineNumber() const {
    AnalysisStageException::lineNumber = this->currentLine;
}
