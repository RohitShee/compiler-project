#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <cctype>
#include <algorithm>

using namespace std;

struct Position
{
    int line;
    int column;
    Position(int l = 1, int c = 1) : line(l), column(c) {}
};

enum TokenType
{
    TOKEN_INT,
    TOKEN_FLOAT,
    TOKEN_VOID,
    TOKEN_IF,
    TOKEN_ELSE,
    TOKEN_READ,
    TOKEN_PRINT,
    TOKEN_ID,
    TOKEN_INT_LIT,
    TOKEN_FLOAT_LIT,
    TOKEN_PLUS,
    TOKEN_MINUS,
    TOKEN_MULTIPLY,
    TOKEN_DIVIDE,
    TOKEN_MOD,
    TOKEN_INCREMENT,
    TOKEN_LT,
    TOKEN_GT,
    TOKEN_EQ,
    TOKEN_EQUALS,
    TOKEN_LBRACE,
    TOKEN_RBRACE,
    TOKEN_LPAREN,
    TOKEN_RPAREN,
    TOKEN_SEMICOLON,
    TOKEN_COMMA,
    TOKEN_RETURN,
    TOKEN_EOF,
    TOKEN_ERROR
};

string tokenTypeToString(TokenType type)
{
    switch (type)
    {
    case TOKEN_INT:
        return "INT";
    case TOKEN_FLOAT:
        return "FLOAT";
    case TOKEN_VOID:
        return "VOID";
    case TOKEN_IF:
        return "IF";
    case TOKEN_ELSE:
        return "ELSE";
    case TOKEN_READ:
        return "READ";
    case TOKEN_PRINT:
        return "PRINT";
    case TOKEN_ID:
        return "ID";
    case TOKEN_INT_LIT:
        return "INT_LIT";
    case TOKEN_FLOAT_LIT:
        return "FLOAT_LIT";
    case TOKEN_PLUS:
        return "PLUS";
    case TOKEN_MINUS:
        return "MINUS";
    case TOKEN_MULTIPLY:
        return "MULTIPLY";
    case TOKEN_DIVIDE:
        return "DIVIDE";
    case TOKEN_MOD:
        return "MOD";
    case TOKEN_INCREMENT:
        return "INCREMENT";
    case TOKEN_LT:
        return "LT";
    case TOKEN_GT:
        return "GT";
    case TOKEN_EQ:
        return "EQ";
    case TOKEN_EQUALS:
        return "EQUALS";
    case TOKEN_LBRACE:
        return "LBRACE";
    case TOKEN_RBRACE:
        return "RBRACE";
    case TOKEN_LPAREN:
        return "LPAREN";
    case TOKEN_RPAREN:
        return "RPAREN";
    case TOKEN_SEMICOLON:
        return "SEMICOLON";
    case TOKEN_COMMA:
        return "COMMA";
    case TOKEN_EOF:
        return "EOF";
    case TOKEN_RETURN:
        return "RETURN";
    default:
        return "ERROR";
    }
}

string positionToString(const Position &pos)
{
    return to_string(pos.line) + ":" + to_string(pos.column);
}

struct Token
{
    TokenType type;
    string lexeme;
    Position pos;
    Token(TokenType t, const string &l, Position p) : type(t), lexeme(l), pos(p) {}
    Token() {}
};

class Lexer
{
    string input;
    int pos;
    Position current_pos;
    unordered_map<string, TokenType> keywords;

public:
    Lexer(const string &input) : input(input), pos(0), current_pos(1, 1)
    {
        keywords = {
            {"int", TOKEN_INT}, {"float", TOKEN_FLOAT}, {"void", TOKEN_VOID}, {"if", TOKEN_IF}, {"else", TOKEN_ELSE}, {"read", TOKEN_READ}, {"print", TOKEN_PRINT}, {"return", TOKEN_RETURN}};
    }

    Token getNextToken(ofstream &tokenFile, ofstream &parseFile)
    {
        skipWhitespaceAndComments();
        if (pos >= input.size())
            return Token(TOKEN_EOF, "", current_pos);

        Position start_pos = current_pos;
        char current = input[pos];
        Token token;
        if (isalpha(current) || current == '_')
            token = readIdentifier(start_pos);
        else if (isdigit(current))
            token = readNumber(start_pos);
        else if (current == '+' || current == '-' || current == '*' || current == '/' || current == '%')
            token = readOperator(start_pos);
        else if (current == '<' || current == '>' || current == '=')
            token = readRelOperator(start_pos);
        else if (current == '{' || current == '}' || current == '(' || current == ')' ||
                 current == ';' || current == ',')
            token = readPunctuation(start_pos);
        else
        {
            pos++;
            current_pos.column++;
            token = Token(TOKEN_ERROR, string(1, current), start_pos);
        }
        tokenFile << setw(10) << left << "[" + positionToString(token.pos) + "]"
                  << setw(15) << left << tokenTypeToString(token.type)
                  << setw(20) << left << token.lexeme << "\n";
        parseFile << tokenTypeToString(token.type) << " ";
        return token;
    }

private:
    void skipWhitespaceAndComments()
    {
        while (pos < input.size())
        {
            if (input[pos] == '\n')
            {
                current_pos.line++;
                current_pos.column = 1;
                pos++;
            }
            else if (isspace(input[pos]))
            {
                current_pos.column++;
                pos++;
            }
            else if (pos + 1 < input.size() && input[pos] == '/' && input[pos + 1] == '/')
            {
                pos += 2;
                while (pos < input.size() && input[pos] != '\n')
                    pos++;
            }
            else
                break;
        }
    }

    Token readIdentifier(Position start_pos)
    {
        int start = pos;
        while (pos < input.size() && (isalnum(input[pos]) || input[pos] == '_'))
        {
            pos++;
            current_pos.column++;
        }
        string lexeme = input.substr(start, pos - start);
        return keywords.count(lexeme)
                   ? Token(keywords[lexeme], lexeme, start_pos)
                   : Token(TOKEN_ID, lexeme, start_pos);
    }

    Token readNumber(Position start_pos)
    {
        int start = pos;
        bool isFloat = false;
        while (pos < input.size() && isdigit(input[pos]))
        {
            pos++;
            current_pos.column++;
        }
        if (pos < input.size() && input[pos] == '.')
        {
            isFloat = true;
            pos++;
            current_pos.column++;
            while (pos < input.size() && isdigit(input[pos]))
            {
                pos++;
                current_pos.column++;
            }
        }
        string lexeme = input.substr(start, pos - start);
        return Token(isFloat ? TOKEN_FLOAT_LIT : TOKEN_INT_LIT, lexeme, start_pos);
    }

    Token readOperator(Position start_pos)
    {
        char c = input[pos++];
        current_pos.column++;

        if (c == '+' && pos < input.size() && input[pos] == '+')
        {
            pos++;
            current_pos.column++;
            return Token(TOKEN_INCREMENT, "++", start_pos);
        }

        TokenType type = TOKEN_ERROR;
        switch (c)
        {
        case '+':
            type = TOKEN_PLUS;
            break;
        case '-':
            type = TOKEN_MINUS;
            break;
        case '*':
            type = TOKEN_MULTIPLY;
            break;
        case '/':
            type = TOKEN_DIVIDE;
            break;
        case '%':
            type = TOKEN_MOD;
            break;
        }
        return Token(type, string(1, c), start_pos);
    }

    Token readRelOperator(Position start_pos)
    {
        char c = input[pos++];
        current_pos.column++;

        if (c == '=' && pos < input.size() && input[pos] == '=')
        {
            pos++;
            current_pos.column++;
            return Token(TOKEN_EQ, "==", start_pos);
        }

        TokenType type = TOKEN_ERROR;
        switch (c)
        {
        case '<':
            type = TOKEN_LT;
            break;
        case '>':
            type = TOKEN_GT;
            break;
        case '=':
            type = TOKEN_EQUALS;
            break;
        }
        return Token(type, string(1, c), start_pos);
    }

    Token readPunctuation(Position start_pos)
    {
        char c = input[pos++];
        current_pos.column++;

        TokenType type = TOKEN_ERROR;
        switch (c)
        {
        case '{':
            type = TOKEN_LBRACE;
            break;
        case '}':
            type = TOKEN_RBRACE;
            break;
        case '(':
            type = TOKEN_LPAREN;
            break;
        case ')':
            type = TOKEN_RPAREN;
            break;
        case ';':
            type = TOKEN_SEMICOLON;
            break;
        case ',':
            type = TOKEN_COMMA;
            break;
        }
        return Token(type, string(1, c), start_pos);
    }
};

struct FunctionInfo
{
    string return_type;
    vector<tuple<string, string, Position>> params; // (type, name, position)
    Position decl_pos;
};

struct SymbolEntry
{
    enum class Kind
    {
        Variable,
        Function
    };
    Kind kind;
    string var_type;        // For variables
    FunctionInfo func_info; // For functions
    Position decl_pos;
};

class Scope
{
public:
    using Ptr = shared_ptr<Scope>;
    unordered_map<string, SymbolEntry> symbols;
    Ptr parent;
    Position scope_start;

    Scope(Ptr p = nullptr, Position start = Position())
        : parent(p), scope_start(start) {}
};

class SymbolTable
{
    Scope::Ptr current_scope;
    Scope::Ptr global_scope;
    vector<Scope::Ptr> all_scopes;

public:
    SymbolTable()
    {
        global_scope = make_shared<Scope>();
        current_scope = global_scope;
        all_scopes.push_back(global_scope);
    }

    void enterScope(Position pos)
    {
        auto new_scope = make_shared<Scope>(current_scope, pos);
        current_scope = new_scope;
        all_scopes.push_back(new_scope);
    }

    void exitScope()
    {
        if (current_scope->parent)
        {
            current_scope = current_scope->parent;
        }
    }

    bool insertVariable(const string &name, const string &type, Position pos)
    {
        if (current_scope->symbols.count(name))
            return false;
        current_scope->symbols[name] = {
            SymbolEntry::Kind::Variable,
            type,
            FunctionInfo(),
            pos};
        return true;
    }

    bool insertFunction(const string &name, const string &return_type,
                        const vector<tuple<string, string, Position>> &params, Position pos)
    {
        if (global_scope->symbols.count(name))
            return false;
        global_scope->symbols[name] = {
            SymbolEntry::Kind::Function,
            "",
            {return_type, params, pos},
            pos};
        return true;
    }

    SymbolEntry *lookup(const string &name)
    {
        Scope::Ptr scope = current_scope;
        while (scope)
        {
            auto it = scope->symbols.find(name);
            if (it != scope->symbols.end())
                return &it->second;
            scope = scope->parent;
        }
        return nullptr;
    }

    void print(ofstream &outFile)
    {
        outFile << "Symbol Table Hierarchy:\n";
        for (auto &scope : all_scopes)
        {
            outFile << "Scope started at " << positionToString(scope->scope_start) << "\n";

            // Print functions first
            for (auto &[name, entry] : scope->symbols)
            {
                if (entry.kind == SymbolEntry::Kind::Function)
                {
                    outFile << "  ◉ Function: " << name << " → " << entry.func_info.return_type
                            << " (declared at " << positionToString(entry.decl_pos) << ")\n";
                    for (auto &[t, n, _] : entry.func_info.params)
                    {
                        outFile << "    ⤷ Parameter: " << n << " : " << t << "\n";
                    }
                }
            }

            // Print variables
            for (auto &[name, entry] : scope->symbols)
            {
                if (entry.kind == SymbolEntry::Kind::Variable)
                {
                    outFile << "  ■ Variable: " << name << " : " << entry.var_type
                            << " (declared at " << positionToString(entry.decl_pos) << ")\n";
                }
            }
            outFile << "━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n";
        }
    }
};

void processFunctionDecl(Lexer &lexer, SymbolTable &symtab, const string &return_type, const Token &id_token, ofstream &tokenFile, ofstream &parseFile)
{
    vector<tuple<string, string, Position>> params;
    Token token = lexer.getNextToken(tokenFile, parseFile);

    // Parse parameters
    while (token.type != TOKEN_RPAREN && token.type != TOKEN_EOF)
    {
        if (token.type == TOKEN_INT || token.type == TOKEN_FLOAT)
        {
            string type = token.lexeme;
            Token name = lexer.getNextToken(tokenFile, parseFile);
            if (name.type == TOKEN_ID)
            {
                params.emplace_back(type, name.lexeme, name.pos);
                Token comma = lexer.getNextToken(tokenFile, parseFile);
                if (comma.type != TOKEN_COMMA)
                    break;
                token = lexer.getNextToken(tokenFile, parseFile);
            }
        }
        else
        {
            break;
        }
    }

    // Insert function into global scope
    if (!symtab.insertFunction(id_token.lexeme, return_type, params, id_token.pos))
    {
        cerr << "Error: Function " << id_token.lexeme << " already declared at "
             << id_token.pos.line << ":" << id_token.pos.column << endl;
    }

    // Enter function scope
    Token lbrace = lexer.getNextToken(tokenFile, parseFile);
    if (lbrace.type == TOKEN_LBRACE)
    {
        symtab.enterScope(lbrace.pos);
        // Insert parameters into function scope
        for (auto &[type, name, pos] : params)
        {
            if (!symtab.insertVariable(name, type, pos))
            {
                cerr << "Error: Parameter " << name << " already declared\n";
            }
        }
    }
}

void processFile(const string &filename)
{
    ifstream file(filename);
    string content((istreambuf_iterator<char>(file)),
                   istreambuf_iterator<char>());

    ofstream symtabFile(filename + ".symtab");
    ofstream tokenFile(filename + ".tokens");
    ofstream parseFile(filename + ".parse");
    if (!symtabFile.is_open() || !tokenFile.is_open() || !parseFile.is_open())
    {
        cerr << "Error opening output files!\n";
        return;
    }

    Lexer lexer(content);
    SymbolTable symtab;
    Token token = lexer.getNextToken(tokenFile, parseFile);

    while (token.type != TOKEN_EOF && token.type != TOKEN_ERROR)
    {
        if (token.type == TOKEN_LBRACE)
        {
            symtab.enterScope(token.pos);
        }
        else if (token.type == TOKEN_RBRACE)
        {
            symtab.exitScope();
        }
        else if (token.type == TOKEN_INT || token.type == TOKEN_FLOAT || token.type == TOKEN_VOID)
        {
            string return_type = token.lexeme;
            Token id_token = lexer.getNextToken(tokenFile, parseFile);
            if (id_token.type == TOKEN_ID)
            {
                Token next = lexer.getNextToken(tokenFile, parseFile);
                if (next.type == TOKEN_LPAREN)
                {
                    processFunctionDecl(lexer, symtab, return_type, id_token, tokenFile, parseFile);
                }
                else
                {
                    // Variable declaration
                    if (!symtab.insertVariable(id_token.lexeme, return_type, id_token.pos))
                    {
                        cerr << "Error: " << id_token.lexeme << " already declared at "
                             << id_token.pos.line << ":" << id_token.pos.column << endl;
                    }
                }
            }
        }
        else if (token.type == TOKEN_ID)
        {
            if (!symtab.lookup(token.lexeme))
            {
                cerr << "Error: Undeclared identifier '" << token.lexeme
                     << "' at " << token.pos.line << ":" << token.pos.column << endl;
            }
        }
        token = lexer.getNextToken(tokenFile, parseFile);
    }
    if (token.type == TOKEN_ERROR)
    {
        throw runtime_error("Unexpected token '" + token.lexeme + "' at " + positionToString(token.pos));
    }
    symtab.print(symtabFile);
    symtabFile.close();
    tokenFile.close();
}

int main(int argc, char *argv[])
{
    if (argc != 2)
    {
        cerr << "Usage: " << argv[0] << " <input_file>\n";
        return 1;
    }

    try
    {
        processFile(argv[1]);
    }
    catch (const exception &e)
    {
        cerr << "Error: " << e.what() << endl;
        return 1;
    }

    return 0;
}
