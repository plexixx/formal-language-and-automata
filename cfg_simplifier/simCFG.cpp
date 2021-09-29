#include "simCFG.h"

using namespace std;

/* 1.消除重复的产生式 */
void delDuplicatedRules()
{
    for (RuleSet &ruleset : grammar)
    {
        Rules &rules = ruleset.second;
        set<Rule> setofrules(rules.begin(), rules.end());           // store in set
        rules = vector<Rule>(setofrules.begin(), setofrules.end()); // convert back to vector form
    }
}

/* 2.消除无用的非终结符 */
void delUnnecessaryDerivingSymbols()
{
    Symbols non_terminals, terminals, deriving_symbols;
    getSymbolType(non_terminals, terminals, deriving_symbols);
    for (Symbol deriving_symbol : deriving_symbols)
    {
        // for each deriving symbol check if any rule it derives
        if (grammar.count(deriving_symbol) > 0 && grammar[deriving_symbol].size() == 0)
        {
            grammar.erase(deriving_symbol); // if symbol deriving no rule remove it ..
        }
    }
}

/* 3.消除文法中的ε产生式 */
void delNullProductions()
{
    Symbols nullables;
    Symbols non_terminals, terminals, deriving_symbols;
    getSymbolType(non_terminals, terminals, deriving_symbols);

    // Find Nullable Non-terminals directly from rules
    for (Symbol nonterminal : non_terminals)
    {
        if (grammar.count(nonterminal) > 0)
        {
            Rules &rules = grammar[nonterminal];
            for (Rule &rule : rules)
            {
                if (rule.size() == 0)
                    nullables.insert(nonterminal); // cout << "Nullable " << nonterminal << endl ;
            }
        }
    }
    cout << "Initial Set of Nullables: ";
    printSymbols(nullables);
    Symbols newnullables = nullables;
    ;
    // Find all nullable non-terminals
    do
    {
        nullables = newnullables;
        for (Symbol nonterminal : non_terminals)
        {
            if (grammar.count(nonterminal) > 0)
            {
                Rules &rules = grammar[nonterminal];
                for (Rule &rule : rules)
                {
                    if (rule.size() != 0 && contained_in(rule, newnullables) && newnullables.count(nonterminal) == 0)
                        cout << "New Nullable Non-terminal: " << nonterminal << endl, newnullables.insert(nonterminal);
                }
            }
        }
    } while (newnullables.size() > nullables.size()); // do it till no more new nullables
    cout << "Final Set of Nullables: ";
    printSymbols(nullables);

    // Now generate new rules using nullables
    for (Symbol nullable : nullables)
    {
        for (RuleSet &ruleset : grammar)
        {
            Symbol deriving_symbol = ruleset.first;
            Rules &rules = ruleset.second;
            for (int i = 0; i < rules.size(); i++)
            {
                for (int j = 0; j < rules[i].size(); j++)
                {
                    if (rules[i][j] == nullable)
                    {
                        Rule newrule = rules[i];
                        newrule.erase(newrule.begin() + j);
                        addRule(deriving_symbol, newrule);
                    }
                }
            }
        }
    }
    //printGrammar();
    // Now remove all NULL rules
    for (RuleSet &ruleset : grammar)
    {
        Rules &rules = ruleset.second;
        for (int i = 0; i < rules.size(); i++)
        {
            if (rules[i].size() == 0)                // if null production remove it
                rules.erase(rules.begin() + i), i--; // decrement count to check next rule
        }
    }
    // since we have deleted rules there might exist some deriving symbols
    // deriving no rules so remove them
    delUnnecessaryDerivingSymbols();
    //printGrammar();
    // Now remove all duplicate rules if any exist
    delDuplicatedRules();
    //printGrammar();
    if (nullables.count(start) > 0)
    {
        // if start symbol is nullable then just warn
        cout << "WARNING: Start Symbol " << start << " is NULLABLE and is ignored." << endl;
    }
}

/* 3.消除文法中的单产生式 */
void delUnitProductions()
{
    map<Symbol, Symbols> unit_productions;
    Symbols non_terminals, terminals, deriving_symbols;
    getSymbolType(non_terminals, terminals, deriving_symbols);

    // set the initial set of unit productions
    for (Symbol nonterminal : non_terminals)
        unit_productions[nonterminal].insert(nonterminal);

    // Find all unit productions
    map<Symbol, Symbols> new_unit_productions = unit_productions;
    do
    {
        unit_productions = new_unit_productions;
        for (RuleSet &ruleset : grammar)
        {
            Symbol deriving_symbol = ruleset.first;
            Rules &rules = ruleset.second;
            for (Rule rule : rules)
            {
                if (rule.size() == 1 && rule[0] != deriving_symbol && !isTerminal(rule[0]))
                {
                    // if unit production found
                    // Note: Terminals and Rules like A->A not included as unit productions
                    Symbol symbol = rule[0]; // deriving_symbol -> symbol
                    for (Symbol unitproduction : unit_productions[symbol])
                    {
                        new_unit_productions[deriving_symbol].insert(unitproduction);
                    }
                }
            }
        }
    } while (new_unit_productions != unit_productions);
    // Print the Unit Productions
    for (pair<const Symbol, Symbols> unitproductionlist : unit_productions)
    {
        cout << unitproductionlist.first << " : ";
        printSymbols(unitproductionlist.second);
    }

    // Now remove all unit productions from grammar
    for (RuleSet &ruleset : grammar)
    {
        Rules &rules = ruleset.second;
        for (int i = 0; i < rules.size(); i++)
        {
            if (rules[i].size() == 1 && !isTerminal(rules[i][0]))
            {
                // if not a terminal then it is unit production remove it
                rules.erase(rules.begin() + i);
                i--; // decrement to check the next rule ..
            }
        }
    }
    //printGrammar();
    // since we have erased rules
    // there may be some non-deriving terminals remove them
    delUnnecessaryDerivingSymbols();
    //printGrammar();
    // Now add new rules
    for (pair<const Symbol, Symbols> unitproductionlist : unit_productions)
    {
        Symbol deriving_symbol = unitproductionlist.first;
        Symbols unitproductionset = unitproductionlist.second;
        for (Symbol symbol : unitproductionset)
        {
            if (symbol != deriving_symbol &&
                grammar.count(symbol) > 0 &&
                grammar[symbol].size() > 0)
            {
                // check if not the same symbol ..
                // then add all rules of symbol to deriving symbol
                for (Rule rule : grammar[symbol])
                {
                    addRule(deriving_symbol, rule);
                }
                // Now remove all duplicate rules if any exist
                delDuplicatedRules();
            }
        }
    }
    //printGrammar();
    // Now remove all duplicate rules if any exist
    // Just in case ensuring not necessay here
    delDuplicatedRules();
    //printGrammar();
}

/* 4.消除文法中的单产生式 */
void delUnitProductions()
{
    map<Symbol, Symbols> unit_productions;
    Symbols non_terminals, terminals, deriving_symbols;
    getSymbolType(non_terminals, terminals, deriving_symbols);

    // set the initial set of unit productions
    for (Symbol non_terminal : non_terminals)
        unit_productions[non_terminal].insert(non_terminal);

    // Find all unit productions
    map<Symbol, Symbols> new_unit_productions = unit_productions;
    do
    {
        unit_productions = new_unit_productions;
        for (RuleSet &ruleset : grammar)
        {
            Symbol deriving_symbol = ruleset.first;
            Rules &rules = ruleset.second;
            for (Rule rule : rules)
            {
                if (rule.size() == 1 && rule[0] != deriving_symbol && !isTerminal(rule[0]))
                {
                    // if unit production found
                    // Note: Terminals and Rules like A->A not included as unit productions
                    Symbol symbol = rule[0]; // deriving_symbol -> symbol
                    for (Symbol unit_production : unit_productions[symbol])
                    { 
                        new_unit_productions[deriving_symbol].insert(unit_production);
                    }
                }
            }
        }
    } while (new_unit_productions != unit_productions);
    // Print the Unit Productions
    for (pair<const Symbol, Symbols> unitproductionlist : unit_productions)
    {
        cout << unitproductionlist.first << " : ";
        printSymbols(unitproductionlist.second);
    }

    // Now remove all unit productions from grammar
    for (RuleSet &ruleset : grammar)
    {
        Rules &rules = ruleset.second;
        for (int i = 0; i < rules.size(); i++)
        {
            if (rules[i].size() == 1 && !isTerminal(rules[i][0]))
            {
                // if not a terminal then it is unit production remove it
                rules.erase(rules.begin() + i);
                i--; // decrement to check the next rule ..
            }
        }
    }
    //printGrammar();
    // since we have erased rules
    // there may be some non-deriving terminals remove them
    delUnnecessaryDerivingSymbols();
    //printGrammar();
    // Now add new rules
    for (pair<const Symbol, Symbols> unitproductionlist : unit_productions)
    {
        Symbol deriving_symbol = unitproductionlist.first;
        Symbols unitproductionset = unitproductionlist.second;
        for (Symbol symbol : unitproductionset)
        {
            if (symbol != deriving_symbol &&
                grammar.count(symbol) > 0 &&
                grammar[symbol].size() > 0)
            {
                // check if not the same symbol ..
                // then add all rules of symbol to deriving symbol
                for (Rule rule : grammar[symbol])
                {
                    addRule(deriving_symbol, rule);
                }
                // Now remove all duplicate rules if any exist
                delDuplicatedRules();
            }
        }
    }
    //printGrammar();
    // Now remove all duplicate rules if any exist
    // Just in case ensuring not necessay here
    delDuplicatedRules();
    //printGrammar();
}

/* 5.消除文法中的无用符号 */
void delNonDerivingSymbols()
{
    Symbols non_terminals, terminals, deriving_symbols;
    getSymbolType(non_terminals, terminals, deriving_symbols);

    Symbols derivingterminals = terminals; // symbols that derive terminals
    // Initialize with set of terminals
    cout << "Initial Deriving Symbols: ";
    printSymbols(derivingterminals);
    Symbols newderivingterminals = derivingterminals;
    do
    {
        derivingterminals = newderivingterminals;
        for (RuleSet &ruleset : grammar)
        {
            Symbol deriving_symbol = ruleset.first; // LHS
            Rules &rules = ruleset.second;          // corresponding rules
            for (Rule &rule : rules)
            {
                if (newderivingterminals.count(deriving_symbol) == 0 && // if not  existing  derivable symbol
                    contained_in(rule, newderivingterminals))
                {
                    cout << "New Deriving Symbol :" << deriving_symbol << endl;
                    newderivingterminals.insert(deriving_symbol);
                }
            }
        }

    } while (newderivingterminals != derivingterminals);
    cout << "All Deriving Symbols: ";
    printSymbols(derivingterminals);

    // eliminate rules not containing derivingterminals
    for (RuleSet &ruleset : grammar)
    {
        Symbol deriving_symbol = ruleset.first; // LHS
        Rules &rules = ruleset.second;          // corresponding rules
        for (int i = 0; i < rules.size(); i++)
        {
            // for each rule check if deriving ...
            if (!contained_in(rules[i], derivingterminals))
            {
                // if non deriving rule remove it
                cout << "Removing Rule: ";
                printRule(deriving_symbol, rules[i]);
                rules.erase(rules.begin() + i);
                i--; // to check the next rule
            }
        }
    }

    // now remove symbols not deriving at all
    delUnnecessaryDerivingSymbols();
}

void delNonReachableSymbols()
{
    // Start from startsymbol and obtain all reachable symbols
    Symbols reachablesymbols;
    // Initialize with start symbols
    reachablesymbols.insert(start);
    //get all reachable symbols
    Symbols newreachablesymbols = reachablesymbols;
    do
    {
        reachablesymbols = newreachablesymbols;
        for (Symbol symbol : reachablesymbols)
        {
            // expand all deriving non-terminals to get more reachable symbols
            if (!isTerminal(symbol) && grammar.count(symbol) > 0 && grammar[symbol].size() > 0)
            {                                                          // If non-terminal and deriving
                for (Rule rule : grammar[symbol])                      // for each rule it derives
                    for (Symbol tmpsymbol : rule)                      // get all symbols
                        if (newreachablesymbols.count(tmpsymbol) == 0) // if not already present in reachable symbols
                            cout << "New Reachable Symbol :" << tmpsymbol << endl,
                                newreachablesymbols.insert(tmpsymbol); // insert into reachable symbols
            }
        }
    } while (newreachablesymbols != reachablesymbols);
    // Print All reachable symbols
    cout << "All Reachable Symbols : ";
    printSymbols(reachablesymbols);
    //printGrammar();
    // remove all rules such that the deriving symbol is not reachable
    Symbols non_terminals, terminals, deriving_symbols;
    getSymbolType(non_terminals, terminals, deriving_symbols);

    for (Symbol deriving_symbol : deriving_symbols)
    {
        if (grammar.count(deriving_symbol) > 0 && // Just for sake of verification
            reachablesymbols.count(deriving_symbol) == 0)
        { // but not a reachable symbol
            grammar.erase(deriving_symbol);
            cout << "Removing Productions of " << deriving_symbol << endl;
        }
    }
    // not required but just for completeness
    delUnnecessaryDerivingSymbols();
}

// checks if given rule (list of Symbols) all symbols are in nullables set
bool contained_in(Rule rule, Symbols nullables)
{
    for (Symbol symbol : rule)
        // for each symbol check if it not belongs to the nullables
        if (nullables.count(symbol) == 0)
            return false;

    return true;
}