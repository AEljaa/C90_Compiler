#include "../include/ast/ast_type_specifier.hpp"



std::string TypeSpecifier::GetType() const
{
    return type_;
}


void TypeSpecifier::EmitRISC(std::ostream &stream, std::string destReg, Context &context) const
{

}

void TypeSpecifier::Print(std::ostream &stream) const
{
    stream << type_;
}
