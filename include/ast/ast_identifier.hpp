#ifndef AST_IDENTIFIER_HPP
#define AST_IDENTIFIER_HPP


#include "ast_declarator.hpp"


class Identifier : public Declarator
{
private:
    std::string identifier_;

public:
    Identifier(std::string identifier): Declarator(false), identifier_(identifier){};
    ~Identifier(){};
    void EmitRISC(std::ostream &stream, std::string destReg, Context &context) const override;
    void Print(std::ostream &stream) const override;
    int GetSize() const override;
    std::string GetIdentifier() const override;
    void InitializerLocalDeclaration(std::ostream &stream, std::string desReg, Context &context) const override;

};

#endif
