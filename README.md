2023/24 Compilers Coursework
==============================

# Overview
This project implements a C90 to RISC-V32I compiler using C++. 
The compiler supports:
- Integer arithmetic, comparisons, logical operations, and bitwise operations
- Selected unary operations
- Local and global variables
- Internal and external function calls
- Recursive functions
- Arrays and array indexing
- Pointers, address operations, and pointer arithmetic
- Characters and strings
- if, if-else, for, and while control flow
- Integer and character sizeof operations
- Unsigned integers

Support for floats, doubles, enums, switch statements, structs, typedefs, and some variable-scoping cases was not fully completed.

# Feedback
## Overall:
- Total Tests Passed: 127/201 (63.18%)
    - Seen Tests Passed: 70/86 (81.40%)
    - Unseen Tests Passed: 57/115 (49.57%)

## Code Quality:
- Class declarations and implementations are nicely split between hpp and cpp files.
- Clean file structure of AST.
- Class inheritance is well-designed and maps nicely onto the grammar.
- Parser has very slight changes from the ANSI C parser given.
- It would have been nice if you could clean up the grammar by getting rid of unused rules.
