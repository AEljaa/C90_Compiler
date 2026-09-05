#include "../ast/ast_node.hpp"

class arithmeticADD : public Node
{
private:
    Node *Left;
    Node *Right;

public:
    arithmeticADD(Node * leftop, Node *  rightop) : Left(leftop), Right(rightop){}

    ~arithmeticADD()
    {}

    // void EmitRISC(std::ostream &stream, std::string destReg, Context &context)
    // {
    //     auto leftreg= context.getFreeRegister();
    //     auto rightreg= context.getFreeRegister();
    //     Left->EmitRISC(stream, leftreg, context);
    //     Right->EmitRISC(stream, rightreg, context);
    //     stream << "add " << destReg << ", " << leftreg << ", " << rightreg << std::endl;

    // }


};
