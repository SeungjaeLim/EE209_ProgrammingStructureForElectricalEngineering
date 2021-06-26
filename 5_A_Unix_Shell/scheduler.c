/* 20190545 / Seungjae Lim / Assign5 / scheduler.c */
/*--------------------------------------------------------------------*/
/* scheduler.c                                                        */
/* management jobs array to manage process ground                     */
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
#include "syntatic.h"
#include "execute.h"
#include "signal.h"
/*--------------------------------------------------------------------*/
/*extern global variable such that defined in ish.c*/
extern struct job_t jobs[MAXJOBS];
extern int nextjid;
/*
    struct job_t jobs : job_t array that has information of jobs
                    (for implement foreground, background)
    int nextjid : next job id for save in jobs array
*/
/*--------------------------------------------------------------------*/
/*
function addjob
  create job and append jobs array
parameter
  struct job_t *jobs - jobs array
  pid_t pid - process's pid 
  int state - state FG, BG, ST
  char *cmdline - command line
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
int addjob(struct job_t *jobs, pid_t pid, 
            int state, char *cmdline, char** argv) 
{
    int i;
    
    if (pid < 1)
	return FALSE;

    assert(jobs);
    assert(cmdline);

    for (i = 0; i < MAXJOBS; i++) 
    {
	    if (jobs[i].pid == 0) 
        {
	        jobs[i].pid = pid;
	        jobs[i].state = state;
	        jobs[i].jid = nextjid++;
	        if (nextjid > MAXJOBS)
		    nextjid = 1;
	        strcpy(jobs[i].cmdline, cmdline);
            return TRUE;
	    }
    }
    fprintf(stderr,"%s: Tried to create too many jobs\n", argv[0]);
    return FALSE;
}
/*--------------------------------------------------------------------*/
/*
function getjobpid
  search and get job pointer by pid
parameter
  struct job_t *jobs - jobs array
  pid_t pid - process's pid
return
  struct job_t * - pointer pointing job_t has pid
accessing global variable
  jobs - update for schedule for fg
*/
struct job_t *getjobpid(struct job_t *jobs, pid_t pid) 
{
    int i;

    if (pid < 1)
	return NULL;
    assert(jobs);
    for (i = 0; i < MAXJOBS; i++)
    {
        if (jobs[i].pid == pid)
        {
            return &jobs[i];
        }
    }
    return NULL;
}
/*--------------------------------------------------------------------*/
/*
function pid2jid
  search jid by pid
parameter
  pid_t pid - pid for searching
return
  int - jid such that pid
  FALSE - otherwise
accessing global variable
  jobs - update for schedule for fg
*/
int pid2jid(pid_t pid) 
{
    int i;

    if (pid < 1)
	return FALSE;
    for (i = 0; i < MAXJOBS; i++)
    {
        if (jobs[i].pid == pid) 
        {
            return jobs[i].jid;
        }
    }
    return FALSE;
}
/*--------------------------------------------------------------------*/
/*
function fgpid
  search foreground pid
parameter
  struct job_t *jobs - jobs array
return
  pid_t pid running forground
  FALSE - otherwise
accessing global variable
  jobs - update for schedule for fg
*/
pid_t fgpid(struct job_t *jobs) 
{
    assert(jobs);
    int i;

    for (i = 0; i < MAXJOBS; i++)
    {
        if (jobs[i].state == FG)
        {
            return jobs[i].pid;
        }
    }
	
    return FALSE;
}
/*--------------------------------------------------------------------*/
/*
function deletejob
  delete pid job in jobs
parameter
  struct job_t *jobs - jobs array
  pid_t pid - pid for delete
return
  TRUE - delete success
  FALSE - otherwise
accessing global variable
  jobs - update for schedule for fg
*/
int deletejob(struct job_t *jobs, pid_t pid) 
{
    int i;
    if (pid < 1)
    {
        return FALSE;
    }
    assert(jobs);
    for (i = 0; i < MAXJOBS; i++) 
    {
	    if (jobs[i].pid == pid) 
        {
	        clearjob(&jobs[i]);
	        nextjid = maxjid(jobs)+1;
	        return TRUE;
	    }
    }
    return FALSE;
}
/*--------------------------------------------------------------------*/
/*
function clearjob
  clear job_t
parameter
  struct job_t *job - job for clean
return
  void
*/
void clearjob(struct job_t *job) 
{
    assert(job);
    job->pid = 0;
    job->jid = 0;
    job->state = UNDEF;
    job->cmdline[0] = '\0';
}
/*--------------------------------------------------------------------*/
/*
function maxjid
  get max jid
parameter
  struct job_t *jobs - jobs array
return
  int - max jid
accessing global variable
  jobs - update for schedule for fg
*/
int maxjid(struct job_t *jobs) 
{
    assert(jobs);
    int i, max=0;

    for (i = 0; i < MAXJOBS; i++)
    {
        if (jobs[i].jid > max)
	    {
            max = jobs[i].jid;
        }
    }
    return max;
}
/*--------------------------------------------------------------------*/
/*
function initjobs
  initialize jobs
parameter
  struct job_t *jobs - jobs array
return
  void
accessing global variable
  jobs - update for schedule for fg
*/
void initjobs(struct job_t *jobs) 
{
    assert(jobs);
    int i;
    for (i = 0; i < MAXJOBS; i++)
	  {
        clearjob(&jobs[i]);
    }
    return;
}