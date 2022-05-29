/*
 * LMFS Limer Program Language - Parser - Header File
 */

#ifndef LIM_PARSER_H
#define LIM_PARSER_H

#include <stdint.h>

typedef double (*FuncPtr)(double);

typedef struct L_AST{
#   include "LIM_OpCode.h"
    union{
        struct{
            struct L_AST *left;
            struct L_AST *right;
        }CaseOpera;
        struct{
            struct L_AST *child;
            FuncPtr Function;
        }CaseFunc;
        double CaseConst;
        double *CaseParmPtr;
    }Content;
}L_AST;

#include "LIM_lex.h"

extern void InitParser();
extern void CleanTrash();
extern TokenList NumCompleter(L_Token*, uint64_t);

#endif

