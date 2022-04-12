#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <ctype.h>

//ps aux | grep root | grep sbin

int main(int argc, char **argv){


    pid_t pid;
    int pipe1[2];
    int pipe2[2];

    if (pipe(pipe1) == -1)
    {
        perror(" pipe 1");
        exit(EXIT_FAILURE);
        
    }


    switch (pid=fork())
    {

    case -1:
        perror("fork(1)");
        exit(EXIT_FAILURE);
        break;
    case 0:
        
        if (close(pipe1[0]) == -1)
        {
            perror("close(1)");
            exit(EXIT_FAILURE);
        }

        if (dup2(pipe1[1], STDOUT_FILENO) == -1)
        {
            perror("dup2(1)");
            exit(EXIT_FAILURE);
        }

        if (close(pipe1[1]) == -1)
        {
            perror("close(2)");
            exit(EXIT_FAILURE);
        }

         
        execlp("./merge_files","./merge_files","t","1024","fi1","fi2","fi3",NULL);
        //execlp("./merge_files","./merge_files","fi1","fi2","fi3",NULL);
        //execlp("ps", "ps", "aux", NULL);
        perror("exec merge");
        
        exit(EXIT_FAILURE);
        break;
    
    default:
        break;
    }



    if (pipe(pipe2) == -1)
    {
        perror(" pipe 1");
        exit(EXIT_FAILURE);
        
    }



    switch (fork())
    {
    case -1:
        perror("fork(2)");
        exit(EXIT_FAILURE);
        break;
    case 0: 
        

        if (close(pipe1[1]) == -1)
        {
            perror("close(3)");
            exit(EXIT_FAILURE);
        }


        if (dup2(pipe1[0], STDIN_FILENO) == -1)
        {
            perror("dup2(2)");
            exit(EXIT_FAILURE);
        }


        if (close(pipe1[0]) == -1)
        {
            perror("close(4)");
            exit(EXIT_FAILURE);
        }


        //* pipe 2



        if (close(pipe2[0]) == -1)
        {
            perror("close(pipe2 salida)");
            exit(EXIT_FAILURE);
        }

        if (dup2(pipe2[1], STDOUT_FILENO) == -1)
        {
            perror("dup2(pipe2 salida)");
            exit(EXIT_FAILURE);
        }

        if (close(pipe2[1]) == -1)
        {
            perror("close(pipe2 salida)");
            exit(EXIT_FAILURE);
        }

        execlp("./mystrings","./mystrings","t","1024","n","4",NULL);
        //execlp("grep", "grep", "root", NULL);
        perror("execlp(my strings)");
        exit(EXIT_FAILURE);
        break;
    default: 
        break;
    
    }
  
    if (close(pipe1[0]) == -1)
    {
        perror("close(pipefds[0])");
        exit(EXIT_FAILURE);
    }


    if (close(pipe1[1]) == -1)
    {
        perror("close(pipefds[1])");
        exit(EXIT_FAILURE);
    }

    if (wait(NULL) == -1)
    {
        perror("wait(1)");
        exit(EXIT_FAILURE);
    }

        
    switch (fork())
    {
    case -1:
        perror("fork( pipe 2 entrada)");
        exit(EXIT_FAILURE);
        break;
    case 0:

        if (close(pipe2[1]) == -1)
        {
            perror("close( pipe 2 entrada)");
            exit(EXIT_FAILURE);
        }

        if (dup2(pipe2[0], STDIN_FILENO) == -1)
        {
            perror("dup2(pipe 2 entrada)");
            exit(EXIT_FAILURE);
        }

        if (close(pipe2[0]) == -1)
        {
            perror("close( pipe 2 entrada)");
            exit(EXIT_FAILURE);
        }

        execlp("./split_files","./split_files","t","1024","fo1","fo2","fo3",NULL);
        //execlp("grep", "grep", "sbin", NULL);
        perror("execlp(derecho)");
        exit(EXIT_FAILURE);
        break;
    default: 
        break;

    }


    if (close(pipe2[0]) == -1)
    {
        perror("close(pipefds[0])");
        exit(EXIT_FAILURE);
    }


    if (close(pipe2[1]) == -1)
    {
        perror("close(pipefds[1])");
        exit(EXIT_FAILURE);
    }



    
    if (wait(NULL) == -1)
    {
        perror("wait(2)");
        exit(EXIT_FAILURE);
    }

    
    if (wait(NULL) == -1)
    {
        perror("wait(3)");
        exit(EXIT_FAILURE);
    }

           
    


    return EXIT_SUCCESS;











    








}