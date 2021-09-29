#ifndef __SIMCFG_H__
#define __SIMCFG_H__

#include "grammar.h"

void delDuplicatedRules();
void delUnnecessaryDerivingSymbols();
void delNullProductions();
void delUnitProductions();
void delNonDerivingSymbols();
void delNonReachableSymbols();

#endif