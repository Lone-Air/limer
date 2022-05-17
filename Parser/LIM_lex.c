/*
 * LMFS Limer Program language - Scanner - Source Code
 */

#include "limer.h"
#include "LIM_lex.h"

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

static char* const keys[LT_MAX]={
    "Integer", "String", "List", "Bool", "Float", "NoType", "Structure", "Constant", 
    "for", "while", "if", "else", "switch", "case", "default", "include",
    "new", "try", "catch", "nothing", "null", "throw", "+", "-", "*", "/", "&", "|", "%", ";", "(", ")", "{",
    "}", "[", "]" , ">", "<", ">=", "<=", "!=", "==", "&&", "||", ">>", "<<",
    "+=", "-=", "*=", "/=", "&=", "|=", ">>=", "<<=", "=", "#", ".", "\\", "!", ":", ",",
    "'", "\""
};

int IsKey(char* key){
    for(int i=0;i<LT_MAX;i++){
        if(strcmp(key,tokens[i].token)==0){
            return i;
        }
    }
    return -1;
}

int IsLet(char c){
    return (c<='z'&&c>='a')||(c<='Z'&&c>='A');
}

int IsNum(char c){
    return c>='0'&&c<='9';
}

int IsBLet(char c){
    return c>='A'&&c<='Z';
}

int Lower(char c){
    return c+32;
}

int FullNum(char* c){
    int CH;
    while((CH=(*c++))!='\0'){
        if(!IsNum(CH)){
            return false;
        }
    }
    return true;
}

char* toStr(char c){
    char *s=(char*)malloc(sizeof(char)*2);
    s[0]=c;
    s[1]='\0';
    return s;
}

int toInt(char c){
    return c-48;
}

int IsSigned(char* c){
    int CH,FoundPoint=0,FoundSigN=0;
    if(!FullNum(c)) return false;
    while((CH=*(c++))!='\0'){
        if(!IsNum(CH)){
            if(CH=='.'){
                if(FoundPoint<1)
                  FoundPoint++;
                else
                  return false;
            }
            else if(CH=='-'){
                if(FoundSigN<1)
                  FoundSigN++;
                else
                  return false;
            }
            else return false;
        }
    }
    return FoundSigN!=0?true:false;
}

char* getLine(char* str, uint64_t lineno){
    char* result=(char*)malloc(sizeof(char)*256);
    size_t maxmem=256;
    uint64_t nowmem=0;
    int CH;
    uint64_t nowline=0;
    while((CH=*(str++))!='\0'){
        if(ch=='\n'||ch==13){
            nowline++;
            continue;
        }
        if(nowline<lineno) continue;
        else if(nowline>lineno) break;
        if(nowmem<maxmem)
          result[nowmem++]=CH;
        else{
            char* temp=(char*)malloc(sizeof(char)*(maxmem=maxmem+128));
            memcpy(temp, result, sizeof(char)*strlen(result));
            free(result);
            result=temp;
            result[nowmem++]=CH;
        }
    }
    result[nowmem++]='\0';
    return result;
}

char* GenPosTip(uint64_t From, uint64_t To){
    uint64_t pos=0;
    char* result=(char*)malloc(sizeof(char)*256);
    uint64_t nowmem=0;
    size_t maxmem=256;
    int CH;
    while(pos<=To){
        if(From<=pos)
          CH='^';
        else
          CH=' ';
        if(nowmem<maxmem)
          result[nowmem++]=CH;
        else{
            char* temp=(char*)malloc(sizeof(char)*(maxmem=maxmem+128));
            memcpy(temp, result, sizeof(char)*strlen(result));
            free(result);
            result=temp;
            result[nowmem++]=CH;
        }
        pos++;
    }
    result[nowmem++]='\0';
    return result;
}

void initialize(){
    char* key={0};
    int temp_i;
    for(int i=0;i<LT_MAX-2;i++){
        key=keys[i];
        tokens[i].token=(char*)malloc(strlen(key)*sizeof(char));
        strcpy(tokens[i].token, key);
        tokens[i].id=i+1;
        temp_i=i;
    }
    temp_i++;
    tokens[temp_i].token="\"";
    tokens[temp_i++].id=L_STR;
    tokens[temp_i].token="'";
    tokens[temp_i++].id=L_STR;
    RES.token=(char*)malloc(sizeof(char)*255);
    Cache=(L_Token*)malloc(sizeof(L_Token)*CacheMax);
}

int save(L_Token ls){
    if(TokenNum>=nowmaxmem){
        L_Token *TNew=(L_Token*)malloc(sizeof(L_Token)*(nowmaxmem=nowmaxmem+TK_MALLOC));
        if(TNew==NULL){
            fprintf(stderr, "Limer:LexicalAnalyzer:MemoryError: Could not allocate enough memory for Limer Lexical Analyzer Stored Procedure!\n");
            return -1;
        }
        memcpy(TNew, Tokens, sizeof(L_Token)*TokenNum);
        free(Tokens);
        Tokens=TNew;
    }
    Tokens[TokenNum].token=(char*)malloc(strlen(ls.token)*sizeof(char));
    strcpy(Tokens[TokenNum].token, ls.token);
    Tokens[TokenNum].id=ls.id;
    Tokens[TokenNum].From=ls.From;
    Tokens[TokenNum].To=ls.To;
    Tokens[TokenNum].Line=ls.Line;
    TokenNum++;
    return 0;
}

L_Token Gen(char* buf, int id, uint64_t From, uint64_t To, uint64_t Line){
    free(RES.token);
    RES.token=(char*)malloc(strlen(buf)*sizeof(char));
    strcpy(RES.token, buf);
    RES.id=id;
    RES.From=From;
    RES.To=To;
    RES.Line=Line;
    if(CacheLen<CacheMax)
      Cache[CacheLen++]=RES;
    else{
        Cache=realloc(Cache, (CacheMax=CacheMax+8)*sizeof(L_Token));
        Cache[CacheLen++]=RES;
    }
    return RES;
}

L_Token* getToken(char* ls){
    uint64_t ls_index=0, tokenStart=0;
    char *buf=(char*)malloc(sizeof(char)*BUF_MAX);
    int BufMaxMem=BUF_MAX;
    int bufLength=0;
    Tokens=(L_Token*)malloc(sizeof(L_Token)*TK_DEFAULT);
    int TokenID,OrigCH;
    char* DefLS=(char*)malloc(sizeof(char)*strlen(ls));
    strcpy(DefLS, ls);
    char* content={0};
    char* tip={0};
    char* str={0};
    int chr=0;
    int Str_char=-1;
    char* STR_BUF;
    int STR_BUF_len=0;
    int STR_BUF_MemMax=255;
    while((ch=*(ls++))!='\0'){
        ls_index++;
        if(ch==' '||ch=='\t') continue;
        else if(ch=='\n'||ch==13){
            LineNo++;
            ls_index=0;
        }
        else if(IsLet(ch)||IsNum(ch)||ch=='_'||ch>128){
            tokenStart=ls_index-1;
            while(IsLet(ch)||IsNum(ch)||ch=='_'||ch>128){
                OrigCH=ch;
                if(IsBLet(ch))
                  ch=Lower(ch);
                if(BufMaxMem>bufLength)
                  buf[bufLength++]=OrigCH;
                else{
                    char* newbuf=(char*)malloc(sizeof(char)*(BufMaxMem=BufMaxMem+BUF_MAX));
                    memcpy(newbuf, buf, sizeof(char)*strlen(buf));
                    free(buf);
                    buf=newbuf;
                }
                ch=*(ls++);
                ls_index++;
            }
            ls_index-=2;
            ls--;
            buf[bufLength++]='\0';
            ls_index++;
            int id;
            if((TokenID=IsKey(buf))!=-1){
                id=TokenID;
            }else{
                id=FullNum(buf)?L_NORN:L_NORS;
            }
            int success=save(Gen(buf, id, tokenStart, ls_index, LineNo));
            if(success==-1){
                free(buf);
                return NULL;
            }
            if(ch=='\n'||ch==13){
                LineNo++;
                ls_index=0;
                ls++;
            }
            free(buf);
            BufMaxMem=BUF_MAX;
            buf=(char*)malloc(sizeof(char)*BUF_MAX);
            bufLength=0;
        }
        else{
            tokenStart=ls_index-1;
            switch(ch){
              case '+':
                ch=*(ls++);
                ls_index++;
                if(ch=='\n'||ch==13||ch=='\0'){
                    save(Gen("+", IsKey("+"), tokenStart, --ls_index, LineNo));
                    if(ch=='\n'||ch==13)
                      LineNo++;
                    ls_index=0;
                    break;
                }
                switch(ch){
                  case '=':
                    save(Gen("+=", IsKey("+="), tokenStart, ls_index, LineNo));
                    break;
                  default:
                    save(Gen("+", IsKey("+"), tokenStart, --ls_index, LineNo));
                    ls--;
                    break;
                }
                break;
              case '*':
                ch=*(ls++);
                ls_index++;
                if(ch=='\n'||ch==13||ch=='\0'){
                    save(Gen("*", IsKey("*"), tokenStart, --ls_index, LineNo));
                    if(ch=='\n'||ch==13)
                      LineNo++;
                    ls_index=0;
                    break;
                }
                switch(ch){
                  case '=':
                    save(Gen("*=", IsKey("*="), tokenStart, ls_index, LineNo));
                    break;
                  default:
                    save(Gen("*", IsKey("*"), tokenStart, --ls_index, LineNo));
                    ls--;
                    break;
                }
                break;
              case '/':
                ch=*(ls++);
                ls_index++;
                if(ch=='\n'||ch==13||ch=='\0'){
                    save(Gen("/", IsKey("/"), tokenStart, --ls_index, LineNo));
                    if(ch=='\n'||ch==13)
                      LineNo++;
                    ls_index=0;
                    break;
                }
                switch(ch){
                  case '=':
                    save(Gen("/=", IsKey("/="), tokenStart, ls_index, LineNo));
                    break;
                  default:
                    save(Gen("/", IsKey("/"), tokenStart, --ls_index, LineNo));
                    ls--;
                    break;
                }
                break;
              case '&':
                ch=*(ls++);
                ls_index++;
                if(ch=='\n'||ch==13||ch=='\0'){
                    save(Gen("&", IsKey("&"), tokenStart, --ls_index, LineNo));
                    if(ch=='\n'||ch==13)
                      LineNo++;
                    ls_index=0;
                    break;
                }
                switch(ch){
                  case '=':
                    save(Gen("&=", IsKey("&="), tokenStart, ls_index, LineNo));
                    break;
                  case '&':
                    save(Gen("&&", IsKey("&&"), tokenStart, ls_index, LineNo));
                    break;
                  default:
                    save(Gen("&", IsKey("&"), tokenStart, --ls_index, LineNo));
                    ls--;
                    break;
                }
                break;
              case '|':
                ch=*(ls++);
                ls_index++;
                if(ch=='\n'||ch==13||ch=='\0'){
                    save(Gen("|", IsKey("|"), tokenStart, --ls_index, LineNo));
                    if(ch=='\n'||ch==13)
                      LineNo++;
                    ls_index=0;
                    break;
                }
                switch(ch){
                  case '=':
                    save(Gen("|=", IsKey("|="), tokenStart, ls_index, LineNo));
                    break;
                  case '|':
                    save(Gen("||", IsKey("|="), tokenStart, ls_index, LineNo));
                    break;
                  default:
                    save(Gen("|", IsKey("|"), tokenStart, --ls_index, LineNo));
                    ls--;
                    break;
                }
                break;
              case '>':
                ch=*(ls++);
                ls_index++;
                if(ch=='\n'||ch==13||ch=='\0'){
                    save(Gen(">", IsKey(">"), tokenStart, ls_index, LineNo));
                    if(ch=='\n'||ch==13)
                      LineNo++;
                    ls_index=0;
                    break;
                }
                switch(ch){
                  case '=':
                    save(Gen(">=", IsKey(">="), tokenStart, ls_index, LineNo));
                    break;
                  case '>':
                    ch=*(ls++);
                    ls_index++;
                    if(ch=='\n'||ch==13||ch=='\0'){
                        save(Gen(">>", IsKey(">>"), tokenStart, ls_index, LineNo));
                        if(ch=='\n'||ch==13)
                          LineNo++;
                        ls_index=0;
                        break;
                    }
                    switch(ch){
                      case '=':
                        save(Gen(">>=", IsKey(">>="), tokenStart, ls_index, LineNo));
                        break;
                      default:
                        save(Gen(">>", IsKey(">>"), tokenStart, --ls_index, LineNo));
                        ls--;
                        break;
                    }
                    break;
                  default:
                    save(Gen(">", IsKey(">"), tokenStart, --ls_index, LineNo));
                    ls--;
                    break;
                }
                break;
              case '<':
                ch=*(ls++);
                ls_index++;
                if(ch=='\n'||ch==13||ch=='\0'){
                    save(Gen("<", IsKey("<"), tokenStart, ls_index, LineNo));
                    if(ch=='\n'||ch==13)
                      LineNo++;
                    ls_index=0;
                    break;
                }
                switch(ch){
                  case '=':
                    save(Gen("<=", IsKey("<="), tokenStart, ls_index, LineNo));
                    break;
                  case '<':
                    ch=*(ls++);
                    ls_index++;
                    if(ch=='\n'||ch==13||ch=='\0'){
                        save(Gen("<<", IsKey("<<"), tokenStart, ls_index, LineNo));
                        if(ch=='\n'||ch==13)
                          LineNo++;
                        ls_index=0;
                        break;
                    }
                    switch(ch){
                      case '=':
                        save(Gen("<<=", IsKey("<<="), tokenStart, ls_index, LineNo));
                        break;
                      default:
                        save(Gen("<<", IsKey("<<"), tokenStart, --ls_index, LineNo));
                        ls--;
                        break;
                    }
                    break;
                  default:
                    save(Gen("<", IsKey("<"), tokenStart, --ls_index, LineNo));
                    ls--;
                    break;
                }
                break;
              case '%':
                ch=*(ls++);
                ls_index++;
                if(ch=='\n'||ch==13||ch=='\0'){
                    save(Gen("%", IsKey("*"), tokenStart, --ls_index, LineNo));
                    if(ch=='\n'||ch==13)
                      LineNo++;
                    ls_index=0;
                    break;
                }
                switch(ch){
                  case '=':
                    save(Gen("%=", IsKey("%="), tokenStart, ls_index, LineNo));
                    break;
                  default:
                    save(Gen("%", IsKey("%"), tokenStart, --ls_index, LineNo));
                    ls--;
                    break;
                }
                break;
              case '-':
                ch=*(ls++);
                ls_index++;
                if(ch=='\n'||ch==13||ch=='\0'){
                    if(ch=='\n'||ch==13)
                      LineNo++;
                    ls_index=0;
                    break;
                }
                switch(ch){
                  case '=':
                    save(Gen("-=", IsKey("-="), tokenStart, ls_index, LineNo));
                    break;
                  default:
                    save(Gen("-", IsKey("-"), tokenStart, --ls_index, LineNo));
                    ls--;
                    break;
                }
                break;
              case '!':
                ch=*(ls++);
                ls_index++;
                if(ch=='\n'||ch==13||ch=='\0'){
                    save(Gen("!", IsKey("!"), tokenStart, --ls_index, LineNo));
                    if(ch=='\n'||ch==13)
                      LineNo++;
                    ls_index=0;
                    break;
                }
                switch(ch){
                  case '=':
                    save(Gen("!=", IsKey("!="), tokenStart, ls_index, LineNo));
                    break;
                  default:
                    save(Gen("!", IsKey("!"), tokenStart, --ls_index, LineNo));
                    ls--;
                    break;
                }
                break;
              case '(': case ')': case '[': case ']': case '{': case '}': case '.': case ':': case ',': case ';':
                str=toStr(ch);
                save(Gen(str, IsKey(str), tokenStart, ls_index, LineNo));
                free(str);
                break;
              case '"': case '\'':
                /*str=toStr(ch);
                save(Gen(str, IsKey(str), tokenStart, ls_index, LineNo));
                free(str);*/
                Str_char=ch;
                STR_BUF=(char*)malloc(STR_BUF_MemMax);
                while(1){
                    ch=*ls++;
                    ls_index++;
                    if(ch=='\0'){
                        content=getLine(DefLS, LineNo);
                        tip=GenPosTip(ls_index-3, ls_index-2);
                        fprintf(stderr,"Limer found an \033[91;1merror\033[0m:\n");
                        fprintf(stderr,"    %s\n",content);
                        fprintf(stderr,"    %s\n",tip);
                        fprintf(stderr,"Limer:LexicalAnalyzer-StringProcessor-BadSyntax:l%lu:c%lu: Invalid symbol in string!\n",LineNo+1,ls_index-1);
                        free(content);
                        free(tip);
                        free(buf);
                        free(STR_BUF);
                        return NULL;
                    }
                    switch(ch){
                      case '\\':
                        ch=*ls++;
                        ls_index++;
                        if(ch=='\0'){
                            content=getLine(DefLS, LineNo);
                            tip=GenPosTip(ls_index-2, ls_index-1);
                            fprintf(stderr,"Limer found an \033[91;1merror\033[0m:\n");
                            fprintf(stderr,"    %s\n",content);
                            fprintf(stderr,"    %s\n",tip);
                            fprintf(stderr,"Limer:LexicalAnalyzer:l%lu:c%lu: No symbol near `\\'!\n",LineNo+1,ls_index);
                            free(content);
                            free(tip);
                            free(buf);
                            return NULL;
                        }
                        switch(ch){
                          case '\\':
                            if(STR_BUF_len<STR_BUF_MemMax)
                              STR_BUF[STR_BUF_len++]='\\';
                            else{
                                STR_BUF=(char*)realloc(STR_BUF, (STR_BUF_MemMax=STR_BUF_MemMax+255));
                                STR_BUF[STR_BUF_len++]='\\';
                            }
                            break;
                          case '\'':
                            if(STR_BUF_len<STR_BUF_MemMax)
                              STR_BUF[STR_BUF_len++]='\'';
                            else{
                                STR_BUF=(char*)realloc(STR_BUF, (STR_BUF_MemMax=STR_BUF_MemMax+255));
                                STR_BUF[STR_BUF_len++]='\'';
                            }
                            break;
                          case '"':
                            if(STR_BUF_len<STR_BUF_MemMax)
                              STR_BUF[STR_BUF_len++]='"';
                            else{
                                STR_BUF=(char*)realloc(STR_BUF, (STR_BUF_MemMax=STR_BUF_MemMax+255));
                                STR_BUF[STR_BUF_len++]='"';
                            }
                            break;
                          case 't':
                            if(STR_BUF_len<STR_BUF_MemMax)
                              STR_BUF[STR_BUF_len++]='\t';
                            else{
                                STR_BUF=(char*)realloc(STR_BUF, (STR_BUF_MemMax=STR_BUF_MemMax+255));
                                STR_BUF[STR_BUF_len++]='\t';
                            }
                            break;
                          case 'n':
                            if(STR_BUF_len<STR_BUF_MemMax)
                              STR_BUF[STR_BUF_len++]='\n';
                            else{
                                STR_BUF=(char*)realloc(STR_BUF, (STR_BUF_MemMax=STR_BUF_MemMax+255));
                                STR_BUF[STR_BUF_len++]='\n';
                            }
                            break;
                          case 'r':
                            if(STR_BUF_len<STR_BUF_MemMax)
                              STR_BUF[STR_BUF_len++]='\r';
                            else{
                                STR_BUF=(char*)realloc(STR_BUF, (STR_BUF_MemMax=STR_BUF_MemMax+255));
                                STR_BUF[STR_BUF_len++]='\r';
                            }
                            break;
                          case 'e':
                            if(STR_BUF_len<STR_BUF_MemMax)
                              STR_BUF[STR_BUF_len++]='\033';
                            else{
                                STR_BUF=(char*)realloc(STR_BUF, (STR_BUF_MemMax=STR_BUF_MemMax+255));
                                STR_BUF[STR_BUF_len++]='\033';
                            }
                            break;
                          case '\n':
                            if(STR_BUF_len<STR_BUF_MemMax)
                              STR_BUF[STR_BUF_len++]='\\';
                            else{
                                STR_BUF=(char*)realloc(STR_BUF, (STR_BUF_MemMax=STR_BUF_MemMax+255));
                                STR_BUF[STR_BUF_len++]='\\';
                            }
                            break;
                          case '0': case '1': case '2':\
                          case '3': case '4': case '5':\
                          case '6': case '7': case '8': case '9':
                            chr=toInt(ch);
                            ch=*(ls++);
                            if(ch=='\0'||ch=='\n'||ch==13){
                                ls_index--;
                                if(ch=='\0'){
                                    content=getLine(DefLS, LineNo);
                                    tip=GenPosTip(ls_index-2, ls_index-1);
                                    fprintf(stderr,"Limer found an \033[91;1merror\033[0m:\n");
                                    fprintf(stderr,"    %s\n",content);
                                    fprintf(stderr,"    %s\n",tip);
                                    fprintf(stderr,"Limer:LexicalAnalyzer-StringProcessor-BadSyntax:l%lu:c%lu: Invalid symbol in string!\n",LineNo+1,ls_index);
                                    free(content);
                                    free(tip);
                                    free(buf);
                                    free(STR_BUF);
                                    return NULL;
                                }
                                if(ch=='\n'||ch==13){
                                    if(STR_BUF_len<STR_BUF_MemMax)
                                      STR_BUF[STR_BUF_len++]=chr;
                                    else{
                                        STR_BUF=(char*)realloc(STR_BUF, (STR_BUF_MemMax=STR_BUF_MemMax+255));
                                        STR_BUF[STR_BUF_len++]=chr;
                                    }
                                    LineNo++;
                                    ls_index=0;
                                }
                                break;
                            }
                            switch(ch){
                              case '0': case '1': case '2':\
                              case '3': case '4': case '5':\
                              case '6': case '7': case '8': case '9':
                                chr=toInt(ch)+chr*10;
                                ch=*(ls++);
                                ls_index++;
                                if(ch=='\0'||ch=='\n'||ch==13){
                                    ls_index--;
                                    if(ch=='\0'){
                                        content=getLine(DefLS, LineNo);
                                        tip=GenPosTip(ls_index-2, ls_index-1);
                                        fprintf(stderr,"Limer found an \033[91;1merror\033[0m:\n");
                                        fprintf(stderr,"    %s\n",content);
                                        fprintf(stderr,"    %s\n",tip);
                                        fprintf(stderr,"Limer:LexicalAnalyzer-StringProcessor-BadSyntax:l%lu:c%lu: Invalid symbol in string!\n",LineNo+1,ls_index);
                                        free(content);
                                        free(tip);
                                        free(buf);
                                        free(STR_BUF);
                                        return NULL;
                                    }
                                    if(ch=='\n'||ch==13){
                                        if(STR_BUF_len<STR_BUF_MemMax)
                                          STR_BUF[STR_BUF_len++]=chr;
                                        else{
                                            STR_BUF=(char*)realloc(STR_BUF, (STR_BUF_MemMax=STR_BUF_MemMax+255));
                                            STR_BUF[STR_BUF_len++]=chr;
                                        }
                                        LineNo++;
                                        ls_index=0;
                                    }
                                    break;
                                }
                                switch(ch){
                                  case '0': case '1': case '2':\
                                  case '3': case '4': case '5':\
                                  case '6': case '7': case '8': case '9':
                                    chr=toInt(ch)+chr*10;
                                    if(STR_BUF_len<STR_BUF_MemMax)
                                      STR_BUF[STR_BUF_len++]=chr;
                                    else{
                                        STR_BUF=(char*)realloc(STR_BUF, (STR_BUF_MemMax=STR_BUF_MemMax+255));
                                        STR_BUF[STR_BUF_len++]=chr;
                                    }
                                    break;
                                  default:
                                    if(STR_BUF_len<STR_BUF_MemMax)
                                      STR_BUF[STR_BUF_len++]=chr;
                                    else{
                                        STR_BUF=(char*)realloc(STR_BUF, (STR_BUF_MemMax=STR_BUF_MemMax+255));
                                        STR_BUF[STR_BUF_len++]=chr;
                                    }
                                    ls_index--;
                                    ls--;
                                    break;
                                }
                                break;
                              default:
                                if(STR_BUF_len<STR_BUF_MemMax)
                                  STR_BUF[STR_BUF_len++]=chr;
                                else{
                                    STR_BUF=(char*)realloc(STR_BUF, (STR_BUF_MemMax=STR_BUF_MemMax+255));
                                    STR_BUF[STR_BUF_len++]=chr;
                                }
                                ls_index--;
                                ls--;
                                break;
                            }
                            break;
                          default:
                            if(STR_BUF_len<STR_BUF_MemMax)
                              STR_BUF[STR_BUF_len++]='\\';
                            else{
                                STR_BUF=(char*)realloc(STR_BUF, (STR_BUF_MemMax=STR_BUF_MemMax+255));
                                STR_BUF[STR_BUF_len++]='\\';
                            }
                            ls_index--;
                            ls--;
                            break;
                        }
                        break;
                      case '\'': case '"':
                        if(ch==Str_char)
                          goto breakStr;
                        if(STR_BUF_len<STR_BUF_MemMax)
                          STR_BUF[STR_BUF_len++]=ch;
                        else{
                            STR_BUF=(char*)realloc(STR_BUF, (STR_BUF_MemMax=STR_BUF_MemMax+255));
                            STR_BUF[STR_BUF_len++]=ch;
                        }
                        break;
                      default:
                        if(STR_BUF_len<STR_BUF_MemMax)
                          STR_BUF[STR_BUF_len++]=ch;
                        else{
                            STR_BUF=(char*)realloc(STR_BUF, (STR_BUF_MemMax=STR_BUF_MemMax+255));
                            STR_BUF[STR_BUF_len++]=ch;
                        }
                        break;
                    }
                }
breakStr:
                save(Gen(STR_BUF, L_STR, tokenStart, ls_index, LineNo));
                free(STR_BUF);
                STR_BUF_len=0;
                STR_BUF_MemMax=255;
                break;
              case '\\':
                ch=*(ls++);
                ls_index++;
                if(ch=='\0'){
                    content=getLine(DefLS, LineNo);
                    tip=GenPosTip(tokenStart+1, ls_index-1);
                    fprintf(stderr,"Limer found an \033[91;1merror\033[0m:\n");
                    fprintf(stderr,"    %s\n",content);
                    fprintf(stderr,"    %s\n",tip);
                    fprintf(stderr,"Limer:LexicalAnalyzer:l%lu:c%lu: No symbol near `\\'!\n",LineNo+1,ls_index);
                    free(content);
                    free(tip);
                    free(buf);
                    return NULL;
                }
                switch(ch){
                  case 't':
                    save(Gen("\t", IsKey("\\"), tokenStart, ls_index, LineNo));
                    break;
                  case 'n':
                    save(Gen("\n", IsKey("\\"), tokenStart, ls_index, LineNo));
                    break;
                  case 'r':
                    save(Gen("\r", IsKey("\\"), tokenStart, ls_index, LineNo));
                    break;
                  case 'e':
                    save(Gen("\033", IsKey("\\"), tokenStart, ls_index, LineNo));
                    break;
                  case '\n':
                    save(Gen("\\", IsKey("\\"), tokenStart, ls_index, LineNo));
                    break;
                  case '0': case '1': case '2':\
                  case '3': case '4': case '5':\
                  case '6': case '7': case '8': case '9':
                    chr=toInt(ch);
                    ch=*(ls++);
                    ls_index++;
                    if(ch=='\0'||ch=='\n'||ch==13){
                        save(Gen(toStr(chr), IsKey("\\"), tokenStart, --ls_index, LineNo));
                        if(ch=='\n'||ch==13){
                            LineNo++;
                            ls_index=0;
                        }
                        break;
                    }
                    switch(ch){
                      case '0': case '1': case '2':\
                      case '3': case '4': case '5':\
                      case '6': case '7': case '8': case '9':
                        chr=toInt(ch)+chr*10;
                        ch=*(ls++);
                        ls_index++;
                        if(ch=='\0'||ch=='\n'||ch==13){
                            save(Gen(toStr(chr), IsKey("\\"), tokenStart, --ls_index, LineNo));
                            if(ch=='\n'||ch==13){
                                LineNo++;
                                ls_index=0;
                            }
                            break;
                        }
                        switch(ch){
                          case '0': case '1': case '2':\
                          case '3': case '4': case '5':\
                          case '6': case '7': case '8': case '9':
                            chr=toInt(ch)+chr*10;
                            save(Gen(toStr(chr), IsKey("\\"), tokenStart, ls_index, LineNo));
                            break;
                          default:
                            save(Gen(toStr(chr), IsKey("\\"), tokenStart, --ls_index, LineNo));
                            ls--;
                            break;
                        }
                        break;
                      default:
                        save(Gen(toStr(chr), IsKey("\\"), tokenStart, --ls_index, LineNo));
                        ls--;
                        break;
                    }
                    break;
                  default:
                    save(Gen("\\", IsKey("\\"), tokenStart, --ls_index, LineNo));
                    ls--;
                    break;
                }
                break;
              case '#':
                //save(Gen("#", IsKey("#"), tokenStart, ls_index, LineNo));
                while((ch=*ls++)!='\0'){
                    if(ch=='\n'||ch==13){
                        LineNo++;
                        ls_index=0;
                        break;
                    }
                    ls_index++;
                }
                break;
              case '=':
                ch=*(ls++);
                ls_index++;
                if(ch=='\n'||ch==13||ch=='\0'){
                    save(Gen("=", IsKey("="), tokenStart, --ls_index, LineNo));
                    if(ch=='\n'||ch==13)
                      LineNo++;
                    ls_index=0;
                    break;
                }
                switch(ch){
                  case '=':
                    save(Gen("==", IsKey("=="), tokenStart, ls_index, LineNo));
                    break;
                  default:
                    save(Gen("=", IsKey("="), tokenStart, --ls_index, LineNo));
                    ls--;
                    break;
                }
                break;
              default:
                /*content=getLine(DefLS, LineNo);
                tip=GenPosTip(tokenStart, ls_index-1);
                fprintf(stderr,"Limer found an \033[91;1merror\033[0m:\n");
                fprintf(stderr,"    %s\n",content);
                fprintf(stderr,"    %s\n",tip);
                fprintf(stderr,"Limer:LexicalAnalyzer:l%lu:c%lu: Invalid symbol `%c'(0x%x)!\n",LineNo+1,ls_index,ch,ch);
                free(content);
                free(tip);
                free(buf);
                return NULL;*/
                save(Gen(toStr(ch), L_NORS, tokenStart, ls_index, LineNo));
                break;
            }
        }
    }
    free(buf);
    return Tokens;
}

void FreeMem(){
    for(int i=0;i<LT_MAX-2;i++){
        free(tokens[i].token);
    }
    for(uint64_t i=0;i<TokenNum;i++){
        free(Tokens[i].token);
    }
    free(Tokens);
    free(Cache);
    CacheLen=0;
    CacheMax=8;
    LineNo=0;
    TokenNum=0;
    nowmaxmem=TK_DEFAULT;
}

int getLen(){
    return TokenNum;
}

#ifdef TESTMODE1
int main(){
//#include <readline/readline.h>
    initialize();
/*    char* string=readline("LimerLex> ");
    if(string==NULL) return 0;*/
    char string[114514];
    printf("LimerLex> ");
    fgets(string, 114514, stdin);
    L_Token *T=getToken(string);
    if(T!=NULL){
        for(uint64_t i=0;i<TokenNum;i++){
            printf("%s,%d,%lu,%lu,%lu\n",T[i].token, T[i].id, T[i].From, T[i].To, T[i].Line);
        }
    }
    FreeMem();
    return 0;
}
#endif
