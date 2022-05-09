/*
 * LMFS Limer Parser
 */

#include "LIM_parser.h"

int saveS(L_Syntax ls){
    if(Syn_NowM>=Syn_MaxM){
        L_Syntax *SNew=(L_Syntax*)malloc(sizeof(L_Syntax)*(Syn_MaxM=Syn_MaxM+ST_MALLOC));
        if(SNew==NULL){
            fprintf(stderr, "Limer:Parser:MemoryError: Could not allocate enough memory for Limer Parser Stored Procedure!\n");
            return -1;
        }
        memcpy(SNew, Syntax, sizeof(L_Syntax)*SyntaxNum);
        free(Syntax);
        Syntax=SNew;
    }
    Syntax[SyntaxNum].token=(char*)malloc(strlen(ls.token)*sizeof(char));
    strcpy(Syntax[SyntaxNum].token, ls.token);
    Syntax[SyntaxNum].id=ls.id;
    Syntax[SyntaxNum].From=ls.From;
    Syntax[SyntaxNum].To=ls.To;
    Syntax[SyntaxNum].Line=ls.Line;
    Syntax[SyntaxNum].Hierarchy=ls.Hierarchy;
    SyntaxNum++;
    return 0;
}

L_Syntax GenS(char* buf, int id, uint64_t From, uint64_t To, uint64_t Line, uint64_t Hierarchy){
    free(RES_S.token);
    RES_S.token=(char*)malloc(strlen(buf)*sizeof(char));
    strcpy(RES_S.token, buf);
    RES_S.id=id;
    RES_S.From=From;
    RES_S.To=To;
    RES_S.Line=Line;
    RES_S.Hierarchy=Hierarchy;
    return RES_S;
}

void InitS(){
    RES_S.token=(char*)malloc(sizeof(char)*255);
}

void FreeP(){
    free(RES_S.token);
    for(int i=0;i<SyntaxNum;i++)
        free(Syntax[i].token);
    free(Syntax);
}

