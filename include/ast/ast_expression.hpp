#ifndef AST_EXPRESSION_HPP
#define AST_EXPRESSION_HPP


#include "ast_node.hpp"



enum OperatorType
{
    LE_OP_TYPE = 257,
    GE_OP_TYPE = 258,
    EQ_OP_TYPE = 259,
    NE_OP_TYPE = 260,
    INC_OP_TYPE = 261,
    DEC_OP_TYPE = 262,
    AND_OP_TYPE = 263,
    OR_OP_TYPE = 264,
    ADD_ASSIGN_OP_TYPE = 265,
    SUB_ASSIGN_OP_TYPE = 266,
    MUL_ASSIGN_OP_TYPE = 267,
    LEFT_OP_TYPE = 268,
    RIGHT_OP_TYPE = 269,
};



class DeclarationSpecifiers;


class Expression : public Node
{
private:
    bool isIdentifier_;


public:
    Expression();
    virtual void EmitRISCLeft(std::ostream &stream, std::string destReg, Context &context) const = 0;
    bool IsIdentifier() const;
    void SetIsIdentifier(bool isIdentifier);

};

class ExpressionOperator : public Node
{
private:
    int operator_;

public:
    ExpressionOperator(int op);
    void EmitRISC(std::ostream &stream, std::string srcReg, Context &context) const override;
    void Print(std::ostream &stream) const override;
    int GetOperator() const;
};


class PostfixExpression : public Expression
{
public:
    PostfixExpression();
    virtual bool GetSymbolOffset(Context &context, SymbolOffset **ppSymbolOffset) const = 0;


};

class IdentifierExpression : public PostfixExpression
{
private:
    std::string identifier_;

public:
    IdentifierExpression(std::string identifier);
    ~IdentifierExpression();
    void EmitRISC(std::ostream &stream, std::string destReg, Context &context) const override;
    void Print(std::ostream &stream) const override;
    void EmitRISCLeft(std::ostream &stream, std::string srcReg, Context &context) const;
    bool GetSymbolOffset(Context &context, SymbolOffset **ppSymbolOffset) const;
    std::string GetIdentifier() const;

};

class ArrayExpression : public Expression
{
private:
    Node *size_;
    PostfixExpression *identifier_;

public:
    ArrayExpression(Node *  identifier, Node * size);
    ~ArrayExpression();
    void EmitRISC(std::ostream &stream, std::string destReg, Context &context) const override;
    void Print(std::ostream &stream) const override;
    void EmitRISCLeft(std::ostream &stream, std::string srcReg, Context &context) const;


};



class PostfixOpExpression : public Expression
{
private:
    ExpressionOperator *operator_;
    Node *left_;


public:
    PostfixOpExpression(Node *left, Node *op);
    PostfixOpExpression(Node *left, int op);
    ~PostfixOpExpression();

    void EmitRISC(std::ostream &stream, std::string desReg, Context &context) const override;
    void Print(std::ostream &stream) const override;
    void EmitRISCLeft(std::ostream &stream, std::string srcReg, Context &context) const;

};



class UnaryExpression : public Expression
{
private:
    ExpressionOperator *operator_;
    Node *right_;


public:
    UnaryExpression(Node *op, Node *right);
    UnaryExpression(int op, Node *right);
    ~UnaryExpression();

    void EmitRISC(std::ostream &stream, std::string desReg, Context &context) const override;
    void Print(std::ostream &stream) const override;
    void EmitRISCLeft(std::ostream &stream, std::string srcReg, Context &context) const;
    int GetOperator() const;
    Node *GetLeft() const;
    Node *GetRight() const;

};

class GenericExpression : public Expression
{
private:
    ExpressionOperator *operator_;
    Node *left_;
    Node *right_;


public:
    GenericExpression(Node *left, Node *op, Node *right);
    GenericExpression(Node *left, int op, Node *right);
    ~GenericExpression();

    void EmitRISC(std::ostream &stream, std::string desReg, Context &context) const override;
    void Print(std::ostream &stream) const override;
    void EmitRISCLeft(std::ostream &stream, std::string srcReg, Context &context) const;
    void ProcessPointerOffset(std::ostream &stream, std::string desReg, Context &context) const;
    int GetOperator() const;
    Node *GetLeft() const;
    Node *GetRight() const;

};



class FunctionCallExpression : public Expression
{
private:
    IdentifierExpression *identifier_;
    Node *argument_expression_list_;

public:
    FunctionCallExpression(Node *  identifier);
    FunctionCallExpression(Node *  identifier, Node * argument_expression_list_);
    ~FunctionCallExpression();
    void EmitRISC(std::ostream &stream, std::string destReg, Context &context) const override;
    void Print(std::ostream &stream) const override;
    void EmitRISCLeft(std::ostream &stream, std::string srcReg, Context &context) const;
    void SaveOneRegisters(std::ostream &stream, Context &context, const std::string &registerNum) const;
    void RestoreOneRegisters(std::ostream &stream, Context &context, const std::string &registerNum) const;


};


class SizeofExpression : public Expression
{
private:
    bool isTypeName_;
    Node *expression_;
    DeclarationSpecifiers *declarationSpecifiers;

public:
    SizeofExpression(Node *expression, bool isTypeName);
    ~SizeofExpression();
    void EmitRISC(std::ostream &stream, std::string destReg, Context &context) const override;
    void Print(std::ostream &stream) const override;
    void EmitRISCLeft(std::ostream &stream, std::string srcReg, Context &context) const;


};


class StringExpression : public Expression
{
private:
    std::string content_;

public:
    StringExpression(const std::string &content);
    ~StringExpression();
    void EmitRISC(std::ostream &stream, std::string destReg, Context &context) const override;
    void Print(std::ostream &stream) const override;
    void EmitRISCLeft(std::ostream &stream, std::string srcReg, Context &context) const;


};


#endif
