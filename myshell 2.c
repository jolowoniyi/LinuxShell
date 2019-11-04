#include "command_line.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/errno.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>




#define MAX_LINE_LENGTH 512
#define MAXARGS 512

/*
 * TODO: ctrl c kill process but not the whole shell
 * TEAM: Joy Olowoniyi
 */
extern char **environ;


void unix_error(char *msg);
pid_t Fork(void);
int cdHandler(char **argv);
void childHandler(int sig);

void exithandler(int sig){}


int main(int argc, const char **argv)
{
    
    
    
    //char cmdline[MAX_LINE_LENGTH];
    //    struct CommandLine command;
    
    //silently reap children
    struct sigaction sa;
    sa.sa_handler = &childHandler;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = SA_RESTART;
    if (sigaction(SIGCHLD, &sa, 0) == -1)
    {
        perror(0);
        exit(1);
    }
    
    
    
    signal(SIGINT, exithandler);
    //kill foreground processes without killing shell
    
    char cmdline[MAX_LINE_LENGTH];
    struct CommandLine command;
    
    for (;;)
    {
        printf("> ");
        fgets(cmdline, MAX_LINE_LENGTH, stdin);
        
        if (feof(stdin))
        {
            printf("exit");
            exit(0);
        }
        
        bool gotLine = parseLine(&command, cmdline);
        
        //pid_t pid = Fork();
        int stat;
        
        if (gotLine)
        {
            if(command.arguments[0] == NULL ){
                return;
            }
            
            if(!cdHandler(command.arguments))
            {
                
                
                
                
                //child runs user job
                
                
                pid_t pid = Fork();
                
                if (pid  == 0){
                    
                    printCommand(&command);
                    
                    char *com [MAX_ARGS];
                    int i = 0;
                    int j;
                    for(; i < command.argCount; i++){
                        if(strcmp( command.arguments[i], ">") != 0  && strcmp( command.arguments[i], "<") != 0) {
                            com[i] = command.arguments[i];
                            
                            
                        }
                        
                        else if(strcmp(command.arguments[i], ">") == 0) {
                            j = i;
                            char *filename = command.arguments[i+1];
                            printf("-----%s------  /n",filename);
                            int  outputfileptr = open(command.arguments[i+1], O_WRONLY |  O_CREAT);
                            
                            if (outputfileptr == -1)
                                
                            {
                                perror("Cannot create output file!");
                                return;
                            }
                            dup2(outputfileptr, 1);
                            close(outputfileptr);
                            i++;
                        }
                        
                        else if(strcmp( command.arguments[i], "<") == 0) {
                            j = i;
                            char *filename = command.arguments[i+1];
                            int inputfileptr = open(filename, O_RDONLY);
                            
                            if (inputfileptr == -1)
                            {
                                perror("Input file does not exist!");
                                return;
                            }
                            // Run dup2 again to redirect input/output
                            dup2(inputfileptr, 0);
                            close(inputfileptr);
                            i++;
                        }
                        
                    }
                    
                    
                    printf("%s /n", com[0]);
                    
                    if (execvp(command.arguments[0], command.arguments[j-1]) < 0) {
                        
                        
                        printf("There was a Command error \n");
                        exit(0);
                    }
                }
                
                
                
                else{
                    //parent
                    if(!command.background)
                        waitpid(pid, &stat, WUNTRACED);
                    
                    //                            dup2(outputfileptr, 1);
                    //                            close(outputfileptr);
                    
                    
                }
                
            }
            
            
            
            
            
        }
        
        
        
        
        
    }
    
    return;
    
    
    printCommand(&command);
    freeCommand(&command);
    
    
    
}











//error handling for fork method

pid_t Fork(void){
    pid_t pid =0;
    if ((pid = fork()) < 0){
        unix_error("fork error");
    }
    return pid;
}

//prints out message is an error to address error occured

void unix_error(char *msg){
    fprintf(stderr, "%s : %s\n", msg, strerror(errno));
    exit(0);
}


int cdHandler(char **argv){
    if(!strcmp(argv[0], "cd"))
    {
        if(chdir(argv[1]) != 0)
            perror ("cd did not change directory");
        return 1;
    }
    if(!strcmp(argv[0], "exit"))
        exit (0);
    return 0;
}


void childHandler(int sig){
    int ps;
    int saved_errno = errno;
    while (waitpid((pid_t)(-1), &ps, WNOHANG) > 0)
    {
        
        if ( WIFEXITED(ps) )
        {
            printf("Exit status: %d\n", WEXITSTATUS(ps));
        }
        else if(WTERMSIG(ps))
        {
            
            printf("Exit signal: %d\n", WTERMSIG(ps));
        }
    }
    errno = saved_errno;
}
