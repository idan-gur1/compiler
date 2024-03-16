// treeNodes.h

#ifndef COMPILER_TREENODES_H
#define COMPILER_TREENODES_H

#include <utility>
#include <unordered_set>

#include "lexer.h"

enum class VariableType {
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

class ParenthesisNodeExpr : public NodeExpr {
public:
    NodeExpr *expr;

    explicit ParenthesisNodeExpr(NodeExpr *expr) {
        this->expr = expr;
    }

    ~ParenthesisNodeExpr() override {
        delete expr;
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

class NodeImIntTerminal : public TerminalNodeExpr {
public:
    std::string value;
    explicit NodeImIntTerminal(const Token& t) : value(t.val) {
    }
};

/*class NodeIdentTerminal : public TerminalNodeExpr {
public:

    explicit NodeIdentTerminal(Token t) : TerminalNodeExpr(std::move(t)) {
    }
};*/
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
//endregion

//region Statement nodes
class NodeStmt {
public:
    virtual ~NodeStmt() = default;
};

//class NodeAssignmentStmt : public NodeStmt{
//public:
//    Token ident;
//    NodeExpr *expr;
//
//    NodeAssignmentStmt(Token ident, NodeExpr *expr) {
//        this->ident = std::move(ident);
//        this->expr = expr;
//    }
//
//    ~NodeAssignmentStmt() override {
//        delete expr;
//    }
//    ~NodeAssignmentStmt() override = default;
//};
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

class NodePointerAddrAssignmentStmt : public NodeStmt {
public:
    Variable pointer;
    Variable variable;

    NodePointerAddrAssignmentStmt(Variable ptr, Variable var) : pointer(std::move(ptr)), variable(std::move(var)) {
    }

    ~NodePointerAddrAssignmentStmt() override = default;
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
//endregion

class NodeScope {
public:
    std::vector<NodeStmt *> stmts;
    std::vector<Variable> vars;

    explicit NodeScope() = default;

    ~NodeScope() {
        for (auto &stmt: stmts) {
            delete stmt;
        }
    }
};

typedef NodeScope *NodeScopeP;
typedef NodeStmt *NodeStmtP;
//typedef NodeAssignmentStmt *NodeAssignmentStmtP;
typedef NodePrimitiveAssignmentStmt *NodePrimitiveAssignmentStmtP;
typedef NodePointerAddrAssignmentStmt *NodePointerAddrAssignmentStmtP;
typedef NodePointerValueAssignmentStmt *NodePointerValueAssignmentStmtP;
typedef NodeArrayAssignmentStmt *NodeArrayAssignmentStmtP;
typedef NodeExpr *NodeExprP;
typedef BinaryNodeExpr *BinaryNodeExprP;
typedef ParenthesisNodeExpr *ParenthesisNodeExprP;
typedef TerminalNodeExpr *TerminalNodeExprP;
typedef NodeAddExpr *NodeAddExprP;
typedef NodeSubExpr *NodeSubExprP;
typedef NodeMultExpr *NodeMultExprP;
typedef NodeDivExpr *NodeDivExprP;
typedef NodeImIntTerminal *NodeImIntTerminalP;
typedef NodeVariableTerminal *NodeVariableTerminalP;
typedef NodeSubscriptableVariableTerminal *NodeSubscriptableVariableTerminalP;

#endif //COMPILER_TREENODES_H
