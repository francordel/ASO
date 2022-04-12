#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>

/* Alumno : Antonio Martínez Fernández GROUP 3º  2.3 */

// programa -v visor imagen .. .. imagen n
// 0         1   2    3              n


int main(int argc, char **argv){
    
    if (argc==1){
        perror("Uso: ./openimg -v VISOR [IMGs]");
        return EXIT_FAILURE;
    }
    
    if(argc <4){

        perror("No hay imagenes que visualizar"); 
        return EXIT_FAILURE;
    }
    
    int n_imgs=argc-3;
    int array[n_imgs];
    int status;

    
    int pid=1;

    for (int i=0 ;i<n_imgs;i++){

        if (pid>0){

            pid=fork();

        }

        if (pid>0){


            array[i]=pid;
        }

        else if(pid==0){

            execlp(argv[2],argv[2],argv[i+3],NULL);

            fprintf(stderr, "the binary doesnt exists ; exec() failed\n");
            return EXIT_FAILURE;

        }
            
        

    }

    if (pid>0){

        for(int i=0 ;i<n_imgs;i++){

            waitpid(array[i],&status,WUNTRACED);

            if (WIFEXITED(status)){

                printf ("%s IMG: %s, status= %d\n",argv[0],argv[3+i],WEXITSTATUS ( status ) ) ;             
            }

        }

    }
      
    
    return EXIT_SUCCESS;
}


