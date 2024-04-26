//
// Created by idang on 26/04/2024.
//

#ifndef COMPILER_BUILTINFUNCTIONS_H
#define COMPILER_BUILTINFUNCTIONS_H

#include <vector>
#include <tuple>
#include <string>
#include "treeNodes.h"


class BuiltinFunctions
{
public:
    // A vector of tuples representing built-in functions with their names, return types, pointer types, and parameters
    inline static std::vector<std::tuple<std::string, VariableType, bool,
            std::vector<Variable>>> builtInFunctions = {
            {"outputChar",VariableType::voidType, false, {Variable("", VariableType::charType, false)}},
            {"inputChar", VariableType::charType, false, {}},
            {"exit", VariableType::voidType, false, {Variable("", VariableType::intType, false)}},
    };

    // Disallow creating an instance of this object
    BuiltinFunctions() = delete;
};

#endif //COMPILER_BUILTINFUNCTIONS_H
