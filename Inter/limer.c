/*
 * LMFS Limer Program Language
 */

#include <stdio.h>
#include <stdlib.h>

#include "linfo.h"
#include "limer.h"

#ifdef IS_GNUC
#include <gnu/libc-version.h>
#else
#define __GLIBC__ "<ukn>"
#define __GLIBC_MINOR__ "<ukn>"
#endif

#ifdef HAVE_READLINE
#include <readline/readline.h>
#endif

int main(){
    printf("LMFS-Limer %s Interpreter (Build with GCC %d.%d.%d, on glibc %d.%d)\n",__L_VERSION__,__GNUC__,__GNUC_MINOR__,__GNUC_PATCHLEVEL__,__GLIBC__,__GLIBC_MINOR__);
#ifndef HAVE_READLINE
    printf("Warn: Limer compiled without GNU ReadLine\n");
#endif
    char* STDIN;
    while(1){
        STDIN=(char*)malloc(sizeof(char*));
#ifdef HAVE_READLINE
        STDIN=readline("Lim> ");
        if(STDIN==NULL) exit(0);
#else
        printf("Lim> ");
        scanf("%[^\n]",STDIN);
#endif
        LEXEC(STDIN);
        free(STDIN);
    }
}

