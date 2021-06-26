/* 20190545 / Seungjae Lim / Assign5 / signal.c */
/*--------------------------------------------------------------------*/
/* signal.c                                                           */
/* code set for signal handler                                        */
/*--------------------------------------------------------------------*/
/*Header File*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <signal.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <string.h>
#include <assert.h>
#include <fcntl.h>
/*--------------------------------------------------------------------*/
/*User Define Header File*/
#include "dynarray.h"
#include "lexical.h"
#include "scheduler.h"
#include "signal.h"
#include "syntatic.h"
#include "execute.h"
/*--------------------------------------------------------------------*/
/*extern global variable such that defined in ish.c*/
extern struct job_t jobs[MAXJOBS];
extern int nextjid;
extern int is_quit;
/*
struct job_t jobs : job_t array that has information of jobs
                    (for implement foreground, background)
int nextjid : next job id for save in jobs array
int is_quit : 1 : SIGQUIT signal was comed in 5sec
              0 : SIGQUIT signal wasn't comed
*/
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
void sigchld_handler(int sig)
{
    int status;
    pid_t pid;
    struct job_t *jobp;
    for(;((pid = waitpid(-1, &status, WNOHANG|WUNTRACED))>0);)
    {
        jobp = getjobpid(jobs, pid);
        if(jobp->state == BG)
        {
          fprintf(stdout, "child %d terminated normally\n", pid);
        }
        if(WIFEXITED(status))
        {
            deletejob(jobs,pid);
        }
        else if(WIFSIGNALED(status))
        { 
            deletejob(jobs,pid); 
        }
        else if(WIFSTOPPED(status))
        {
            jobp->state = ST;
        }
    }
    return;
}
/*--------------------------------------------------------------------*/
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
void sigint_handler(int sig)
{
  for(int i = 0; i < MAXJOBS; i++)
  {
    if(jobs[i].pid != 0)
    {
      kill(-jobs[i].pid,SIGINT);
    }
  }
  return;
}

/*--------------------------------------------------------------------*/
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
void sigalrm_handler(int sig) 
{
    is_quit = 0;
    fprintf(stdout,"%% ");
    fflush(stdout);
    return;
}
/*--------------------------------------------------------------------*/
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
void sigquit_handler(int sig) 
{
    pid_t pid = fgpid(jobs);
    if(pid != 0)
    {
        kill(-pid,SIGQUIT);
    }
    if(is_quit)
    {
        exit(0);
    }
    fprintf(stdout, 
            "\nType Ctrl-\\ again within 5 seconds to exit.\n");
    fflush(stdout);
    is_quit = 1;
    alarm(5);
    return;
}