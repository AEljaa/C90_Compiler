#ifndef AST_CONSTANT_HPP
#define AST_CONSTANT_HPP


#include "ast_expression.hpp"


class IntConstant : public Expression
{
private:
    int value_;

public:
    IntConstant(int value) : value_(value) {}

    int GetValue() const;
    void EmitRISC(std::ostream &stream, std::string destReg, Context &context) const override;
    void Print(std::ostream &stream) const override;
    void EmitRISCLeft(std::ostream &stream, std::string destReg, Context &context) const override;
};

#endif
