#include "../include/ast/ast_context.hpp"
#include "../include/ast/ast_declarator.hpp"
#include "../include/ast/ast_program.h"
#include <algorithm>
#include <sstream>


Program *GetProgram();


SymbolOffset::SymbolOffset()
{

}


SymbolOffset::SymbolOffset(bool isInRegisert, bool isFunctionArgument, int offset, Declarator *d, const std::string &reg)
    : isRegisterSymbol(isInRegisert), isFunctionArgumentSymbol(isFunctionArgument), localOffset(offset), declarator(d), registerNum(reg)
{


}



Context::Context()
{
    labelIndex = 1;

    functionArgumentsRegisters.push_back("a0");
    functionArgumentsRegisters.push_back("a1");
    functionArgumentsRegisters.push_back("a2");
    functionArgumentsRegisters.push_back("a3");
    functionArgumentsRegisters.push_back("a4");
    functionArgumentsRegisters.push_back("a5");
    functionArgumentsRegisters.push_back("a6");
    functionArgumentsRegisters.push_back("a7");

    savedRegisters.push_back("s2");
    savedRegisters.push_back("s3");
    savedRegisters.push_back("s4");
    savedRegisters.push_back("s5");
    savedRegisters.push_back("s6");
    savedRegisters.push_back("s7");
    savedRegisters.push_back("s8");
    savedRegisters.push_back("s9");
    savedRegisters.push_back("s10");
    savedRegisters.push_back("s11");

    tempRegisters.push_back("t0");
    tempRegisters.push_back("t1");
    tempRegisters.push_back("t2");
    tempRegisters.push_back("t3");
    tempRegisters.push_back("t4");
    tempRegisters.push_back("t5");
    tempRegisters.push_back("t6");
}


void Context::functionInit()
{
    localOffset = 0;
    localSymbolMap.clear();
    initFreeRegister();

}

void Context::initFreeRegister()
{
    for (auto r : functionArgumentsRegisters)
    {
        setRegister(r);
    }
    for (auto r : savedRegisters)
    {
        deleteRegister(r);
    }
    for (auto r : tempRegisters)
    {
        deleteRegister(r);
    }
}


void Context::AddParameterSymbol(Declarator *declarator, int index)
{
    std::string registerNum;

    registerNum = functionArgumentsRegisters[index];
    localSymbolMap[declarator->GetIdentifier()] = SymbolOffset(true, true, localOffset, declarator, registerNum);
}

void Context::AddLocalSymbol(Declarator *declarator)
{
    std::string registerNum;

    if (declarator->IsArray())
    {
        localOffset = localOffset - declarator->GetSize();
        localSymbolMap[declarator->GetIdentifier()] = SymbolOffset(false, false, localOffset, declarator, "");
    }
    else
    {
        localOffset = localOffset - declarator->GetSize();
        localSymbolMap[declarator->GetIdentifier()] = SymbolOffset(false, false, localOffset, declarator, "");
        /*
        if (GetFreeSavedRegister(registerNum))
        {
            localSymbolMap[declarator->GetIdentifier()] = SymbolOffset(true, localOffset, declarator, registerNum);
        }
        else
        {
            localOffset = localOffset - declarator->GetSize();
            localSymbolMap[declarator->GetIdentifier()] = SymbolOffset(false, localOffset, declarator, "");
        }
        */
    }
}


int Context::GetLocalOffset() const
{
    return localOffset;
}


bool Context::GetFreeSavedRegister(std::string &registerNum)
{
    for (auto r : savedRegisters)
    {
        if (!Registers[r])
        {
            Registers[r] = 1;
            registerNum = r;
            return true;
        }
    }

    return false;
}

bool Context::GetFreeTempedRegister(std::string &registerNum)
{
    for (auto r : tempRegisters)
    {
        if (!Registers[r])
        {
            Registers[r] = 1;
            registerNum = r;
            return true;
        }
    }

    return false;
}

bool Context::GetSymbolOffset(const std::string &identifier, SymbolOffset **ppSymbolOffset)
{
    std::unordered_map<std::string, SymbolOffset>::iterator iterator;

    iterator = localSymbolMap.find(identifier);
    if (localSymbolMap.end() != iterator)
    {
        *ppSymbolOffset = &iterator->second;
        return true;
    }

    return false;
}


std::string Context::GetStringName(const std::string &content)
{
    std::string stringName;
    std::unordered_map<std::string, std::string>::iterator iterator;

    iterator = stringMap.find(content);
    if (stringMap.end() == iterator)
    {
        stringName = CreateLabel("GLOBAL_STRING");
        stringMap[content] = stringName;
        GetProgram()->AddStringDeclarations(new String_Declaration(stringName, content));
    }
    else
    {
        stringName = iterator->second;
    }

    return stringName;
}


void Context::SetCurrentFunctionName(const char *functionName)
{
    currentFunctionName = functionName;
}


std::string Context::GetReturnLabel() const
{
    return ToUpperCase(std::string(currentFunctionName) + "_return_label");
}

void Context::SetBreakContinueLabel(const std::string &breakLabelStr, const std::string &continueLabelStr)
{
    breakLabel = breakLabelStr;
    continueLabel = continueLabelStr;
}

std::string Context::GetBreakLabel() const
{
    return breakLabel;
}

std::string Context::GetContinueLabel() const
{
    return continueLabel;
}

void Context::SetFunctionArgumentsCount(int value)
{
    functionArgumentsCount = value;
}

int Context::GetFunctionArgumentsCount() const
{
    return functionArgumentsCount;
}

std::string Context::GetFunctionArgumentsRegister(int index) const
{
    return functionArgumentsRegisters[index];
}

void Context::GetUsedSaveRegisterList(std::vector<std::string> &savedRegisterList)
{
    std::string registerNum;

    savedRegisterList.clear();
    for (std::vector<std::string> ::size_type loop = 0;
        loop < savedRegisters.size();
        loop++)
    {
        registerNum = savedRegisters[loop];
        if (Registers[registerNum])
        {
            savedRegisterList.push_back(registerNum);
        }
    }
}

void Context::GetUsedTempRegisterList(std::vector<std::string> &tempRegisterList)
{
    std::string registerNum;

    tempRegisterList.clear();
    for (std::vector<std::string> ::size_type loop = 0;
        loop < tempRegisters.size();
        loop++)
    {
        registerNum = tempRegisters[loop];
        if (Registers[registerNum])
        {
            tempRegisterList.push_back(registerNum);
        }
    }
}

std::string Context::CreateLabel(const char *labelName)
{
    std::string label;

    label = currentFunctionName + "_" + labelName + "_" + IntToString(labelIndex);
    labelIndex = labelIndex + 1;
    label = ToUpperCase(label);

    return label;
}

std::string Context::ToUpperCase(const std::string &str) const
{
    std::string upperCaseStr;

    upperCaseStr = str;
    std::transform(str.begin(), str.end(), upperCaseStr.begin(), ::toupper);
    return upperCaseStr;
}


std::string Context::IntToString(int intValue) const
{
    std::stringstream ss;

    ss << intValue;
    return ss.str();
}




