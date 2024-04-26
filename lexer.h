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

/**
 * @brief Class responsible for lexical analysis of source code.
 */
class Lexer {
public:
    // Current line being analyzed across all analysis phases
    int currentLine = 1;

    /**
     * @brief Constructor to initialize Lexer with source code.
     *
     * @param src The source code to be analyzed.
     */
    explicit Lexer(std::string &src) :
            srcCode(std::move(src)) {
    }

    /**
     * @brief Analyzes the source code and generates tokens based on the recognized patterns.
     *
     * The function iterates through the source code character by character, identifying tokens and adding them
     * to the token list. Tokens include single-character tokens, keywords, identifiers, numerical values,
     * and special characters. If an unrecognized character is encountered, a LexicalAnalysisException is thrown.
     */
    void analyseSource();

    /**
     * @brief Check if there are more tokens available.
     *
     * @return True if more tokens are available, otherwise false.
     */
    bool hasNextToken();

    /**
     * @brief Gets the current token.
     *
     * @return The current token.
     */
    Token currentToken();

    /**
     * @brief Gets the current token and move to the next one.
     *
     * @return The current token before moving to the next one.
     */
    Token currentAndProceedToken();

private:
    // Map of single-character token types
    static std::unordered_map<char, TokenType> singleTypes;
    // Map of double-character token types
    static std::unordered_map<char, std::tuple<TokenType, TokenType>> doubleTypes;
    // Map of token types with '='
    static std::unordered_map<char, std::tuple<TokenType, TokenType>> equalTypes;
    // Map of keywords to their token types
    static std::unordered_map<std::string, TokenType> keywords;
    // Map of escape characters to their representations
    static std::unordered_map<char, char> escapeChars;

    // Current index in the source code
    int nCurIndex = 0;
    // Source code being analyzed
    std::string srcCode;
    // Linked list of tokens generated during lexical analysis
    std::list<Token> tokens;

    /**
     * @brief Generates a token from a single-digit integer.
     *
     * @param cCur The current character.
     */
    void generateTokenByDigit(char cCur);

    /**
     * @brief Generates a token from an alphanumeric sequence.
     *
     * @param cCur The current character.
     */
    void generateTokenByChar(char cCur);

    /**
     * @brief Generate token from double-character token types.
     *
     * @param cCur The current character.
     */
    void generateTokenFromDoubleType(char cCur);

    /**
     * @brief Generate token from token types with '='.
     *
     * @param cCur The current character.
     */
    void generateTokenFromEqualType(char cCur);

    /**
     * @brief Generate number token from a character declaration.
     *
     */
    void generateNumTokenFromCharDec();

    /**
     * @brief Retrieves the current character in the source code.
     *
     * @return The current character.
     */
    char current();

    /**
     * @brief Checks if there are more characters to process in the source code.
     *
     * @return True if there are more characters, otherwise false.
     */
    char currentAndProceed();

    /**
     * @brief Check if there are characters left in the source code.
     *
     * @return True if there are characters left, otherwise false.
     */
    bool hasBuffer();

    /**
     * @brief Keeps count of the line number while reading the source code.
     */
    void countLines();

    /**
     * @brief Update the line number in the error class in case of error.
     */
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

inline std::unordered_map<char, char> Lexer::escapeChars = {
        {'n', '\n'},
        {'t', '\t'},
        {'r', '\r'},
        {'b', '\b'},
        {'a', '\a'},
        {'0', '\0'},
};
#endif //COMPILER_LEXER_H
