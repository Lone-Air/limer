/*
 * LMFS Limer Programming language
 */

#ifndef LIMLEX_H
#define LIMLEX_H

#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define SIZE(x) sizeof(x)/sizeof(x[0])
#define LT_MAX 61
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

L_Token tokens[LT_MAX];
L_Token* Cache;
int CacheLen=0;
int CacheMax=8;
int ch;
int lch=0;
uint64_t LineNo=0;
uint64_t nowmaxmem=TK_DEFAULT;
L_Token *Tokens;
uint64_t TokenNum=0;
L_Token RES;

enum LIM_LEX{
    L_INT=1, L_STRT, L_LIST, L_BOOL, L_FLT, L_NTPE, 
    L_FOR, L_WHL, L_IF, L_ELS, L_SWI, L_CAS, L_DFT, L_INC, L_NEW, L_TRY, L_CAT, L_NTG, L_NULL, L_THRW, L_PLUS,
    L_MINU, L_TMS, L_DEVI, L_POS, L_ORT, L_FTR, L_SP, L_PSS, L_PSE, L_PMS, L_PME, L_PBS, L_PBE, L_MR, L_LR, L_MRE,
    L_LRE, L_NE, L_EQ, L_AND, L_OR, L_RMP, L_LMP, L_ADD, L_RM, L_TM, L_DV, L_PE, L_OE, L_RPS, L_LPS,
    L_SET, L_NOTE, L_PINT, L_ESC, L_NOT, L_IS, L_NEXT,
    L_NORS, L_NORN, L_NORNU, L_STR /* L_NORS: "example", L_NORN: 123456 */
};

L_Token* getToken(char* ls);
int IsKey(char* key);
int IsLet(char c);
int IsNum(char c);
int IsBLet(char c);
int Lower(char c);
int FullNum(char* c);
void initialize();
int save(L_Token ls);
void FreeMem();
int IsSigned(char* c);
char* getLine(char* str, uint64_t lineno);
char* GenPosTip(uint64_t From, uint64_t To);
L_Token Gen(char* buf, int id, uint64_t From, uint64_t To, uint64_t Line);
char* toStr(char c);
int toInt(char c);
#endif

