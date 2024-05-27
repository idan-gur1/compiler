//
// Created by idang on 25/04/2024.
//

#ifndef COMPILER_TOKENDEFINE_H
#define COMPILER_TOKENDEFINE_H

#include <string>

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
    stringLiteral,
    identifier,
    longKeyword,
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

#endif //COMPILER_TOKENDEFINE_H
