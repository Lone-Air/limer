/*
 * LMFS Limer Program Language - Parser - Source Code
 */

#include "LIM_parser.h"

#define ID(x) id=x[count].id
#define TK(x) x[count].token
#define OPT(x) x[count]
#define SAVE(V, a, b, c, d, e) if(nowmem<maxmem) V[nowmem++]=Gen(a, b, c, d, e); \
                            else {\
                                V=(L_Token*)realloc(new, sizeof(L_Token)*(maxmem=maxmem+TK_DEFAULT)); \
                                V[nowmem++]=Gen(a, b, c, d, e);\
                            }

TokenList NumCompleter(L_Token* old, uint64_t len){
    L_Token* new=(L_Token*)malloc(sizeof(L_Token)*TK_DEFAULT);
    uint64_t nowmem=0;
    uint64_t maxmem=TK_DEFAULT;
    char* str_tmp;
    int int_tmp;
    uint64_t count=0;
    int id;
    while(count<len){
        ID(old);
        switch(id){
          case L_NORN:
            count++;
            if(count>=len){
                count--;
                SAVE(new, TK(old), L_NORN, OPT(old).From, OPT(old).To, OPT(old).Line);
                break;
            }
            ID(old);
            switch(id){
              case L_PINT:
                count++;
                if(count>=len){
                    count--; count--;
                    SAVE(new, TK(old), L_NORN, OPT(old).From, OPT(old).To, OPT(old).Line);
                    break;
                }
                ID(old);
                switch(id){
                  case L_NORN:
                    str_tmp=(char*)malloc(sizeof(char)*(strlen(old[count-2].token+strlen(old[count-1].token)+strlen(TK(old))+1)));
                    strcpy(str_tmp, old[count-2].token);
                    strcat(str_tmp, old[count-1].token);
                    strcat(str_tmp, TK(old));
                    SAVE(new, str_tmp, L_NORN, old[count-2].From, OPT(old).To, OPT(old).Line);
                    free(str_tmp);
                    break;
                  default:
                    count--; count--;
                    str_tmp=(char*)malloc(sizeof(char)*(strlen(TK(old))+2));
                    strcpy(str_tmp, TK(old));
                    strcat(str_tmp, ".");
                    count++; count++;
                    SAVE(new, str_tmp, L_NORN, old[count-2].From, OPT(old).To, OPT(old).Line);
                    free(str_tmp);
                    count++;
                    break;
                }
                break;
              default:
                count--; count--;
                SAVE(new, TK(old), L_NORN, OPT(old).From, OPT(old).To, OPT(old).Line);
                break;
            }
            break;
          case L_MINU:
            count++;
            if(count<len){
                count--;
                SAVE(new, TK(old), L_NORN, OPT(old).From, OPT(old).To, OPT(old).Line);
            }
            ID(old);
            switch(id){
              case L_NORN:
                str_tmp=(char*)malloc(sizeof(char)*(strlen(TK(old))+strlen(old[count-1].token)+1));
                strcpy(str_tmp, old[count-1].token);
                strcat(str_tmp, TK(old));
                SAVE(new, str_tmp, L_NORN, old[count-1].From, OPT(old).To, OPT(old).Line);
                free(str_tmp);
                break;
              default:
                count--;
                SAVE(new, TK(old), L_MINU, OPT(old).From, OPT(old).To, OPT(old).Line);
                break;
            }
            break;
          case L_PLUS:
            count++;
            if(count<len){
                count--;
                SAVE(new, TK(old), L_NORN, OPT(old).From, OPT(old).To, OPT(old).Line);
            }
            ID(old);
            switch(id){
              case L_NORN:
                str_tmp=(char*)malloc(sizeof(char)*(strlen(TK(old))+strlen(old[count-1].token)+1));
                strcpy(str_tmp, old[count-1].token);
                strcat(str_tmp, TK(old));
                SAVE(new, str_tmp, L_NORN, old[count-1].From, OPT(old).To, OPT(old).Line);
                free(str_tmp);
                break;
              default:
                count--;
                SAVE(new, TK(old), L_PLUS, OPT(old).From, OPT(old).To, OPT(old).Line);
                break;
            }
            break;
          case L_PINT:
            count++;
            if(count<len){
                count--;
                SAVE(new, TK(old), L_NORN, OPT(old).From, OPT(old).To, OPT(old).Line);
            }
            ID(old);
            switch(id){
              case L_NORN:
                str_tmp=(char*)malloc(sizeof(char)*(strlen(TK(old))+strlen(old[count-1].token)+1));
                strcpy(str_tmp, old[count-1].token);
                strcat(str_tmp, TK(old));
                SAVE(new, str_tmp, L_NORN, old[count-1].From, OPT(old).To, OPT(old).Line);
                free(str_tmp);
                break;
              default:
                count--;
                SAVE(new, TK(old), L_PINT, OPT(old).From, OPT(old).To, OPT(old).Line);
                break;
            }
            break;
          default:
            SAVE(new, TK(old), OPT(old).id, OPT(old).From, OPT(old).To, OPT(old).Line);
            //count++;
            break;
        }
        count++;
    }
    /*free(Tokens);*/
    TokenList Tokens;
    Tokens.Tokens=(L_Token*)malloc(sizeof(L_Token)*nowmem+1);
    for(int i=0;i<nowmem;i++){
        Tokens.Tokens[i].token=(char*)malloc(strlen(new[i].token));
        strcpy(Tokens.Tokens[i].token, new[i].token);
        Tokens.Tokens[i].id=new[i].id;
        Tokens.Tokens[i].From=new[i].From;
        Tokens.Tokens[i].To=new[i].To;
        Tokens.Tokens[i].Line=new[i].Line;
    }
    Tokens.len=nowmem;
    /*TokenNum=nowmem;
    nowmaxmem=nowmem;*/
    free(new);
    return Tokens;
}

#ifdef TESTMODE2
int main(){
//#include <readline/readline.h>
    initialize();
/*    char* string=readline("LimerLex> ");
    if(string==NULL) return 0;*/
    char string[114514];
    printf("LimerParser> ");
    fgets(string, 114514, stdin);
    L_Token *T=getToken(string);
    TokenList T_=NumCompleter(T, getLen());
    if(T!=NULL){
        for(uint64_t i=0;i<T_.len;i++){
            printf("%s,%d,%lu,%lu,%lu\n",T_.Tokens[i].token, T_.Tokens[i].id, T_.Tokens[i].From, T_.Tokens[i].To, T_.Tokens[i].Line);
        }
    }
    FreeMem();
    return 0;
}
#endif
