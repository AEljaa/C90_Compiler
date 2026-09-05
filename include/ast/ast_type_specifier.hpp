#ifndef AST_TYPE_SPECIFIER
#define AST_TYPE_SPECIFIER



#include "ast_declarator.hpp"



class TypeSpecifier : public DeclarationSpecifiersBase
{
private:
    std::string type_;

public:
    TypeSpecifier(std::string type): DeclarationSpecifiersBase(true), type_(type){};
    ~TypeSpecifier(){};
    void EmitRISC(std::ostream &stream, std::string destReg, Context &context) const override;
    void Print(std::ostream &stream) const override;

    std::string GetType() const;

};

#endif
