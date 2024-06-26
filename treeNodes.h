// treeNodes.h

#ifndef COMPILER_TREENODES_H
#define COMPILER_TREENODES_H

#include <utility>
#include <vector>
#include <list>
#include <unordered_set>
#include "tokenDefine.h"

/**
 * \Description
 * This file contains the definitions of various classes representing nodes in the abstract syntax tree (AST) 
 * generated by the parser component of the compiler. These nodes represent different constructs of the programming 
 * language being compiled. Each node that has a pointer to part below it in the tree also has a destructor to
 * recursively delete the tree.
 * 
 * \Classes
 * - VariableType: Enumerates the types of variables supported by the language, including void, integer, and character types.\n\n
 * - Variable: Represents a variable with a name, type, pointer type indicator, and array size.\n\n
 * - NodeExpr: Abstract base class for expression nodes in the AST.\n\n
 * - BinaryNodeExpr: Represents binary expressions, such as addition, subtraction, multiplication, etc.\n\n
 * - TerminalNodeExpr: Represents terminal nodes in the AST, such as number literals and identifiers.\n\n
 * - UnaryNodeExpr: Represents unary expressions, such as logical negation and numeric negation.\n\n
 * - NodeParenthesisExpr: Represents expressions enclosed within parentheses.\n\n
 * - NodeLogicalNotExpr: Represents logical NOT expressions.\n\n
 * - NodeNumericNegExpr: Represents numeric negation expressions.\n\n
 * - NodeAddExpr, NodeSubExpr, NodeMultExpr, NodeDivExpr, NodeModuloExpr: Represent arithmetic expressions.\n\n
 * - NodeLogicalOrExpr, NodeLogicalAndExpr: Represent logical OR and logical AND expressions.\n\n
 * - NodeBoolEqualsExpr, NodeBoolNotEqualsExpr: Represent equality and inequality expressions.\n\n
 * - NodeBiggerThanExpr, NodeBiggerThanEqualExpr, NodeLessThanExpr, NodeLessThanEqualExpr: Represent relational expressions.\n\n
 * - NodeImIntTerminal: Represents immediate integer literals.\n\n
 * - NodeVariableTerminal: Represents variable references.\n\n
 * - NodeSubscriptableVariableTerminal: Represents subscriptable variable references.\n\n
 * - AddrNodeExpr: Represents address-of expressions.\n\n
 * - NodeStmt: Abstract base class for statement nodes in the AST.\n\n
 * - NodePrimitiveAssignmentStmt: Represents assignments to primitive variables.\n\n
 * - NodeArrayAssignmentStmt: Represents assignments to array elements.\n\n
 * - NodeScope: Represents a block of code enclosed within curly braces.\n\n
 * - NodeIf: Represents an if statement with optional else block.\n\n
 * - NodeWhile: Represents a while loop statement with optional do-while flag.\n\n
 * - NodeReturnStmt: Represents return statements.\n\n
 * - NodeFunction: Represents function definitions.\n\n
 * - NodeFunctionCall: Represents function calls.\n\n
 * - ProgramTree: Represents the entire AST of the program.\n\n
 */


enum class VariableType {
    voidType,  // For functions
    longType,
    intType,
    charType,
};

class Variable {
public:
    std::string name;
    VariableType type;
    bool ptrType;
    int arrSize;

    Variable(std::string name, VariableType type, bool ptrType, int arrSize = 0) {
        this->name = std::move(name);
        this->type = type;
        this->ptrType = ptrType;
        this->arrSize = arrSize;
    }

    bool operator==(const Variable &Ref) const {
        return (this->name == Ref.name) && (this->type == Ref.type) && (this->ptrType == Ref.ptrType) && (this->arrSize == Ref.arrSize);
    }
};

//region Expression nodes
class NodeExpr {
public:
    virtual ~NodeExpr() = default;
};

class BinaryNodeExpr : public NodeExpr {
public:
    NodeExpr *left;
    NodeExpr *right;

    BinaryNodeExpr(NodeExpr *left, NodeExpr *right) {
        this->left = left;
        this->right = right;
    }

    ~BinaryNodeExpr() override {
        delete left;
        delete right;
    }
};

class TerminalNodeExpr : public NodeExpr {
public:
    /*Token val;

    explicit TerminalNodeExpr(Token t) {
        this->val = std::move(t);
    }*/
    ~TerminalNodeExpr() override = default;
};

class UnaryNodeExpr : public NodeExpr {
public:
    NodeExpr *expr;

    explicit UnaryNodeExpr(NodeExpr *expr) {
        this->expr = expr;
    }

    ~UnaryNodeExpr() override {
        delete expr;
    }
};

class NodeParenthesisExpr : public UnaryNodeExpr {
public:

    explicit NodeParenthesisExpr(NodeExpr *expr) : UnaryNodeExpr(expr) {
    }
};

class NodeLogicalNotExpr : public UnaryNodeExpr {
public:

    explicit NodeLogicalNotExpr(NodeExpr *expr) : UnaryNodeExpr(expr) {
    }
};

class NodeNumericNegExpr : public UnaryNodeExpr {
public:

    explicit NodeNumericNegExpr(NodeExpr *expr) : UnaryNodeExpr(expr) {
    }
};

class NodeAddExpr : public BinaryNodeExpr {
public:

    NodeAddExpr(NodeExpr *left, NodeExpr *right) : BinaryNodeExpr(left, right) {
    }
};

class NodeSubExpr : public BinaryNodeExpr {
public:

    NodeSubExpr(NodeExpr *left, NodeExpr *right) : BinaryNodeExpr(left, right) {
    }
};

class NodeMultExpr : public BinaryNodeExpr {
public:

    NodeMultExpr(NodeExpr *left, NodeExpr *right) : BinaryNodeExpr(left, right) {
    }
};

class NodeDivExpr : public BinaryNodeExpr {
public:

    NodeDivExpr(NodeExpr *left, NodeExpr *right) : BinaryNodeExpr(left, right) {
    }
};

class NodeModuloExpr : public BinaryNodeExpr {
public:

    NodeModuloExpr(NodeExpr *left, NodeExpr *right) : BinaryNodeExpr(left, right) {
    }
};

class NodeLogicalOrExpr : public BinaryNodeExpr {
public:

    NodeLogicalOrExpr(NodeExpr *left, NodeExpr *right) : BinaryNodeExpr(left, right) {
    }
};

class NodeLogicalAndExpr : public BinaryNodeExpr {
public:

    NodeLogicalAndExpr(NodeExpr *left, NodeExpr *right) : BinaryNodeExpr(left, right) {
    }
};

class NodeBoolEqualsExpr : public BinaryNodeExpr {
public:

    NodeBoolEqualsExpr(NodeExpr *left, NodeExpr *right) : BinaryNodeExpr(left, right) {
    }
};

class NodeBoolNotEqualsExpr : public BinaryNodeExpr {
public:

    NodeBoolNotEqualsExpr(NodeExpr *left, NodeExpr *right) : BinaryNodeExpr(left, right) {
    }
};

class NodeBiggerThanExpr : public BinaryNodeExpr {
public:

    NodeBiggerThanExpr(NodeExpr *left, NodeExpr *right) : BinaryNodeExpr(left, right) {
    }
};

class NodeBiggerThanEqualExpr : public BinaryNodeExpr {
public:

    NodeBiggerThanEqualExpr(NodeExpr *left, NodeExpr *right) : BinaryNodeExpr(left, right) {
    }
};

class NodeLessThanExpr : public BinaryNodeExpr {
public:

    NodeLessThanExpr(NodeExpr *left, NodeExpr *right) : BinaryNodeExpr(left, right) {
    }
};

class NodeLessThanEqualExpr : public BinaryNodeExpr {
public:

    NodeLessThanEqualExpr(NodeExpr *left, NodeExpr *right) : BinaryNodeExpr(left, right) {
    }
};

class NodeImIntTerminal : public TerminalNodeExpr {
public:
    std::string value;
    explicit NodeImIntTerminal(const Token& t) : value(t.val) {
    }
};

class NodeVariableTerminal : public TerminalNodeExpr {
public:
    Variable variable;
    explicit NodeVariableTerminal(Variable var) : variable(std::move(var)) {
    }
};

class NodeSubscriptableVariableTerminal : public NodeVariableTerminal {
public:
    NodeExpr *index;
    NodeSubscriptableVariableTerminal(Variable var, NodeExpr *index) : NodeVariableTerminal(std::move(var)) {
        this->index = index;
    }

    ~NodeSubscriptableVariableTerminal() override {
        delete index;
    }
};

class AddrNodeExpr : public NodeExpr {
};

class AddrVarNodeExpr : public AddrNodeExpr {
public:
    NodeVariableTerminal *target;

    explicit AddrVarNodeExpr(NodeVariableTerminal *target) {
        this->target = target;
    }

    ~AddrVarNodeExpr() override {
        delete target;
    }
};

class AddrStrNodeExpr : public AddrNodeExpr {
public:
    std::string value;

    explicit AddrStrNodeExpr(std::string value) : value(std::move(value)) {
    }

    ~AddrStrNodeExpr() override = default;
};
//endregion

//region Statement nodes
class NodeStmt {
public:
    virtual ~NodeStmt() = default;
};

class NodePrimitiveAssignmentStmt : public NodeStmt {
public:
    Variable variable;
    NodeExpr *expr;

    NodePrimitiveAssignmentStmt(Variable var, NodeExpr *expr) : variable(std::move(var)) {
//        this->variable = std::move(variable);
        this->expr = expr;
    }

    ~NodePrimitiveAssignmentStmt() override {
        delete expr;
    }
};

class NodeArrayAssignmentStmt : public NodeStmt {
public:
    Variable array;
    NodeExpr *index;
    NodeExpr *expr;

    NodeArrayAssignmentStmt(Variable arr, NodeExpr *index, NodeExpr *expr) : array(std::move(arr)) {
        this->index = index;
        this->expr = expr;
    }

    ~NodeArrayAssignmentStmt() override {
        delete index;
        delete expr;
    }
};

class NodeScope : public NodeStmt {
public:
    std::list<NodeStmt *> stmts;
    std::vector<Variable> vars;

    explicit NodeScope() = default;

    ~NodeScope() override {
        for (auto &stmt: stmts) {
            delete stmt;
        }
    }
};

class NodeIf : public NodeStmt {
public:
    NodeExpr *expr;
    NodeScope *ifBlock;
    NodeScope *elseBlock;

    NodeIf(NodeExpr *expr, NodeScope *ifBlock, NodeScope *elseBlock) {
        this->expr = expr;
        this->ifBlock = ifBlock;
        this->elseBlock = elseBlock;
    }

    ~NodeIf() override {
        delete expr;
        delete ifBlock;
        delete elseBlock;
    }
};

class NodeWhile : public NodeStmt {
public:
    NodeExpr *expr;
    NodeScope *codeBlock;
    bool isDoWhile;

    NodeWhile(NodeExpr *expr, NodeScope *codeBlock, bool isDoWhile) {
        this->expr = expr;
        this->codeBlock = codeBlock;
        this->isDoWhile = isDoWhile;
    }

    ~NodeWhile() override {
        delete expr;
        delete codeBlock;
    }
};

class NodeReturnStmt : public NodeStmt {
public:
    NodeExpr *expr;

    explicit NodeReturnStmt(NodeExpr *expr) {
        this->expr = expr;
    }

    ~NodeReturnStmt() override {
        delete expr;
    }
};
//endregion

class NodeFunction {
public:
    VariableType returnType;
    bool returnPtr;
    std::string name;
    std::vector<Variable> params;
    NodeScope *scope;

    NodeFunction(VariableType returnType, bool returnPtr, std::string name, std::vector<Variable> params) {
        this->returnType = returnType;
        this->returnPtr = returnPtr;
        this->name = std::move(name);
        this->params = std::move(params);
        this->scope = nullptr;
    }

    ~NodeFunction() {
        delete scope;
    }
};

class NodeFunctionCall : public TerminalNodeExpr, public NodeStmt{
public:
    NodeFunction *function;
    std::vector<NodeExpr *> params;
    NodeFunctionCall(NodeFunction *function, std::vector<NodeExpr *> params) {
        this->function = function;
        this->params = std::move(params);
    }

    ~NodeFunctionCall() override {
        for (auto &param : params) {
            delete param;
        }
    }
};

class ProgramTree {
public:
    std::list<NodeFunction *> functions;

    explicit ProgramTree() = default;

    ~ProgramTree() {
        for (auto &function: functions) {
            delete function;
        }
    }
};

typedef ProgramTree *ProgramTreeP;
typedef NodeFunction *NodeFunctionP;
typedef NodeScope *NodeScopeP;

typedef NodeStmt *NodeStmtP;
typedef NodeIf *NodeIfP;
typedef NodeWhile *NodeWhileP;
typedef NodeReturnStmt *NodeReturnStmtP;
typedef NodePrimitiveAssignmentStmt *NodePrimitiveAssignmentStmtP;
typedef NodeArrayAssignmentStmt *NodeArrayAssignmentStmtP;

typedef NodeExpr *NodeExprP;
typedef BinaryNodeExpr *BinaryNodeExprP;
typedef UnaryNodeExpr *UnaryNodeExprP;
typedef NodeParenthesisExpr *NodeParenthesisExprP;
typedef NodeLogicalNotExpr *NodeLogicalNotExprP;
typedef NodeNumericNegExpr *NodeNumericNegExprP;
typedef TerminalNodeExpr *TerminalNodeExprP;
typedef AddrNodeExpr *AddrNodeExprP;
typedef NodeFunctionCall *NodeFunctionCallP;
typedef NodeImIntTerminal *NodeImIntTerminalP;
typedef NodeVariableTerminal *NodeVariableTerminalP;
typedef NodeSubscriptableVariableTerminal *NodeSubscriptableVariableTerminalP;

#endif //COMPILER_TREENODES_H
