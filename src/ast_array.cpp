

#include "../include/arrays/ast_array.h"
#include "../include/ast/ast_constant.hpp"
#include "../include/ast/ast_identifier.hpp"



int Array_Declaration::GetSize() const
{
    return GetTypeSize() * ((IntConstant *)size_)->GetValue();
}

std::string Array_Declaration::GetIdentifier() const
{
    return ((Identifier *)identifier_)->GetIdentifier();
}

void Array_Declaration::EmitRISC(std::ostream &stream, std::string destReg, Context &context) const
{
    identifier_->EmitRISC(stream, destReg, context);


    stream << ":" << "\t" << ".zero" << "\t" << GetSize() ;

    stream << std::endl;
}

void Array_Declaration::Print(std::ostream &stream) const
{
    identifier_->Print(stream);
    stream << "[" ;
    size_->Print(stream);
    stream << "]";
}

