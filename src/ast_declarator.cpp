

#include "../include/ast/ast_declarator.hpp"
#include "../include/ast/ast_type_specifier.hpp"
#include "../include/ast/ast_identifier.hpp"
#include <string.h>



DeclarationSpecifiers::DeclarationSpecifiers()
    : isTypeDef(false), isExtern(false), canBeReference(true), isGlobalVariable(false),
    isRegister(false), isUnsigned(false), isPointer(false), type_(VOID_TYPE)
{

}

DeclarationSpecifiers::~DeclarationSpecifiers()
{

}

int DeclarationSpecifiers::GetSize() const
{

    if (isPointer)
    {
        return 4;
    }

    return GetArrayElementSize();
}

int DeclarationSpecifiers::GetArrayElementSize() const
{
    int size = 0;

    switch (type_)
    {
    case VOID_TYPE:
        break;
    case CHAR_TYPE:
        size = 1;
        break;
    case SHORT_TYPE:
        size = 2;
        break;
    case INT_TYPE:
        size = 4;
        break;
    case LONG_TYPE:
        size = 8;
        break;
    case FLOAT_TYPE:
        size = 4;
        break;
    case DOUBLE_TYPE:
        size = 8;
        break;
    default:
        break;
    }

    return size;
}

void DeclarationSpecifiers::SetDeclarationSpecifiers(NodeList *nodeList, bool isGlobal)
{
    DeclarationSpecifiersBase *node = NULL;

    Reset();
    isGlobalVariable = isGlobal;
    for (int loop = 0; loop < nodeList->size(); loop++)
    {
        node = (DeclarationSpecifiersBase *)nodeList->at(loop);
        if (node->IsTypeSpecifier())
        {
            SetType(node);
        }
        else
        {
            SetStorageClass(node);
        }
    }
}

void DeclarationSpecifiers::Reset()
{
    isTypeDef = false;
    isExtern = false;
    canBeReference = true;
    isGlobalVariable = false;
    isRegister = false;
    isUnsigned = false;
    type_ = VOID_TYPE;
}

void DeclarationSpecifiers::SetType(Node *node)
{
    std::string type;

    type = ((TypeSpecifier *)node)->GetType();
    if (NULL != strstr(type.c_str(), "void"))
    {
        type_ = VOID_TYPE;
    }
    else if (NULL != strstr(type.c_str(), "char"))
    {
        type_ = CHAR_TYPE;
    }
    else if (NULL != strstr(type.c_str(), "short"))
    {
        type_ = SHORT_TYPE;
    }
    else if (NULL != strstr(type.c_str(), "int"))
    {
        type_ = INT_TYPE;
    }
    else if (NULL != strstr(type.c_str(), "long"))
    {
        type_ = LONG_TYPE;
    }
    else if (NULL != strstr(type.c_str(), "float"))
    {
        type_ = FLOAT_TYPE;
    }
    else if (NULL != strstr(type.c_str(), "double"))
    {
        type_ = DOUBLE_TYPE;
    }

    if (0 == strcmp(type.c_str(), "signed"))
    {
        isUnsigned = false;
    }
    else if (0 == strcmp(type.c_str(), "unsigned"))
    {
        isUnsigned = true;
    }
}

void DeclarationSpecifiers::SetStorageClass(Node *node)
{
    std::string storageClass;

    storageClass = ((StorageClassSpecifier *)node)->GetStorageClass();
    if (NULL != strstr(storageClass.c_str(), "typedef"))
    {
        isTypeDef = true;
    }
    else if (NULL != strstr(storageClass.c_str(), "extern"))
    {
        isExtern = true;
    }
    else if (NULL != strstr(storageClass.c_str(), "static"))
    {
        canBeReference = false;
        isGlobalVariable = true;
    }
    else if (NULL != strstr(storageClass.c_str(), "register"))
    {
        isRegister = true;
    }
}



DeclarationSpecifiersBase::DeclarationSpecifiersBase(bool isTypeSpecifier)
    : isTypeSpecifier_(isTypeSpecifier)
{

}

bool DeclarationSpecifiersBase::IsTypeSpecifier() const
{
    return isTypeSpecifier_;
}


StorageClassSpecifier::StorageClassSpecifier(std::string storageClass)
    : DeclarationSpecifiersBase(false), storageClass_(storageClass)
{

}

StorageClassSpecifier::~StorageClassSpecifier()
{

}


std::string StorageClassSpecifier::GetStorageClass() const
{
    return storageClass_;
}


void StorageClassSpecifier::EmitRISC(std::ostream &stream, std::string destReg, Context &context) const
{

}

void StorageClassSpecifier::Print(std::ostream &stream) const
{
    stream << storageClass_;
}


Declaration::Declaration(Node *declaration_specifiers, Node *init_declarator_list)
    : declaration_specifiers_(declaration_specifiers), init_declarator_list_(init_declarator_list)
{

}

Declaration::~Declaration()
{
    delete declaration_specifiers_;
    delete init_declarator_list_;
}



Node *Declaration::GetDeclarationSpecifiers()
{
    return declaration_specifiers_;
}


Node *Declaration::GetInitDeclaratorList()
{
    return init_declarator_list_;
}


void Declaration::EmitRISC(std::ostream &stream, std::string destReg, Context &context) const
{


}


void Declaration::Print(std::ostream &stream) const
{
    Node *declaration_specifier = NULL;
    Node *declarator = NULL;
    NodeList *declaration_specifiers = NULL;
    NodeList *declarator_list = NULL;

    declaration_specifiers = (NodeList *)declaration_specifiers_;
    for (int loop = 0; loop < declaration_specifiers->size(); loop++)
    {
        declaration_specifier = declaration_specifiers->at(loop);
        declaration_specifier->Print(stream);
        stream << " ";
    }

    declarator_list = (NodeList *)init_declarator_list_;
    for (int loop = 0; loop < declarator_list->size(); loop++)
    {
        declarator = declarator_list->at(loop);
        declarator->Print(stream);
        if (loop == (declarator_list->size() - 1))
        {
            stream << ";";
        }
        else
        {
            stream << ",";
        }
    }

    stream << std::endl;

}



Declarator::Declarator(bool isArray)
    : isArray_(isArray), isFunction_(false), initializer_(nullptr), pointer_(nullptr)
{

}

Declarator::~Declarator()
{
    if (nullptr != initializer_)
    {
        delete initializer_;
        initializer_ = nullptr;
    }
    if (nullptr != pointer_)
    {
        delete pointer_;
        pointer_ = nullptr;
    }
}

bool Declarator::IsArray() const
{
    return isArray_;
}

bool Declarator::IsPointer() const
{
    return declarationSpecifiers.isPointer;
}

bool Declarator::IsFunction() const
{
    return isFunction_;
}

void Declarator::SetIsFunction(bool value)
{
    isFunction_ = value;
}

int Declarator::GetTypeSize() const
{
    return declarationSpecifiers.GetSize();
}

int Declarator::GetTypeArrayElementSize() const
{
    return declarationSpecifiers.GetArrayElementSize();
}

void Declarator::SetDeclarationSpecifiers(const DeclarationSpecifiers &value)
{
    declarationSpecifiers = value;
    if (nullptr == pointer_)
    {
        declarationSpecifiers.isPointer = false;
    }
    else
    {
        declarationSpecifiers.isPointer = true;
    }
}

Node *Declarator::GetInitializer() const
{
    return initializer_;
}

void Declarator::SetInitializer(Node *initializer)
{
    initializer_ = initializer;
}

void Declarator::SetPointer(Node *pointer)
{
    pointer_ = pointer;
}

void Declarator::InitializerLocalDeclaration(std::ostream &stream, std::string desReg, Context &context) const
{

}

DeclarationSpecifiers *Declarator::GetDeclarationSpecifiers()
{
    return &declarationSpecifiers;
}


Parameter_Declaration::Parameter_Declaration(Node *declarationSpecifiersParameter, Node *identifier)
    : declarationSpecifiers_(declarationSpecifiersParameter), identifier_((Declarator *)identifier)
{
    DeclarationSpecifiers tempDeclarationSpecifiers;

    tempDeclarationSpecifiers.SetDeclarationSpecifiers((NodeList *)declarationSpecifiers_, false);
    identifier_->SetDeclarationSpecifiers(tempDeclarationSpecifiers);
}

Parameter_Declaration::~Parameter_Declaration()
{
    delete declarationSpecifiers_;
    delete identifier_;
}


Declarator *Parameter_Declaration::GetIdentifier() const
{
    return identifier_;
}

void Parameter_Declaration::EmitRISC(std::ostream &stream, std::string destReg, Context &context) const
{

}

void Parameter_Declaration::Print(std::ostream &stream) const
{
    Node *one_declaration_specifier = NULL;
    NodeList *node_list = NULL;

    node_list = (NodeList *)declarationSpecifiers_;
    for (int loop = 0; loop < node_list->size(); loop++)
    {
        one_declaration_specifier = node_list->at(loop);
        one_declaration_specifier->Print(stream);
        stream << " ";
    }
    identifier_->Print(stream);
}


String_Declaration::String_Declaration(const std::string &name, const std::string &content)
    : name_(name), content_(content)
{

}

String_Declaration::~String_Declaration()
{

}

void String_Declaration::EmitRISC(std::ostream &stream, std::string destReg, Context &context) const
{
    stream << name_ << ":" << "\t" << ".string" << "\t" << "\"" << content_ << "\"";
    stream << std::endl;
}

void String_Declaration::Print(std::ostream &stream) const
{

}


