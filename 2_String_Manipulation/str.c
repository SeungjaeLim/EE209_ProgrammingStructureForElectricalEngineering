/* Seungjae Lim / Assignment2 / str.c */
#include <assert.h> /* to use assert() */
#include <stdio.h>
#include <stdlib.h> /* for strtol() */
#include <string.h>
#include <ctype.h> /* for isspace() */
#include "str.h"

/* Your task is: 
   1. Rewrite the body of "Part 1" functions - remove the current
      body that simply calls the corresponding C standard library
      function.
   2. Write appropriate comment per each function
*/

/* Part 1 */

#define STR_FALSE 0
#define STR_TRUE 1

#define NO_ERROR 0
#define YES_ERROR 1

#define POSITIVE_SIGN 1
#define NEGATIVE_SIGN -1
#define ZERO_SIGN 0
/*MACRO for StrToLong's sign*/

/*------------------------------------------------------------------------*/
/*
function StrGetLength
  get length of string
parameter
  const char* pcSrc - read only string such that get length
return
  size_t(unsigned long) - length of pcSrc
*/
size_t StrGetLength(const char* pcSrc)
{
  const char *pcEnd; /*until String is finish, use index pointer*/
  assert(pcSrc); /* NULL address, 0, and FALSE are identical. */
  pcEnd = pcSrc;
	
  while (*pcEnd) /* null character and FALSE are identical. */
    pcEnd++; /*increment index*/

  return (size_t)(pcEnd - pcSrc);
  /*substract with pointer is identical with substract of index*/
}

/*------------------------------------------------------------------------*/
/*
function StrCopy
  copy String pcSrc to pcDest
parameter
  char* pcDest - pointing destination
  const char* pcSrc - read only string such that pointing source
return
  char* - pointer such that pointing destination
*/
char *StrCopy(char *pcDest, const char* pcSrc)
{
  size_t len_Src; /*memory length of source */
  int i = 0; /* index for copy */
  char *idxDest; /*memory pointer */

  assert(pcDest);
  assert(pcSrc);
  /*
  check pcDest or pcSrc is NULL pointer
  */
  len_Src = StrGetLength(pcSrc); /*get length of string */
  idxDest = pcDest; /*start of index is start of pcDest */
  for(i = 0; i < len_Src; i++)
  {
      *idxDest = *pcSrc; /*copy*/
      pcSrc++;
      idxDest++;
      /*increase index*/   
  }
  *idxDest = '\0'; /*make end of string is NULL*/
  return pcDest;
}

/*------------------------------------------------------------------------*/
/*
function StrNCompare
  compare pcS1, pcS2 for n character
parameter
  const char* pcS1, pcS2 - read only string such that pointing string
  int n - seaching how many integer
return
  int - compare in lexiographic order
        a>b - return 1
        a==b - return 0
        a<b - return -1
*/
int StrNCompare(const char* pcS1, const char* pcS2, int n)
{
  int diff, cnt = 1; 
  /* diff - difference of each char 
     cnt - index of character*/

  assert(pcS1);
  assert(pcS2);
  /*
  check pcS1 or pcS2 is NULL pointer
  */

  while(*pcS1 && *pcS2 != '\0' && cnt < n)
  {
    diff = *pcS1 - *pcS2;
    /*
      compare each char
    */
    if(diff != 0)
    {
        return diff;
    }
    /* each string is different, diff get number
       such that pcS1 is bigger, positive number
       pcS2 is bigger, negative number */
    else
    {
        pcS1++;
        pcS2++;
        cnt++;
        /* search to next char */
    }
  }
  /*when there are same string until meet NULL, we have compare length is same
    or n == cnt case */
  diff = *pcS1 - *pcS2;
  if(diff != 0)
    {
        return diff;
    }
     /* each string is different length, diff get number
       such that pcS1 is longer, positive number
       pcS2 is longer, negative number */
  return 0;
  /* same string, return 0 */
}

/*------------------------------------------------------------------------*/
/*
function StrCompare
  compare pcS1, pcS2
parameter
  const char* pcS1, pcS2 - read only string such that pointing string
return
  int - compare in lexiographic order
        a>b - return 1
        a==b - return 0
        a<b - return -1
*/
int StrCompare(const char* pcS1, const char* pcS2)
{
  int diff; /* difference of each char */

  assert(pcS1);
  assert(pcS2);
  /*
  check pcS1 or pcS2 is NULL pointer
  */
  
  while(*pcS1 && *pcS2)
  {
    diff = *pcS1 - *pcS2;
    /*
      compare each char
    */
    if(diff != 0)
    {
        return diff;
    }
    /* each string is different, diff get number
       such that pcS1 is bigger, positive number
       pcS2 is bigger, negative number */
    else
    {
        pcS1++;
        pcS2++;
        /* search to next char */
    }
  }
  /*when there are same string until meet NULL, we have compare length is same*/
  diff = *pcS1 - *pcS2;
  if(diff != 0)
    {
        return diff;
    }
     /* each string is different length, diff get number
       such that pcS1 is longer, positive number
       pcS2 is longer, negative number */
  return 0;
  /* same string, return 0 */
}
/*------------------------------------------------------------------------*/
/*
function StrFindChr
  find first c's address in pcHaystack
parameter
  const char* pcHaystack - read only string will searched
  int c - find character
return
  char* - address such that first c or NULL when no c
*/
char *StrFindChr(const char* pcHaystack, int c)
{
  assert(pcHaystack);
  /*
  check pcHaystack is NULL pointer
  */
  while(*pcHaystack)
  {
    if(*pcHaystack == c)
    {
      return (char *)pcHaystack;
      /*when meet c, return its address*/
    }
    else
    {
      pcHaystack++;
      /*it isn't c, keep search*/
    }
  }
  if(*pcHaystack == c)
    {
      return (char *)pcHaystack;
      /*finally compare, because of c is NULL*/
    }
  return NULL;
    /*if there are no c, return NULL*/
}
/*------------------------------------------------------------------------*/
/*
function StrFindStr
  find first pcNeedle's address in pcHaystack
parameter
  const char* pcHaystack - read only string will searched
  const char *pcNeedle - read only string will searching
return
  char* - address such that first pcNeedle or NULL when no pcNeedle
*/
char *StrFindStr(const char* pcHaystack, const char *pcNeedle)
{
  int diff;
  size_t len_Needle, len_Haystack;
  /*diff - when diff is 0, pcNeedle is in pcHaystack
    len_Needle - memory length of pcNeedle
    len_Haystack - memory length of pcNeedle*/

  assert(pcHaystack);
  assert(pcNeedle);
  /*
  check pcHaystack or pcNeedle is NULL pointer
  */

  len_Needle = StrGetLength(pcNeedle);
  len_Haystack = StrGetLength(pcHaystack);
  /*save string's length*/
  
  if(len_Needle == 0)
  {
    return (char *)pcHaystack;
  }
  if(len_Needle > len_Haystack)
  {
    return NULL;
    /*when pcNeedle is longer than pcHaystack,
      pcNeedle can't in the pcHaystack*/
  }
  while(*pcHaystack)
  {
    diff = StrNCompare(pcHaystack, pcNeedle, len_Needle);
    if(diff == 0)
    {
      return (char *)pcHaystack;
      /*when meet start point of pcNeedle, return its address*/
    }
    else
    {
      pcHaystack++;
      /*it isn't pcNeedle, keep search*/
    }
  }
  return NULL;
    /*if there are no pcNeedle, return NULL*/
}
/*------------------------------------------------------------------------*/
/*
function StrConcat
  concate pcSrc to pcDest's end
parameter
  char *pcDest - string concating destination
  const char *pcSrc - read only string concating source
return
  char* - address pcSrc
*/
char *StrConcat(char *pcDest, const char* pcSrc)
{
  size_t len_Dest; /*get length of string */
  char *idxDest; /*start point to concate*/

  assert(pcDest);
  assert(pcSrc);
  /*
  check pcDest or pcSrc is NULL pointer
  */
  len_Dest = StrGetLength(pcDest); /*get length of string */
  idxDest = pcDest + len_Dest; /*start point to concate*/
  while(*pcSrc)
  {
    *idxDest = *pcSrc;
    idxDest++;
    pcSrc++;
    /* concate pcSrc to idxDest */
  }
  *idxDest = *pcSrc;
  /* end of source, add '\0' to destination */
  return pcDest;
}

/*------------------------------------------------------------------------*/
/*
function StrToLong
  convert string to long int
parameter
  const char *nptr - read only string to change number
  char **endptr - not NULL - address of pointer which point end of number
        (outpu) - NULL - NULL 
  int base - change number with base num (2,8,10,16) only 10 in here
return
  long int - value such that converted by nptr
*/
long int StrToLong(const char *nptr, char **endptr, int base)
{
  long int converted_num = 0;
  int tmp, sign = POSITIVE_SIGN, is_error = YES_ERROR;
  /* converted_num - return value
    tmp - temp for save one char
    sign - memory converted num's sign
          POSITIVE_SIGN - 1 : positive
          ZERO_SIGN - 0 : zero
          NEGATIVE_SIGN - -1 : negative
    is_error - check string is digit
            number isn't start, YES_ERROR - 1
            number is started, NO_ERROR - 0 */

  assert(nptr);
  /*
  check nptr is NULL pointer
  */
  /* handle only when base is 10 */
  if (base != 10) return STR_FALSE;
  
  while(*nptr && isspace(*nptr))
  {
    nptr++;
    /*when stating with space, find number*/
  }
  if(*nptr == '-')
  {
    sign = NEGATIVE_SIGN;
    is_error = NO_ERROR;
    nptr++;
    /*negative number start with char '-'*/
  }
  else if(*nptr == '+')
  {
    sign = POSITIVE_SIGN;
    is_error = NO_ERROR;
    nptr++;
    /*positive number can start with char '+'*/
  }
  while(*nptr && isdigit(*nptr))
  {
    is_error = NO_ERROR;
    /*digit is started*/
    tmp = *nptr - '0';
    /*save to tmp integer version of char nptr*/
    if(sign >= ZERO_SIGN)
    {
      converted_num = converted_num * 10 + tmp;
      /*shift num 1 block, and add tmp*/
    }
    else
    {
      converted_num = converted_num * 10 - tmp;
      /*shift num 1 block, and substract tmp(add abs of tmp)*/
    }
    /*devide by sign
      because of negative number express one more number*/
    if(sign > 0 && converted_num <= 0)
    {
        return LONG_MAX;
        /*positive overflow*/
    }
    if(sign < 0 && converted_num >= 0)
    {
        return LONG_MIN;
        /*negative overflow*/
    }
    nptr++;
    /*increment pointer*/
  }
  if(endptr != NULL)
  {
    *endptr = (char *)nptr;
    /*endptr isn't NULL,
      give address of pointer which pointing end of number*/
  }
  if(is_error)
  {
    return STR_FALSE;
    /*if digit isn't exist in string*/
  }
  else
  {
    return converted_num;
    /*return converted number*/
  }
}
