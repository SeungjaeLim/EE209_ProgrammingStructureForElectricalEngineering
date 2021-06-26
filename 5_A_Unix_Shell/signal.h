/* 20190545 / Seungjae Lim / Assign5 / signal.h */
#ifndef SIGNAL_INCLUDED
#define SIGNAL_INCLUDED
/*--------------------------------------------------------------------*/
/*
function sigchld_handler
  handling SIGCHLD for creep background process
  managing jobs array
parameter
  int sig - SIGCHLD
return
  void
accessing global variable
  jobs - update for schedule for fg
I/O
  print terminated pid to stdout
*/
void sigchld_handler(int sig);

/*
function sigint_handler
  handling SIGINT for send child SIGINT signal
  ish don't EXIT
parameter
  int sig - SIGINT
return
  void
accessing global variable
  jobs - update for schedule for fg
*/
void sigint_handler(int sig);

/*
function sigalrm_handler
  handling SIGALRM for check quit 5sec
parameter
  int sig - SIGALRM
return
  void
accessing global variable
  is_quit - memory SIGQUIT
*/
void sigalrm_handler(int sig);

/*
function sigquit_handler
  handling SIGQUIT for EXIT ish
parameter
  int sig - SIGQUIT
return
  void
accessing global variable
  is_quit - memory SIGQUIT
*/
void sigquit_handler(int sig);
/*--------------------------------------------------------------------*/
#endif