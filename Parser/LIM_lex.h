/*
 * LMFS Limer Program language - Scanner - Header File
 */

#ifndef LIMLEX_H
#define LIMLEX_H

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#include "LIM_OpCode.h"

#define SIZE(x) sizeof(x)/sizeof(x[0])
#define LT_MAX 63
#define BUF_MAX 255
#define TK_DEFAULT 1
#define TK_MALLOC 1

typedef struct{
    char *token;
    int id;
    uint64_t From;
    uint64_t To;
    uint64_t Line;
}L_Token;

typedef struct{
    L_Token* Tokens;
    uint64_t len;
}TokenList;

L_Token* getToken(char* ls);
extern int IsKey(char* key);
extern int IsLet(char c);
extern int IsNum(char c);
extern int IsBLet(char c);
extern int Lower(char c);
extern int FullNum(char* c);
extern void initialize();
extern int save(L_Token ls);
extern void FreeMem();
extern int IsSigned(char* c);
extern char* getLine(char* str, uint64_t lineno);
extern char* GenPosTip(uint64_t From, uint64_t To);
extern L_Token Gen(char* buf, int id, uint64_t From, uint64_t To, uint64_t Line);
extern char* toStr(char c);
extern int toInt(char c);
extern int getLen();

#endif

