/* 20190545 / Seungjae Lim / Assign5 / ish.c */
/*--------------------------------------------------------------------*/
/* ish.c                                                              */
/* Main function for ish and define global variable                   */
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
/*User Define File*/
#include "dynarray.h"
#include "lexical.h"
#include "syntatic.h"
#include "scheduler.h"
#include "signal.h"
#include "execute.h"
/*--------------------------------------------------------------------*/
/*Global variable*/
struct job_t jobs[MAXJOBS];
char prompt[] = "% ";
int nextjid = 1;
int is_quit = 0;
/*
struct job_t jobs : job_t array that has information of jobs
                    (for implement foreground, background)
char array prompt : Prompt of Shell input stream (% )
int nextjid : next job id for save in jobs array
int is_quit : 1 : SIGQUIT signal was comed in 5sec
              0 : SIGQUIT signal wasn't comed
*/
/*--------------------------------------------------------------------*/
/*
function main
  main function of ish
parameter
  int argc, char* argv[]
  (argv[0] : ./ish)
return
  int - 0 for success
        otherwise for failure
accessing global variable
  char prompt[] - print each loop
  jobs - initialize array
I/O
   when .ishrc in HOME directory, then read .ishrc and run in HOME
   else read in stdin and run current directory
   print error message on stderr
*/
int main(int argc, char* argv[])
{
    /*install signal handler*/
    void (*pfRet)(int);

    pfRet = signal(SIGCHLD, sigchld_handler);
    if(pfRet == SIG_ERR)
    {
        perror(argv[0]);
        exit(1);
    }
    pfRet = signal(SIGINT, sigint_handler);
    if(pfRet == SIG_ERR)
    {
        perror(argv[0]);
        exit(1);
    }
    pfRet = signal(SIGALRM, sigalrm_handler);
    if(pfRet == SIG_ERR)
    {
        perror(argv[0]);
        exit(1);
    }
    pfRet = signal(SIGQUIT, sigquit_handler);
    if(pfRet == SIG_ERR)
    {
        perror(argv[0]);
        exit(1);
    }

    int iLexSuccessful, iSynSuccessful;
    char cmdline[MAXLINE] = {0,};
    int ground, cnt_pipe;
    DynArray_T oTokens;
    /*
        iLexSuccessful, iSynSuccessfull
            - return value of lexline, synline
        cmdline - input string
        ground - FG : foreground
                 BG : background
                 ST : stopped
        cnt - number of pipe in command
    */

    initjobs(jobs);

    FILE *fp;
    char fp_command[350][MAXLINE] = {0,};
    int fp_cnt = 0;
    char dir[MAXLINE];
    char curdir[MAXLINE];
    char *home = getenv("HOME");
    /*
        fp - file pointer of .ishrc
        fp_command 
            - array of command in .ishrc
        dir - location of ,ishrc
        curdir - current working directory
        home - home directory
    */

    getcwd(curdir,1024);

    /*open .ishrc*/
    strcpy(dir, home);
    strcat(dir,"/.ishrc");
    fp = fopen(dir, "r");
    
    /*if .ishrc exist, save to fp_command*/
    if(fp)
    {
        while((fp != NULL) && (fgets(cmdline, MAXLINE, fp) != NULL))
        {
            strcpy(fp_command[fp_cnt], cmdline);
            fp_cnt++;
        }
        fclose(fp);
    }
    /*do command for saved in fp_command*/
    chdir(home);
    for(int k = 0; k < fp_cnt; k++)
    {
        fprintf(stdout, "%s",prompt);
        fflush(NULL);
        /*preprocessing each line of fp_command*/
        int len_cmd = strlen(fp_command[k]);
        while(fp_command[k][len_cmd] == ' ')
        {
            fp_command[k][len_cmd] = '\0';
            len_cmd--;
        }
        if(!strcmp(fp_command[k], "\n"))
        {
            fprintf(stdout, "\n");
            fflush(NULL);
            continue;
        }
        if(!strchr(fp_command[k], '\n'))
        {
            fprintf(stdout, "%s\n",fp_command[k]);
            fflush(NULL);
        }
        else
        {
            fprintf(stdout, "%s",fp_command[k]);
            fflush(NULL);
        }
        /*do : lexical -> syntatic -> excute*/
        oTokens = DynArray_new(0);
        if (oTokens == NULL)
        {
            fprintf(stderr, "%s: Cannot allocate memory\n",argv[0]);
            exit(EXIT_FAILURE);
        }
        iLexSuccessful = lexLine(fp_command[k], oTokens, argv);
        if (!iLexSuccessful)
        {   
            DynArray_map(oTokens, freeToken, NULL);
            DynArray_free(oTokens);
            fflush(NULL);
            continue;
        }
        ground = iLexSuccessful;
        iSynSuccessful = synLine(oTokens, &ground, argv);
        if (iSynSuccessful < 0)
        {
            DynArray_map(oTokens, freeToken, NULL);
            DynArray_free(oTokens);
            fflush(NULL);
            continue;
        }
        cnt_pipe = iSynSuccessful;
        execute(oTokens, ground, cnt_pipe, argv);
        fflush(NULL);
        DynArray_map(oTokens, freeToken, NULL);
        DynArray_free(oTokens);
        for(int i = 0; i < MAXLINE; i++)
        {
            fp_command[k][i] = 0;
        }
        fflush(NULL);
    }
    fflush(NULL);
    /*end of .ishrc
      move to currend directory and run*/
    chdir(curdir);
    fprintf(stdout, "%s", prompt);
    fflush(stdout);
    /*get cmdline from stdin*/
    while(fgets(cmdline, MAXLINE, stdin) != NULL)
    {
        if(!strcmp(cmdline, "\n"))
        {
            fprintf(stdout,"%s",prompt);
            fflush(NULL);
            continue;
        }
        /*do : lexical -> syntatic -> excute*/
        oTokens = DynArray_new(0);
        if (oTokens == NULL)
        {
            fprintf(stderr, "./ish : Cannot allocate memory\n");
            exit(EXIT_FAILURE);
        }
        iLexSuccessful = lexLine(cmdline, oTokens, argv);
        if (!iLexSuccessful)
        {   
            DynArray_map(oTokens, freeToken, NULL);
            DynArray_free(oTokens);
            fprintf(stdout,"%s",prompt);
            fflush(NULL);
            continue;
        }
        ground = iLexSuccessful;
        iSynSuccessful = synLine(oTokens, &ground, argv);
        if (iSynSuccessful < 0)
        {
            DynArray_map(oTokens, freeToken, NULL);
            DynArray_free(oTokens);
            fprintf(stdout,"%s",prompt);
            fflush(NULL);
            continue;
        }
        cnt_pipe = iSynSuccessful;
        execute(oTokens, ground, cnt_pipe, argv);
        DynArray_map(oTokens, freeToken, NULL);
        DynArray_free(oTokens);
        fprintf(stdout,"%s",prompt);
        fflush(NULL);
    }
    fprintf(stdout,"\n");
    fflush(NULL);
    exit(0);
    return 0;
}