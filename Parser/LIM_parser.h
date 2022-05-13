/*
 * LMFS Limer Program Language - Parser - Header File
 */

#ifndef LIM_PARSER_H
#define LIM_PARSER_H

#include "LIM_lex.h"
#include <stdint.h>

struct L_AST{
    L_Token Token;
    struct L_AST* left;
    struct L_AST* right;
};

typedef struct L_AST L_AST;

TokenList NumCompleter(L_Token*, uint64_t);

#endif

