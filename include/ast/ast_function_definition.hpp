#ifndef AST_FUNCTION_DEFINITION_HPP
#define AST_FUNCTION_DEFINITION_HPP

#include "ast_node.hpp"

class FunctionDefinition : public Node
{
private:
    Node *declaration_specifiers_;
    Node *declarator_;
    Node *compound_statement_;
    NodeList *localDeclarations;

public:
    FunctionDefinition();
    ~FunctionDefinition();
    void SetFunctionDefinition(Node *declaration_specifiers, Node *declarator, Node *compound_statement);
    void AddLocalDeclarationsList(Node *localDeclarationsList);
    void EmitRISC(std::ostream &stream, std::string destReg, Context &context) const override;
    void Print(std::ostream &stream) const override;

private:
    int GetFunctionArgumentsCount() const;
    std::string GetFunctionName() const;
    void SaveFunctionContext(std::vector<std::string> &savedRegisterList, std::ostream &stream, Context &context) const;
    void RestoreFunctionContext(std::vector<std::string> &savedRegisterList, std::ostream &stream, Context &context) const;
    void InitParameterList(Context &context) const;
    void AllocateLocalDeclarationsSpace(Context &context) const;
    void SaveRegisters(std::vector<std::string> &savedRegisterList, std::ostream &stream, Context &context) const;
    void RestoreRegisters(std::vector<std::string> &savedRegisterList, std::ostream &stream, Context &context) const;
    void SaveOneRegisters(std::ostream &stream, Context &context, const std::string &registerNum) const;
    void RestoreOneRegisters(std::ostream &stream, Context &context, const std::string &registerNum) const;
    void AddLocalDeclarations(Node *localDeclaration);


};

#endif
