

#ifndef AST_DECLARATOR_HPP
#define AST_DECLARATOR_HPP


#include "ast_node.hpp"


enum BaseDataType
{
    VOID_TYPE = 1,
    CHAR_TYPE = 2,
    SHORT_TYPE = 3,
    INT_TYPE = 4,
    LONG_TYPE = 5,
    FLOAT_TYPE = 6,
    DOUBLE_TYPE = 7,
};

class DeclarationSpecifiers
{
public:
    bool isTypeDef;
    bool isExtern;
    bool canBeReference;
    bool isGlobalVariable;
    bool isRegister;
    bool isUnsigned;
    bool isPointer;
    BaseDataType type_;

public:
    DeclarationSpecifiers();
    ~DeclarationSpecifiers();

    int GetSize() const;
    int GetArrayElementSize() const;
    void SetDeclarationSpecifiers(NodeList *nodeList, bool isGlobal);


private:
    void Reset();
    void SetType(Node *node);
    void SetStorageClass(Node *node);


};


class DeclarationSpecifiersBase : public Node
{
private:
    bool isTypeSpecifier_;

public:
    DeclarationSpecifiersBase(bool isTypeSpecifier);
    bool IsTypeSpecifier() const;

};


class StorageClassSpecifier : public DeclarationSpecifiersBase
{
private:
    std::string storageClass_;

public:
    StorageClassSpecifier(std::string storageClass);
    ~StorageClassSpecifier();
    void EmitRISC(std::ostream &stream, std::string destReg, Context &context) const override;
    void Print(std::ostream &stream) const override;
    std::string GetStorageClass() const;

};




class Declaration : public Node
{
private:
    Node *declaration_specifiers_;
    Node *init_declarator_list_;

public:
    Declaration(Node *declaration_specifiers, Node *init_declarator_list);
    ~Declaration();

    Node *GetDeclarationSpecifiers();
    Node *GetInitDeclaratorList();
    void EmitRISC(std::ostream &stream, std::string destReg, Context &context) const override;
    void Print(std::ostream &stream) const override;


};



class Declarator : public Node
{
private:
    bool isArray_;
    bool isFunction_;
    Node *initializer_;
    Node *pointer_;
    DeclarationSpecifiers declarationSpecifiers;

public:
    Declarator(bool isArray);
    virtual ~Declarator();

    virtual int GetSize() const = 0;
    virtual std::string GetIdentifier() const = 0;
    virtual bool IsPointer() const;
    bool IsArray() const;
    bool IsFunction() const;
    void SetIsFunction(bool value);
    int GetTypeSize() const;
    int GetTypeArrayElementSize() const;
    void SetDeclarationSpecifiers(const DeclarationSpecifiers &value);
    DeclarationSpecifiers *GetDeclarationSpecifiers();
    void SetInitializer(Node *initializer);
    Node *GetInitializer() const;
    void SetPointer(Node *pointer);
    virtual void InitializerLocalDeclaration(std::ostream &stream, std::string desReg, Context &context) const;

};




class Parameter_Declaration : public Node
{
private:
    Node *declarationSpecifiers_;
    Declarator *identifier_;

public:
    Parameter_Declaration(Node *declarationSpecifiersParameter, Node *identifier);
    ~Parameter_Declaration();

    void EmitRISC(std::ostream &stream, std::string destReg, Context &context) const override;
    void Print(std::ostream &stream) const override;
    Declarator *GetIdentifier() const;



};



class String_Declaration : public Node
{
private:
    std::string name_;
    std::string content_;

public:
    String_Declaration(const std::string &name, const std::string &content);
    ~String_Declaration();

    void EmitRISC(std::ostream &stream, std::string destReg, Context &context) const override;
    void Print(std::ostream &stream) const override;
    Declarator *GetIdentifier() const;



};




#endif
