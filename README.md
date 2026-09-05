2023/24 Compilers Coursework
==============================

There are two components to the coursework:

- Worth 90%:
    - **A C compiler**. The source language is pre-processed C90, and the target language is RISC-V assembly. The target environment is Ubuntu 22.04, as described [here](docs/environment_guide.md). See [here](docs/c_compiler.md) for the full set of requirements and more information about the testing environment.

- Worth 10%:
    - **Overall design style and code readability.** This has been introduced to reward thoughtful planning throughout the project, rather than penalise you. There are no strict guidelines, but you should consider the use of abstraction, your internal representation of instructions, proper Git use, signs of appropriate testing, as well as general code hygiene.
    - **Evidence of time-tracking/project management.** This will be assessed orally at the start of Summer term. See [here](docs/management.md) for more information about this component.

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
