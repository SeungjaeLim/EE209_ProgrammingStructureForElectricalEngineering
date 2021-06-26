/* 20190545 / Seungjae Lim / Assign5 / execute.c */
/*--------------------------------------------------------------------*/
/* execute.c                                                          */
/* excuting program with pipe, redirection or built in command        */
/*--------------------------------------------------------------------*/
/*header file*/
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
#include "syntatic.h"
#include "scheduler.h"
#include "signal.h"
#include "execute.h"
/*--------------------------------------------------------------------*/
/*extern global variable define in ish.c*/
extern int nextjid;
extern struct job_t jobs[MAXJOBS];
/*
    struct job_t jobs : job_t array that has information of jobs
                    (for implement foreground, background)
    int nextjid : next job id for save in jobs array
*/
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
int execute(DynArray_T oTokens, int ground, int cnt_pipe, char** argv)
{   
    assert(oTokens); 
    pid_t pid = 0;
    sigset_t mask;
    /*make sigset for sigblock to catch race condition*/
    void (*pfRet)(int);
    if(sigemptyset(&mask) != 0)
    {
        fprintf(stderr, "%s: sigemptyset error\n", argv[0]);
    }
    if(sigaddset(&mask,SIGCHLD) != 0)
    {
        fprintf(stderr, "%s: sigaddset error\n", argv[0]);
    }
    if(sigaddset(&mask,SIGINT) != 0)
    {
        fprintf(stderr, "%s: sigaddset error\n", argv[0]);
    }
    if(sigaddset(&mask,SIGTSTP) != 0)
    {
        fprintf(stderr, "%s: sigaddset error\n", argv[0]);
    }
    if(sigaddset(&mask,SIGALRM) != 0)
    {
        fprintf(stderr, "%s: sigaddset error\n", argv[0]);
    }
    if(sigprocmask(SIG_UNBLOCK, &mask, NULL) != 0)
    {
        fprintf(stderr, "%s: sigprocmask error\n", argv[0]);
    }

    /*
        cmdset - array of (program array)
        we can look array of array-linked list of command
        _________________________________
        program1 -> option1 -> option2...
        _________________________________
        program2 -> option2 -> option2...
        _________________________________
        ...
        _________________________________
        For pipe
    */
    char ***cmdset = (char ***)calloc(cnt_pipe + 1, sizeof(char **));
    if(cmdset == NULL)
    {
        fprintf(stderr, "%s: Cannot allocate memory\n", argv[0]);
        return FALSE;
    }
    /*
        cmdset[i] - array-linked list of each program and option
        program1 -> option1 -> option2...
    */
    for(int i = 0; i < cnt_pipe + 1; i++)
    {
        cmdset[i] = (char**)calloc(MAXLINE, sizeof(char *));
        if(cmdset[i] == NULL)
        {
            fprintf(stderr, "%s: Cannot allocate memory\n", argv[0]);
            for(int j = 0; j <i ; j++)
            {
                free(cmdset[j]);
            }
            free(cmdset);
            return FALSE;        
        }
    }
    char *in_redir = NULL, *out_redir = NULL;
    enum execState {STATE_CMD, STATE_IN, STATE_OUT};
    enum execState eState = STATE_CMD;
    int cnt_param = 0, cnt_cmd = 0;
    int fd_in, fd_out, iRet;
    int cnt_command = 0, is_inredir = 0, is_outredir = 0;
    /*
        in_redir, out_redir - File name of redirect input, output
        eState - DFA state
        cnt_param - tmp of length of each Array Linked list
        cnt_cmd - element of program
        fd_in, fd_out - file discriptor of redirect
        iRet - return value of error handling
        cnt_command - length of First Array Linked list(when No pipe)
    */
    /*preprocessing to execute, make Array of Arraylist*/
    for(int i = 0 ; i < DynArray_getLength(oTokens) ; i++)
    {
        struct Token *psToken = 
                      (struct Token*)DynArray_get(oTokens, i);
        /*
         DFA STATE
         - STATE_CMD : in command
         - STATE_IN : meet <
         - STATE_OUT : meet >
        */
        switch (eState)
        {
            case STATE_CMD:
                if(psToken->eType == TOKEN_CMD)
                {
                    cmdset[cnt_cmd][cnt_param] = psToken->pcValue;
                    cnt_param++;
                    cnt_command++;
                }
                if(psToken->eType == TOKEN_META)
                {
                    if(!strcmp(psToken->pcValue,"<"))
                    {
                        eState = STATE_IN;
                        is_inredir = 1;
                    }
                    if(!strcmp(psToken->pcValue,"|"))
                    {
                        cnt_cmd++;
                        cnt_param = 0;
                    }
                    if(!strcmp(psToken->pcValue,">"))
                    {
                        eState = STATE_OUT;
                        is_outredir = 1;
                    }    
                }
                break;
            case STATE_IN:
                in_redir = psToken->pcValue;
                eState = STATE_CMD;
                break;
            case STATE_OUT:
                out_redir = psToken->pcValue;
                eState = STATE_CMD;
                break;
            default:
                assert(0);
                break;
        }
    }
    fflush(NULL);
    /* check NULL command */
    if(cmdset[0][0] == NULL)
    {
        return FALSE;
    }
    /* check command is builtin_cmd */
    fflush(NULL);
    if(builtin_cmd(oTokens, cnt_command,cmdset[0], 
                   cmdset, cnt_pipe ,argv) == FALSE)
    {
        /* | in oTokens */
        if(cnt_pipe > 0)
        {
            /*no background with pipe, ignore SIGCHLD*/
            pfRet = signal(SIGCHLD, SIG_DFL);
            if(pfRet == SIG_ERR)
            {
                perror(argv[0]);
                exit(1);
            }
            if(sigprocmask(SIG_BLOCK, &mask, NULL) != 0)
            {
                fprintf(stderr, "%s: sigprocmask error\n", argv[0]);
            }
            int fdpipe[2];
            int prevfd = 0;
            int arg = 0;
            pid_t pid_pipe;
            /*
                fdpipe - file descriptor of pipe
                prevfd - file descriptor of prev program 
                         for match pipe
                arg - argument program
                pid_pipe - process id of child
            */
            /*execute with pipe*/
            while(arg <= cnt_pipe)
            {
                if(pipe(fdpipe) == -1)
                {
                    fprintf(stderr,"%s: pipe error\n",argv[0]);
                    signal(SIGCHLD, sigchld_handler);
                    return FALSE;
                }
                fflush(NULL);
                /*children*/
                if((pid_pipe = fork()) == 0)
                {
                    setpgid(0,0);
                    if(sigprocmask(SIG_UNBLOCK, &mask, NULL) != 0)
                    {
                        fprintf(stderr, 
                        "%s: sigprocmask error\n", argv[0]);
                    }
                    /*redirection*/
                    if(in_redir != 0)
                    {
                        fd_in = open(in_redir, O_RDONLY);
                        if(fd_in == -1)
                        {
                            fprintf(stderr,
                            "%s: No such file or directory\n",argv[0]);
                            for(int i = 0; i < cnt_pipe + 1; i++)
                            {
                                free(cmdset[i]);
                            }
                            free(cmdset);
                            DynArray_map(oTokens, freeToken, NULL);
                            DynArray_free(oTokens);
                            exit(1);
                        }
                        iRet = dup2(fd_in,0);
                        if(iRet == -1)
                        {
                            perror(argv[0]);
                            for(int i = 0; i < cnt_pipe + 1; i++)
                            {
                                free(cmdset[i]);
                            }
                            free(cmdset);
                            DynArray_map(oTokens, freeToken, NULL);
                            DynArray_free(oTokens);
                            exit(1);
                        }
                        iRet = close(fd_in);
                        if(iRet == -1)
                        {
                            perror(argv[0]);
                            for(int i = 0; i < cnt_pipe + 1; i++)
                            {
                                free(cmdset[i]);
                            }
                            free(cmdset);
                            DynArray_map(oTokens, freeToken, NULL);
                            DynArray_free(oTokens);
                            exit(1);
                        }
                    }
                    if(out_redir != 0 && arg == cnt_pipe)
                    {
                        fd_out = creat(out_redir, 0600);
                        if(fd_out == -1)
                        {
                            fprintf(stderr,
                            "%s: creat error\n",argv[0]);
                            for(int i = 0; i < cnt_pipe + 1; i++)
                            {
                                free(cmdset[i]);
                            }
                            free(cmdset);
                            DynArray_map(oTokens, freeToken, NULL);
                            DynArray_free(oTokens);
                            exit(1);
                        }
                        iRet = dup2(fd_out,1);
                        if(iRet == -1)
                        {
                            perror(argv[0]);
                            for(int i = 0; i < cnt_pipe + 1; i++)
                            {
                                free(cmdset[i]);
                            }
                            free(cmdset);
                            DynArray_map(oTokens, freeToken, NULL);
                            DynArray_free(oTokens);
                            exit(1);
                        }
                        iRet = close(fd_out);
                        if(iRet == -1)
                        {
                            perror(argv[0]);
                            for(int i = 0; i < cnt_pipe + 1; i++)
                            {
                                free(cmdset[i]);
                            }
                            free(cmdset);
                            DynArray_map(oTokens, freeToken, NULL);
                            DynArray_free(oTokens);
                            exit(1);
                        }
                    }
                    /*connect with previous pipe*/
                    if(arg > 0)
                    {
                        iRet = dup2(prevfd, 0);
                        if(iRet == -1)
                        {
                            perror(argv[0]);
                            for(int i = 0; i < cnt_pipe + 1; i++)
                            {
                                free(cmdset[i]);
                            }
                            free(cmdset);
                            DynArray_map(oTokens, freeToken, NULL);
                            DynArray_free(oTokens);
                            exit(1);
                        }
                        iRet = close(prevfd);
                        if(iRet == -1)
                        {
                            perror(argv[0]);
                            for(int i = 0; i < cnt_pipe + 1; i++)
                            {
                                free(cmdset[i]);
                            }
                            free(cmdset);
                            DynArray_map(oTokens, freeToken, NULL);
                            DynArray_free(oTokens);
                            exit(1);
                        }
                    }
                    if(arg < cnt_pipe)
                    {
                        iRet = close(fdpipe[0]);
                        if(iRet == -1)
                        {
                            perror(argv[0]);
                            for(int i = 0; i < cnt_pipe + 1; i++)
                            {
                                free(cmdset[i]);
                            }
                            free(cmdset);
                            DynArray_map(oTokens, freeToken, NULL);
                            DynArray_free(oTokens);
                            exit(1);
                        }
                        iRet = dup2(fdpipe[1], 1);
                        if(iRet == -1)
                        {
                            perror(argv[0]);
                            for(int i = 0; i < cnt_pipe + 1; i++)
                            {
                                free(cmdset[i]);
                            }
                            free(cmdset);
                            DynArray_map(oTokens, freeToken, NULL);
                            DynArray_free(oTokens);
                            exit(1);
                        }
                        iRet = close(fdpipe[1]);
                        if(iRet == -1)
                        {
                            perror(argv[0]);
                            for(int i = 0; i < cnt_pipe + 1; i++)
                            {
                                free(cmdset[i]);
                            }
                            free(cmdset);
                            DynArray_map(oTokens, freeToken, NULL);
                            DynArray_free(oTokens);
                            exit(1);
                        }
                    }
                    if((execvp(cmdset[arg][0], cmdset[arg]) < 0))
                    {
                        fprintf(stderr, 
                        "%s: %s: No such file or directory\n", 
                        argv[0], cmdset[arg][0]);
                        for(int i = 0; i < cnt_pipe + 1; i++)
                        {
                            free(cmdset[i]);
                        }
                        free(cmdset);
                        DynArray_map(oTokens, freeToken, NULL);
                        DynArray_free(oTokens);
                        exit(0);
                    }
                    fprintf(stderr, 
                    "%s: %s: execvp error\n", argv[0], cmdset[arg][0]);
                    for(int i = 0; i < cnt_pipe + 1; i++)
                    {
                        free(cmdset[i]);
                    }
                    free(cmdset);
                    DynArray_map(oTokens, freeToken, NULL);
                    DynArray_free(oTokens);
                    exit(0);
                }
                if(pid_pipe < 0)
                {
                    fprintf(stderr, "%s: fork error\n", argv[0]);
                    for(int i = 0; i < cnt_pipe + 1; i++)
                    {
                        free(cmdset[i]);
                    }
                    free(cmdset);
                    return FALSE;
                }
                /*parent*/
                else
                {
                    if(sigprocmask(SIG_UNBLOCK, &mask, NULL) != 0)
                    {
                        fprintf(stderr, 
                        "%s: sigprocmask error\n", argv[0]);
                    }
                    pid_pipe = wait(NULL);
                    iRet = close(fdpipe[1]);
                    if(iRet == -1)
                    {
                        perror(argv[0]);
                        return FALSE;
                    }
                    prevfd = fdpipe[0];
                    arg++;
                }
            }
            /*end of pipe, reinstall handler*/
            pfRet = signal(SIGCHLD, sigchld_handler);
            if(pfRet == SIG_ERR)
            {
                perror(argv[0]);
                exit(1);
            }
            for(int i = 0; i < cnt_pipe + 1; i++)
            {
                free(cmdset[i]);
            }
            free(cmdset);
            return TRUE;
        }
        /* single program */
        else
        {
            if(sigprocmask(SIG_BLOCK, &mask, NULL) != 0)
            {
                fprintf(stderr, "%s: sigprocmask error\n", argv[0]);
            }
            fflush(NULL);
            /*child*/
            if((pid = fork()) == 0)
            {
                setpgid(0,0);
                if(sigprocmask(SIG_UNBLOCK, &mask, NULL) != 0)
                {
                    fprintf(stderr, 
                    "%s: sigprocmask error\n", argv[0]);
                }
                /*redirection*/
                if(is_inredir)
                {
                    fd_in = open(in_redir, O_RDONLY);
                    if(fd_in == -1)
                    {
                        fprintf(stderr,
                        "%s: No such file or directory\n",argv[0]);
                        for(int i = 0; i < cnt_pipe + 1; i++)
                        {
                            free(cmdset[i]);
                        }
                        free(cmdset);
                        DynArray_map(oTokens, freeToken, NULL);
                        DynArray_free(oTokens);
                        exit(1);
                    }
                    iRet = dup2(fd_in,0);
                    if(iRet == -1)
                    {
                        perror(argv[0]);
                        for(int i = 0; i < cnt_pipe + 1; i++)
                        {
                            free(cmdset[i]);
                        }
                        free(cmdset);
                        DynArray_map(oTokens, freeToken, NULL);
                        DynArray_free(oTokens);
                        exit(1);
                    }
                    iRet = close(fd_in);
                    if(iRet == -1)
                    {
                        perror(argv[0]);
                        for(int i = 0; i < cnt_pipe + 1; i++)
                        {
                            free(cmdset[i]);
                        }
                        free(cmdset);
                        DynArray_map(oTokens, freeToken, NULL);
                        DynArray_free(oTokens);
                        exit(1);
                    }
                }
                if(is_outredir)
                {
                    fd_out = creat(out_redir, 0600);
                    if(fd_out == -1)
                    {
                        fprintf(stderr,"%s: creat error\n",argv[0]);
                        for(int i = 0; i < cnt_pipe + 1; i++)
                        {
                            free(cmdset[i]);
                        }
                        free(cmdset);
                        DynArray_map(oTokens, freeToken, NULL);
                        DynArray_free(oTokens);
                        exit(1);
                    }
                    iRet = dup2(fd_out,1);
                    if(iRet == -1)
                    {
                        perror(argv[0]);
                        for(int i = 0; i < cnt_pipe + 1; i++)
                        {
                            free(cmdset[i]);
                        }
                        free(cmdset);
                        DynArray_map(oTokens, freeToken, NULL);
                        DynArray_free(oTokens);
                        exit(1);
                    }
                    iRet = close(fd_out);
                    if(iRet == -1)
                    {
                        perror(argv[0]);
                        for(int i = 0; i < cnt_pipe + 1; i++)
                        {
                            free(cmdset[i]);
                        }
                        free(cmdset);
                        DynArray_map(oTokens, freeToken, NULL);
                        DynArray_free(oTokens);
                        exit(1);
                    }
                }
                if((execvp(cmdset[0][0], cmdset[0]) < 0))
                {
                    fprintf(stderr, 
                            "%s: %s: No such file or directory\n", 
                            argv[0], cmdset[0][0]);
                    for(int i = 0; i < cnt_pipe + 1; i++)
                    {
                        free(cmdset[i]);
                    }
                    free(cmdset);
                    DynArray_map(oTokens, freeToken, NULL);
                    DynArray_free(oTokens);
                    exit(1);
                }
                fprintf(stderr, 
                        "%s: %s: execvp error\n", 
                        argv[0], cmdset[0][0]);
                for(int i = 0; i < cnt_pipe + 1; i++)
                {
                    free(cmdset[i]);
                }
                free(cmdset);
                DynArray_map(oTokens, freeToken, NULL);
                DynArray_free(oTokens);
                exit(1);
            }
            else if(pid < 0)
            {
                if(sigprocmask(SIG_UNBLOCK, &mask, NULL) != 0)
                {
                    fprintf(stderr, 
                            "%s: sigprocmask error\n", argv[0]);
                }
                fprintf(stderr, "%s: fork error\n", argv[0]);
                for(int i = 0; i < cnt_pipe + 1; i++)
                {
                    free(cmdset[i]);
                }
                free(cmdset);
                return FALSE;
            }
            /*parent*/
            else
            {
                if(ground == FG)
                {
                    addjob(jobs, pid, FG, cmdset[0][0], argv);
                    if(sigprocmask(SIG_UNBLOCK, &mask, NULL) != 0)
                    {
                        fprintf(stderr, 
                        "%s: sigprocmask error\n", argv[0]);
                    }
                    waitfg(pid);
                    for(int i = 0; i < cnt_pipe + 1; i++)
                    {
                        free(cmdset[i]);
                    }
                    free(cmdset);   
                    return TRUE;
                }
                else
                {
                    addjob(jobs, pid, BG, cmdset[0][0], argv);
                    if(sigprocmask(SIG_UNBLOCK, &mask, NULL) != 0)
                    {
                        fprintf(stderr, 
                        "%s: sigprocmask error\n", argv[0]);
                    }
                    for(int i = 0; i < cnt_pipe + 1; i++)
                    {
                        free(cmdset[i]);
                    }
                    free(cmdset);  
                    return TRUE;
                }
            }
        }
    }
    /*prevent memory leak*/
    fflush(NULL);
    for(int i = 0; i < cnt_pipe + 1; i++)
    {
        free(cmdset[i]);
    }
    free(cmdset);
    return FALSE;
}
/*--------------------------------------------------------------------*/
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
                char*** cmdset, int cnt_pipe, char **argv)
{
    assert(oTokens);
    assert(command);
    assert(cmdset);
    /*first command*/
    char *cmd = command[0];
    /*exit ish*/
    if(!strcmp(cmd, "exit"))
    {
        for(int i = 0; i < cnt_pipe + 1; i++)
        {
            free(cmdset[i]);
        }
        free(cmdset);
        DynArray_map(oTokens, freeToken, NULL);
        DynArray_free(oTokens);
        exit(0);
        return TRUE;
    }
    /*set environment value*/
    else if(!strcmp(cmd, "setenv"))
    {
        if(cnt_command > 3 || cnt_command < 2 )
        {
            fprintf(stderr, 
                    "%s: setenv takes one or two parameters\n",
                    argv[0]);
            return TRUE;
        }
        if(cnt_command == 2)
        {
            if(setenv(command[1], "\0", 1) == -1)
            {
                fprintf(stderr, "%s: setenv error\n",argv[0]);
                return TRUE;
            }
        }
        else if(cnt_command == 3)
        {
            if(setenv(command[1],command[2],1) == -1)
            {
                fprintf(stderr, "%s: setenv error\n", argv[0]);
                return TRUE;
            }
        }
        return TRUE;
    }
    /*unset environment value*/
    else if(!strcmp(cmd, "unsetenv"))
    {
        if(cnt_command != 2)
        {
            fprintf(stderr, 
                "%s: unsetenv takes one parameters\n",argv[0]);
            return TRUE;
        }
        if(unsetenv(command[1]) == -1)
        {
            fprintf(stderr, "%s: unsetenv error\n",argv[0]);
            return TRUE;
        }
        return TRUE;
    }
    /*change directory*/
    else if(!strcmp(cmd, "cd"))
    {
        if(cnt_command > 3)
        {
            fprintf(stderr, "%s: cd takes one parameters\n",argv[0]);
            return TRUE;
        }
        if(cnt_command == 1)
        {
            char *home = getenv("HOME");
            if(chdir(home) == -1)
            {
                fprintf(stderr, "%s: chdir error\n",argv[0]);
                return TRUE;
            }
        }
        if(cnt_command == 2)
        {
            if(chdir(command[1]) == -1)
            {
                fprintf(stderr, 
                        "%s: %s: No such file or directory\n",
                        argv[0], command[1]);
                return TRUE;
            }
        }
        return TRUE;
    }
    /*foreground*/
    else if(!strcmp(cmd,"fg"))
    {
        do_fg(argv);
        return TRUE;
    }
    return FALSE;
}

/*--------------------------------------------------------------------*/
/*
function waitfg
  wait until foreground program is running
parameter
  pid_t pid - pid for wait
return
  void
*/
void waitfg(pid_t pid) 
{
    struct job_t *jobp = getjobpid(jobs, pid);
    /*pointer of pid's job_t*/
    if(jobp == 0){
        return;
    }
    /*waiting when foreground is running*/
    /*creeping zombie process in SIGCHLD*/
    for(;(jobp -> state == FG);){
        sleep(1);
    }
    return;
}

/*--------------------------------------------------------------------*/
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
void do_fg(char **argv)
{
    /*check exist background process*/
    if(nextjid == 1)
    {
        fprintf(stdout, 
                "%s: There is no background process\n",argv[0]);
        return;
    }
    int max_jid = 0;
    pid_t run_pid = 0;
    /*get recently jid*/
    for(int i = 0; i< MAXJOBS; i++)
    {
        if(jobs[i].jid >= max_jid)
        {
            max_jid = jobs[i].jid;
        }
    }
    /*change state FG*/
    for(int i = 0; i< MAXJOBS; i++)
    {
        if(jobs[i].jid == max_jid)
        {
            run_pid = jobs[i].pid;
            if(jobs[i].state == ST)
            {
                jobs[i].state = FG;
                kill(-jobs[i].pid, SIGCONT);
            }
            if(jobs[i].state == BG)
            {
                jobs[i].state = FG;
                kill(-jobs[i].pid, SIGCONT);
            }
            fprintf(stdout, 
                    "[%d] Latest background process is excuting\n", 
                    run_pid);
        waitfg(jobs[i].pid);
        fprintf(stdout, "[%d]                         Done\n", run_pid);
        return;
        }
    }
    return;
}