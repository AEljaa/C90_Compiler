#include "../include/ast/ast_function_definition.hpp"
#include "../include/ast/ast_declarator.hpp"
#include "../include/ast/ast_program.h"
#include "../include/ast/ast_identifier.hpp"
#include "../include/ast/ast_direct_declarator.hpp"


Program *GetProgram();


FunctionDefinition *currentFunction = NULL;

FunctionDefinition::FunctionDefinition() 
    : declaration_specifiers_(NULL), declarator_(NULL), compound_statement_(NULL)
{
    localDeclarations = new NodeList();
}

FunctionDefinition::~FunctionDefinition()
{
    localDeclarations->clear();

    if (NULL != declaration_specifiers_)
    {
        delete declaration_specifiers_;
        declaration_specifiers_ = NULL;
    }
    if (NULL != declarator_)
    {
        delete declarator_;
        declarator_ = NULL;
    }
    if (NULL != compound_statement_)
    {
        delete compound_statement_;
        compound_statement_ = NULL;
    }
    if (NULL != localDeclarations)
    {
        delete localDeclarations;
        localDeclarations = NULL;
    }
}


void FunctionDefinition::SetFunctionDefinition(Node *declaration_specifiers, Node *declarator, Node *compound_statement)
{
    declaration_specifiers_ = declaration_specifiers;
    declarator_ = declarator;
    compound_statement_ = compound_statement;
}


void FunctionDefinition::AddLocalDeclarationsList(Node *localDeclarationsList)
{
    DeclarationSpecifiers declarationSpecifiers;
    NodeList *declarationList = NULL;
    Declaration *declaration = NULL;
    NodeList *declarationSpecifiersList = NULL;

    declarationList = (NodeList *)localDeclarationsList;
    for (int loop = 0; loop < declarationList->size(); loop++)
    {
        declaration = (Declaration *)declarationList->at(loop);
        declarationSpecifiersList = (NodeList *)declaration->GetDeclarationSpecifiers();
        declarationSpecifiers.SetDeclarationSpecifiers(declarationSpecifiersList, false);

        if (declarationSpecifiers.isGlobalVariable)
        {
            GetProgram()->AddGlobalDeclarations(declaration, false);
        }
        else
        {
            AddLocalDeclarations(declaration);
        }
    }
}


void FunctionDefinition::EmitRISC(std::ostream &stream, std::string destReg, Context &context) const
{
    // Emit assembler directives.
    // TODO: these are just examples ones, make sure you understand
    // the concept of directives and correct them.
    std::vector<std::string> savedRegisterList;

    stream << ".globl " << GetFunctionName() << std::endl;

    declarator_->EmitRISC(stream, destReg, context);

    SaveFunctionContext(savedRegisterList, stream, context);

    if (compound_statement_ != nullptr)
    {
        compound_statement_->EmitRISC(stream, destReg, context);
    }

    stream << context.GetReturnLabel() << ":" << std::endl;
    RestoreFunctionContext(savedRegisterList, stream, context);

    stream << "      ";
    stream << "ret" << std::endl;
}

void FunctionDefinition::Print(std::ostream &stream) const
{
    declaration_specifiers_->Print(stream);
    stream << " ";

    declarator_->Print(stream);
    stream << std::endl;

    if (compound_statement_ != nullptr)
    {
        compound_statement_->Print(stream);
    }
    stream << std::endl;
}


void FunctionDefinition::AddLocalDeclarations(Node *localDeclaration)
{
    DeclarationSpecifiers declarationSpecifiers;
    Declaration *declaration = NULL;
    Declarator *declarator = NULL;
    NodeList *declarationSpecifiersList = NULL;
    NodeList *declaratorList = NULL;

    declaration = (Declaration *)localDeclaration;
    declarationSpecifiersList = (NodeList *)declaration->GetDeclarationSpecifiers();
    declaratorList = (NodeList *)declaration->GetInitDeclaratorList();

    declarationSpecifiers.SetDeclarationSpecifiers(declarationSpecifiersList, false);

    for (int loop = 0; loop < declaratorList->size(); loop++)
    {
        declarator = (Declarator *)declaratorList->at(loop);
        declarator->SetDeclarationSpecifiers(declarationSpecifiers);
        localDeclarations->PushBack(declarator);
    }

}


void FunctionDefinition::SaveFunctionContext(std::vector<std::string> &savedRegisterList, std::ostream &stream, Context &context) const
{
    context.SetFunctionArgumentsCount(GetFunctionArgumentsCount());
    context.SetCurrentFunctionName(GetFunctionName().c_str());
    context.functionInit();

    InitParameterList(context);
    AllocateLocalDeclarationsSpace(context);
    SaveRegisters(savedRegisterList, stream, context);
}

void FunctionDefinition::RestoreFunctionContext(std::vector<std::string> &savedRegisterList, std::ostream &stream, Context &context) const
{
    RestoreRegisters(savedRegisterList, stream, context);
}

void FunctionDefinition::InitParameterList(Context &context) const
{
    DirectDeclarator *directDeclarator = NULL;
    Parameter_Declaration *parameterDeclaration = NULL;

    directDeclarator = (DirectDeclarator *)declarator_;
    for (int loop = 0; loop < directDeclarator->GetParameterCount(); loop++)
    {
        parameterDeclaration = (Parameter_Declaration *)directDeclarator->GetParameter(loop);
        context.AddParameterSymbol(parameterDeclaration->GetIdentifier(), loop);
    }
}

void FunctionDefinition::AllocateLocalDeclarationsSpace(Context &context) const
{
    Declarator *declarator = NULL;

    for (int loop = 0; loop < localDeclarations->size(); loop++)
    {
        declarator = (Declarator *)localDeclarations->at(loop);
        context.AddLocalSymbol(declarator);
    }
}


int FunctionDefinition::GetFunctionArgumentsCount() const
{
    DirectDeclarator *directDeclarator = NULL;

    directDeclarator = (DirectDeclarator *)declarator_;
    return directDeclarator->GetParameterCount();
}

std::string FunctionDefinition::GetFunctionName() const
{
    DirectDeclarator *directDeclarator = NULL;

    directDeclarator = (DirectDeclarator *)declarator_;

    return directDeclarator->GetIdentifier();
}

void FunctionDefinition::SaveRegisters(std::vector<std::string> &savedRegisterList, std::ostream &stream, Context &context) const
{
    std::string registerNum;

    SaveOneRegisters(stream, context, "ra");
    SaveOneRegisters(stream, context, "s0");
    SaveOneRegisters(stream, context, "s1");

    context.GetUsedSaveRegisterList(savedRegisterList);
    for (std::vector<std::string> ::size_type loop = 0;
        loop < savedRegisterList.size();
        loop++)
    {
        registerNum = savedRegisterList[loop];
        SaveOneRegisters(stream, context, registerNum);
    }

    stream << "      ";
    stream << "add " << "s0" << ", " << "sp" << ", " << "zero" << std::endl;

    stream << "      ";
    stream << "addi " << "sp" << ", " << "sp" << ", " << context.GetLocalOffset() << std::endl;

}

void FunctionDefinition::RestoreRegisters(std::vector<std::string> &savedRegisterList, std::ostream &stream, Context &context) const
{
    std::string registerNum;

    stream << "      ";
    stream << "add " << "sp" << ", " << "s0" << ", " << "zero" << std::endl;

    for (std::vector<std::string> ::size_type loop = 0;
        loop < savedRegisterList.size();
        loop++)
    {
        registerNum = savedRegisterList[savedRegisterList.size() - 1 - loop];
        RestoreOneRegisters(stream, context, registerNum);
    }

    RestoreOneRegisters(stream, context, "s1");
    RestoreOneRegisters(stream, context, "s0");
    RestoreOneRegisters(stream, context, "ra");
}


void FunctionDefinition::SaveOneRegisters(std::ostream &stream, Context &context, const std::string &registerNum) const
{
    stream << "      ";
    stream << "addi " << "sp" << ", " << "sp" << ", " << "-4" << std::endl;

    stream << "      ";
    stream << "sw " << registerNum << ", " << "0(sp)" << std::endl;
}

void FunctionDefinition::RestoreOneRegisters(std::ostream &stream, Context &context, const std::string &registerNum) const
{
    stream << "      ";
    stream << "lw " << registerNum << ", " << "0(sp)" << std::endl;

    stream << "      ";
    stream << "addi " << "sp" << ", " << "sp" << ", " << "4" << std::endl;
}


