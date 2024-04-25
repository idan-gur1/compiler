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
#include "tokenDefine.h"


class Lexer {
public:
    int currentLine = 1;

    explicit Lexer(std::string &src) :
            srcCode(std::move(src)) {
        doubleTypes = {
                {'=', {TokenType::equal,     TokenType::doubleEqual}},
                {'&', {TokenType::ampersand, TokenType::logicalAnd}},
                {'|', {TokenType::pipe,      TokenType::logicalOr}},
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
    static std::unordered_map<char, TokenType> singleTypes;
    static std::unordered_map<char, std::tuple<TokenType, TokenType>> doubleTypes;
    static std::unordered_map<char, std::tuple<TokenType, TokenType>> equalTypes;
    static std::unordered_map<std::string, TokenType> keywords;

    int nCurIndex = 0;
    std::string srcCode;
    std::list<Token> tokens;

    void generateTokenByDigit(char cCur);

    void generateTokenByChar(char cCur);

    void generateTokenFromDoubleType(char cCur);

    void generateTokenFromEqualType(char cCur);

    void generateNumTokenFromCharDec();

    char current();

    char currentAndProceed();

    bool hasBuffer();

    void countLines();

    void updateErrorLineNumber() const;
};

inline std::unordered_map<char, TokenType> Lexer::singleTypes = {
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

inline std::unordered_map<char, std::tuple<TokenType, TokenType>> Lexer::doubleTypes = {
        {'=', {TokenType::equal,     TokenType::doubleEqual}},
        {'&', {TokenType::ampersand, TokenType::logicalAnd}},
        {'|', {TokenType::pipe,      TokenType::logicalOr}},
};

inline std::unordered_map<char, std::tuple<TokenType, TokenType>> Lexer::equalTypes = {
        {'!', {TokenType::exclamation, TokenType::notEqual}},
        {'>', {TokenType::relationalG, TokenType::relationalGE}},
        {'<', {TokenType::relationalL, TokenType::relationalLE}},
};

inline std::unordered_map<std::string, TokenType> Lexer::keywords = {
        {"int",    TokenType::intKeyword},
        {"char",   TokenType::charKeyword},
        {"void",   TokenType::voidKeyword},
        {"if",     TokenType::ifKeyword},
        {"else",   TokenType::elseKeyword},
        {"while",  TokenType::whileKeyword},
        {"do",     TokenType::doKeyword},
        {"return", TokenType::returnKeyword},
};
#endif //COMPILER_LEXER_H
