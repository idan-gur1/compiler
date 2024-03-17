//
// Created by idang on 17/03/2024.
//

#ifndef COMPILER_ERRORHANDLING_H
#define COMPILER_ERRORHANDLING_H

#include <exception>
#include <iostream>
#include <utility>

class CompilationException : public std::exception {
private:
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

#endif //COMPILER_ERRORHANDLING_H
