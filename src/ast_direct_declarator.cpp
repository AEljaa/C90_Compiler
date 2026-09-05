#include "../include/ast/ast_direct_declarator.hpp"
#include "../include/ast/ast_identifier.hpp"



DirectDeclarator::DirectDeclarator(Node *identifier)
    : Declarator(false), identifier_(identifier), parameter_list_(nullptr)
{
    SetIsFunction(true);
}

DirectDeclarator::DirectDeclarator(Node *identifier, Node *parameter_list)
    : Declarator(false), identifier_(identifier), parameter_list_((NodeList *)parameter_list)
{
    SetIsFunction(true);
}

DirectDeclarator::~DirectDeclarator()
{
    if (nullptr != identifier_)
    {
        delete identifier_;
        identifier_ = nullptr;
    }
    if (nullptr != parameter_list_)
    {
        delete parameter_list_;
        parameter_list_ = nullptr;
    }
}


int DirectDeclarator::GetSize() const
{
    return 0;
}

std::string DirectDeclarator::GetIdentifier() const
{
    return ((Identifier *)identifier_)->GetIdentifier();
}

int DirectDeclarator::GetParameterCount() const
{
    if (nullptr == parameter_list_)
    {
        return 0;
    }

    return parameter_list_->size();
}

Node *DirectDeclarator::GetParameter(int index) const
{
    if (nullptr == parameter_list_)
    {
        return 0;
    }

    return parameter_list_->at(index);
}


void DirectDeclarator::EmitRISC(std::ostream &stream, std::string destReg, Context &context) const
{
    identifier_->EmitRISC(stream, destReg, context);
    stream << ":" << std::endl;
}

void DirectDeclarator::Print(std::ostream &stream) const
{
    Parameter_Declaration *parameterDeclaration = NULL;

    identifier_->Print(stream);
    stream << "(";
    for (int loop = 0; loop < GetParameterCount(); loop++)
    {
        parameterDeclaration = (Parameter_Declaration *)GetParameter(loop);
        if (loop > 0)
        {
            stream << ", ";
        }
        parameterDeclaration->Print(stream);
    }
    stream << ")";
}
