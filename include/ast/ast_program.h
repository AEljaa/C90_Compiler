#ifndef AST_PROGRAM
#define AST_PROGRAM

#include "ast_node.hpp"



class Program : public Node
{
private:
    NodeList *globalDeclarations;
    NodeList *stringDeclarations;
    NodeList *functionDefinitions;
    NodeList *functionDeclarations;
    NodeList *printTranslationList;

public:
    Program();
    ~Program();
    void EmitRISC(std::ostream &stream, std::string destReg, Context &context) const override;
    void Print(std::ostream &stream) const override;

    void AddGlobalDeclarations(Node *globalDeclaration, bool isToPrint);
    void AddStringDeclarations(Node *stringDeclaration);
    void AddFunctionDefinition(Node *functionDefinition);

private:
    bool IsFunctionDeclarations(Node *globalDeclaration) const;
    void AddFunctionDeclarations(Node *globalDeclaration);


};

#endif




