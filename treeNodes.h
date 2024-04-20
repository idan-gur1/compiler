// treeNodes.h

#ifndef COMPILER_TREENODES_H
#define COMPILER_TREENODES_H

#include <utility>
#include <vector>
#include <unordered_set>
#include <variant>

#include "lexer.h"

enum class VariableType {
    voidType,  // For functions
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
public:
    NodeVariableTerminal *target;

    explicit AddrNodeExpr(NodeVariableTerminal *target) {
        this->target = target;
    }

    ~AddrNodeExpr() override {
        delete target;
    }
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

class NodePointerValueAssignmentStmt : public NodeStmt {
public:
    Variable pointer;
    NodeExpr *expr;

    NodePointerValueAssignmentStmt(Variable ptr, NodeExpr *expr) : pointer(std::move(ptr)) {
        this->expr = expr;
    }

    ~NodePointerValueAssignmentStmt() override {
        delete expr;
    };
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
    std::vector<NodeStmt *> stmts;
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

class NodePointerAddrAssignmentStmt : public NodeStmt {
public:
    Variable pointer;
//    AddrNodeExpr *addressable;
    std::variant<NodeFunctionCall *, AddrNodeExpr *> addressable;

    NodePointerAddrAssignmentStmt(Variable ptr, AddrNodeExpr *addressable) : pointer(std::move(ptr)) {
        this->addressable = addressable;
    }

    NodePointerAddrAssignmentStmt(Variable ptr, NodeFunctionCall *addressable) : pointer(std::move(ptr)) {
        this->addressable = addressable;
    }

    ~NodePointerAddrAssignmentStmt() override {
        if (holds_alternative<NodeFunctionCall *>(addressable)) {
            delete get<NodeFunctionCall *>(addressable);
        } else if (holds_alternative<AddrNodeExpr *>(addressable)) {
            delete get<AddrNodeExpr *>(addressable);
        }
    };
};

class ProgramTree {
public:
    std::vector<NodeFunction *> functions;

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
typedef NodePointerAddrAssignmentStmt *NodePointerAddrAssignmentStmtP;
typedef NodePointerValueAssignmentStmt *NodePointerValueAssignmentStmtP;
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
typedef NodeAddExpr *NodeAddExprP;
typedef NodeSubExpr *NodeSubExprP;
typedef NodeMultExpr *NodeMultExprP;
typedef NodeDivExpr *NodeDivExprP;
typedef NodeLogicalOrExpr *NodeLogicalOrExprP;
typedef NodeLogicalAndExpr *NodeLogicalAndExprP;
typedef NodeBoolEqualsExpr *NodeBoolEqualsExprP;
typedef NodeBoolNotEqualsExpr *NodeBoolNotEqualsExprP;
typedef NodeBiggerThanExpr *NodeBiggerThanExprP;
typedef NodeBiggerThanEqualExpr *NodeBiggerThanEqualExprP;
typedef NodeLessThanExpr *NodeLessThanExprP;
typedef NodeLessThanEqualExpr *NodeLessThanEqualExprP;
typedef NodeImIntTerminal *NodeImIntTerminalP;
typedef NodeVariableTerminal *NodeVariableTerminalP;
typedef NodeSubscriptableVariableTerminal *NodeSubscriptableVariableTerminalP;

#endif //COMPILER_TREENODES_H
