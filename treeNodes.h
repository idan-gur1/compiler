// treeNodes.h

#ifndef COMPILER_TREENODES_H
#define COMPILER_TREENODES_H

#include <utility>

#include "lexer.h"

class NodeExpr {
public:
    virtual ~NodeExpr() = default;
};

/*class NodeTerm : public NodeExpr{
public:
    ~NodeTerm() override = default;
};

class NodeFactor : public NodeTerm {
public:
    ~NodeFactor() override = default;
};*/

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

// old factors
/*class NodeParenthesisFactor : public NodeFactor {
public:
    NodeExpr *expr;

    explicit NodeParenthesisFactor(NodeExpr *expr) {
        this->expr = expr;
    }

    ~NodeParenthesisFactor() override {
        delete expr;
    }
};

class NodeImIntFactor : public NodeFactor {
public:
    Token val;

    explicit NodeImIntFactor(Token t) {
        this->val = std::move(t);
    }
};

class NodeIdentFactor : public NodeFactor {
public:
    Token val;

    explicit NodeIdentFactor(Token t) {
        this->val = std::move(t);
    }
};*/

// old terms
/*class NodeMultTerm : public NodeTerm {
public:
    NodeTerm *left;
    NodeTerm *right;

    NodeMultTerm(NodeTerm *left, NodeTerm *right) {
        this->left = left;
        this->right = right;
    }

    ~NodeMultTerm() override {
        delete left;
        delete right;
    }
};

class NodeDivTerm : public NodeTerm {
public:
    NodeTerm *left;
    NodeTerm *right;

    NodeDivTerm(NodeTerm *left, NodeTerm *right) {
        this->left = left;
        this->right = right;
    }

    ~NodeDivTerm() override {
        delete left;
        delete right;
    }
};

class NodeFactorTerm : public NodeTerm {
public:
    NodeFactor *factor;

    explicit NodeFactorTerm(NodeFactor *factor) {
        this->factor = factor;
    }

    ~NodeFactorTerm() override {
        delete factor;
    }
};*/


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

/*class NodeTermExpr : public NodeExpr {
public:
    NodeTerm *term;

    explicit NodeTermExpr(NodeTerm *term) {
        this->term = term;
    }

    ~NodeTermExpr() override {
        delete term;
    }
};*/

typedef NodeExpr *NodeExprP;
typedef NodeAddExpr *NodeAddExprP;
typedef NodeSubExpr *NodeSubExprP;
typedef NodeMultExpr *NodeMultExprP;
typedef NodeDivExpr *NodeDivExprP;
typedef NodeImIntTerminal *NodeImIntTerminalP;
typedef NodeIdentTerminal *NodeIdentTerminalP;
/*typedef NodeTermExpr *NodeTermExprP;

typedef NodeTerm *NodeTermP;
typedef NodeMultTerm *NodeMultTermP;
typedef NodeDivTerm *NodeDivTermP;
typedef NodeFactorTerm *NodeFactorTermP;

typedef NodeFactor *NodeFactorP;
typedef NodeImIntFactor *NodeImIntFactorP;
typedef NodeIdentFactor *NodeIdentFactorP;
typedef NodeParenthesisFactor *NodeParenthesisFactorP;*/

#endif //COMPILER_TREENODES_H
