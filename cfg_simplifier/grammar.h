#ifndef __GRAMMAR_H__
#define __GRAMMAR_H__

#include <algorithm>
#include <iomanip>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <vector>

using namespace std;

/* 类型声明 */
typedef map<string, vector<vector<string>>> Grammar;        // 文法G
typedef pair<const string, vector<vector<string>>> RuleSet; // 非终结符和对应产生式
typedef vector<vector<string>> Rules;                       // 产生式集合
typedef vector<string> Rule;                                // 产生式
typedef set<string> Symbols;                                // 符号集合，包含终结符和非终结符
typedef string Symbol;                                      // 符号，首字母大写为非终结符，其余为终结符
typedef map<Symbol, Symbols> FFSet;                         // used to store first and follow sets

extern Grammar grammar;
extern Symbol start; // 起始符

void initGrammar();                                                               
void addRule(Symbol nonterminal, Rule &rule);                                     
bool isTerminal(Symbol symbol);                                                   
void getSymbolType(Symbols &nonterminals, Symbols &terminals, Symbols &deriving); 
void printGrammar();                                                              
void printRule(Symbol nonterminal, Rule rule);
void printSymbols(Symbols symbols);

#endif