/* 20190545 / Seungjae Lim / Assign5 / lexical.c */
/*--------------------------------------------------------------------*/
/* lexical.c                                                          */
/* lexical analysis - make Token from commandline, make Dynarray      */
/*--------------------------------------------------------------------*/
/* dfa.c                                                              */
/* Original Author: Bob Dondero                                       */
/* Illustrate lexical analysis using a deterministic finite state     */
/* automaton (DFA)                                                    */
/*--------------------------------------------------------------------*/
/*Header File*/
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
/*--------------------------------------------------------------------*/
/*User define Header File*/
#include "dynarray.h"
#include "lexical.h"
#include "syntatic.h"
#include "scheduler.h"
#include "signal.h"
#include "execute.h"
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
void freeToken(void *pvItem, void *pvExtra)
/* Free token pvItem.  pvExtra is unused. */
{
   assert(pvItem);
   struct Token *psToken = (struct Token*)pvItem;
   free(psToken->pcValue);
   free(psToken);
}
/*--------------------------------------------------------------------*/
/*
function makeToken
  make Token by TokenType and string
parameter
  enum TokenType eTokenType - Token type of meta and cmd
  char *pcValue - string that each part of command
return
  struct Token* - pointer that pointing made Token
*/
struct Token *makeToken(enum TokenType eTokenType,
   char *pcValue)

/* Create and return a Token whose type is eTokenType and whose
   value consists of string pcValue.  Return NULL if insufficient
   memory is available.  The caller owns the Token. */

{
   assert(pcValue);
   struct Token *psToken;

   psToken = (struct Token*)malloc(sizeof(struct Token));
   if (psToken == NULL)
      return NULL;

   psToken->eType = eTokenType;

   psToken->pcValue = (char*)malloc(strlen(pcValue) + 1);
   if (psToken->pcValue == NULL)
   {
      free(psToken);
      return NULL;
   }
   /*copy string to Token*/
   strcpy(psToken->pcValue, pcValue);

   return psToken;
}
/*--------------------------------------------------------------------*/
/*
function isbg
  check c is background character &
parameter
  char c - character that search
return
  TRUE - if c is & 
  FALSE - otherwise
*/
int isbg(char c)
{
   if(c == '&')
   {
      return TRUE;
   }
   return FALSE;
}
/*--------------------------------------------------------------------*/
/*
function ismeta
  check c is meta character | < > &
parameter
  char c - character that search
return
  TRUE - if c is meta character
  FALSE - otherwise
*/
int ismeta(char c)
{
    if( (c == '|') || (c == '<') || (c == '>') || (c == '&'))
    {
        return TRUE;
    }
    else
    {
        return FALSE;
    }
}
/*--------------------------------------------------------------------*/
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
int lexLine(const char *pcLine, DynArray_T oTokens, char **argv)

/* Lexically analyze string pcLine.  Populate oTokens with the
   tokens that pcLine contains.  Return 1 (TRUE) if successful, or
   0 (FALSE) otherwise.  In the latter case, oTokens may contain
   tokens that were discovered before the error. The caller owns the
   tokens placed in oTokens. */

/* lexLine() uses a DFA approach.  It "reads" its characters from
   pcLine. */

/* lexLine() return FALSE when fail
    return 1 when foreground, 0 when background*/

{
   assert(pcLine);
   assert(oTokens);

   enum LexState {STATE_START, STATE_IN_CMD, 
                  STATE_IN_LQUOTE, STATE_IN_SQUOTE};
   enum LexState eState = STATE_START;
   int iLineIndex = 0;
   int iValueIndex = 0;
   int ground = L_FG;
   char c;
   char meta_bg;
   char acValue[MAX_LINE_SIZE];
   struct Token *psToken;
   /*
      eState - state of DFA
      iLineIndex - index of command line
      iValueIndex - index of acValue
      ground - foreground, background, default : foreground
      c - tmp character
      meta_bg - check last character is meta character
      acValue - tmp string will make Token
      psToken - tmp Token for append DynArray
   */

   for (;;)
   {
      /*
         DFA STATE
         - STATE_START : start & in space
         - STATE_IN_CMD : in character
         - STATE_IN_LQUOTE : in '
         - STATE_IN_SQUOTE : in "
      */
      c = pcLine[iLineIndex++];
      switch (eState)
      {
        case STATE_START:
            iValueIndex = 0;
            if ((c == '\n') || (c == '\0') || (c == EOF))
               return ground;
            else if(isspace(c))
            {
                eState = STATE_START;
            }
            else if(ismeta(c))
            {
                /*create META Token*/
                acValue[iValueIndex++] = c;
                acValue[iValueIndex] = '\0';
                psToken = makeToken(TOKEN_META, acValue);
                if(psToken == NULL)
                {
                    fprintf(stderr, 
                           "%s: Cannot allocate memory\n", argv[0]);
                    return FALSE;
                }
                if (!DynArray_add(oTokens, psToken))
                {
                    fprintf(stderr, 
                           "%s: Cannot allocate memory\n", argv[0]);
                    return FALSE;
                }
                iValueIndex = 0;
                eState = STATE_START;
            }
            else if(c == '"')
            {
                eState = STATE_IN_LQUOTE;
            }
            else if(c == 0x27)
            {
               eState = STATE_IN_SQUOTE;
            }
            else
            {
               acValue[iValueIndex++] = c;
               eState = STATE_IN_CMD;
            }
            break;

        case STATE_IN_CMD:
            if ((c == '\n') || (c == '\0') || (c == EOF))
            {
               /* Create a CMD token. */
               if(meta_bg == '&')
               {
                  /*avoid last &*/
                  ground = L_BG;
                  acValue[iValueIndex - 1] = '\0';
               }
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_CMD, acValue);
               if (psToken == NULL)
               {
                  fprintf(stderr, 
                          "%s: Cannot allocate memory\n", argv[0]);
                  return FALSE;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  free(psToken->pcValue);
                  free(psToken);
                  fprintf(stderr, 
                          "%s: Cannot allocate memory\n", argv[0]);
                  return FALSE;
               }
               iValueIndex = 0;

               return ground;
            }
            else if (ismeta(c))
            {
               if(!isbg(c))
               {
                  /*create CMD Token*/
                  acValue[iValueIndex] = '\0';
                  psToken = makeToken(TOKEN_CMD, acValue);
                  if (psToken == NULL)
                  {
                     fprintf(stderr, 
                             "%s: Cannot allocate memory\n", argv[0]);
                     return FALSE;
                  }
                  if (! DynArray_add(oTokens, psToken))
                  {
                     free(psToken->pcValue);
                     free(psToken);
                     fprintf(stderr, 
                             "%s: Cannot allocate memory\n", argv[0]);
                     return FALSE;
                  }
                  iValueIndex = 0;

                  /*create META Token*/
                  acValue[iValueIndex++] = c;
                  acValue[iValueIndex] = '\0';
                  psToken = makeToken(TOKEN_META, acValue);
                  if(psToken == NULL)
                  {
                     fprintf(stderr, 
                             "%s: Cannot allocate memory\n", argv[0]);
                     return FALSE;
                  }
                  if (!DynArray_add(oTokens, psToken))
                  {
                     fprintf(stderr, 
                             "%s: Cannot allocate memory\n", argv[0]);
                     return FALSE;
                  }
                  iValueIndex = 0;
                  eState = STATE_START;
               }
               else
               {
                  acValue[iValueIndex++] = c;
                  meta_bg = c;
                  eState = STATE_IN_CMD;
               }
            }
            else if (isspace(c))
            {
               /*create CMD Token*/
               acValue[iValueIndex] = '\0';
               psToken = makeToken(TOKEN_CMD, acValue);
               if (psToken == NULL)
               {
                  fprintf(stderr, 
                          "%s: Cannot allocate memory\n", argv[0]);
                  return FALSE;
               }
               if (! DynArray_add(oTokens, psToken))
               {
                  free(psToken->pcValue);
                  free(psToken);
                  fprintf(stderr, 
                          "%s: Cannot allocate memory\n", argv[0]);
                  return FALSE;
               }
               iValueIndex = 0;

               eState = STATE_START;
            }
            else if (c =='"')
            {
               eState = STATE_IN_LQUOTE;
            }
            else if (c == 0x27)
            {
               eState = STATE_IN_SQUOTE;
            }
            else
            {
               acValue[iValueIndex++] = c;
               meta_bg = c;
               eState = STATE_IN_CMD;
            }
            break;

        case STATE_IN_LQUOTE:
            if ((c == '\n') || (c == '\0') || (c == EOF))
            {
               fprintf(stderr, 
                     "%s: ERROR - unmatched quote\n", argv[0]);
               return FALSE;
            }
            else if (c =='"')
            {
                eState = STATE_IN_CMD;
            }
            else
            {
               acValue[iValueIndex++] = c;
               eState = STATE_IN_LQUOTE;
            }
            break;
         case STATE_IN_SQUOTE:
            if ((c == '\n') || (c == '\0') || (c == EOF))
            {
               fprintf(stderr, 
                       "%s: ERROR - unmatched quote\n", argv[0]);
               return FALSE;
            }
            else if (c == 0x27)
            {
                eState = STATE_IN_CMD;
            }
            else
            {
               acValue[iValueIndex++] = c;
               eState = STATE_IN_SQUOTE;
            }
            break;
         default:
            assert(FALSE);
      }
   }
}