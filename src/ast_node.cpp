#include "../include/ast/ast_node.hpp"

Node::~Node()
{
    for (auto branch : branches_)
    {
        delete branch;
    }
}

void NodeList::PushBack(Node *item)
{
    if (NULL != item)
    {
        nodes_.push_back(item);
    }
}

int NodeList::size() const
{
    return (int)nodes_.size();
}

Node *NodeList::at(int index)
{
    return nodes_[index];
}

void NodeList::clear()
{
    nodes_.clear();
}

void NodeList::EmitRISC(std::ostream &stream, std::string destReg, Context &context) const
{
    for (auto node : nodes_)
    {
        if (node == nullptr)
        {
            continue;
        }
        node->EmitRISC(stream, "a0", context);
    }
}

void NodeList::Print(std::ostream &stream) const
{
    for (auto node : nodes_)
    {
        if (node == nullptr)
        {
            continue;
        }
        node->Print(stream);
    }
}
