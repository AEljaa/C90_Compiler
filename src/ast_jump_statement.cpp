#include "../include/ast/ast_jump_statement.hpp"

void ReturnStatement::EmitRISC(std::ostream &stream, std::string destReg, Context &context) const
{
    if (expression_ != nullptr)
    {
        expression_->EmitRISC(stream, destReg, context);
    }
//    stream << "ret" << std::endl;
    stream << "      ";
    stream << "jal " << "zero" << ", " << context.GetReturnLabel() << std::endl;
}

void ReturnStatement::Print(std::ostream &stream) const
{
    stream << "return";
    if (expression_ != nullptr)
    {
        stream << " ";
        expression_->Print(stream);
    }
    stream << ";";
}
