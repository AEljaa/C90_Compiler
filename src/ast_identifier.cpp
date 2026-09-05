#include "../include/ast/ast_identifier.hpp"

void Identifier::EmitRISC(std::ostream &stream, std::string destReg, Context &context) const
{
    stream << identifier_;
}

void Identifier::Print(std::ostream &stream) const
{
    stream << identifier_;
};

int Identifier::GetSize() const
{
    return GetTypeSize();
}


std::string Identifier::GetIdentifier() const
{
    return identifier_;
}


void Identifier::InitializerLocalDeclaration(std::ostream &stream, std::string desReg, Context &context) const
{
    std::string reg1;
    SymbolOffset *pSymbolOffset = NULL;

    if (context.GetSymbolOffset(identifier_, &pSymbolOffset))
    {
        context.GetFreeTempedRegister(reg1);
        GetInitializer()->EmitRISC(stream, reg1, context);
        if (pSymbolOffset->isRegisterSymbol)
        {
            stream << "      ";
            stream << "add " << pSymbolOffset->registerNum << ", " << reg1 << ", " << "zero" << std::endl;
        }
        else
        {
            stream << "      ";
            stream << "sw " << reg1 << ", " << pSymbolOffset->localOffset << "(s0)" << std::endl;
        }
        context.deleteRegister(reg1);
    }
}

