/* 20190545 / Seungjae Lim / Assign5 / syntatic.c */
#ifndef SYN_INCLUDED
#define SYN_INCLUDED
/*--------------------------------------------------------------------*/
/*ish MACRO*/
#define SYNERR -1
/*--------------------------------------------------------------------*/
/*
function synLine
  syntatic analysis to appropriate DynArray
parameter
  DynArray_T oTokens - DynArray of command Token
  int* groundp - return pointer for if & occur end
  char **argv - argv[0] : ./ish
return
  cnt_pipe - success, number of pipe Token
  FALSE - otherwise
I/O
   print error message on stderr
*/
int synLine(DynArray_T oTokens, int* groundp, char **argv);
/*--------------------------------------------------------------------*/
#endif