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
/*      ./mystrings [-t BUFSIZE] [-n MINLENGTH]         */


int t=1024;             /* tamaño buffer */
int n=4;               /* tamaño minimo cadena */

int actual_length=0; /* Longitud caracteres printeables actual */

int offset_out=0;
int offset_in=0;



int read_buffers(int fdin, char *buf, unsigned buf_size)  // ->  devuelve numero leido en el buffer 
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

    return total;   // devuelve lo que no se ha podido escribir

    
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


void tratar_cadena( char * buf_in,char * buf_out,int num_readed){

    int added=0;

    for(int i=0;i<num_readed;i++){
        //printf("letra = %c y num %d\n",buf_in[i],buf_in[i]);
        if(isprint(buf_in[i])||buf_in[i]==10||buf_in[i]==9||buf_in[i]==32){

            actual_length+=1;
            buf_out[offset_out]=buf_in[i];
            offset_out+=1;
            added+=1;

        }

        else{

            if(actual_length>=n){

                buffer_to_fdout(buf_out,STDOUT_FILENO,offset_out);
            }
            offset_out=0;
            actual_length=0;

        }

        //printf("buffout = %s\n",buf_out);


    }

    if(actual_length>=n){

        buffer_to_fdout(buf_out,STDOUT_FILENO,offset_out);

    }




}



int main(int argc, char **argv){

    int opt,error_flag=0;

    optind = 1;

 while ((opt = getopt(argc, argv, "t:n:")) != -1)
    {
        switch (opt)
        {
        case 't':
            t = atoi(optarg);
            
            if (t<n || t>1048576){

                printf("Error : Tamaño de buffer incorrecto.\n");
                error_flag=1;
            }
            
            break;
        case 'n':
    
            n = atoi(optarg);
            if (n<1 || n>255){

                printf("La longitud mínima de cadena tiene que ser mayor que 0 y menor de 256.\n");
                error_flag=1;
            }
            
            break;
        default:
            //fprintf(stderr, "Uso: %s [-t BUFSIZE] [-n MINLENGTH]\n", argv[0]);
            error_flag=1;
            
        }
    }

    int fdin=open(argv[optind],O_RDONLY);


    if (error_flag==1){

        printf("Uso: ./mystrings [-t BUFSIZE] [-n MINLENGTH]\nLee de la entrada estándar el flujo de bytes recibido y escribe en la\nsalida estándar\nlas cadenas compuestas por caracteres imprimibles incluyendo espacios,\ntabuladores y\nsaltos de línea, que tengan una longitud mayor o igual a un tamaño dado.\n-t BUFSIZE Tamaño de buffer donde MINLENGTH <= BUFSIZE <= 1MB (por\ndefecto 1024).\n-n MINLENGTH Longitud mínima de la cadena. Mayor que 0 y menor que 256\n(por defecto 4).\n");

        return EXIT_FAILURE;

    }


    char * buf_in=malloc(sizeof(char)*t);
    char * buf_out=malloc(sizeof(char)*t);


    int num_readed;
    int added_bytes=0;


    while((num_readed=read_buffers(STDIN_FILENO,buf_in,t))>0){  // lee tmb \n
    //while((num_readed=read_buffers(fdin,buf_in,t))>0){

        //printf("bufin %s\n",buf_in);

        tratar_cadena(buf_in,buf_out,num_readed); // que devuelva los numeros metidos en el buffer

    

    }



    

    free(buf_in);
    free(buf_out);

    close(fdin);
    //printf("\n");

    return EXIT_SUCCESS;
}