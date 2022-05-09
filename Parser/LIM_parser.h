/*
 * LMFS Limer
 */

#include "LIM_lex.h"

#define ST_DEFAULT 1
#define ST_MALLOC 1

typedef struct{
    char* token;
    int id;
    uint64_t From;
    uint64_t To;
    uint64_t Line;
    uint64_t Hierarchy;
}L_Syntax;

L_Syntax* Syntax;
uint64_t Syn_NowM=0;
uint64_t Syn_MaxM=ST_DEFAULT;
uint64_t SyntaxNum=0;
L_Syntax RES_S;

void InitS();
void FreeP();
int saveS(L_Syntax ls);
L_Syntax GenS(char* buf, int id, uint64_t From, uint64_t To, uint64_t Line, uint64_t Hierarchy);
L_Syntax* parser(L_Token*);
