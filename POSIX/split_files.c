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

/* Antonio Martínez Fernández 49973734R TAREA SEMANA 4*/
/*      ./split_files [-t BUFSIZE] FILEOUT1 [FILEOUT2 ... FILEOUTn]         */


int read_buffers(int fdin, char *buf, unsigned buf_size)  
{
    ssize_t num_written,num_left=buf_size;
    int total=0;
    char * buf_left=buf;

    while((num_left>0) && (num_written=read(fdin,buf_left,num_left))>0){
        total+=num_written;
        buf_left+=num_written;
        num_left-=num_written;
    }

    if(num_written == -1){
        perror("write(fdin)");
        exit(EXIT_FAILURE);
    }

    return total;   // ->  devuelve numero de bytes leido en el buffer 
    
}



void write_in_buffer(char * buf_in,char * buff_out,int offset_out,int offset_in){

    buff_out[offset_out]=buf_in[offset_in];

}



void buffer_to_fdout(char * buf,int fdout,unsigned buf_size){ // hace escritura total de un buffer a la salida 

    ssize_t num_written,num_left=buf_size;
    int total=0;
    char * buf_left=buf;
    while((num_left>0) && (num_written=write(fdout,buf_left,num_left))>0){
        total+=num_written;
        buf_left+=num_written;
        num_left-=num_written;
    }

    if(num_written == -1){
        perror("write(fdin)");
        exit(EXIT_FAILURE);
    }

    

}





//* -----------------------------------------------------


int main(int argc, char **argv){

    int opt,error_flag=0;
    int t=1024;

    optind = 1;



 while ((opt = getopt(argc, argv, "t:")) != -1)
    {
        switch (opt)
        {
        case 't':
            t = atoi(optarg);
            
            if (t<1 || t>1048576){

                printf("Error : Tamaño de buffer incorrecto.\n");
                error_flag=1;
            }
            
            break;
            
        default:
            //fprintf(stderr, "Uso: ./split_files [-t BUFSIZE] FILEOUT1 [FILEOUT2 ... FILEOUTn]\n");
            error_flag=1;
            
        }
    }



    if(argc-optind<1 && error_flag!=1){

        printf("Error: No hay ficheros de salida\n.");
        error_flag=1;
        
    }


    else if(argc-optind>16 && error_flag!=1){

        printf("Error: Hay demasiados ficheros de salida\n.");
        error_flag=1;
        
    }




    if(error_flag==1){

        printf("Uso: ./split_files [-t BUFSIZE] FILEOUT1 [FILEOUT2 ... FILEOUTn]\n");
        printf("Divide en ficheros el flujo de bytes que recibe por la entrada estandar\n");
        printf("El numero maximo de de ficheros de salida es 16.\n");
        printf("-t BUFSIZE Tamaño de buffer donde 1 <= BUFSIZE <= 1MB (por defecto 1024).\n");
        return EXIT_FAILURE;

    }






    int num_ficheros=argc-optind;
    int j=0;
    int num_readed=0;




    /* fd y buffers salida */

    int fdout[num_ficheros];
    char * buff_out[num_ficheros];
    int offset_out[num_ficheros];
 

    for(int i=optind;i<argc;i++){  // abrimos los buffers de salida y sus respectivos buffers

        offset_out[j]=0;
        fdout[j]=open(argv[i],O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        

        if (fdout[j] == -1)
        {
            perror("open(fileout)");
            exit(EXIT_FAILURE);
        }
        if ((buff_out[j] = (char *) malloc(t * sizeof(char))) == NULL)  // buffer reservado
        {
            perror("malloc() buff out");
            exit(EXIT_FAILURE);
        }
        j++;


    }

    j=0;

    /* fd y buffer entrada */

    char * buf_in;
    int offset_in=0;

    if ((buf_in = (char *) malloc(t * sizeof(char))) == NULL){  // buffer reservado
            perror("malloc() buff in");
            exit(EXIT_FAILURE);
    }
    
    


    //int fdin=open("prueba.txt",O_RDONLY);

    int current_file=0;
    

    

    //while((num_readed=read_buffers(fdin,buf_in,t))>0){
    while((num_readed=read_buffers(STDIN_FILENO,buf_in,t))>0){

        offset_in=0;
        
        // para cada fichero darle un byte a su buffer

        while(offset_in<num_readed){


            for(int i=0;i<num_ficheros;i++){

                if(offset_in>=num_readed){
                    break;
                }

                if(current_file==i){

                    write_in_buffer(buf_in,buff_out[i],offset_out[i],offset_in);
                    offset_out[i]+=1;
                    offset_in+=1;
                    current_file+=1;


                }

                if(current_file==num_ficheros){

                    current_file=0;
                }
                

            }

        }

        for(int i=0;i<num_ficheros;i++){

            //printf("buff %d = %s\n",i,buff_out[i]);

            if(offset_out[i]==t || num_readed<t){ // escribimos si se llena o son los ultimos bytes
                //printf(" buff out %d = %s\n",i,buff_out[i]);
                buffer_to_fdout(buff_out[i],fdout[i],offset_out[i]);
                offset_out[i]=0;
            }

        
        }

        // cuando el offset de un buffer este lleno printeamos y reseteamos offset buffer
    
    
    
    }



    /* free buffs and close fd */

    free(buf_in);

    for(int i=0;i<num_ficheros;i++){


        free(buff_out[i]);
        if (close(fdout[i]) == -1)
        {
            perror("close(fdout)");
            exit(EXIT_FAILURE);
        }
    }


    //close(fdin);


    return EXIT_SUCCESS;

}



