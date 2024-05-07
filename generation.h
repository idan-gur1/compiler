//
// Created by idang on 04/02/2024.
//

#ifndef COMPILER_GENERATION_H
#define COMPILER_GENERATION_H

#include <utility>
#include <stack>

#include "intermediateCodeGenerator.h"

/**
 * @brief Represents data associated with a variable's stack position and size.
 *
 * Used by the code generator to keep track of variable data
 */
class VariableStackData {
public:
    int stackPos;
    int varSize;

    /**
     * @brief Constructor for the VariableStackData class.
     *
     * @param stackPos The stack position of the variable.
     * @param varSize The size of the variable.
     */
    VariableStackData(int stackPos, int varSize) : stackPos(stackPos), varSize(varSize) {

    }
};

/**
 * @brief Represents a scope frame with frame size and a set of variables used in the scope frame.
 */
class ScopeFrame {
public:
    int frameSize;
    std::unordered_set<std::string> frameVars;

    /**
     * @brief Constructor for the ScopeFrame class.
     *
     * @param frameSize The size of the scope frame.
     * @param frameVars The set of variables within the scope.
     */
    ScopeFrame(int frameSize, std::unordered_set<std::string> frameVars) : frameSize(frameSize),
                                                                           frameVars(std::move(frameVars)) {

    }
};

/**
 * @brief Class responsible for generating assembly code from an intermediate representation.
 */
class Generator {
public:
    /**
     * @brief Constructor for the  Generator class.
     *
     * @param ilProgram The ThreeAddressProgram instance (IL representation) used for code generation.
     * @param outFileName The name of the output file for the generated assembly code.
     */
    Generator(ThreeAddressProgramP ilProgram, std::string outFileName) : outFileName(std::move(outFileName)) {
        this->ilProgram = ilProgram;
    }

    void generateProgram();

private:
    // Path to the folder containing built-in functions
    inline static const std::string PATH_TO_BUILTIN_FUNCTIONS_FOLDER = "../builtins/";
    // The threshold limit of continuous function calls for stack overflow
    static const int STACK_OVERFLOW_LIMIT = 200;

    // Size of temporary variables in bytes
    static const int TEMP_SIZE = 8;
    // Size of a 64-bit register in bytes
    static const int BIT_64_REG_SIZE = 8;
    // Size of a pointer data type in bytes
    static const int PTR_SIZE = 8;

    // Map to associate VariableType with its corresponding size on the stack
    static std::unordered_map<VariableType, int> typeSizes;
    // Map to associate sizes on the stack to the type identifiers (BYTE, WORD, DWORD, QWORD)
    static std::unordered_map<int, std::string> sizeIdentifiers;
    // Map to associate binary expression operators to the steps needed to be taken
    // in assembly to perform the operation
    static std::unordered_map<ExprOperator, std::string> BinaryExprToAsmStrSteps;

    // Map to keep track of each variable current size and position on the stack,
    // that can be changed by scopes
    std::unordered_map<std::string, std::stack<VariableStackData>> variableStack;
    // Stack to keep track of scope frames, used to know the side for deallocation
    // and which variables to pop from the 'variableStack' map
    std::stack<ScopeFrame> scopeFrameStack;

    // Pointer to the intermediate representation of the program
    ThreeAddressProgramP ilProgram;
    // The name of the output file where the generated assembly code will be written
    std::string outFileName;
    // A stringstream used to accumulate the generated assembly code for the program
    std::stringstream programOut;
    // Counter for generating unique labels within the assembly code
    int labelCount = 0;
    // Current relative stack pointer (SP) position within the function's stack frame
    int currentRelativeSP = 0;
    // Size of function parameters to subtract after the function call is generated
    int FuncParamsOffsetSP = 0;
    // Size of function parameters in bytes, used at function exit with the 'ret' instruction
    int paramsSize = 0;

    /**
     * @brief Constructs the stack address based on the provided VariableStackData.
     *
     * @param var The VariableStackData containing stack position information.
     * @return A string representing the stack address relative to the base pointer (rbp).
     *         If the stack position is positive, the address is calculated as "rbp + stackPos".
     *         If the stack position is negative, the address is calculated as "rbp - |stackPos|".
     */
    static std::string getStackAddr(const VariableStackData &);

    /**
     * @brief Constructs an assembly instruction to move a value into a 64-bit register.
     *
     * @param reg The destination 64-bit register.
     * @param val The value to be moved into the register.
     * @param size The size of the value being moved (in bytes).
     * @return An assembly instruction string that moves the specified value into the register.
     *         If the size of the value is less than 64 bits, sign extension (movsx) is used.
     */
    static std::string movTo64BitReg(const std::string &, const std::string &, int);

    /**
     * @brief Retrieves the stack position of a subscriptable variable.
     *
     * @param subVar Pointer to a SubscriptableVariableVal instance.
     * @param freeReg A string specifying a free register to use for intermediate operations.
     * @return A string representing the memory location accessed by the subscripted variable,
     *         computed as "[varBaseAddr + typeSize * offset]", where:\n
     *         - varBaseAddr is the base address of the variable on the stack.
     *           If the subscriptable is a pointer then the base address is stored in the 'freeReg' register\n
     *         - typeSize is the size of the variable's type.\n
     *         - offset is the index or offset value used for subscripting.\n
     */
    std::string getSubscriptableStackPosition(SubscriptableVariableValP subVar, const std::string &reg);

    /**
     * @brief Generates assembly code to move the value of a UniExpr to the specified register.
     *
     *
     * @param expr The UniExpr to be converted.
     * @param reg The register where the value will be stored.
     */
    void convertUniExprToRegister(UniExprP, const std::string &);

    /**
     * @brief Generates assembly code to load the address an addressable to the specified register.
     *
     * @param expr The AddrExpr to be converted.
     * @param reg The register where the address will be stored.
     */
    void convertAddrExprToRegister(AddrExprP, const std::string &);

    /**
     *  @brief Converts a binary expression into a set of assembly instructions.
     *  The value of the expression is saved in the 'rax' register.
     *
     * @param expr The binary expression to convert.
     */
    void convertBinaryExprToRegister(BinaryExprP);

    /**
     * @brief Converts a three-address expression into a set of assembly instructions.
     * The value of the expression is saved in the 'rax' register.
     *
     * @param expr The three-address expression to convert.
     */
    void convertTAExprToRaxRegister(ThreeAddressExprP);

    /**
     * @brief Converts a Three Address Statement into a set of assembly instructions based on its type.
     *
     * @param taStmt The Three Address Statement to convert.
     */
    void convertTAStmtToAsm(ThreeAddressStmtP);

    /**
     * @brief Converts a temporary assignment statement to assembly code.
     *
     * If the assignment is a function call expression, this function generates assembly code
     * for the function call and handles the return value, adjusting the stack and parameter offset.
     * Otherwise, it converts the three-address expression to the rax register.
     *
     * @param tempAssignment The temporary assignment statement to convert.
     */
    void convertTempAssignmentToAsm(TempAssignmentTAStmtP);

    /**
     * @brief Converts a variable assignment statement to assembly code.
     *
     * Retrieves the value of the right-hand side expression and assigns it to the left-hand side variable.
     *
     * @param varAssignmentStmt Pointer to the VarAssignmentTAStmt representing the assignment statement.
     */
    void convertVarAssignmentToAsm(VarAssignmentTAStmtP);

    /**
     * @brief Convert a function parameter push statement to assembly code.
     *
     * This function takes a function parameter push statement and converts its associated
     * expression into assembly code. It determines the size required on the stack based
     * on the variable type and pointer status.
     *
     * @param funcParamPushStmt A pointer to a FunctionParamPushStmt representing the push statement.
     */
    void convertFunctionParamPushToAsm(FunctionParamPushStmtP);

    /**
     * @brief Convert a function call expression to assembly code.
     *
     * This function takes a function call expression and generates the corresponding assembly
     * code by calling the target function. It then adjusts the stack pointer and resets the
     * function parameter offset.
     *
     * @param functionCallStmt A pointer to a FunctionCallExprP containing the function call expression.
     */
    void convertFunctionCallToAsm(FunctionCallExprP);

    /**
     * @brief Convert a label statement to assembly code.
     *
     * This function generates assembly code for a label statement, adding the label with colon to the output.
     *
     * @param labelStmt A pointer to a LabelStmtP representing the label statement.
     */
    void convertLabelToAsm(LabelStmtP);

    /**
     * @brief Convert a goto statement to assembly code.
     *
     * This function generates assembly code to perform an unconditional jump to a specified label.
     *
     * @param gotoStmt A pointer to a GotoStmtP representing the goto statement.
     */
    void convertGotoToAsm(GotoStmtP);

    /**
     * @brief Convert a goto-if-zero statement to assembly code.
     *
     * This function generates assembly code to test the value in the 'rax' register and
     * perform a conditional jump to a specified label if the value is zero.
     *
     * @param gotoIfZeroStmt A pointer to a GotoIfZeroStmtP representing the goto-if-zero statement.
     */
    void convertGotoIfZeroToAsm(GotoIfZeroStmtP);

    /**
     * @brief Convert a goto-if-not-zero statement to assembly code.
     *
     * This function generates assembly code to test the value in the 'rax' register and
     * perform a conditional jump to a specified label if the value is not zero.
     *
     * @param gotoIfNotZeroStmt A pointer to a GotoIfNotZeroStmtP representing the goto-if-not-zero statement.
     */
    void convertGotoIfNotZeroToAsm(GotoIfNotZeroStmtP);

    /**
     * @brief Convert a set return value statement to assembly code.
     *
     * This function generates assembly code to set the return value of a function by converting
     * a three-address expression's result to the 'rax' register.
     *
     * @param setReturnValueStmt A pointer to a SetReturnValueStmtP representing the set return value statement.
     */
    void convertSetReturnValueToAsm(SetReturnValueStmtP);

    /**
     * @brief Convert a scope enter statement to assembly code.
     *
     * This function generates assembly code to handle entering a new scope. It allocates stack
     * space for local variables within the scope and updates the 'variableStack' and
     * 'scopeFrameStack' accordingly.
     *
     * @param scopeEnterStmt A pointer to a ScopeEnterStmtP representing the scope enter statement.
     */
    void convertScopeEnterToAsm(ScopeEnterStmtP);

    /**
     * @brief Convert a scope exit statement to assembly code.
     *
     * This function generates assembly code to handle exiting from the current scope. It removes
     * variables from the 'variableStack' that belong to the exiting scope and adjusts the stack
     * pointer (rsp) to deallocate the stack space reserved for the scope's local variables.
     */
    void convertScopeExitToAsm();

    /**
     * @brief Convert a function declaration statement to assembly code.
     *
     * This function generates assembly code for function declaration statements. It sets up the
     * function's prologue, initializes the stack frame and allocates space for local temporaries
     * usage.
     *
     * @param functionDeclarationStmt Pointer to the function declaration statement.
     */
    void convertFunctionDeclarationToAsm(FunctionDeclarationStmtP);

    /**
     * @brief Generate assembly code for function exit.
     *
     * This function generates assembly code for function epilogue. It restores the stack frame
     * and returns from the function using the 'leave' and 'ret' instructions.
     */
    void convertFunctionExitToAsm();

    /**
     * @brief Generate assembly code for a function call.
     *
     * This function generates assembly code to call a function. It increments the stack overflow
     * counter ('r8') to track recursive function calls, calls the target function, and
     * decrements the stack overflow counter afterwards.
     *
     * @param funcName The name of the function to be called.
     */
    void generateAsmFunctionCall(const std::string& funcName);

    /**
     * @brief Get the corresponding 'a' register by size.
     *
     * @param size The size of the register (in bytes).
     * @return The name of the corresponding register ('al', 'ax', 'eax', 'rax').
     */
    static std::string getAxRegisterBySize(int);

    /**
     * @brief Calculate the size (in bytes) of a variable based on its type, pointer status, and array size.
     *
     * If the variable is a pointer, it returns the size of a pointer ('PTR_SIZE').
     * If the variable is an array, it calculates the total size by multiplying the element
     * size ('typeSizes[type]') with the array size. Otherwise, it returns the size of the variable
     * based on its type from the 'typeSizes' map.
     *
     * @param type The type of the variable.
     * @param isPtr A boolean indicating whether the variable is a pointer.
     * @param arrSize The size of the array (if applicable).
     * @return The size (in bytes) of the variable.
     */
    static int sizeByTypeAndPtr(VariableType, bool, int);
};

inline std::unordered_map<VariableType, int> Generator::typeSizes = {
        {VariableType::longType, 8},
        {VariableType::intType,  4},
        {VariableType::charType, 1},
};

inline std::unordered_map<int, std::string> Generator::sizeIdentifiers = {
        {8, "QWORD"},
        {4, "DWORD"},
        {2, "WORD"},
        {1, "BYTE"},
};

#endif //COMPILER_GENERATION_H
