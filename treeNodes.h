// treeNodes.h

#ifndef COMPILER_TREENODES_H
#define COMPILER_TREENODES_H

#include <utility>
#include <unordered_set>

#include "lexer.h"

enum class VariableType {
    intType,
    intPtrType,
    charType,
    charPtrType,
};

class Variable {
public:
    std::string name;
    VariableType type;
    int arrSize;

    Variable(std::string name, VariableType type, int arrSize=0) {
        this->name = std::move(name);
        this->type = type;
        this->arrSize = arrSize;
    }

    bool operator == (const Variable& Ref) const
    {
        return (this->name == Ref.name) && (this->type == Ref.type) && (this->arrSize == Ref.arrSize);
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
    Token val;

    explicit TerminalNodeExpr(Token t) {
        this->val = std::move(t);
    }
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

    explicit NodeImIntTerminal(Token t) : TerminalNodeExpr(std::move(t)) {
    }
};

class NodeIdentTerminal : public TerminalNodeExpr {
public:

    explicit NodeIdentTerminal(Token t) : TerminalNodeExpr(std::move(t)) {
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
class NodePointerAssignmentStmt : public NodeStmt {
public:
    Variable pointer;
    Variable variable;

    NodePointerAssignmentStmt(Variable ptr, Variable var) : pointer(std::move(ptr)), variable(std::move(var)) {
    }

    ~NodePointerAssignmentStmt() override = default;
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
        for (int i = 0; i < stmts.size(); ++i) {
            delete stmts[i];
        }
    }
};

typedef NodeScope *NodeScopeP;
typedef NodeStmt *NodeStmtP;
//typedef NodeAssignmentStmt *NodeAssignmentStmtP;
typedef NodePrimitiveAssignmentStmt *NodePrimitiveAssignmentStmtP;
typedef NodePointerAssignmentStmt *NodePointerAssignmentStmtP;
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
typedef NodeIdentTerminal *NodeIdentTerminalP;

#endif //COMPILER_TREENODES_H
