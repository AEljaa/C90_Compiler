#ifndef AST_DIRECT_DECLARATOR_HPP
#define AST_DIRECT_DECLARATOR_HPP


#include "../ast/ast_declarator.hpp"


class DirectDeclarator : public Declarator
{
private:
    Node *identifier_;
    NodeList *parameter_list_;

public:
    DirectDeclarator(Node *identifier);
    DirectDeclarator(Node *identifier, Node *parameter_list);
    ~DirectDeclarator();
    void EmitRISC(std::ostream &stream, std::string destReg, Context &context) const override;
    void Print(std::ostream &stream) const override;
    int GetSize() const override;
    std::string GetIdentifier() const override;

    int GetParameterCount() const;
    Node *GetParameter(int index) const;


};

#endif
