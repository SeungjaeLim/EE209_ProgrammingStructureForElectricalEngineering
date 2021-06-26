/* 20190545 / Seungjae Lim / Assign5 / execute.h */
#ifndef EXEC_INCLUDED
#define EXECINCLUDED
/*--------------------------------------------------------------------*/
/*
function execute
  excute program by oToken DyArray
parameter
  DynArray_T oTokens - DynArray has command Token
  int ground - foreground, background state
  int cnt_pipe - number of pipe meta character
  char** argv - argv[0] : ./ish
return
  TRUE - end success
  FALSE - otherwise
accessing global variable
  jobs - update for schedule for fg
  nextjid - update for get recently excuted background pid
I/O
  print error message on stderr
*/
int execute(DynArray_T oTokens, int ground, int cnt_pipe, char** argv);

/*
function builtin_cmd
  run builtin command
parameter
  DynArray_T oTokens - DynArray has command Token for free
  int cnt_command - number of element of command
  char** command - command and option Arraylist
  char*** cmdset - commend set for free
  int cnt_pipe - number of command for free cmdset
  char** argv - argv[0] : ./ish
return
  TRUE - command[0] is Builtin_cmd
  FALSE - command[0] isn't Builtin_cmd
I/O
  print error message on stderr
*/
int builtin_cmd(DynArray_T oTokens, int cnt_command, char** command, 
                char*** cmdset, int cnt_pipe, char **argv);

/*
function do_fg
  run recently executed background process to foreground 
parameter
  char** argv - argv[0] : ./ish
return
  void
I/O
  print which process move foreground to message on stdout
  done foregrounded job on stdout
*/
void do_fg(char **argv);

/*
function waitfg
  wait until foreground program is running
parameter
  pid_t pid - pid for wait
return
  void
*/
void waitfg(pid_t pid);
/*--------------------------------------------------------------------*/
#endif