/* 20190545 / Seungjae Lim / Assign5 / lexical.h */
#ifndef LEX_INCLUDED
#define LEX_INCLUDED

#include "dynarray.h"
/*--------------------------------------------------------------------*/
/*MACRO*/
#define L_FG   1;
#define L_BG   2;

/*Upper bound of cmdline's length*/
enum {MAX_LINE_SIZE = 1024};

/*TF*/
enum {FALSE, TRUE};

/*Type of Token*/
enum TokenType {TOKEN_CMD, TOKEN_META};
/*--------------------------------------------------------------------*/
/*Token Structure*/
struct Token
{
   enum TokenType eType;
   /* The type of the token. */

   char *pcValue;
   /* The string which is the token's value. */
};

/*--------------------------------------------------------------------*/
/*
function freeToken
  free Token
parameter
  void* pvItem - Token which free
  void* pvExtra - NULL;
return
  void
*/
void freeToken(void *pvItem, void *pvExtra);

/*
function lexLine
  lexical analysis to make Token and append to DynArray
parameter
  const char *pcLine - each line of shell input
  DynArray_T oTokens - return pointer parameter of DynArray
  char **argv - argv[0] : ./ish
return
  ground - end with success, and fore/back ground state
         L_FG : Foreground
         L_BG : Background
  FALSE - otherwise
I/O
   print error message on stderr  
*/
int lexLine(const char *pcLine, DynArray_T oTokens, char **argv);

/*
function makeToken
  make Token by TokenType and string
parameter
  enum TokenType eTokenType - Token type of meta and cmd
  char *pcValue - string that each part of command
return
  struct Token* - pointer that pointing made Token
*/
struct Token *makeToken(enum TokenType eTokenType, char *pcValue);

/*
function ismeta
  check c is meta character | < > &
parameter
  char c - character that search
return
  TRUE - if c is meta character
  FALSE - otherwise
*/
int ismeta(char c);

/*
function isbg
  check c is background character &
parameter
  char c - character that search
return
  TRUE - if c is & 
  FALSE - otherwise
*/
int isbg(char c);
/*--------------------------------------------------------------------*/
#endif