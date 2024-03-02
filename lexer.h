// lexer.h

#ifndef COMPILER_LEXER_H
#define COMPILER_LEXER_H

#include <string>
#include <vector>
#include <iostream>

enum class TokenType {
    NO_TOKEN,
    plus,
    minus,
    mult,
    div,
    equal,
    openParenthesis,
    closeParenthesis,
    immediateInteger,
    identifier,
    tempIdentifier,
    keyword,
    exit,
    semiColon,
    openCurly,
    closeCurly
};

std::string getTokenName(TokenType tokenType);

struct Token {
    TokenType type;
    std::string val;
};

class Lexer {
public:
//    Lexer(std::string src);
    explicit Lexer(std::string& src) :
    srcCode(std::move(src)) {

    }

    std::vector<Token> analyseSource();
    bool hasNextToken(int offset = 0);
    Token currentToken();
    Token currentAndProceedToken();

private:
    int nCurIndex = 0;
    int nTokenIndex = 0;
    std::string srcCode;
    std::vector<Token> tokens;

    char current();
    char currentAndProceed();
    bool hasBuffer();
};


#endif //COMPILER_LEXER_H
