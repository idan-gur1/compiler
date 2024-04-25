#include <iostream>
#include <fstream>
#include <sstream>
#include "lexer.h"
#include "parser.h"
#include "intermediateCodeGenerator.h"
#include "errorHandling.h"
#include "generation.h"
#include "compiler.h"


int main(int argc, char *argv[]) {
    Compiler compiler(argc, argv);

    return compiler.compileProgram();
}
