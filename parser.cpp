#include <iostream>
#include <fstream>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <sstream>
#include <algorithm>
#include <cctype>
#include <stack>
#include <functional>

using namespace std;

struct Production
{
    string lhs;
    vector<string> rhs;
    int id;

    bool operator==(const Production &other) const
    {
        return lhs == other.lhs && rhs == other.rhs;
    }
};

namespace std
{
    template <>
    struct hash<Production>
    {
        size_t operator()(const Production &p) const
        {
            size_t h1 = hash<string>()(p.lhs);
            size_t h2 = 0;
            for (const auto &sym : p.rhs)
            {
                h2 ^= hash<string>()(sym) + 0x9e3779b9 + (h2 << 6) + (h2 >> 2);
            }
            return h1 ^ (h2 << 1);
        }
    };
}

class Grammar
{
public:
    vector<Production> productions;
    unordered_set<string> terminals;
    unordered_set<string> non_terminals;
    string start_symbol;
    string augmented_start = "<$START>";
    unordered_map<string, unordered_set<string>> first;
    unordered_map<string, unordered_set<string>> follow;

    void load(const string &filename)
    {
        ifstream file(filename);
        if (!file.is_open())
        {
            cerr << "Error opening grammar file." << endl;
            return;
        }

        string line;
        bool first_production = true;
        int prod_id = 0;

        while (getline(file, line))
        {
            line = trim(line);
            if (line.empty())
                continue;

            size_t arrow_pos = line.find("->");
            if (arrow_pos == string::npos)
                continue;

            string lhs = trim(line.substr(0, arrow_pos));
            string rhs_str = trim(line.substr(arrow_pos + 2));

            if (first_production)
            {
                start_symbol = lhs;
                first_production = false;
            }

            vector<string> alternatives = split_alternatives(rhs_str);

            for (const auto &alt : alternatives)
            {
                Production prod;
                prod.lhs = lhs;
                prod.rhs = split_symbols(alt);
                prod.id = prod_id++;
                productions.push_back(prod);
            }
        }

        augment_grammar();
        compute_first();
        compute_follow();
    }

    void augment_grammar()
    {
        Production aug_prod;
        aug_prod.lhs = augmented_start;
        aug_prod.rhs = {start_symbol};
        aug_prod.id = productions.size();
        productions.insert(productions.begin(), aug_prod);
        non_terminals.insert(augmented_start);
        start_symbol = augmented_start;
    }

    void compute_first()
    {
        for (const auto &prod : productions)
        {
            for (const auto &sym : prod.rhs)
            {
                if (is_terminal(sym))
                {
                    terminals.insert(sym);
                }
                else
                {
                    non_terminals.insert(sym);
                }
            }
        }
        terminals.insert("$");

        for (const auto &t : terminals)
        {
            first[t].insert(t);
        }

        for (const auto &nt : non_terminals)
        {
            first[nt] = unordered_set<string>();
        }

        bool changed;
        do
        {
            changed = false;
            for (const auto &prod : productions)
            {
                const string &lhs = prod.lhs;
                const auto &rhs = prod.rhs;

                unordered_set<string> new_first = compute_first_for_sequence(rhs);

                for (const auto &sym : new_first)
                {
                    if (first[lhs].find(sym) == first[lhs].end())
                    {
                        first[lhs].insert(sym);
                        changed = true;
                    }
                }
            }
        } while (changed);
    }

    unordered_set<string> compute_first_for_sequence(const vector<string> &seq)
    {
        unordered_set<string> result;
        bool can_derive_epsilon = true;

        for (const auto &sym : seq)
        {
            const auto &sym_first = first[sym];
            bool has_epsilon = sym_first.find("EPSILON") != sym_first.end();

            for (const auto &s : sym_first)
            {
                if (s != "EPSILON")
                {
                    result.insert(s);
                }
            }

            if (!has_epsilon)
            {
                can_derive_epsilon = false;
                break;
            }
        }

        if (can_derive_epsilon)
        {
            result.insert("EPSILON");
        }

        return result;
    }

    void compute_follow()
    {
        follow[start_symbol].insert("$");

        bool changed;
        do
        {
            changed = false;
            for (const auto &prod : productions)
            {
                const auto &rhs = prod.rhs;
                for (size_t i = 0; i < rhs.size(); ++i)
                {
                    const string &B = rhs[i];
                    if (non_terminals.find(B) == non_terminals.end())
                        continue;

                    vector<string> beta(rhs.begin() + i + 1, rhs.end());
                    unordered_set<string> first_beta = compute_first_for_sequence(beta);

                    size_t before_size = follow[B].size();
                    for (const auto &s : first_beta)
                    {
                        if (s != "EPSILON")
                        {
                            follow[B].insert(s);
                        }
                    }
                    if (follow[B].size() != before_size)
                    {
                        changed = true;
                    }

                    if (first_beta.find("EPSILON") != first_beta.end() || beta.empty())
                    {
                        before_size = follow[B].size();
                        for (const auto &s : follow[prod.lhs])
                        {
                            follow[B].insert(s);
                        }
                        if (follow[B].size() != before_size)
                        {
                            changed = true;
                        }
                    }
                }
            }
        } while (changed);
    }

    bool is_terminal(const string &sym)
    {
        return !sym.empty() && sym.front() != '<';
    }

    static string trim(const string &s)
    {
        size_t start = s.find_first_not_of(" \t");
        size_t end = s.find_last_not_of(" \t");
        if (start == string::npos)
            return "";
        return s.substr(start, end - start + 1);
    }

    static vector<string> split_alternatives(const string &s)
    {
        vector<string> alternatives;
        stringstream ss(s);
        string alt;
        while (getline(ss, alt, '|'))
        {
            alternatives.push_back(trim(alt));
        }
        return alternatives;
    }

    static vector<string> split_symbols(const string &s)
    {
        vector<string> symbols;
        stringstream ss(s);
        string sym;
        while (ss >> sym)
        {
            if (sym == "EPSILON")
                continue;
            symbols.push_back(sym);
        }
        return symbols;
    }

    void write_augmented_grammar(const string &filename)
    {
        ofstream file(filename);
        file << "Augmented Grammar:\n";
        file << "Start Symbol: " << start_symbol << "\n\n";
        for (const auto &prod : productions)
        {
            file << prod.lhs << " -> ";
            for (const auto &sym : prod.rhs)
            {
                file << sym << " ";
            }
            file << "\n";
        }
    }

    void write_symbols(const string &filename)
    {
        ofstream file(filename);
        file << "Terminals:\n";
        for (const auto &t : terminals)
            file << t << "\n";
        file << "\nNon-Terminals:\n";
        for (const auto &nt : non_terminals)
            file << nt << "\n";
    }
};

struct LR1Item
{
    const Production *prod;
    int dot_pos;
    string lookahead;

    bool operator==(const LR1Item &other) const
    {
        return prod == other.prod && dot_pos == other.dot_pos && lookahead == other.lookahead;
    }
};

namespace std
{
    template <>
    struct hash<LR1Item>
    {
        size_t operator()(const LR1Item &item) const
        {
            size_t h1 = hash<const Production *>()(item.prod);
            size_t h2 = hash<int>()(item.dot_pos);
            size_t h3 = hash<string>()(item.lookahead);
            return h1 ^ (h2 << 1) ^ (h3 << 2);
        }
    };
}

class CanonicalLR1
{
public:
    Grammar *grammar;
    vector<unordered_set<LR1Item>> states;
    unordered_map<int, unordered_map<string, int>> goto_table;
    unordered_map<int, unordered_map<string, string>> action_table;

    void build(Grammar &g)
    {
        grammar = &g;
        const Production &aug_prod = g.productions[0];
        LR1Item initial_item{&aug_prod, 0, "$"};
        auto initial_closure = closure({initial_item});
        states.push_back(initial_closure);

        queue<int> process_queue;
        process_queue.push(0);

        unordered_map<int, bool> processed;

        while (!process_queue.empty())
        {
            int state_idx = process_queue.front();
            process_queue.pop();

            if (processed[state_idx])
                continue;
            processed[state_idx] = true;

            unordered_set<string> symbols;
            for (const auto &item : states[state_idx])
            {
                if (item.dot_pos < item.prod->rhs.size())
                {
                    symbols.insert(item.prod->rhs[item.dot_pos]);
                }
            }

            for (const auto &sym : symbols)
            {
                auto new_state = goto_state(states[state_idx], sym);
                if (new_state.empty())
                    continue;

                int new_state_idx = -1;
                for (size_t i = 0; i < states.size(); ++i)
                {
                    if (states[i] == new_state)
                    {
                        new_state_idx = i;
                        break;
                    }
                }

                if (new_state_idx == -1)
                {
                    states.push_back(new_state);
                    new_state_idx = states.size() - 1;
                    process_queue.push(new_state_idx);
                }

                if (grammar->terminals.count(sym))
                {
                    action_table[state_idx][sym] = "s" + to_string(new_state_idx);
                }
                else
                {
                    goto_table[state_idx][sym] = new_state_idx;
                }
            }
        }

        for (size_t state_idx = 0; state_idx < states.size(); ++state_idx)
        {
            for (const auto &item : states[state_idx])
            {
                if (item.dot_pos == item.prod->rhs.size())
                {
                    string la = item.lookahead;
                    if (item.prod->lhs == grammar->augmented_start && la == "$")
                    {
                        action_table[state_idx][la] = "acc";
                    }
                    else
                    {
                        int prod_idx = find_production_index(*item.prod);
                        if (prod_idx == -1)
                            continue;
                        if (action_table[state_idx].count(la) && action_table[state_idx][la] != "r" + to_string(prod_idx))
                        {
                            cerr << "Conflict in action table!" << endl;
                        }
                        action_table[state_idx][la] = "r" + to_string(prod_idx);
                    }
                }
            }
        }
    }

    unordered_set<LR1Item> closure(const unordered_set<LR1Item> &items)
    {
        unordered_set<LR1Item> closure_set = items;
        queue<LR1Item> q;
        for (const auto &item : items)
            q.push(item);

        while (!q.empty())
        {
            LR1Item item = q.front();
            q.pop();

            if (item.dot_pos >= item.prod->rhs.size())
                continue;
            string B = item.prod->rhs[item.dot_pos];
            if (grammar->non_terminals.find(B) == grammar->non_terminals.end())
                continue;

            vector<string> beta(item.prod->rhs.begin() + item.dot_pos + 1, item.prod->rhs.end());
            beta.push_back(item.lookahead);
            auto first_beta = grammar->compute_first_for_sequence(beta);

            for (const auto &prod : grammar->productions)
            {
                if (prod.lhs == B)
                {
                    for (const auto &b : first_beta)
                    {
                        if (b == "EPSILON")
                            continue;
                        LR1Item new_item{&prod, 0, b};
                        if (closure_set.insert(new_item).second)
                        {
                            q.push(new_item);
                        }
                    }
                }
            }
        }

        return closure_set;
    }

    unordered_set<LR1Item> goto_state(const unordered_set<LR1Item> &state, const string &sym)
    {
        unordered_set<LR1Item> moved;

        for (const auto &item : state)
        {
            if (item.dot_pos < item.prod->rhs.size() && item.prod->rhs[item.dot_pos] == sym)
            {
                LR1Item new_item = item;
                new_item.dot_pos++;
                moved.insert(new_item);
            }
        }

        return closure(moved);
    }

    int find_production_index(const Production &p)
    {
        for (size_t i = 0; i < grammar->productions.size(); ++i)
        {
            if (grammar->productions[i] == p)
            {
                return i;
            }
        }
        return -1;
    }

    void write_item_sets(const string &filename)
    {
        ofstream file(filename);
        for (size_t i = 0; i < states.size(); i++)
        {
            file << "State " << i << ":\n";
            unordered_map<string, int> transitions;

            // Collect transitions
            if (goto_table.count(i))
            {
                for (const auto &entry : goto_table[i])
                {
                    transitions[entry.first] = entry.second;
                }
            }
            if (action_table.count(i))
            {
                for (const auto &entry : action_table[i])
                {
                    if (entry.second[0] == 's')
                    {
                        transitions[entry.first] = stoi(entry.second.substr(1));
                    }
                }
            }

            // Print items
            for (const auto &item : states[i])
            {
                file << "  ";
                file << item.prod->lhs << " -> ";
                for (size_t j = 0; j < item.prod->rhs.size(); j++)
                {
                    if (j == item.dot_pos)
                        file << ". ";
                    file << item.prod->rhs[j] << " ";
                }
                if (item.dot_pos == item.prod->rhs.size())
                    file << ". ";
                file << "[" << item.lookahead << "]\n";
            }

            // Print transitions
            file << "\n  Transitions:\n";
            for (const auto &[sym, state] : transitions)
            {
                file << "    " << sym << " -> " << state << "\n";
            }
            file << "------------------------\n";
        }
    }

    void write_parsing_table(const string &filename)
    {
        ofstream file(filename);
        file << "Parsing Table:\n";
        file << "State\tAction\n";
        for (const auto &[state, actions] : action_table)
        {
            file << state << "\t";
            for (const auto &[term, action] : actions)
            {
                file << term << ":" << action << " ";
            }
            file << "\n";
        }

        file << "\nGoto Table:\n";
        for (const auto &[state, gotos] : goto_table)
        {
            file << state << "\t";
            for (const auto &[nonterm, dest] : gotos)
            {
                file << nonterm << ":" << dest << " ";
            }
            file << "\n";
        }
    }
};

class Parser
{
    ofstream step_file;

public:
    CanonicalLR1 &clr;
    stack<pair<int, string>> state_stack;

    Parser(CanonicalLR1 &clr, const string &filename) : clr(clr)
    {
        state_stack.push({0, ""});
        step_file.open(filename);
        step_file << "Parsing Steps:\n";
    }

    ~Parser() { step_file.close(); }

    void log_state(size_t pos, const vector<string> &tokens)
    {
        step_file << "Stack: ";
        stack<pair<int, string>> temp = state_stack;
        while (!temp.empty())
        {
            step_file << temp.top().first << " ";
            temp.pop();
        }
        step_file << "\nInput: ";
        for (size_t i = pos; i < tokens.size(); i++)
        {
            step_file << tokens[i] << " ";
        }
        step_file << "\n";
    }

    bool parse(const vector<string> &input)
    {
        vector<string> tokens = input;
        tokens.push_back("$");
        size_t pos = 0;

        while (pos < tokens.size())
        {
            log_state(pos, tokens);
            int current_state = state_stack.top().first;
            string current_token = tokens[pos];

            if (clr.action_table[current_state].find(current_token) == clr.action_table[current_state].end())
            {
                return false;
            }

            string action = clr.action_table[current_state][current_token];

            if (action == "acc")
            {
                return true;
            }
            else if (action[0] == 's')
            {
                int new_state = stoi(action.substr(1));
                state_stack.push({new_state, current_token});
                pos++;
            }
            else if (action[0] == 'r')
            {
                int prod_idx = stoi(action.substr(1));
                const Production &prod = clr.grammar->productions[prod_idx];
                for (size_t i = 0; i < prod.rhs.size(); ++i)
                {
                    state_stack.pop();
                }
                int new_state = state_stack.top().first;
                string lhs = prod.lhs;
                if (clr.goto_table[new_state].find(lhs) == clr.goto_table[new_state].end())
                {
                    return false;
                }
                int goto_state = clr.goto_table[new_state][lhs];
                state_stack.push({goto_state, lhs});
            }
            else
            {
                return false;
            }
            step_file << "Action: " << action << "\n\n";
        }

        return false;
    }
};

vector<string> read_input(const string &filename)
{
    ifstream file(filename);
    vector<string> tokens;
    string token;
    while (file >> token)
    {
        tokens.push_back(token);
    }
    return tokens;
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        cerr << "Usage: " << argv[0] << " <input_file>" << " <grammar_file>" << endl;
        return 1;
    }

    Grammar grammar;
    grammar.load(argv[2]);

    CanonicalLR1 clr;
    clr.build(grammar);

    vector<string> input = read_input(argv[1]);
    Parser parser(clr, "parsing_steps.txt");

    grammar.write_augmented_grammar("augmented_grammar.txt");
    grammar.write_symbols("terminals_non_terminals.txt");
    clr.write_item_sets("item_sets.txt");
    clr.write_parsing_table("parsing_table.txt");

    bool result = parser.parse(input);

    if (result)
    {
        cout << "Input is valid." << endl;
    }
    else
    {
        cout << "Input is invalid." << endl;
    }

    return 0;
}