/*
 * LMFS Limer Program Language - Operater Code - Header File
 */

#ifndef LIM_OPCODE_H
#define LIM_OPCODE_H

enum{
    L_INT=0, L_STRT, L_LIST, L_BOOL, L_FLT, L_NTPE, L_STRUCT,
    /* Types */
    L_CONST,
    /* Qualifier */
    L_FOR, L_WHL, L_IF, L_ELS, L_SWI, L_CAS, L_DFT, L_INC, L_NEW, L_TRY, L_CAT, L_NTG, L_NULL, L_THRW, L_PLUS,
    L_MINU, L_TMS, L_DEVI, L_POS, L_ORT, L_FTR, L_SP, L_PSS, L_PSE, L_PMS, L_PME, L_PBS, L_PBE, L_MR, L_LR, L_MRE,
    L_LRE, L_NE, L_EQ, L_AND, L_OR, L_RMP, L_LMP, L_ADD, L_RM, L_TM, L_DV, L_PE, L_OE, L_RPS, L_LPS,
    L_SET, L_NOTE, L_PINT, L_ESC, L_NOT, L_IS, L_NEXT,
    /* Operater Code */
    L_NORS, L_NORN, L_NORNU, L_STR
    /* Data: L_NORS: "example", L_NORN: -123456, L_NORNU: 123456, L_STR: (LimCode->)"ABCDEFG" */
}LIM_OPCODE;

#endif

