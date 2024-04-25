cmake_minimum_required(VERSION 3.22)
project(compiler)

set(CMAKE_CXX_STANDARD 20)

add_executable(compiler main.cpp
        lexer.cpp
        lexer.h
        parser.cpp
        parser.h
        treeNodes.h
        intermediateCodeGenerator.cpp
        intermediateCodeGenerator.h
        generation.cpp
        generation.h
        parserStatements.cpp
        parserExpressions.cpp
        errorHandling.h
        tokenDefine.h
        threeAddressExpressionsAndStatements.h
        compiler.cpp
        compiler.h
)
