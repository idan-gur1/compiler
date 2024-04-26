//
// Created by idang on 24/12/2023.
//

#ifndef COMPILER_INTERMEDIATECODEGENERATOR_H
#define COMPILER_INTERMEDIATECODEGENERATOR_H

#include <string>
#include <utility>
#include <sstream>
#include <typeindex>
#include <fstream>
#include <list>
#include <unordered_map>
#include "treeNodes.h"
#include "threeAddressExpressionsAndStatements.h"
#include "errorHandling.h"

/**
 * @brief The ILGenerator class handles generating intermediate code (IL) from an abstract syntax tree (AST).
 *
 * ILGenerator processes the AST and generates three-address code representations of the program's logic.
 */
class ILGenerator {
public:
    // linked list to store the generated intermediate code statements
    std::list<ThreeAddressStmt *> ilStmts;
    // Set to store the names of built-in functions used in the program
    // Functions included here will be added to the assembly generation
    std::unordered_set<std::string> builtinFunctionsUsed;

    /**
     * @brief Constructor for the ILGenerator class.
     *
     * @param program The abstract syntax tree of the program.
     * @param outfileName The name of the output file for the generated intermediate code.
     */
    ILGenerator(ProgramTreeP program, std::string outfileName) : outfileName(std::move(outfileName)) {
        this->program = program;
    }

    /**
     * @brief Destructor to free memory allocated for generated intermediate code statements.
     */
    ~ILGenerator() {
        for (const auto &ilStmt: ilStmts) {
            delete ilStmt;
        }
    }

    /**
     * @brief Generates the intermediate code (IL) for the entire program.
     *
     * This method iterates over each function in the program and generates intermediate code
     * for each function. It then writes the generated intermediate code statements to an
     * output file specified during object creation.
     *
     * @throws CompilationException if there is an error opening the output file.
     */
    void generateProgramIL();

    /**
     * @brief Generates intermediate code (IL) for a specific function.
     *
     * This method generates intermediate code for the given function by creating a function
     * declaration statement and appending it to the list of IL statements.
     * It then resets the temporary counters, sets the current function name, and proceeds to
     * generate the IL for the function's scope. After processing the function's scope, it adds
     * a label statement marking the end of the function and a function exit statement.
     * Finally, it updates the maximum temporary value in the function declaration statement.
     *
     * @param function The function node for which to generate IL.
     */
    void generateFunctionIL(NodeFunctionP function);

    /**
     * @brief Generates intermediate code (IL) for a scope.
     *
     * This method generates IL for a given scope by adding a scope enter statement to the list
     * of IL statements, processing each statement within the scope using, and concluding with
     * a scope exit statement.
     *
     * @param scope The scope node for which to generate IL.
     */
    void generateScopeIL(NodeScopeP scope);

    /**
     * @brief Generates intermediate code (IL) for a given statement node.
     *
     * This method generates IL for various types of statement nodes:\n
     * - For primitive assignment statements, it generates a variable assignment ('VarAssignmentTAStmt').\n
     * - For array assignment statements, it generates a subscriptable variable assignment ('VarAssignmentTAStmt').\n
     * - For function call statements, it generates the IL for the function call expression.\n
     * - For if statements, it generates the IL for conditional branching using conditional goto statements.\n
     * - For while statements, it generates the IL for looping constructs using conditional goto statements.\n
     * - For return statements, it sets the return value and jumps to the end of the current function.\n
     * - For scope nodes, it recursively generates IL for the contained statements.
     *
     * @param stmt The statement node for which to generate IL.
     */
    void generateStmtIL(NodeStmtP stmt);

    /**
     * @brief Generates intermediate code (IL) for a given expression node.
     *
     * This method generates IL for a specific expression node, handling different types of expressions
     * and producing the corresponding IL expression representation.
     *
     * @param expr The expression node for which IL is being generated.
     * @return A pointer to the generated intermediate code (IL) expression.
     */
    ThreeAddressExpr *generateExprIL(NodeExprP expr);

private:
    // Map to associate node expression types with their corresponding expression operators
    static std::unordered_map<std::type_index, ExprOperator> NodeExprToExprOperator;
    // Map to associate expression operators with their string representations
    static std::unordered_map<ExprOperator, std::string> exprOperatorToStr;

    // The abstract syntax tree of the program
    ProgramTreeP program;
    // The name of the output file for the generated intermediate code
    std::string outfileName;

    // The name of the current function being processed
    std::string currentFunctionName;
    // Counter for generating unique identifiers for if statements
    int currentIfId = 0;
    // Counter for generating unique identifiers for while statements
    int currentWhileId = 0;
    // Counter for generating temporary variables for expression generating
    int currentTemp = 0;
    // Maximum temporary variable identifier used in each function (getting zeroed each function generation)
    int maxTemp = 0;

    /**
     * @brief Generates intermediate representation (IL) for a numeric expression.
     *
     * This function handles different types of expressions and converts them into
     * appropriate IL representations.
     *
     * @param expr The input expression to generate IL for.
     * @return A UniExpr pointer representing the numeric expression in IL.
     */
    UniExpr *generateNumericExprIL(NodeExprP expr);

    /**
     * @brief Generates a temporary assignment statement for a binary expression.
     *
     * Manages temporary variable usage by reusing existing ones from the expression when possible,
     * if both sides of the expressions are temporaries then free the latter,
     * if none are temporary then increase the temporary count and use the new as the result.
     *
     * @param uniLhs The left-hand side of the binary expression.
     * @param uniRhs The right-hand side of the binary expression.
     * @param op The operator for the binary expression.
     * @return The temporary assignment statement for the binary expression.
     */
    TempAssignmentTAStmt *generateBinaryTempAssignmentIL(UniExpr *uniLhs, UniExpr *uniRhs, ExprOperator op);

    /**
     * @brief Generates intermediate code for a binary expression.
     *
     * Converts the operands into appropriate intermediate representations and creates a
     * temporary assignment statement.
     *
     * @param binExpr Pointer to a BinaryNodeExprP object representing the binary expression.
     */
    void generateBinaryExprIL(BinaryNodeExprP);

    /**
     * @brief Generates intermediate representation (IL) for a unary expression.
     * @param unaryExpr The unary expression to process.
     * @return The generated intermediate representation for the unary expression.
     */
    UniExpr *generateUnaryExprIL(UnaryNodeExprP);

    /**
     * @brief Converts a terminal node expression into an intermediate representation.
     *
     * Handles different types of terminal expressions, such as immediate integers, subscriptable variables,
     * regular variables, and function calls, converting them into appropriate intermediate expressions.
     * Generates necessary intermediate code for complex conversions.
     *
     * @param terminalExpr Pointer to a TerminalNodeExprP object representing the terminal expression to convert.
     * @return Pointer to a UniExpr object representing the converted intermediate expression.
     */
    UniExpr *convertTerminalToUniExpr(TerminalNodeExprP terminalExpr);

    /**
     * @brief Generates intermediate code for a function call expression including pushing parameters to the stack.
     *
     * @param funcCall The function call node to generate code for.
     * @return A FunctionCallExpr representing the function call.
     */
    FunctionCallExpr *generateFunctionCallExprIL(NodeFunctionCallP);

    /**
     * @brief Increments the current temporary variable counter and updates the maximum temporary value if needed.
     *
     * @return The updated current temporary variable value.
     */
    int incCurrentTemp();

    /**
     * Converts a Three Address Statement to a string representation.
     *
     * @param taStmt The Three Address Statement to convert.
     * @return The string representation of the Three Address Statement.
     */
    static std::string ilStmtToStr(ThreeAddressStmt *);

    /**
     * Converts a Three Address Expression to its corresponding string representation.
     *
     * @param taExpr The Three Address Expression to convert to a string.
     * @return The string representation of the Three Address Expression.
     */
    static std::string ilExprToStr(ThreeAddressExpr *);
};

#endif //COMPILER_INTERMEDIATECODEGENERATOR_H
