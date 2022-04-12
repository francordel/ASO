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
/*      ./merge_mystrings_split [-t BUFSIZE] [-n MINLENGTH] -i  FILEOUT1 [,FILEOUT2 ... FILEOUTn]         */

int main(int argc, char **argv)
{

    int opt, error_flag, flag_i = 0;
    int t = 1024;
    int n = 4;
    char *i;

    optind = 1;

    while ((opt = getopt(argc, argv, "t:n:i:")) != -1)
    {
        switch (opt)
        {
        case 't':
            t = atoi(optarg);

            if (t < 1 || t > 1048576)
            {

                printf("Error: El tamaño de buffer debe ser mayor que 0 y menor que 1048576.\n");
                error_flag = 1;
            }

            break;

        case 'n':

            n = atoi(optarg);
            if (n < 1 || n > 255)
            {

                printf("La longitud mínima de cadena tiene que ser mayor que 0 y menor de 256.\n");
                error_flag = 1;
            }

            break;

        case 'i':

            i = optarg;
            flag_i = 1;  // control de si nos dan un argumento i o no

            break;

        default:

            error_flag = 1;
        }
    }

    int numficheros_out = argc - optind;
    char *fich_out[numficheros_out]; /* FICHEROS SALIDA GUARDADOS EN CADENA*/

    int j = 0;

    for (int i = optind; i < argc; i++)
    {

        fich_out[j] = argv[i];
        j++;
    }

    j = 0;

    if (flag_i == 0) // si no se proporcionan ficheros de entrada
    { 

        printf("Error: Deben proporcionarse ficheros de entrada con la opción -i\n");
        error_flag = 1;
    }

    else if (argc - optind < 1) // si no se proporcionan ficheros de salida
    {

        printf("Error: Debe proporcionarse la lista de ficheros de salida\n");
        error_flag = 1;
    }

    int numficheros_in = 0;

    if (flag_i == 1) // tratar ficheros entrada
    {

        int len = strlen(i);

        for (int j = 0; j < len; j++)
        {

            if (i[j] == 44)
            {

                numficheros_in += 1;
            }
        }

        numficheros_in += 1;
    }

    char *fich_in[numficheros_in]; /* FICHEROS SALIDA GUARDADOS EN CADENA*/

    if (flag_i == 1)
    {

        char *ptrToken;
        char *saveptr;
        ptrToken = strtok_r(i, ",", &saveptr);

        while (ptrToken != NULL)
        {
            fich_in[j] = ptrToken;
            if (access(fich_in[j], F_OK) != 0)
            {
                printf("Error: El fichero %s no existe o no tiene permisos de lectura\n", fich_in[j]);
                return EXIT_FAILURE;
            }

            ptrToken = strtok_r(NULL, ",", &saveptr);
            j += 1;
        }
    }

    if (flag_i == 0 && error_flag != 1)
    {

        printf("Error : Deben proporcionarse ficheros de entrada con la opción -i\n");
        error_flag = 1;
    }

    if (error_flag == 1)
    {

        printf("Uso: ./merge_mystrings_split [-t BUFSIZE] [-n MIN_LENGTH] -i FILEIN1[,FILEIN2,...,FILEINn] FILEOUT1 [FILEOUT2 ...]\n");
        printf("-t BUFSIZE Tamaño de buffer donde 1 <= BUFSIZE <= 1MB (por defecto 1024).\n");
        printf("-n MINLENGTH Longitud mínima de la cadena. Mayor que 0 y menor que 256 (por defecto 4).\n");
        printf("-i Lista de ficheros de entrada separados por comas.\n");
        return EXIT_FAILURE;
    }

    int pipe1[2]; // tuberia 1 y 2
    int pipe2[2];

    if (pipe(pipe1) == -1) // creacion 1a tuberia
    {
        perror(" pipe 1");
        exit(EXIT_FAILURE);
    }

    char *merge_out[4 + numficheros_in];  // array como parametro para exec (merge)
    char t_arg[10];
    char n_arg[10];

    switch (fork())
    {

    case -1:
        perror("fork(1)");
        exit(EXIT_FAILURE);
        break;
    case 0:

        j = 0;

        sprintf(t_arg, "%d", t);

        merge_out[0] = "./merge_files";
        merge_out[1] = "-t";
        merge_out[2] = t_arg;

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

        for (int i = 3; i < numficheros_in + 3; i++)
        {

            merge_out[i] = fich_in[j];
            j++;
        }

        merge_out[3 + numficheros_in] = NULL;

        execvp("./merge_files", merge_out);
        perror("exec merge");

        exit(EXIT_FAILURE);
        break;

    default:
        break;
    }

    if (pipe(pipe2) == -1) // creacion 2a tuberia
    {
        perror(" pipe 1");
        exit(EXIT_FAILURE);
    }

    char *mystrings_out[6]; // array como parametro para exec (mystrings)

    switch (fork())
    {
    case -1:
        perror("fork(2)");
        exit(EXIT_FAILURE);
        break;
    case 0:

        // cerramos el extremo de escritura del receptor en la tuberia 1 , redireccionamos la entrada a la entrada standar y cerramos

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

        sprintf(t_arg, "%d", t);
        sprintf(n_arg, "%d", n);

        mystrings_out[0] = "./mystrings";
        mystrings_out[1] = "t";
        mystrings_out[2] = t_arg;
        mystrings_out[3] = "n";
        mystrings_out[4] = n_arg;
        mystrings_out[5] = NULL;

        execlp("./mystrings", "./mystrings", NULL);
        perror("execlp(mystrings)");
        exit(EXIT_FAILURE);
        break;

    default:
        break;
    }

    // cerramos en el padre

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

    if (wait(NULL) == -1) // esperamos a que acabe merge
    {
        perror("wait(1)");
        exit(EXIT_FAILURE);
    }

    char *split_files[numficheros_out + 4]; // array como parametro para exec (split_files)

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

        sprintf(t_arg, "%d", t);
        j = 0;
        split_files[0] = "./split_files";
        split_files[1] = "-t";
        split_files[2] = t_arg;

        split_files[numficheros_out + 3] = NULL;

        for (int i = 3; i < numficheros_out + 3; i++)
        {

            split_files[i] = fich_out[j];
            j++;
        }

        execvp("./split_files", split_files);

        perror("execlp(split_files)");
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

    // esperamos a los dos últimos hijos

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
