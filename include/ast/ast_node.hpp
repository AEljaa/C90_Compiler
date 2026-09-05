#ifndef AST_NODE_HPP
#define AST_NODE_HPP

#include <iostream>
#include <vector>

#include "ast_context.hpp"

class Node
{
protected:
    std::vector<Node *> branches_;
    bool isGenericExpression_;

public:
    Node(): isGenericExpression_(false) {};
    virtual ~Node();
    virtual void EmitRISC(std::ostream &stream, std::string destReg, Context &context) const = 0;
    virtual void Print(std::ostream &stream) const = 0;

    bool IsGenericExpression() const
    {
        return isGenericExpression_;
    }

    void SetIsGenericExpression(bool isGenericExpression)
    {
        isGenericExpression_ = isGenericExpression;
    }
};

// Represents a list of nodes.
class NodeList : public Node
{
private:
    std::vector<Node *> nodes_;

public:
    NodeList() {}
    NodeList(Node *first_node) : nodes_({ first_node }) {}

    ~NodeList()
    {
        for (auto node : nodes_)
        {
            delete node;
        }
    }

    void PushBack(Node *item);
    int size() const;
    Node *at(int index);
    void clear();

    virtual void EmitRISC(std::ostream &stream, std::string desReg, Context &context) const override;
    virtual void Print(std::ostream &stream) const override;
};

#endif
