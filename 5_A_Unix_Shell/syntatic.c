/* 20190545 / Seungjae Lim / Assign5 / syntatic.c */
/*--------------------------------------------------------------------*/
/* syntatic.c                                                         */
/* syntatic analysis - search DynArray for check syntatic correction  */
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
#include "syntatic.h"
#include "signal.h"
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
int synLine(DynArray_T oTokens, int* groundp, char **argv)
{
   assert(oTokens);
   assert(argv);

   enum synState 
         {STATE_START, STATE_CMD, STATE_PIPE, STATE_IN, STATE_OUT};
   enum synState eState = STATE_START;
   int cnt_pipe = 0, cnt_input = 0, cnt_output = 0;
   int lenToken = DynArray_getLength(oTokens);
   /*
      eState - DFA state
      cnt_pipe, cnt_input, cnt_output - number of each meta char
   */
   for(int i = 0; i < lenToken; i++)
   {
      struct Token *psToken = (struct Token*)DynArray_get(oTokens, i);
      /*
         DFA STATE
         - STATE_START : start of Token
         - STATE_CMD : in command Token
         - STATE_PIPE : in |
         - STATE_IN : in <
         - STATE_OUT : in >
      */
      switch (eState)
      {
         case STATE_START:
            if(psToken->eType == TOKEN_CMD)
            {
               eState = STATE_CMD;
            }
            if(psToken->eType == TOKEN_META)
            {
               fprintf(stderr, 
                       "%s: Invalid: missing command name\n", 
                       argv[0]);
               return SYNERR;
            }
            break;
         case STATE_CMD:
            if(psToken->eType == TOKEN_CMD)
            {
               continue;
            }
            if(psToken->eType == TOKEN_META)
            {
               if(!strcmp(psToken->pcValue, "&"))
               {
                  if(i != lenToken - 1)
                  {
                     fprintf(stderr, 
                             "%s: Wrong Syntax using &\n", argv[0]);
                     return SYNERR;
                  }
                  else
                  {
                     *groundp = BG;
                  }
               }
               if(!strcmp(psToken->pcValue,"<"))
               {
                  if(cnt_pipe > 0 || cnt_input > 0)
                  {
                     fprintf(stderr, 
            "%s: Invalid: Multiple redirection of standard input\n", 
                             argv[0]);
                     return SYNERR;
                  }
                  cnt_input++;
                  eState = STATE_IN;
               }
               if(!strcmp(psToken->pcValue,"|"))
               {
                  if(cnt_output > 0)
                  {
                     fprintf(stderr, 
            "%s: Invalid: Multiple redirection of standard output\n",
                             argv[0]);
                     return SYNERR;
                  }
                  cnt_pipe++;
                  eState = STATE_PIPE;
               }
               if(!strcmp(psToken->pcValue,">"))
               {
                  if(cnt_output > 0)
                  {
                     fprintf(stderr, 
            "%s: Invalid: Multiple redirection of standard output\n", 
                             argv[0]);
                     return SYNERR;
                  }
                  cnt_output++;
                  eState = STATE_OUT;
               }
            }
            break;    
         case STATE_PIPE:
            if(psToken->eType == TOKEN_CMD)
            {
               eState = STATE_CMD;
            }
            if(psToken->eType == TOKEN_META)
            {
               fprintf(stderr, 
                      "%s: Invalid: Missing command name\n", argv[0]);
               return SYNERR;
            }
            break;
         case STATE_IN:
            if(psToken->eType == TOKEN_CMD)
            {
               eState = STATE_CMD;
            }
            if(psToken->eType == TOKEN_META)
            {
               fprintf(stderr, 
      "%s: Invalid: Standard input redirection without file name\n", 
                       argv[0]);
               return SYNERR;
            }
            break;
         case STATE_OUT:
            if(psToken->eType == TOKEN_CMD)
            {
               eState = STATE_CMD;
            }
            if(psToken->eType == TOKEN_META)
            {
               fprintf(stderr, 
      "%s: Invalid: Standard output redirection without file name\n",
                      argv[0]);
               return SYNERR;
            }
            break;
         default:
            assert(0);
      }
   }
   /*
      if END state isn't CMD
      -> No command after meta character
   */
   if(eState == STATE_CMD)
   {
      return cnt_pipe;
   }
   else if(eState == STATE_PIPE)
   {
      fprintf(stderr, "%s: Invalid: Missing command name\n", argv[0]);
      return SYNERR;
   }
   else if(eState == STATE_IN)
   {
      fprintf(stderr, 
      "%s: Invalid: Standard input redirection without file name\n", 
              argv[0]);
      return SYNERR;
   }
   else if(eState == STATE_OUT)
   {
      fprintf(stderr, 
      "%s: Invalid: Standard output redirection without file name\n", 
              argv[0]);
      return SYNERR;
   }
   else
   {
      return SYNERR;
   }
}