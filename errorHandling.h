//
// Created by idang on 17/03/2024.
//

#ifndef COMPILER_ERRORHANDLING_H
#define COMPILER_ERRORHANDLING_H

#include <exception>
#include <iostream>
#include <utility>
#include "treeNodes.h"

class CompilationException : public std::exception {
protected:
    std::string message;

public:
    // Constructor accepts a const char* that is used to set
    // the exception message
    explicit CompilationException(std::string msg)
            : message(std::move(msg))
    {
        message = "Compilation Error: " + message + ".";
    }

    // Override the what() method to return our message
    [[nodiscard]] const char* what() const noexcept override
    {
        return message.c_str();
    }
};

class FileOpenException : public CompilationException {
public:
    explicit FileOpenException(std::string msg)
            : CompilationException("Error while opening the file " + std::move(msg))
    {

    }
};

class AnalysisStageException : public CompilationException {
public:
    inline static int lineNumber = 1;

    explicit AnalysisStageException(std::string msg)
            : CompilationException(std::move(msg))
    {
        message = message + " On line " + std::to_string(lineNumber);
    }
};

class LexicalAnalysisException : public AnalysisStageException {
public:
    explicit LexicalAnalysisException(std::string msg)
            : AnalysisStageException("[Lexer] " + std::move(msg))
    {

    }
};

class ParserException : public AnalysisStageException {
public:
    inline static ProgramTreeP programTree = nullptr;

    explicit ParserException(std::string msg)
            : AnalysisStageException("[Parser] " + std::move(msg))
    {
        delete programTree;
    }
};

class SyntaxAnalysisException : public ParserException {
public:
    explicit SyntaxAnalysisException(std::string msg)
            : ParserException("[Syntax Error] " + std::move(msg))
    {

    }
};

class SemanticAnalysisException : public ParserException {
public:
    explicit SemanticAnalysisException(std::string msg)
            : ParserException("[Semantic Error] " + std::move(msg))
    {

    }
};

#endif //COMPILER_ERRORHANDLING_H
