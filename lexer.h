// lexer.h

#ifndef COMPILER_LEXER_H
#define COMPILER_LEXER_H

#include <string>
#include <utility>
#include <tuple>
#include <list>
#include <iostream>
#include <unordered_map>
#include "errorHandling.h"

enum class TokenType {
    NO_TOKEN,
    NEW_LINE,
    plus,
    minus,
    mult,
    div,
    modulo,
    equal,
    openParenthesis,
    closeParenthesis,
    immediateInteger,
    identifier,
    intKeyword,
    charKeyword,
    voidKeyword,
    ifKeyword,
    elseKeyword,
    whileKeyword,
    doKeyword,
    returnKeyword,
    exit,
    semiColon,
    coma,
    openCurly,
    closeCurly,
    openSquare,
    closeSquare,
    exclamation,
    ampersand,
    pipe,
    logicalOr,
    logicalAnd,
    doubleEqual,
    notEqual,
    relationalG,
    relationalGE,
    relationalL,
    relationalLE,
};


struct Token {
    TokenType type;
    std::string val;
};

class Lexer {
public:
    int currentLine = 1;

    explicit Lexer(std::string& src) :
    srcCode(std::move(src)) {
        keywords = {
                {"int", TokenType::intKeyword},
                {"char", TokenType::charKeyword},
                {"void", TokenType::voidKeyword},
                {"if", TokenType::ifKeyword},
                {"else", TokenType::elseKeyword},
                {"while", TokenType::whileKeyword},
                {"do", TokenType::doKeyword},
                {"return", TokenType::returnKeyword},
        };

        singleTypes = {
                {'+', TokenType::plus},
                {'-', TokenType::minus},
                {'*', TokenType::mult},
                {'/', TokenType::div},
                {'%', TokenType::modulo},
                {'(', TokenType::openParenthesis},
                {')', TokenType::closeParenthesis},
                {'{', TokenType::openCurly},
                {'}', TokenType::closeCurly},
                {'[', TokenType::openSquare},
                {']', TokenType::closeSquare},
                {',', TokenType::coma},
                {';', TokenType::semiColon}
        };

        doubleTypes = {
                {'=', {TokenType::equal, TokenType::doubleEqual}},
                {'&', {TokenType::ampersand, TokenType::logicalAnd}},
                {'|', {TokenType::pipe, TokenType::logicalOr}},
        };

        equalTypes = {
                {'!', {TokenType::exclamation, TokenType::notEqual}},
                {'>', {TokenType::relationalG, TokenType::relationalGE}},
                {'<', {TokenType::relationalL, TokenType::relationalLE}},
        };
    }

    void analyseSource();
    bool hasNextToken();
    Token currentToken();
    Token currentAndProceedToken();

private:
    void throwLexerError(const std::string &errorMsg) const;

    int nCurIndex = 0;
//    int nTokenIndex = 0;
    std::string srcCode;
//    std::vector<Token> tokens;
    std::list<Token> tokens;

    std::unordered_map<std::string , TokenType> keywords;
    std::unordered_map<char, TokenType> singleTypes;
    std::unordered_map<char, std::tuple<TokenType, TokenType>> doubleTypes;
    std::unordered_map<char, std::tuple<TokenType, TokenType>> equalTypes;

    void generateTokenByDigit(char cCur);

    void generateTokenByChar(char cCur);

    void generateTokenFromDoubleType(char cCur);

    void generateTokenFromEqualType(char cCur);

    void generateNumTokenFromCharDec();

    char current();
    char currentAndProceed();
    bool hasBuffer();

    void countLines();
};


#endif //COMPILER_LEXER_H
