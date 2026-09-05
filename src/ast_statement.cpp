

#include "../include/ast/ast_statement.h"
#include "../include/ast/ast_expression.hpp"
#include "../include/ast/ast_declarator.hpp"


ExpressionStatement::ExpressionStatement(Node *expression)
    : expression_(expression)
{

}


ExpressionStatement::~ExpressionStatement()
{
    if (nullptr != expression_)
    {
        delete expression_;
        expression_ = NULL;
    }
}


Node *ExpressionStatement::GetExpression() const
{
    return expression_;
}

void ExpressionStatement::EmitRISC(std::ostream &stream, std::string desReg, Context &context) const
{
    if (nullptr != expression_)
    {
        expression_->EmitRISC(stream, "", context);
    }
}


void ExpressionStatement::Print(std::ostream &stream) const
{
    if (nullptr != expression_)
    {
        expression_->Print(stream);
    }
    stream << ";" ;
}


CompoundStatement::~CompoundStatement()
{
    if (nullptr != declaration_list_)
    {
        delete declaration_list_;
        declaration_list_ = NULL;
    }
    if (nullptr != statement_list_)
    {
        delete statement_list_;
        statement_list_ = NULL;
    }
}


void CompoundStatement::EmitRISC(std::ostream &stream, std::string desReg, Context &context) const
{
    if (nullptr != declaration_list_)
    {
        InitializerLocalDeclarationsList(declaration_list_, stream, desReg, context);
    }
    if (nullptr != statement_list_)
    {
        statement_list_->EmitRISC(stream, desReg, context);
    }
}

void CompoundStatement::InitializerLocalDeclarationsList(Node *localDeclarationsList, std::ostream &stream, std::string desReg, Context &context) const
{
    Node *node = NULL;
    NodeList *nodeList = NULL;

    nodeList = (NodeList *)localDeclarationsList;
    for (int loop = 0; loop < nodeList->size(); loop++)
    {
        node = nodeList->at(loop);
        InitializerLocalDeclarations(node, stream, desReg, context);
    }

}

void CompoundStatement::InitializerLocalDeclarations(Node *localDeclaration, std::ostream &stream, std::string desReg, Context &context) const
{
    Declaration *declaration = NULL;
    Declarator *declarator = NULL;
    NodeList *declaratorList = NULL;

    declaration = (Declaration *)localDeclaration;
    declaratorList = (NodeList *)declaration->GetInitDeclaratorList();

    for (int loop = 0; loop < declaratorList->size(); loop++)
    {
        declarator = (Declarator *)declaratorList->at(loop);
        if (nullptr != declarator->GetInitializer())
        {
            declarator->InitializerLocalDeclaration(stream, desReg, context);
        }
    }
}


void CompoundStatement::Print(std::ostream &stream) const
{
    Node *node = nullptr;
    NodeList *nodeList = nullptr;

    stream << "{" << std::endl;

    if (nullptr != declaration_list_)
    {
        declaration_list_->Print(stream);
    }
    if (nullptr != statement_list_)
    {
        nodeList = (NodeList *)statement_list_;
        for (int loop = 0; loop < nodeList->size(); loop++)
        {
            node = nodeList->at(loop);
            node->Print(stream);
            stream << std::endl;
        }
    }

    stream << "}";
}


void BranchStatement::EmitIfBranchRISC(std::ostream &stream,
    Context &context,
    Node *node,
    bool isTrue,
    const char *branchLabel) const
{
    bool isToSwapPos = false;
    std::string reg1;
    std::string reg2;
    std::string branchType;
    GenericExpression *genericExpression = nullptr;

    if (IsCompareExpression(node))
    {
        genericExpression = (GenericExpression *)node;
        context.GetFreeTempedRegister(reg1);
        context.GetFreeTempedRegister(reg2);

        if (isTrue)
        {
            switch (genericExpression->GetOperator())
            {
            case EQ_OP_TYPE:
                branchType = "beq";
                break;
            case NE_OP_TYPE:
                branchType = "bne";
                break;
            case '<':
                branchType = "blt";
                break;
            case LE_OP_TYPE:
                branchType = "bge";
                isToSwapPos = true;
                break;
            case GE_OP_TYPE:
                branchType = "bge";
                break;
            case '>':
                branchType = "blt";
                isToSwapPos = true;
                break;
            default:
                break;
            }
        }
        else
        {
            switch (genericExpression->GetOperator())
            {
            case EQ_OP_TYPE:
                branchType = "bne";
                break;
            case NE_OP_TYPE:
                branchType = "beq";
                break;
            case '<':
                branchType = "bge";
                break;
            case LE_OP_TYPE:
                branchType = "blt";
                isToSwapPos = true;
                break;
            case GE_OP_TYPE:
                branchType = "blt";
                break;
            case '>':
                branchType = "bge";
                isToSwapPos = true;
                break;
            default:
                break;
            }
        }
        genericExpression->GetLeft()->EmitRISC(stream, reg1, context);
        genericExpression->GetRight()->EmitRISC(stream, reg2, context);
        if (isToSwapPos)
        {
            stream << "      ";
            stream << branchType << " " << reg2 << ", " << reg1 << ", " << branchLabel << std::endl;
        }
        else
        {
            stream << "      ";
            stream << branchType << " " << reg1 << ", " << reg2 << ", " << branchLabel << std::endl;
        }
        context.deleteRegister(reg1);
        context.deleteRegister(reg2);
    }
    else
    {
        context.GetFreeTempedRegister(reg1);
        node->EmitRISC(stream, reg1, context);
        if (isTrue)
        {
            stream << "      ";
            stream << "bne " << reg1 << ", " << "zero" << ", " << branchLabel << std::endl;
        }
        else
        {
            stream << "      ";
            stream << "beq " << reg1 << ", " << "zero" << ", " << branchLabel << std::endl;
        }
        context.deleteRegister(reg1);

    }
}


bool BranchStatement::IsCompareExpression(Node *node) const
{
    GenericExpression *genericExpression = nullptr;

    if (!node->IsGenericExpression())
    {
        return false;
    }
    genericExpression = (GenericExpression *)node;
    switch (genericExpression->GetOperator())
    {
    case '<':
    case '>':
    case LE_OP_TYPE:
    case GE_OP_TYPE:
    case EQ_OP_TYPE:
    case NE_OP_TYPE:

        return true;
    default:
        break;
    }


    return false;
}

IfStatement::IfStatement(Node *expression, Node *statement, Node *elseStatement)
    : expression_(expression), statement_(statement), elseStatement_(elseStatement)
{

}

IfStatement::~IfStatement()
{
    if (nullptr != expression_)
    {
        delete expression_;
        expression_ = NULL;
    }
    if (nullptr != statement_)
    {
        delete statement_;
        statement_ = NULL;
    }
    if (nullptr != elseStatement_)
    {
        delete elseStatement_;
        elseStatement_ = NULL;
    }
}

void IfStatement::EmitRISC(std::ostream &stream, std::string desReg, Context &context) const
{
    std::string ifEndLabel;
    std::string elseLabel;

    if (nullptr == elseStatement_)
    {
        ifEndLabel = context.CreateLabel("IF_END");

        EmitIfBranchRISC(stream, context, expression_, false, ifEndLabel.c_str());
        if (nullptr != statement_)
        {
            statement_->EmitRISC(stream, desReg, context);
        }

        stream << ifEndLabel << ":" << std::endl;
    }
    else
    {
        elseLabel = context.CreateLabel("ELSE");
        ifEndLabel = context.CreateLabel("IF_END");

        EmitIfBranchRISC(stream, context, expression_, false, elseLabel.c_str());
        if (nullptr != statement_)
        {
            statement_->EmitRISC(stream, desReg, context);
        }
        stream << "      ";
        stream << "jal " << "zero" << ", " << ifEndLabel << std::endl;

        stream << elseLabel << ":" << std::endl;
        if (nullptr != elseStatement_)
        {
            elseStatement_->EmitRISC(stream, desReg, context);
        }
        stream << ifEndLabel << ":" << std::endl;

    }
}

void IfStatement::Print(std::ostream &stream) const
{
    stream << "if (";
    if (NULL != expression_)
    {
        expression_->Print(stream);
    }
    stream << ")" << std::endl;
    if (NULL != statement_)
    {
        statement_->Print(stream);
    }

    if (NULL != elseStatement_)
    {
        stream << std::endl;
        stream << "else" << std::endl;
        elseStatement_->Print(stream);
    }

}

WhileStatement::WhileStatement(Node *expression, Node *statement)
    : expression_(expression), statement_(statement)
{

}

WhileStatement::~WhileStatement()
{
    if (nullptr != expression_)
    {
        delete expression_;
        expression_ = NULL;
    }
    if (nullptr != statement_)
    {
        delete statement_;
        statement_ = NULL;
    }
}

void WhileStatement::EmitRISC(std::ostream &stream, std::string desReg, Context &context) const
{
    std::string whileStatementStartLabel;
    std::string whileTestLabel;
    std::string whileEndLabel;
    Node *middleExpression = nullptr;

    whileStatementStartLabel = context.CreateLabel("WHILE_STATEMENT_START");
    whileTestLabel = context.CreateLabel("WHILE_TEST");
    whileEndLabel = context.CreateLabel("WHILE_END");
    context.SetBreakContinueLabel(whileEndLabel, whileTestLabel);

    stream << "      ";
    stream << "jal " << "zero" << ", " << whileTestLabel << std::endl;

    stream << whileStatementStartLabel << ":" << std::endl;
    if (nullptr != statement_)
    {
        statement_->EmitRISC(stream, desReg, context);
    }

    stream << whileTestLabel << ":" << std::endl;
    if (nullptr != expression_)
    {
        EmitIfBranchRISC(stream, context, expression_, true, whileStatementStartLabel.c_str());
    }

    stream << whileEndLabel << ":" << std::endl;
}

void WhileStatement::Print(std::ostream &stream) const
{
    stream << "while (";
    if (NULL != expression_)
    {
        expression_->Print(stream);
    }
    stream << ")" << std::endl;

    if (NULL != statement_)
    {
        statement_->Print(stream);
    }

}



ForStatement::ForStatement(Node *left, Node *middle, Node *right, Node *statement)
    : left_(left), middle_(middle), right_(right), statement_(statement)
{

}

ForStatement::~ForStatement()
{
    if (nullptr != left_)
    {
        delete left_;
        left_ = NULL;
    }
    if (nullptr != middle_)
    {
        delete middle_;
        middle_ = NULL;
    }
    if (nullptr != right_)
    {
        delete right_;
        right_ = NULL;
    }
    if (nullptr != statement_)
    {
        delete statement_;
        statement_ = NULL;
    }
}

void ForStatement::EmitRISC(std::ostream &stream, std::string desReg, Context &context) const
{
    std::string forStatementStartLabel;
    std::string forUpdateLabel;
    std::string forTestLabel;
    std::string forEndLabel;
    Node *middleExpression = nullptr;

    forStatementStartLabel = context.CreateLabel("FOR_STATEMENT_START");
    forUpdateLabel = context.CreateLabel("FOR_UPDATE");
    forTestLabel = context.CreateLabel("FOR_TEST");
    forEndLabel = context.CreateLabel("FOR_END");
    context.SetBreakContinueLabel(forEndLabel, forUpdateLabel);

    if (nullptr != left_)
    {
        left_->EmitRISC(stream, desReg, context);
    }
    stream << "      ";
    stream << "jal " << "zero" << ", " << forTestLabel << std::endl;

    stream << forStatementStartLabel << ":" << std::endl;
    if (nullptr != statement_)
    {
        statement_->EmitRISC(stream, desReg, context);
    }

    stream << forUpdateLabel << ":" << std::endl;
    if (nullptr != right_)
    {
        right_->EmitRISC(stream, "", context);
    }

    stream << forTestLabel << ":" << std::endl;
    if (nullptr != middle_)
    {
        middleExpression = ((ExpressionStatement *)middle_)->GetExpression();
        if (nullptr != middleExpression)
        {
            EmitIfBranchRISC(stream, context, middleExpression, true, forStatementStartLabel.c_str());
        }
    }

    stream << forEndLabel << ":" << std::endl;
}

void ForStatement::Print(std::ostream &stream) const
{
    stream << "for (";
    if (NULL != left_)
    {
        left_->Print(stream);
    }
    if (NULL != middle_)
    {
        middle_->Print(stream);
    }
    if (NULL != right_)
    {
        right_->Print(stream);
    }
    stream << ")" << std::endl;

    if (NULL != statement_)
    {
        statement_->Print(stream);
    }

}

















