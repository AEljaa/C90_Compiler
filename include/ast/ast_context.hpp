#ifndef AST_CONTEXT_HPP
#define AST_CONTEXT_HPP

#include <unordered_map>
#include <string>
#include <iostream>
#include <vector>


// An object of class Context is passed between AST nodes during compilation.
// This can be used to pass around information about what's currently being
// compiled (e.g. function scope and variable names).


class Declarator;


class SymbolOffset
{
public:
    bool isRegisterSymbol;
    bool isFunctionArgumentSymbol;
    int localOffset;
    Declarator *declarator;
    std::string registerNum;

    SymbolOffset();
    SymbolOffset(bool isInRegisert, bool isFunctionArgument, int offset, Declarator *d, const std::string &reg);

};



class Context
{
    /* TODO decide what goes inside here */
    private:
        std::unordered_map<std::string, int> Registers = {
            {"zero", 1}, // zero (x0)
            {"ra", 1}, // ra
            {"sp", 1}, // stack pointer
            {"gp", 1}, // global pointer
            {"tp", 1}, // thread pointer
            {"t0", 0}, // t0 (x5)
            {"t1", 0},{"t2", 0},
            {"s0", 1}, // (frame pointer)
            {"s1", 1}, // Saved register
            {"a0", 1},{"a1", 1}, // Function arguments
            {"a2", 1},{"a3", 1},{"a4", 1},{"a5", 1}, // Function arguments
            {"a6", 1},{"a7", 1}, // Function arguments
            {"s2", 1},{"s3", 1},{"s4", 1},{"s5", 1},{"s6", 1},{"s7", 1},{"s8", 1},{"s9", 1},{"s10", 1},{"s11", 1}, // Saved registers
            {"t3", 0},{"t4", 0},{"t5", 0},{"t6", 0} // Temporary registers
        };

        std::unordered_map<std::string, SymbolOffset> localSymbolMap;
        std::unordered_map<std::string, std::string> stringMap;

        int localOffset;
        int labelIndex;
        int functionArgumentsCount;
        std::vector<std::string> functionArgumentsRegisters;
        std::vector<std::string> savedRegisters;
        std::vector<std::string> tempRegisters;

        // int Registers[32] = {
        //     1, // zero (x0)
        //     1, // ra
        //     1, // stack pointer
        //     1, // global pointer
        //     1, // thread pointer
        //     0, // t0 (x5)
        //     0,0, // t1, t2
        //     1, // (frame pointer)
        //     1, // Saved register
        //     1,1, // Function arguments
        //     1,1,1,1, // Function arguments
        //     1,1, // Function arguments
        //     1,1,1,1,1,1,1,1,1,1, // Saved registers
        //     0,0,0,0 // Temporary registers
        // };

        std::string currentFunctionName;
        std::string breakLabel;
        std::string continueLabel;


    public:
        Context();
        ~Context(){};


        void setRegister(std::string reg){
            Registers[reg] = 1;
        }

        void deleteRegister(std::string reg){
            Registers[reg] = 0;
        }

        std::string getFreeRegister(){
            for (auto const& reg : Registers){
                if (reg.second == 0){
                    Registers[reg.first] = 1;
                    return reg.first;
                }
            }
            std::cerr << "No free registers" << std::endl;
        }

        void functionInit();
        void initFreeRegister();
        void AddParameterSymbol(Declarator *declarator, int index);
        void AddLocalSymbol(Declarator *declarator);
        int GetLocalOffset() const;
        bool GetFreeSavedRegister(std::string &registerNum);
        bool GetFreeTempedRegister(std::string &registerNum);
        bool GetSymbolOffset(const std::string &identifier, SymbolOffset **ppSymbolOffset);
        std::string GetStringName(const std::string &content);

        void SetCurrentFunctionName(const char *functionName);
        std::string GetReturnLabel() const;

        void SetBreakContinueLabel(const std::string &breakLabelStr, const std::string &continueLabelStr);
        std::string GetBreakLabel() const;
        std::string GetContinueLabel() const;

        void SetFunctionArgumentsCount(int value);
        int GetFunctionArgumentsCount() const;
        std::string GetFunctionArgumentsRegister(int index) const;

        void GetUsedSaveRegisterList(std::vector<std::string> &savedRegisterList);
        void GetUsedTempRegisterList(std::vector<std::string> &tempRegisterList);

        std::string CreateLabel(const char *labelName);

        std::string ToUpperCase(const std::string &str) const;
        std::string IntToString(int intValue) const;


};

#endif
