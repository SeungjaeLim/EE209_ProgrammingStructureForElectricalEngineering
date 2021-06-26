/* Seungjae Lim / Assignment2 / str.h */
#ifndef _STR_H_
#define _STR_H_
#include <limits.h> /* for LONG_MIN, LONG_MAX */
#include <unistd.h> /* for typedef of size_t */

/* Part 1 */
size_t StrGetLength(const char* pcSrc);
/* get length of string */
char *StrCopy(char *pcDest, const char* pcSrc);
/* copy String pcSrc to pcDest */
int StrCompare(const char* pcS1, const char* pcS2);
/* compare pcS1, pcS2 */
int StrNCompare(const char* pcS1, const char* pcS2, int n);
/* compare n char pcS1 with pcS2 */
char *StrFindChr(const char *pcHaystack, int c);
/* find first c's address in pcHaystack */
char *StrFindStr(const char* pcHaystack, const char *pcNeedle);
/* find first pcNeedle's address in pcHaystack */
char *StrConcat(char *pcDest, const char* pcSrc);
/* concate pcSrc to pcDest's end */
long int StrToLong(const char *nptr, char **endptr, int base);
/* convert string to long int */

#endif /* _STR_H_ */
