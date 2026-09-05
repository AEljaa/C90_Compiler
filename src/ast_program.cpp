

#include "../include/ast/ast_program.h"
#include "../include/ast/ast_function_definition.hpp"
#include "../include/ast/ast_declarator.hpp"



Program *program = NULL;


Program::Program()
{
    globalDeclarations = new NodeList();
    functionDefinitions = new NodeList();
    functionDeclarations = new NodeList();
    stringDeclarations = new NodeList();
    printTranslationList = new NodeList();
}

Program::~Program()
{
    globalDeclarations->clear();
    functionDefinitions->clear();
    functionDeclarations->clear();

    delete globalDeclarations;
    delete functionDefinitions;
    delete functionDeclarations;
    delete stringDeclarations;
    delete printTranslationList;
}


void Program::AddGlobalDeclarations(Node *globalDeclaration, bool isToPrint)
{
    DeclarationSpecifiers declarationSpecifiers;
    Declaration *declaration = NULL;
    Declarator *declarator = NULL;
    NodeList *declarationSpecifiersList = NULL;
    NodeList *declaratorList = NULL;

    if (IsFunctionDeclarations(globalDeclaration))
    {
        AddFunctionDeclarations(globalDeclaration);
    }
    else
    {
        declaration = (Declaration *)globalDeclaration;
        declarationSpecifiersList = (NodeList *)declaration->GetDeclarationSpecifiers();
        declaratorList = (NodeList *)declaration->GetInitDeclaratorList();
        declarationSpecifiers.SetDeclarationSpecifiers(declarationSpecifiersList, true);
        for (int loop = 0; loop < declaratorList->size(); loop++)
        {
            declarator = (Declarator *)declaratorList->at(loop);
            declarator->SetDeclarationSpecifiers(declarationSpecifiers);
            globalDeclarations->PushBack(declarator);
        }
        if (isToPrint)
        {
            printTranslationList->PushBack(globalDeclaration);
        }
    }
}


void Program::AddFunctionDeclarations(Node *globalDeclaration)
{
    Declaration *declaration = NULL;
    Declarator *declarator = NULL;
    NodeList *declaratorList = NULL;

    declaration = (Declaration *)globalDeclaration;
    declaratorList = (NodeList *)declaration->GetInitDeclaratorList();
    for (int loop = 0; loop < declaratorList->size(); loop++)
    {
        declarator = (Declarator *)declaratorList->at(loop);
        functionDeclarations->PushBack(declarator);
    }
    printTranslationList->PushBack(globalDeclaration);
}


void Program::AddFunctionDefinition(Node *functionDefinition)
{
    functionDefinitions->PushBack(functionDefinition);
    printTranslationList->PushBack(functionDefinition);
}


void Program::AddStringDeclarations(Node *stringDeclaration)
{
    stringDeclarations->PushBack(stringDeclaration);
}

bool Program::IsFunctionDeclarations(Node *globalDeclaration) const
{
    DeclarationSpecifiers declarationSpecifiers;
    Declaration *declaration = NULL;
    Declarator *declarator = NULL;
    NodeList *declaratorList = NULL;

    declaration = (Declaration *)globalDeclaration;
    declaratorList = (NodeList *)declaration->GetInitDeclaratorList();

    for (int loop = 0; loop < declaratorList->size(); loop++)
    {
        declarator = (Declarator *)declaratorList->at(loop);
        if (declarator->IsFunction())
        {
            return true;
        }
    }
    return false;
}





void Program::EmitRISC(std::ostream &stream, std::string destReg, Context &context) const
{
    Node *node = nullptr;
    Declarator *declarator = nullptr;
    FunctionDefinition *functionDefinition = nullptr;

    stream << ".text" << std::endl;
    for (int loop = 0; loop < functionDeclarations->size(); loop++)
    {
        declarator = (Declarator *)functionDeclarations->at(loop);
        stream << ".extern" << " " << declarator->GetIdentifier() << std::endl;
    }

    for (int loop = 0; loop < functionDefinitions->size(); loop++)
    {
        node = functionDefinitions->at(loop);
        if (node == nullptr)
        {
            continue;
        }
        functionDefinition = (FunctionDefinition *)node;
        functionDefinition->EmitRISC(stream, "a0", context);
    }

    if ((globalDeclarations->size() > 0) || (stringDeclarations->size() > 0))
    {
        stream << ".data" << std::endl;
        for (int loop = 0; loop < globalDeclarations->size(); loop++)
        {
            declarator = (Declarator *)globalDeclarations->at(loop);
            declarator->EmitRISC(stream, destReg, context);
        }

        stringDeclarations->EmitRISC(stream, destReg, context);
    }
}


void Program::Print(std::ostream &stream) const
{
    Node *node = nullptr;
    FunctionDefinition *functionDefinition = nullptr;

    for (int loop = 0; loop < printTranslationList->size(); loop++)
    {
        node = printTranslationList->at(loop);
        if (node == nullptr)
        {
            continue;
        }
        functionDefinition = (FunctionDefinition *)node;
        functionDefinition->Print(stream);
    }
}












