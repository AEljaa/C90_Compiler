#ifndef AST_STATEMENT_HPP
#define AST_STATEMENT_HPP


#include "ast_node.hpp"


class Statement : public Node
{
private:


public:
    Statement() {};

};


class ExpressionStatement : public Statement
{
private:
    Node *expression_;

public:
    ExpressionStatement(Node *expression);
    ~ExpressionStatement();

    void EmitRISC(std::ostream &stream, std::string desReg, Context &context) const override;
    void Print(std::ostream &stream) const override;
    Node *GetExpression() const;

};

class CompoundStatement : public Statement
{
private:
    Node *declaration_list_;
    Node *statement_list_;


public:
    CompoundStatement(Node *declaration_list, Node *statement_list): declaration_list_(declaration_list), statement_list_(statement_list) {};
    ~CompoundStatement();

    void EmitRISC(std::ostream &stream, std::string desReg, Context &context) const override;
    void Print(std::ostream &stream) const override;

private:
    void InitializerLocalDeclarationsList(Node *localDeclarationsList, std::ostream &stream, std::string desReg, Context &context) const;
    void InitializerLocalDeclarations(Node *localDeclaration, std::ostream &stream, std::string desReg, Context &context) const;

};




class BranchStatement : public Statement
{
private:


public:
    BranchStatement() {};
    void EmitIfBranchRISC(std::ostream &stream,
        Context &context,
        Node *node,
        bool isTrue,
        const char *branchLabel) const;
    bool IsCompareExpression(Node *node) const;


};


class IfStatement : public BranchStatement
{
private:
    Node *expression_;
    Node *statement_;
    Node *elseStatement_;

public:
    IfStatement(Node *expression, Node *statement, Node *elseStatement);
    ~IfStatement();

    void EmitRISC(std::ostream &stream, std::string desReg, Context &context) const override;
    void Print(std::ostream &stream) const override;

};




class WhileStatement : public BranchStatement
{
private:
    Node *expression_;
    Node *statement_;

public:
    WhileStatement(Node *expression, Node *statement);
    ~WhileStatement();

    void EmitRISC(std::ostream &stream, std::string desReg, Context &context) const override;
    void Print(std::ostream &stream) const override;

};


class ForStatement : public BranchStatement
{
private:
    Node *left_;
    Node *middle_;
    Node *right_;
    Node *statement_;

public:
    ForStatement(Node *left, Node *middle, Node *right, Node *statement);
    ~ForStatement();

    void EmitRISC(std::ostream &stream, std::string desReg, Context &context) const override;
    void Print(std::ostream &stream) const override;

};





#endif



