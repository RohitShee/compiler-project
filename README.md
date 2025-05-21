# 🔧 Compiler Project - Lexical & Syntax Analyzer

This project implements a mini compiler front-end that performs **Lexical Analysis**, **Symbol Table Generation**, **Canonical LR(1) Parsing**, and **Syntax Validation** for a simplified C-like language.

---

## 📄 Components

- **`lexer.cpp`**  
  Performs **lexical analysis**, breaks input code into tokens, and builds a **symbol table** with support for **nested scopes**. Outputs:
  - `*.tokens`: token stream with position info
  - `*.symtab`: scope-wise symbol table
  - `*.parse`: token list for the parser

- **`parser.cpp`**  
  Implements a **Canonical LR(1)** parser. It:
  - Loads grammar from `Grammar.txt`
  - Computes FIRST/FOLLOW sets
  - Constructs item sets and parsing table
  - Parses token stream using LR(1) logic
  - Outputs: `augmented_grammar.txt`, `item_sets.txt`, `parsing_table.txt`, `parsing_steps.txt`

- **`Grammar.txt`**  
  Contains the **context-free grammar** of the language in BNF format. Includes rules for declarations, expressions, control structures, and function definitions.

- **`sample.txt`**  
  A sample **source program** that demonstrates the capabilities of the compiler components. It includes variable and function declarations, conditionals, expressions, and function calls.

---

## 🧠 Features

- ✅ Tokenization with line/column tracking
- ✅ Symbol Table with hierarchical scoping
- ✅ FIRST and FOLLOW set computation
- ✅ Canonical LR(1) item set and parsing table generation
- ✅ Grammar augmentation and syntax parsing
- ✅ Sample test input (`sample.txt`) with full walkthrough

---

## ⚙️ How to Compile

Ensure you have a C++ compiler (e.g., `g++`).

```bash
# Compile Lexer
g++ lexer.cpp -o lexer

# Compile Parser
g++ parser.cpp -o parser
```
## 🧪 How to Run the Project

Follow these steps to run the compiler components on a sample input:

### 🔹 Step 1: Lexical Analysis & Symbol Table Generation

Run the lexer to tokenize the source code and generate the symbol table.

```bash
./lexer sample.txt
```
## This will produce the following output files:

sample.txt.tokens → Token stream with types and lexemes.

sample.txt.parse → Space-separated token types for the parser.

sample.txt.symtab → Hierarchical symbol table with scopes and declarations.

### 🔹 Step 2: Grammar-Based LR(1) Parsing
Run the parser with the output token stream and the grammar file:

```bash

./parser sample.txt.parse Grammar.txt
```
## This will generate:

augmented_grammar.txt → Augmented grammar with the start symbol.

terminals_non_terminals.txt → List of terminals and non-terminals.

item_sets.txt → Canonical LR(1) item sets and their transitions.

parsing_table.txt → Action and GOTO parsing tables.

parsing_steps.txt → Step-by-step parsing trace for debugging.
