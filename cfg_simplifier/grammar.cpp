#include "grammar.h"
#include "simCFG.h"

Grammar grammar;
Symbol start;

// 初始化语法
void initGrammar()
{
    Symbol next_symbol, non_terminal_new, non_terminal_old;
    Rule rule;

    while (cin >> next_symbol)
    { // for each symbol
        if (next_symbol == "->")
        {
            // 若为生成符号
            // 上个读取的符号为非终结符
            non_terminal_new = rule.back();
            if (isTerminal(non_terminal_new))
            {
                cout << "WARNING: INVALID GRAMMAR! terminal deriving symbols." << endl;
            }
            rule.pop_back();
            if (!rule.empty())
            {
                addRule(non_terminal_old, rule);
            }
            if (grammar.empty())
            {
                // 若语法为空
                start = non_terminal_new; // 当前非终结符为起始符号
                cout << "Note: Start Symbol (" + start + ")" << endl;
            }
            non_terminal_old = non_terminal_new; // 更新非终结符
        }
        else if (next_symbol == "|")
        {
            // 保持当前非终结符不变
            // 将上条规则加入语法
            if (!rule.empty())
                addRule(non_terminal_old, rule);
        }
        else if (next_symbol == "null")
        {
            // 若为ε
            if (!rule.empty())
                cout << "WARNING: symbols exist before a null production." << endl;
            rule.clear();                    // 清空rule
            addRule(non_terminal_old, rule); // 将空规则加入语法
        }
        else
        {
            // 若为其他符号
            rule.push_back(next_symbol);
        }
    }
    if (!rule.empty())
    {
        addRule(non_terminal_old, rule);
    }

    delDuplicatedRules();
}

// 添加产生式
void addRule(Symbol nonterminal, Rule &rule)
{
    grammar[nonterminal].push_back(rule);
    rule.clear();
}

// 判断是否为终结符
bool isTerminal(Symbol symbol)
{
    if (isupper(symbol[0])) // 若首字母为大写
        return false;
    else
        return true;
}

// 获取符号类型
void getSymbolType(Symbols &nonterminals, Symbols &terminals, Symbols &deriving)
{
    terminals.clear();
    nonterminals.clear();
    deriving.clear();
    for (RuleSet &ruleset : grammar)
    {
        // this cannot be modified ruleset.first
        Symbol derivingsymbol = ruleset.first; // get the symbol on the RHS of rules
        deriving.insert(derivingsymbol);
        Rules &rules = ruleset.second; // get the set of rules derived by the deriving symbol
        if (isTerminal(derivingsymbol))
        {
            // if deriving symbol is a terminal INVALID
            terminals.insert(derivingsymbol);
            cout << "WARNING: getterminals(): Deriving Symbol (" + derivingsymbol + ") is a terminal. " << endl;
        }
        else
            nonterminals.insert(derivingsymbol); // Add it as terminal

        for (Rule &rule : rules)
            for (Symbol symbol : rule)  // for each rule
                if (isTerminal(symbol)) // check for terminals
                    terminals.insert(symbol);
                else
                    nonterminals.insert(symbol);
    }
}

// 打印语法
void printGrammar()
{
    Symbols nonterminals, terminals, deriving;
    getSymbolType(nonterminals, terminals, deriving);
    // Print the description of grammar
    cout << "Start Symbol : " << start << endl;
    cout << "Terminals : ";
    printSymbols(terminals);
    cout << "Non-Terminals : ";
    printSymbols(nonterminals);
    cout << "Deriving Symbols : ";
    printSymbols(deriving); // usefull in debugging
    cout << "Rules : " << endl;
    int count = 1; // to display with rule number just to keep track of count
    // get maz size of deriving symbol ..
    int maxsize = 0; // for the formating for aesthetics
    for (RuleSet &ruleset : grammar)
    {
        Symbol derivingsymbol = ruleset.first;
        if (derivingsymbol.size() > maxsize)
            maxsize = derivingsymbol.size();
    }
    for (RuleSet &ruleset : grammar)
    {
        Symbol derivingsymbol = ruleset.first; // get the symbol on the RHS of rules
        Rules &rules = ruleset.second;         // get the set of rules derived by the deriving symbol
        if (rules.size() == 0)
        {
            // if no rule
            cout << "WARNING: Unneccessary deriving symbol (" << derivingsymbol << ")" << endl;
        }

        for (Rule &rule : rules)
        {
            cout << setw(4) << count << ". " << setw(maxsize); // print count
            printRule(derivingsymbol, rule);                   // print rule
            count++;                                           // increment count
        }
    }
}

void printRule(Symbol nonterminal, Rule rule)
{
    cout << nonterminal << " -> ";
    if (rule.empty()) // null production
        cout << "--null--";
    for (Symbol symbol : rule)
        cout << " " << symbol << " ";
    cout << endl;
}

void printSymbols(Symbols symbols)
{
    cout << "(" << symbols.size() << ")";
    for (Symbol terminal : symbols)
        cout << " " << terminal << " ";
    cout << endl;
}
