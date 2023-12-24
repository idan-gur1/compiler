// treeNodes.h

#ifndef COMPILER_TREENODES_H
#define COMPILER_TREENODES_H

#include <utility>

#include "lexer.h"

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
class NodeAssignmentStmt {
public:
    Token ident;
    NodeExpr *expr;

    NodeAssignmentStmt(Token ident, NodeExpr *expr) {
        this->ident = std::move(ident);
        this->expr = expr;
    }

    ~NodeAssignmentStmt() {
        delete expr;
    }
};
//endregion

class NodeScope {
public:
    std::vector<NodeAssignmentStmt *> stmts;

    explicit NodeScope() = default;

    ~NodeScope() {
        for (int i = 0; i < stmts.size(); ++i) {
            delete stmts[i];
        }
    }
};

typedef NodeScope *NodeScopeP;
typedef NodeAssignmentStmt *NodeAssignmentStmtP;
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
