

#include "../include/ast/ast_expression.hpp"
#include "../include/ast/ast_declarator.hpp"



Expression::Expression()
    : isIdentifier_(false)
{

}

bool Expression::IsIdentifier() const
{
    return isIdentifier_;
}

void Expression::SetIsIdentifier(bool isIdentifier)
{
    isIdentifier_ = isIdentifier;
}

PostfixExpression::PostfixExpression()
{

}

IdentifierExpression::IdentifierExpression(std::string identifier)
    : identifier_(identifier)
{
    SetIsIdentifier(true);
}


IdentifierExpression::~IdentifierExpression()
{

}

void IdentifierExpression::EmitRISC(std::ostream &stream, std::string desReg, Context &context) const
{
    SymbolOffset *pSymbolOffset = NULL;

    if (context.GetSymbolOffset(identifier_, &pSymbolOffset))
    {
        if (pSymbolOffset->isRegisterSymbol)
        {
            stream << "      ";
            stream << "add " << desReg << ", " << pSymbolOffset->registerNum << ", " << "zero" << std::endl;
        }
        else
        {
            stream << "      ";
            stream << "lw " << desReg << ", " << pSymbolOffset->localOffset << "(s0)" << std::endl;
        }
    }
}


void IdentifierExpression::Print(std::ostream &stream) const
{
    stream << identifier_;
}

void IdentifierExpression::EmitRISCLeft(std::ostream &stream, std::string srcReg, Context &context) const
{
    SymbolOffset *pSymbolOffset = NULL;

    if (context.GetSymbolOffset(identifier_, &pSymbolOffset))
    {
        if (pSymbolOffset->isRegisterSymbol)
        {
            stream << "      ";
            stream << "add " << pSymbolOffset->registerNum << ", " << srcReg << ", " << "zero" << std::endl;
        }
        else
        {
            stream << "      ";
            stream << "sw " << srcReg << ", " << pSymbolOffset->localOffset << "(s0)" << std::endl;
        }
    }
}


bool IdentifierExpression::GetSymbolOffset(Context &context, SymbolOffset **ppSymbolOffset) const
{
    return context.GetSymbolOffset(identifier_, ppSymbolOffset);
}

std::string IdentifierExpression::GetIdentifier() const
{
    return identifier_;
}


ArrayExpression::ArrayExpression(Node *  identifier, Node * size)
    : size_(size), identifier_((PostfixExpression *)identifier)
{

}

ArrayExpression::~ArrayExpression()
{
    if (NULL != size_)
    {
        delete size_;
        size_ = NULL;
    }
    if (NULL != identifier_)
    {
        delete identifier_;
        identifier_ = NULL;
    }
}

void ArrayExpression::EmitRISC(std::ostream &stream, std::string desReg, Context &context) const
{
    std::string reg1;
    std::string reg2;
    SymbolOffset *pSymbolOffset = NULL;

    if (identifier_->GetSymbolOffset(context, &pSymbolOffset))
    {
        context.GetFreeTempedRegister(reg1);
        size_->EmitRISC(stream, reg1, context);

        context.GetFreeTempedRegister(reg2);
        stream << "      ";
        stream << "li " << reg2 << ", " << pSymbolOffset->declarator->GetTypeSize() << std::endl; //todo -> fetch register name

        stream << "      ";
        stream << "mul " << reg1 << ", " << reg1 << ", " << reg2 << std::endl;

        if (pSymbolOffset->declarator->IsPointer())
        {
            if (pSymbolOffset->isFunctionArgumentSymbol)
            {
                stream << "      ";
                stream << "add " << reg1 << ", " << reg1 << ", " << pSymbolOffset->registerNum << std::endl;
            }
            else
            {
                stream << "      ";
                stream << "lw " << reg2 << ", " << pSymbolOffset->localOffset << "(s0)" << std::endl;

                stream << "      ";
                stream << "add " << reg1 << ", " << reg1 << ", " << reg2 << std::endl;
            }
        }
        else
        {
            if (pSymbolOffset->isFunctionArgumentSymbol)
            {
                stream << "      ";
                stream << "add " << reg1 << ", " << reg1 << ", " << pSymbolOffset->registerNum << std::endl;
            }
            else
            {
                stream << "      ";
                stream << "addi " << reg1 << ", " << reg1 << ", " << pSymbolOffset->localOffset << std::endl;

                stream << "      ";
                stream << "add " << reg1 << ", " << reg1 << ", " << "s0" << std::endl;
            }
        }

        stream << "      ";
        if (1 == pSymbolOffset->declarator->GetTypeArrayElementSize())
        {
            stream << "lbu " << desReg << ", " << "0(" << reg1 << ")" << std::endl;
        }
        else if (2 == pSymbolOffset->declarator->GetTypeArrayElementSize())
        {
            stream << "lhu " << desReg << ", " << "0(" << reg1 << ")" << std::endl;
        }
        else if (4 == pSymbolOffset->declarator->GetTypeArrayElementSize())
        {
            stream << "lw " << desReg << ", " << "0(" << reg1 << ")" << std::endl;
        }

        context.deleteRegister(reg1);
        context.deleteRegister(reg2);
    }
}


void ArrayExpression::Print(std::ostream &stream) const
{
    identifier_->Print(stream);
    stream << "[";
    size_->Print(stream);
    stream << "]";
}

void ArrayExpression::EmitRISCLeft(std::ostream &stream, std::string srcReg, Context &context) const
{
    std::string reg1;
    std::string reg2;

    SymbolOffset *pSymbolOffset = NULL;

    if (identifier_->GetSymbolOffset(context, &pSymbolOffset))
    {
        context.GetFreeTempedRegister(reg1);        
        size_->EmitRISC(stream, reg1, context);

        context.GetFreeTempedRegister(reg2);
        stream << "      ";
        stream << "li " << reg2 << ", " << pSymbolOffset->declarator->GetTypeSize() << std::endl; //todo -> fetch register name

        stream << "      ";
        stream << "mul " << reg1 << ", " << reg1 << ", " << reg2 << std::endl;

        if (pSymbolOffset->isFunctionArgumentSymbol)
        {
            stream << "      ";
            stream << "add " << reg1 << ", " << reg1 << ", " << pSymbolOffset->registerNum << std::endl;
        }
        else
        {
            stream << "      ";
            stream << "addi " << reg1 << ", " << reg1 << ", " << pSymbolOffset->localOffset << std::endl;

            stream << "      ";
            stream << "add " << reg1 << ", " << reg1 << ", " << "s0" << std::endl;
        }

        stream << "      ";
        stream << "sw " << srcReg << ", " << "0(" << reg1 << ")" << std::endl;

        context.deleteRegister(reg1);
        context.deleteRegister(reg2);
    }
}


ExpressionOperator::ExpressionOperator(int op)
    : operator_(op)
{

}

int ExpressionOperator::GetOperator() const
{
    return operator_;
}

void ExpressionOperator::EmitRISC(std::ostream &stream, std::string desReg, Context &context) const
{

}



void ExpressionOperator::Print(std::ostream &stream) const
{

}



PostfixOpExpression::PostfixOpExpression(Node *left, Node *op)
    : operator_((ExpressionOperator *)op), left_(left)
{

}

PostfixOpExpression::PostfixOpExpression(Node *left, int op)
    : operator_(nullptr), left_(left)
{
    operator_ = new ExpressionOperator(op);
}

PostfixOpExpression::~PostfixOpExpression()
{
    if (nullptr != operator_)
    {
        delete operator_;
        operator_ = nullptr;
    }
    if (nullptr != left_)
    {
        delete left_;
        left_ = nullptr;
    }
}


void PostfixOpExpression::EmitRISC(std::ostream &stream, std::string desReg, Context &context) const
{
    std::string reg1;
    std::string reg2;
    int addCount = 1;
    PostfixExpression *postfixExpression = nullptr;
    SymbolOffset *pSymbolOffset = NULL;

    switch (operator_->GetOperator())
    {
    case INC_OP_TYPE:
        postfixExpression = (PostfixExpression *)left_;
        if (postfixExpression->GetSymbolOffset(context, &pSymbolOffset))
        {
            if (pSymbolOffset->declarator->IsArray()
                || pSymbolOffset->declarator->IsPointer())
            {
                addCount = pSymbolOffset->declarator->GetTypeSize();
            }
        }

        context.GetFreeTempedRegister(reg1);
        context.GetFreeTempedRegister(reg2);

        postfixExpression->EmitRISC(stream, reg1, context);
        stream << "      ";
        stream << "li " << reg2 << ", " << addCount << std::endl;
        if (desReg.length() > 0)
        {
            stream << "      ";
            stream << "add " << desReg << ", " << reg1 << ", " << "zero" << std::endl;
        }
        stream << "      ";
        stream << "add " << reg1 << ", " << reg1 << ", " << reg2 << std::endl;
        postfixExpression->EmitRISCLeft(stream, reg1, context);

        context.deleteRegister(reg1);
        context.deleteRegister(reg2);


        break;
    case DEC_OP_TYPE:
        postfixExpression = (PostfixExpression *)left_;
        if (postfixExpression->GetSymbolOffset(context, &pSymbolOffset))
        {
            if (pSymbolOffset->declarator->IsArray())
            {
                addCount = pSymbolOffset->declarator->GetTypeSize();
            }
        }
        addCount = -addCount;

        context.GetFreeTempedRegister(reg1);
        context.GetFreeTempedRegister(reg2);

        postfixExpression->EmitRISC(stream, reg1, context);
        stream << "      ";
        stream << "li " << reg2 << ", " << addCount << std::endl;
        if (desReg.length() > 0)
        {
            stream << "      ";
            stream << "add " << desReg << ", " << reg1 << ", " << "zero" << std::endl;
        }
        stream << "      ";
        stream << "add " << reg1 << ", " << reg1 << ", " << reg2 << std::endl;
        postfixExpression->EmitRISCLeft(stream, reg1, context);

        context.deleteRegister(reg1);
        context.deleteRegister(reg2);


        break;
    default:
        break;
    }
}


void PostfixOpExpression::Print(std::ostream &stream) const
{
    std::string reg1;

    switch (operator_->GetOperator())
    {
    case INC_OP_TYPE:
        left_->Print(stream);
        stream << " ++ ";
        break;
    default:
        break;
    }

}


void PostfixOpExpression::EmitRISCLeft(std::ostream &stream, std::string srcReg, Context &context) const
{

}

UnaryExpression::UnaryExpression(Node *op, Node *right)
    : operator_((ExpressionOperator *)op), right_(right)
{

}

UnaryExpression::UnaryExpression(int op, Node *right)
    : operator_(nullptr), right_(right)
{
    operator_ = new ExpressionOperator(op);
}

UnaryExpression::~UnaryExpression()
{
    if (nullptr != operator_)
    {
        delete operator_;
        operator_ = nullptr;
    }
    if (nullptr != right_)
    {
        delete right_;
        right_ = nullptr;
    }
}

int UnaryExpression::GetOperator() const
{
    return operator_->GetOperator();
}

Node *UnaryExpression::GetRight() const
{
    return right_;
}

void UnaryExpression::EmitRISC(std::ostream &stream, std::string desReg, Context &context) const
{
    std::string reg1;
    SymbolOffset *pSymbolOffset = NULL;

    switch (operator_->GetOperator())
    {
    case '-':
        context.GetFreeTempedRegister(reg1);
        right_->EmitRISC(stream, reg1, context);

        stream << "      ";
        stream << "neg " << desReg << ", " << reg1 << std::endl;

        context.deleteRegister(reg1);
        break;
    case '&':
        if (context.GetSymbolOffset(((IdentifierExpression *)right_)->GetIdentifier(), &pSymbolOffset))
        {
            if (!pSymbolOffset->isFunctionArgumentSymbol)
            {
                stream << "      ";
                stream << "addi " << desReg << ", " << "s0" << ", " << pSymbolOffset->localOffset << std::endl;
            }
        }
        break;

    case '*':
        if (context.GetSymbolOffset(((IdentifierExpression *)right_)->GetIdentifier(), &pSymbolOffset))
        {
            if (pSymbolOffset->isFunctionArgumentSymbol)
            {
                stream << "      ";
                if (1 == pSymbolOffset->declarator->GetTypeArrayElementSize())
                {
                    stream << "lbu " << desReg << ", " << "0(" << pSymbolOffset->registerNum << ")" << std::endl;
                }
                else if (2 == pSymbolOffset->declarator->GetTypeArrayElementSize())
                {
                    stream << "lhu " << desReg << ", " << "0(" << pSymbolOffset->registerNum << ")" << std::endl;
                }
                else if (4 == pSymbolOffset->declarator->GetTypeArrayElementSize())
                {
                    stream << "lw " << desReg << ", " << "0(" << pSymbolOffset->registerNum << ")" << std::endl;
                }
            }
            else
            {
                stream << "      ";
                stream << "lw " << desReg << ", " << pSymbolOffset->localOffset << "(s0)" << std::endl;

                stream << "      ";
                stream << "lw " << desReg << ", " << "0(" << desReg << ")" << std::endl;
            }
        }
        break;

    default:
        break;
    }
}


void UnaryExpression::Print(std::ostream &stream) const
{
    std::string reg1;

    switch (operator_->GetOperator())
    {
    case '-':
        stream << "-";
        right_->Print(stream);
        break;
    default:
        break;
    }

}


void UnaryExpression::EmitRISCLeft(std::ostream &stream, std::string srcReg, Context &context) const
{
    std::string reg1;
    SymbolOffset *pSymbolOffset = NULL;

    switch (operator_->GetOperator())
    {
    case '*':
        if (context.GetSymbolOffset(((IdentifierExpression *)right_)->GetIdentifier(), &pSymbolOffset))
        {
            if (!pSymbolOffset->isFunctionArgumentSymbol)
            {
                context.GetFreeTempedRegister(reg1);

                stream << "      ";
                stream << "lw " << reg1 << ", " << pSymbolOffset->localOffset << "(s0)" << std::endl;

                stream << "      ";
                stream << "sw " << srcReg << ", " << "0(" << reg1 << ")" << std::endl;

                context.deleteRegister(reg1);
            }
        }
        break;

    default:
        break;
    }
}


GenericExpression::GenericExpression(Node *left, Node *op, Node *right)
    : operator_((ExpressionOperator *)op), left_(left), right_(right)
{
    SetIsGenericExpression(true);
}

GenericExpression::GenericExpression(Node *left, int op, Node *right)
    : operator_(nullptr), left_(left), right_(right)
{
    operator_ = new ExpressionOperator(op);
    SetIsGenericExpression(true);
}

GenericExpression::~GenericExpression()
{
    if (nullptr != operator_)
    {
        delete operator_;
        operator_ = nullptr;
    }
    if (nullptr != left_)
    {
        delete left_;
        left_ = nullptr;
    }
    if (nullptr != right_)
    {
        delete right_;
        right_ = nullptr;
    }
}

int GenericExpression::GetOperator() const
{
    return operator_->GetOperator();
}

Node *GenericExpression::GetLeft() const
{
    return left_;
}

Node *GenericExpression::GetRight() const
{
    return right_;
}

void GenericExpression::EmitRISC(std::ostream &stream, std::string desReg, Context &context) const
{
    std::string reg1;
    std::string reg2;

    switch (operator_->GetOperator())
    {
    case '=':
        context.GetFreeTempedRegister(reg1);
        right_->EmitRISC(stream, reg1, context);
        ((Expression *)left_)->EmitRISCLeft(stream, reg1, context);
        if (desReg.length() > 0)
        {
            stream << "      ";
            stream << "add " << desReg << ", " << reg1 << ", " << "zero" << std::endl;
        }
        context.deleteRegister(reg1);
        break;
    case '+':
        context.GetFreeTempedRegister(reg1);
        left_->EmitRISC(stream, reg1, context);
        context.GetFreeTempedRegister(reg2);
        right_->EmitRISC(stream, reg2, context);
        ProcessPointerOffset(stream, reg2, context);
        stream << "      ";
        stream << "add " << desReg << ", " << reg1 << ", " << reg2 << std::endl;
        context.deleteRegister(reg1);
        context.deleteRegister(reg2);
        break;
    case '-':
        context.GetFreeTempedRegister(reg1);
        left_->EmitRISC(stream, reg1, context);
        context.GetFreeTempedRegister(reg2);
        right_->EmitRISC(stream, reg2, context);
        ProcessPointerOffset(stream, reg2, context);
        stream << "      ";
        stream << "sub " << desReg << ", " << reg1 << ", " << reg2 << std::endl;
        context.deleteRegister(reg1);
        context.deleteRegister(reg2);
        break;
    case '*':
        context.GetFreeTempedRegister(reg1);
        left_->EmitRISC(stream, reg1, context);
        context.GetFreeTempedRegister(reg2);
        right_->EmitRISC(stream, reg2, context);
        stream << "      ";
        stream << "mul " << desReg << ", " << reg1 << ", " << reg2 << std::endl;
        context.deleteRegister(reg1);
        context.deleteRegister(reg2);
        break;
    case '/':
        context.GetFreeTempedRegister(reg1);
        left_->EmitRISC(stream, reg1, context);
        context.GetFreeTempedRegister(reg2);
        right_->EmitRISC(stream, reg2, context);
        stream << "      ";
        stream << "div " << desReg << ", " << reg1 << ", " << reg2 << std::endl;
        context.deleteRegister(reg1);
        context.deleteRegister(reg2);
        break;
    case '&':
        context.GetFreeTempedRegister(reg1);
        left_->EmitRISC(stream, reg1, context);
        context.GetFreeTempedRegister(reg2);
        right_->EmitRISC(stream, reg2, context);
        stream << "      ";
        stream << "and " << desReg << ", " << reg1 << ", " << reg2 << std::endl;
        context.deleteRegister(reg1);
        context.deleteRegister(reg2);
        break;
    case '|':
        context.GetFreeTempedRegister(reg1);
        left_->EmitRISC(stream, reg1, context);
        context.GetFreeTempedRegister(reg2);
        right_->EmitRISC(stream, reg2, context);
        stream << "      ";
        stream << "or " << desReg << ", " << reg1 << ", " << reg2 << std::endl;
        context.deleteRegister(reg1);
        context.deleteRegister(reg2);
        break;
    case '^':
        context.GetFreeTempedRegister(reg1);
        left_->EmitRISC(stream, reg1, context);
        context.GetFreeTempedRegister(reg2);
        right_->EmitRISC(stream, reg2, context);
        stream << "      ";
        stream << "xor " << desReg << ", " << reg1 << ", " << reg2 << std::endl;
        context.deleteRegister(reg1);
        context.deleteRegister(reg2);
        break;
    case '<':
        context.GetFreeTempedRegister(reg1);
        left_->EmitRISC(stream, reg1, context);
        context.GetFreeTempedRegister(reg2);
        right_->EmitRISC(stream, reg2, context);
        stream << "      ";
        stream << "slt " << desReg << ", " << reg1 << ", " << reg2 << std::endl;
        context.deleteRegister(reg1);
        context.deleteRegister(reg2);
        break;
    case LEFT_OP_TYPE:
        context.GetFreeTempedRegister(reg1);
        left_->EmitRISC(stream, reg1, context);
        context.GetFreeTempedRegister(reg2);
        right_->EmitRISC(stream, reg2, context);
        stream << "      ";
        stream << "sll " << desReg << ", " << reg1 << ", " << reg2 << std::endl;
        context.deleteRegister(reg1);
        context.deleteRegister(reg2);
        break;
    case RIGHT_OP_TYPE:
        context.GetFreeTempedRegister(reg1);
        left_->EmitRISC(stream, reg1, context);
        context.GetFreeTempedRegister(reg2);
        right_->EmitRISC(stream, reg2, context);
        stream << "      ";
        stream << "sra " << desReg << ", " << reg1 << ", " << reg2 << std::endl;
        context.deleteRegister(reg1);
        context.deleteRegister(reg2);
        break;
    case EQ_OP_TYPE:
        context.GetFreeTempedRegister(reg1);
        left_->EmitRISC(stream, reg1, context);
        context.GetFreeTempedRegister(reg2);
        right_->EmitRISC(stream, reg2, context);

        stream << "      ";
        stream << "sub " << reg2 << ", " << reg1 << ", " << reg2 << std::endl;
        stream << "      ";
        stream << "sltu " << reg2 << ", " << "zero" << ", " << reg2 << std::endl;
        stream << "      ";
        stream << "li " << reg1 << ", " << 1 << std::endl;
        stream << "      ";
        stream << "sub " << desReg << ", " << reg1 << ", " << reg2 << std::endl;

        context.deleteRegister(reg1);
        context.deleteRegister(reg2);
        break;
    case LE_OP_TYPE:
        context.GetFreeTempedRegister(reg1);
        left_->EmitRISC(stream, reg1, context);
        context.GetFreeTempedRegister(reg2);
        right_->EmitRISC(stream, reg2, context);
        stream << "      ";
        stream << "slt " << reg2 << ", " << reg2 << ", " << reg1 << std::endl;
        stream << "      ";
        stream << "li " << reg1 << ", " << 1 << std::endl;
        stream << "      ";
        stream << "sub " << desReg << ", " << reg1 << ", " << reg2 << std::endl;

        context.deleteRegister(reg1);
        context.deleteRegister(reg2);
        break;

    case AND_OP_TYPE:
        context.GetFreeTempedRegister(reg1);
        left_->EmitRISC(stream, reg1, context);
        context.GetFreeTempedRegister(reg2);
        right_->EmitRISC(stream, reg2, context);
        stream << "      ";
        stream << "sltu " << reg1 << ", " << "zero" << ", " << reg1 << std::endl;
        stream << "      ";
        stream << "sltu " << reg2 << ", " << "zero" << ", " << reg2 << std::endl;
        stream << "      ";
        stream << "and " << desReg << ", " << reg1 << ", " << reg2 << std::endl;

        context.deleteRegister(reg1);
        context.deleteRegister(reg2);
        break;

    case OR_OP_TYPE:
        context.GetFreeTempedRegister(reg1);
        left_->EmitRISC(stream, reg1, context);
        context.GetFreeTempedRegister(reg2);
        right_->EmitRISC(stream, reg2, context);
        stream << "      ";
        stream << "sltu " << reg1 << ", " << "zero" << ", " << reg1 << std::endl;
        stream << "      ";
        stream << "sltu " << reg2 << ", " << "zero" << ", " << reg2 << std::endl;
        stream << "      ";
        stream << "or " << desReg << ", " << reg1 << ", " << reg2 << std::endl;

        context.deleteRegister(reg1);
        context.deleteRegister(reg2);
        break;


    case ADD_ASSIGN_OP_TYPE:
        context.GetFreeTempedRegister(reg1);
        left_->EmitRISC(stream, reg1, context);
        context.GetFreeTempedRegister(reg2);
        right_->EmitRISC(stream, reg2, context);
        stream << "      ";
        stream << "add " << reg1 << ", " << reg1 << ", " << reg2 << std::endl;
        ((Expression *)left_)->EmitRISCLeft(stream, reg1, context);
        if (desReg.length() > 0)
        {
            stream << "      ";
            stream << "add " << desReg << ", " << reg1 << ", " << "zero" << std::endl;
        }
        context.deleteRegister(reg1);
        context.deleteRegister(reg2);
        break;

    default:
        break;
    }
}


void GenericExpression::Print(std::ostream &stream) const
{
    std::string reg1;

    switch (operator_->GetOperator())
    {
    case '=':
        left_->Print(stream);
        stream << " = ";
        right_->Print(stream);
        break;
    case '+':
        left_->Print(stream);
        stream << " + ";
        right_->Print(stream);
        break;
    case '-':
        left_->Print(stream);
        stream << " - ";
        right_->Print(stream);
        break;
    case '<':
        left_->Print(stream);
        stream << " < ";
        right_->Print(stream);
        break;
    case '>':
        left_->Print(stream);
        stream << " > ";
        right_->Print(stream);
        break;
    default:
        break;
    }

}


void GenericExpression::EmitRISCLeft(std::ostream &stream, std::string srcReg, Context &context) const
{

}


void GenericExpression::ProcessPointerOffset(std::ostream &stream, std::string desReg, Context &context) const
{
    std::string reg1;
    SymbolOffset *pSymbolOffset = NULL;

    if (((Expression *)left_)->IsIdentifier())
    {
        if (context.GetSymbolOffset(((IdentifierExpression *)left_)->GetIdentifier(), &pSymbolOffset))
        {
            if (pSymbolOffset->declarator->IsArray() || pSymbolOffset->declarator->IsPointer())
            {
                context.GetFreeTempedRegister(reg1);

                stream << "      ";
                stream << "li " << reg1 << ", " << pSymbolOffset->declarator->GetTypeArrayElementSize() << std::endl; //todo -> fetch register name

                stream << "      ";
                stream << "mul " << desReg << ", " << desReg << ", " << reg1 << std::endl;

                context.deleteRegister(reg1);
            }
        }
    }
}


FunctionCallExpression::FunctionCallExpression(Node *  identifier)
    : identifier_((IdentifierExpression *)identifier), argument_expression_list_(new NodeList())
{

}

FunctionCallExpression::FunctionCallExpression(Node *  identifier, Node * argument_expression_list)
    : identifier_((IdentifierExpression *)identifier), argument_expression_list_(argument_expression_list)
{

}

FunctionCallExpression::~FunctionCallExpression()
{
    if (NULL != argument_expression_list_)
    {
        delete argument_expression_list_;
        argument_expression_list_ = NULL;
    }
    if (NULL != identifier_)
    {
        delete identifier_;
        identifier_ = NULL;
    }
}

void FunctionCallExpression::EmitRISC(std::ostream &stream, std::string desReg, Context &context) const
{
    int functionArgumentsCount = 0;
    std::string registerNum;
    std::vector<std::string> tempRegisterList;
    std::vector<std::string> tempArgumentRegisterList;
    Node *argumentExpression = NULL;
    NodeList *functionArgumentsList = NULL;

    functionArgumentsList = (NodeList *)argument_expression_list_;
    functionArgumentsCount = context.GetFunctionArgumentsCount();
    if (functionArgumentsCount > functionArgumentsList->size())
    {
        functionArgumentsCount = functionArgumentsList->size();
    }
    context.GetUsedTempRegisterList(tempRegisterList);

    for (int loop = 0; loop < functionArgumentsCount; loop++)
    {
        SaveOneRegisters(stream, context, context.GetFunctionArgumentsRegister(loop));
    }
    for (std::vector<std::string> ::size_type loop = 0;
        loop < tempRegisterList.size();
        loop++)
    {
        registerNum = tempRegisterList[loop];
        SaveOneRegisters(stream, context, registerNum);
        context.deleteRegister(registerNum);
    }

    for (int loop = 0; loop < functionArgumentsList->size(); loop++)
    {
        argumentExpression = functionArgumentsList->at(loop);
        context.GetFreeTempedRegister(registerNum);
        argumentExpression->EmitRISC(stream, registerNum, context);
        tempArgumentRegisterList.push_back(registerNum);
    }
    for (int loop = 0; loop < functionArgumentsList->size(); loop++)
    {
        registerNum = tempArgumentRegisterList[loop];
        stream << "      ";
        stream << "add " << context.GetFunctionArgumentsRegister(loop) << ", " << registerNum << ", " << "zero" << std::endl;
        context.deleteRegister(registerNum);
    }

    stream << "      ";
    stream << "jal " << "ra" << ", " << identifier_->GetIdentifier() << std::endl;

    for (std::vector<std::string> ::size_type loop = 0;
        loop < tempRegisterList.size();
        loop++)
    {
        registerNum = tempRegisterList[tempRegisterList.size() - 1 - loop];
        RestoreOneRegisters(stream, context, registerNum);
        context.setRegister(registerNum);
    }
    if (desReg.length() > 0)
    {
        stream << "      ";
        stream << "add " << desReg << ", " << "a0" << ", " << "zero" << std::endl;
    }

    for (int loop = 0; loop < functionArgumentsCount; loop++)
    {
        RestoreOneRegisters(stream, context, context.GetFunctionArgumentsRegister(functionArgumentsCount - 1 - loop));
    }

}


void FunctionCallExpression::Print(std::ostream &stream) const
{


}

void FunctionCallExpression::EmitRISCLeft(std::ostream &stream, std::string srcReg, Context &context) const
{


}

void FunctionCallExpression::SaveOneRegisters(std::ostream &stream, Context &context, const std::string &registerNum) const
{
    stream << "      ";
    stream << "addi " << "sp" << ", " << "sp" << ", " << "-4" << std::endl;

    stream << "      ";
    stream << "sw " << registerNum << ", " << "0(sp)" << std::endl;
}

void FunctionCallExpression::RestoreOneRegisters(std::ostream &stream, Context &context, const std::string &registerNum) const
{
    stream << "      ";
    stream << "lw " << registerNum << ", " << "0(sp)" << std::endl;

    stream << "      ";
    stream << "addi " << "sp" << ", " << "sp" << ", " << "4" << std::endl;
}


SizeofExpression::SizeofExpression(Node *expression, bool isTypeName)
    : isTypeName_(isTypeName), expression_(expression), declarationSpecifiers(nullptr)
{
    if (isTypeName_)
    {
        declarationSpecifiers = new DeclarationSpecifiers();
        declarationSpecifiers->SetDeclarationSpecifiers((NodeList *)expression_, false);
    }
}


SizeofExpression::~SizeofExpression()
{
    if (nullptr != expression_)
    {
        delete expression_;
        expression_ = nullptr;
    }
    if (nullptr != declarationSpecifiers)
    {
        delete declarationSpecifiers;
        declarationSpecifiers = nullptr;
    }
}


void SizeofExpression::EmitRISC(std::ostream &stream, std::string destReg, Context &context) const
{
    SymbolOffset *pSymbolOffset = NULL;

    if (isTypeName_)
    {
        stream << "      ";
        stream << "li " << destReg << ", " << declarationSpecifiers->GetSize() << std::endl;
    }
    else
    {
        if (context.GetSymbolOffset(((IdentifierExpression *)expression_)->GetIdentifier(), &pSymbolOffset))
        {
            stream << "      ";
            stream << "li " << destReg << ", " << pSymbolOffset->declarator->GetSize() << std::endl;
        }
    }
}

void SizeofExpression::Print(std::ostream &stream) const
{

}

void SizeofExpression::EmitRISCLeft(std::ostream &stream, std::string srcReg, Context &context) const
{

}



StringExpression::StringExpression(const std::string &content)
{
    for (std::string::size_type loop = 1; loop < content.length() - 1; loop++)
    {
        content_.push_back(content.at(loop));
    }
}


StringExpression::~StringExpression()
{

}


void StringExpression::EmitRISC(std::ostream &stream, std::string destReg, Context &context) const
{
    std::string stringName;

    stringName = context.GetStringName(content_);

    stream << "      ";
    stream << "la " << destReg << ", " << stringName << std::endl;

}

void StringExpression::Print(std::ostream &stream) const
{

}

void StringExpression::EmitRISCLeft(std::ostream &stream, std::string srcReg, Context &context) const
{

}

