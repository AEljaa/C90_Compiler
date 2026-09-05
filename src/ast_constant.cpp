#include "../include/ast/ast_constant.hpp"


int IntConstant::GetValue() const
{
    return value_;
}

void IntConstant::EmitRISC(std::ostream &stream, std::string destReg, Context &context) const
{
    stream << "      ";
    stream << "li " << destReg << ", " << value_ << std::endl; //todo -> fetch register name
}

void IntConstant::Print(std::ostream &stream) const
{
    stream << value_;
}

void IntConstant::EmitRISCLeft(std::ostream &stream, std::string destReg, Context &context) const
{

}


