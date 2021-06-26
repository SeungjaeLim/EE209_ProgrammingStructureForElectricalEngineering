/* 20190545 / Seungjae Lim / Assign5 / scheduler.h */
#ifndef SCHEDULER_INCLUDED
#define SCHEDULER_INCLUDED
/*--------------------------------------------------------------------*/
/*size of jobs*/
#define MAXLINE    1024   /* max line size */
#define MAXARGS     128   /* max args on a command line */
#define MAXJOBS      16   /* max jobs at any point in time */
#define MAXJID    1<<16   /* max job ID */

/* Job states */
#define UNDEF   0    /* undefined */
#define FG      1    /* running in foreground */
#define BG      2    /* running in background */
#define ST      3    /* stopped */

/*pid_t typedef*/
typedef int pid_t;
/*--------------------------------------------------------------------*/
/*struct job_t - job*/
struct job_t
{
    pid_t pid;
    /*process id*/
    int jid;
    /*job id*/
    int state;
    /*state of process*/
    char cmdline[MAXLINE];
    /*command of run process*/
};
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
int addjob(struct job_t *jobs, pid_t pid, int state, 
           char *cmdline, char** argv);

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
struct job_t *getjobpid(struct job_t *jobs, pid_t pid);

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
int pid2jid(pid_t pid);

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
pid_t fgpid(struct job_t *jobs);

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
int deletejob(struct job_t *jobs, pid_t pid);

/*
function clearjob
  clear job_t
parameter
  struct job_t *job - job for clean
return
  void
*/
void clearjob(struct job_t *job);

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
int maxjid(struct job_t *jobs);

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
void initjobs(struct job_t *jobs);
/*--------------------------------------------------------------------*/
#endif