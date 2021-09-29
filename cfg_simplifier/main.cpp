/*
 * 1. Eliminate NULL productions
 * 2. Eliminate UNIT productions
 * 3. Eliminate Non-deriving symbols
 * 4. Eliminate Symbols not appearing in Sentential form 
 */

#include "grammar.h"
#include "simCFG.h"

int main(int argc, char *argv[])
{
    initGrammar();
    return 0;
}
