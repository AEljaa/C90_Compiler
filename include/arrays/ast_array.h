

#ifndef AST_ARRAY_DECLARATION_HPP
#define AST_ARRAY_DECLARATION_HPP



#include "../ast/ast_declarator.hpp"


class Array_Declaration : public Declarator
{
private:
    Node *size_;
    Node *identifier_;


public:
    Array_Declaration(Node *  identifier, Node * size): Declarator(true), size_(size), identifier_(identifier) {}
    ~Array_Declaration()
    {
        delete size_;
        delete identifier_;
    }

    int GetSize() const;
    std::string GetIdentifier() const;
    void EmitRISC(std::ostream &stream, std::string destReg, Context &context) const override;
    void Print(std::ostream &stream) const override;


};




#endif


