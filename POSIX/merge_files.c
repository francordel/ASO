#define _POSIX_C_SOURCE 200809L
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <assert.h>


/* Antonio Martínez Fernández 49973734R TAREA SEMANA 3 */

/* ./merge_files [-t BUFSIZE] [-o FILEOUT] FILEIN1 [FILEIN2 ... FILEINn]*/



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




int write_buffer(char * buff,char * buff_write,int t,int offset,int bufferoffset){ // escribe el siguiente byte del buffer fdin a buffer fdout                                                                                 
                                                                                 // y devuelve el nº de bytes escritos ( 0 || 1)

    int total_written=0;
    

    buff_write[bufferoffset]=buff[offset];


    total_written+=1;

    
    return total_written;

}



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





int main(int argc, char **argv){


    int opt,error_flag=0;
    int fdout;
    char *o = NULL;
    char * buf_wrt;
    int t=1024;


    optind = 1;
    while ((opt = getopt(argc, argv, "t:o:h")) != -1)
    {
        switch (opt)
        {
        case 't':
            t = atoi(optarg);
            
            if (t<1 || t>134217728){ //* en los otros problemas el buffer es de 1048576 como max 

                printf("Error : Tamaño de buffer incorrecto\n");
                error_flag=1;
            }
            
            break;
        case 'o':
            o = optarg;
            break;
        default:
            fprintf(stderr, "Uso: %s [-f] [-n NUMERO] [-s STRING] [PARAMS]\n", argv[0]);
            exit(EXIT_FAILURE);
        }
    }

    

    if(argc==1){
        printf("Error : No hay ficheros de entrada\n");
        error_flag=1;
    }
   

    

    if ((argc-optind )>16){

        printf("Error : Demasiados ficheros de entrada . Máximo 16 ficheros\n");
        error_flag=1;
    }

    

    if (error_flag==1){

        printf("Uso : ./merge_files [-t BUFSIZE] [-o FILEOUT] FILEIN1 [FILEIN2...FILEINn]\n");
        printf("No se admite lectura de la entrada estandar\n");
        printf("-t BUFSIZE tamaño de buffer donde 1<= BUFSIZE <= 128MB\n");
        printf("-o FILEOUT Usa FILEOUT en lugar de salida estandar\n");
        return EXIT_FAILURE;

    }

    
    
    
    if (o != NULL)
    {
        fdout = open(o, O_WRONLY | O_CREAT | O_TRUNC, S_IRUSR | S_IWUSR);
        if (fdout == -1)
        {
            perror("open(fileout)");
            exit(EXIT_FAILURE);
        }
    }
    else {/* Por defecto, la salida estándar */
        fdout = STDOUT_FILENO;
    }




    


    if ((buf_wrt = (char *) malloc(t * sizeof(char))) == NULL)  // buffer reservado
    {
        perror("malloc()");
        exit(EXIT_FAILURE);
    }



    // comprobar que no falle el leer un fichero porque se sigue sin el



    int num_ficheros=argc-optind;
    int no_file=0;

    int list[num_ficheros];     // guardar cuales son validos o no

    for(int i=0;i<num_ficheros;i++){

        list[i]=0;

    }

    int aux=0;
    for (int i = optind; i < argc; i++){
        if( access( argv[i], F_OK ) != 0 ) {
            printf("Error: No se puede abrir %s: 'No such file or directory\n",argv[i]);
            list[aux]=1;
            no_file+=1;
            
        }
        aux++;

    }

    num_ficheros-=no_file; // eliminamos los ficheros que no se pueden abrir

    

    int array[num_ficheros];            // guardan fdin's
    char * array_buff[num_ficheros];    // guarda un buffer por fdin valida

    for (int i=0;i<num_ficheros;i++){

        
        if ((array_buff[i] = (char *) malloc(t * sizeof(char))) == NULL)  // buffer reservado
        {
            perror("malloc() buffers ficheros");
            exit(EXIT_FAILURE);
        }


    }



    
    if (optind < argc)  
    {


        int j=0;
        aux=0;



        for (int i = optind; i < argc; i++){

                if(list[aux]==0){

                    array[j]=open(argv[i], O_RDONLY);  // guardamos en array los fdin
                    j++;
            

                }
                aux++;


            
        
        }    
        

        int offset[num_ficheros];  // array para guardar los offsets de cada buffer fdin
         

        for(int i=0;i<num_ficheros;i++){  /* inicializamos los offsets a 0 */

            offset[i]=0;
        }



        int bytes_buffer_entrada[num_ficheros];  /* CUANTOS BYTES TIENE CADA BUFFER ( sin contar basura)*/




        while (1)
        {

            int check=0; // control de cuantos dejan de leer para saber cuando finalizar
            aux=0;


            /* COMPROBAR SI HEMOS LLEGADO AL FINAL EN LOS N FICHEROS */

            for (int i=0;i<num_ficheros;i++){

                aux=read_buffers(array[i],array_buff[i],t);

                if(aux==0){

                    check+=1;
                    
                }
                bytes_buffer_entrada[i]=aux; // TAMAÑO DE CADA BUFFER

            }

            /* SI SE LLEGÓ AL FINAL DE TODOS LOS ARCHIVOS */

            if(check==num_ficheros){

                for(int i=0;i<num_ficheros;i++){

                    if (close(array[i]) == -1)
                    {
                        perror("close(fdin)");
                        exit(EXIT_FAILURE);
                    }
                }

                break;

            }


            for(int i=0;i<num_ficheros;i++){  /* reinicializamos los offsets si aun queda por escribir algo*/

                if(bytes_buffer_entrada[i]!=0){
                    
                    offset[i]=0;

                }
                
            }
            

            while(1){

                int vaciados=0;
                buf_wrt=(char *) malloc(t * sizeof(char));
                


                for(int i=0;i<num_ficheros;i++){  /* CONTROLAMOS CUANDO SE HA LEIDO TODO DE UN BUFFER*/

                    if (offset[i]==bytes_buffer_entrada[i] || bytes_buffer_entrada[i]==0){


                        vaciados+=1;  
                    }


                }

                if(vaciados==num_ficheros){ // SI TODOS ESTAN VACIOS SALIMOS

                    break;
                }
                

                int buffer_out=0;  // OFFSET DEL BUFFER DE SALIDA


                while(buffer_out<t){         // MERGE + aumenta offset buffer  // SI EL BUFFER ESTA VACIO O TERMINARON DE LEERSE LOS BUFFERS 

                    int check2=0;
                    int aux;

                    for (int i=0;i<num_ficheros;i++){ 


                        if(offset[i]!=bytes_buffer_entrada[i] && bytes_buffer_entrada[i]!=0){

                            aux=write_buffer(array_buff[i],buf_wrt,t,offset[i],buffer_out);

                            if(aux>0){  

                                offset[i]+=aux;
                                buffer_out++;

                            }

                        }
                        else{

                            check2+=1;
                        }


                        
                        
                    }

                    if (check2==num_ficheros){
                        break;
                    }
                }

                

                buffer_to_fdout(buf_wrt,fdout,buffer_out); // ESCRIBIMOS EN FDOUT desde el buffer fdout

            }




        }
        
       
        
    }    
    /*
    if(fdout==STDOUT_FILENO){
        printf("\n");
    }
    */



    /* CLOSE FDOUT */

    if (close(fdout) == -1)
    {
        perror("close(fdout)");
        exit(EXIT_FAILURE);
    }


    /* FREE */


    free(buf_wrt);

    for (int i=0;i<num_ficheros;i++){

        
        free(array_buff[i]);


    }


    return(EXIT_SUCCESS);
}
