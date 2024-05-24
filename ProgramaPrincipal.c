//Librerías
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <dirent.h>
#include <string.h>
#include <ctype.h>
#include <pthread.h>
#include <time.h>
#include <semaphore.h> 

// Inclusión condicional de librerías específicas del sistema operativo para compatibilidad. Se ejecuta lo primero para comprobar su compatibilidad
#if defined(_WIN32)
    #include <windows.h>
#elif defined(_WIN64)
    #include <windows.h>
#elif defined(__CYGWIN__) && !defined(_WIN32)
    #include <windows.h>
#else
    #include <sys/param.h>
    #if defined(BSD)
       #include <unistd.h>
    #endif
#endif

sem_t sem; // Declaramos un semáforo para controlar el acceso a recuersos compartidos

typedef struct {
    int NUM_PROCESOS;
    int SIMULATE_SLEEP;
    char INVENTORY_FILE[20];
    char LOG_FILE[20];
    char PATH_FILES[45];
    int SU001;
    int SU002;
    int SU003;
    int SU004;
} ArgumentosHilosConfiguracion;

// Definición de constantes
#define MAX_CARACTER 150 // Para manejar el tamaño máximo de caracteres en operaciones de cadena.

// Declaración de las funcionnes
void BorrarPantalla();
void PulsaContinuar();

// Declaración de los hilos
void* funcion_hilo1(void *arg);
void* funcion_hilo2(void *arg);
void* funcion_hilo3(void *arg);
void* funcion_hilo4(void *arg);
void* funcion_hilo5(void *arg);


int main() {
    BorrarPantalla();

    // Declaración de variables
    char* token;
    int FinFich = 0, i = 0;
    FILE *archivo_config; // Puntero a archivo para leer las configuraciones
    char* cpToken;
    char Cadena[MAX_CARACTER];
    char CadenaLeida[MAX_CARACTER] = "";

    ArgumentosHilosConfiguracion args; //Creamos una estructura para almacenar los valores del archivo de configuración

    //Abrimos el archivo de configuración
    archivo_config = fopen("/Users/alvaro/fp.txt", "r"); //Cambiar ruta
    //Si no se puede abrir el archivo se mostrará un mensaje por pantalla
    if (archivo_config == NULL) {
        // Control de errores
        printf("No se puede abrir el archivo de configuración.\n");
    }
    else{
        while (!FinFich){
          Cadena[0]='\0';
          fgets(Cadena, MAX_CARACTER, archivo_config);

          if (Cadena[strlen (Cadena )-1]=='\n')
            Cadena[strlen (Cadena )-1]= '\0';

          cpToken = strtok (Cadena, "=");

          while (cpToken!=NULL){
            cpToken = strtok (NULL, "=");
            strcat(CadenaLeida, cpToken);
            if(i != 4)
                strcat(CadenaLeida,";" );
            cpToken = strtok (NULL, "=");
          }
          FinFich=feof(archivo_config);
          i++;

        }
        fclose(archivo_config); //Se cierra el fichero de texto
        
        // Asignación de configuraciones a la estructura args.
        token = strtok(CadenaLeida, ";"); // Obtener el primer token
        strcpy (args.PATH_FILES, token);
        token = strtok(NULL, ";"); // Obtener el siguiente token
        strcpy(args.INVENTORY_FILE,token );
        token = strtok(NULL, ";"); // Obtener el siguiente token
        strcpy(args.LOG_FILE, token);
        token = strtok(NULL, ";"); // Obtener el siguiente token
        args.NUM_PROCESOS = atoi(token);
        token = strtok(NULL, ";"); // Obtener el siguiente token
        args.SIMULATE_SLEEP = atoi(token);
        token = strtok(NULL, ";"); // Obtener el siguiente token
    }


    // Inicialización y lanzamiento de huilos
    pthread_t hilos[args.NUM_PROCESOS];

    pthread_create(&hilos[0], NULL, funcion_hilo1, &args); 
    pthread_create(&hilos[1], NULL, funcion_hilo2, &args);
    pthread_create(&hilos[2], NULL, funcion_hilo3, &args);
    pthread_create(&hilos[3], NULL, funcion_hilo4, &args);
    pthread_create(&hilos[4], NULL, funcion_hilo5, &args);
   
    // Espera a que terminen todos los hilos
    for (int i = 0; i < args.NUM_PROCESOS; i++) {
        pthread_join(hilos[i], NULL);
    }
    
    return 0;
}

void* funcion_hilo1(void *arg) {
    time_t tiempo = time(NULL);
    struct tm *tlocal = localtime(&tiempo);

    // Definición de variables
    ArgumentosHilosConfiguracion *args = (ArgumentosHilosConfiguracion*)arg;
    DIR *dir;
    struct dirent *ent;
    
    char ruta_archivo[1024];
    char buffer[1024];
    FILE *ARCHIVO_CONSOLIDADO;
    FILE *ARCHIVO_LOG;
    FILE *en_archivo;
    int status;

    char fecha[20] = "";
    char fechaFin[20] = "";

    strftime(fecha, sizeof(fecha), "%d%m%Y:::%H%M", tlocal);
    
    while (1) {
        dir = opendir(args->PATH_FILES); // Abrir la carpeta
        if (dir == NULL) {
            // Control de errores
            printf("Error al abrir el directorio común\n");
        }
        else{
           
            ARCHIVO_CONSOLIDADO = fopen(args->INVENTORY_FILE, "a");

            if (ARCHIVO_CONSOLIDADO == NULL) {
                // Control de errores
                printf("No se puede abrir el archivo consolidado.\n");
            }
            else{
                // Leer los contenidos de la carpeta
                while ((ent = readdir(dir)) != NULL) {
                    if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                        continue;
                    }
                    if(ent->d_name[4] == '1'){
                        sprintf(ruta_archivo, "%s/%s", args->PATH_FILES, ent->d_name);
                        printf("\n");

                        // Abre el archivo
                        en_archivo = fopen(ruta_archivo, "r");

                        // Control de errores
                        if (en_archivo == NULL) {
                            // Control de errores
                            printf("No se puede abrir el archivo %s.\n", args->PATH_FILES);
                        }
                        else{
                            args->SU001++;
                            while (fgets(buffer, 1024, en_archivo) != NULL) {
                                fputs(buffer, ARCHIVO_CONSOLIDADO);
                            }

                            //Escritura en el log
                            ARCHIVO_LOG = fopen(args->LOG_FILE, "a");
                            if (ARCHIVO_LOG == NULL) {
                                // Control de errores
                                printf("No se puede abrir el archivo log.\n");
                            }
                            else{
                                strftime(fechaFin, sizeof(fechaFin), "%d%m%Y:::%H%M", tlocal);
                                fprintf(ARCHIVO_LOG, "%s:::%lu:::%s:::%s:::%d\n", fecha, (unsigned long)pthread_self(), fechaFin, ent->d_name, args->SU001 + args->SU002 + args->SU003 + args->SU004);
                                printf("%s:::%lu:::%s:::%s:::%d\n", fecha, (unsigned long)pthread_self(), fechaFin, ent->d_name, args->SU001 + args->SU002 + args->SU003 + args->SU004);
                            }

                            status = remove(ruta_archivo);
                            
                        }
                    }
                }
            }   
        }
        closedir(dir); // Cerrar la carpeta
        fclose(ARCHIVO_CONSOLIDADO);
        fclose(ARCHIVO_LOG);
        sleep(args->SIMULATE_SLEEP);
    }
    pthread_exit(NULL);
}

void* funcion_hilo2(void *arg) {
    time_t tiempo = time(NULL);
    struct tm *tlocal = localtime(&tiempo);

    // Definición de variables
    ArgumentosHilosConfiguracion *args = (ArgumentosHilosConfiguracion*)arg;
    DIR *dir;
    struct dirent *ent;
    
    char ruta_archivo[1024];
    char buffer[1024];
    FILE *ARCHIVO_CONSOLIDADO;
    FILE *ARCHIVO_LOG;
    FILE *en_archivo;
    int status;

    char fecha[20] = "";
    char fechaFin[20] = "";

    strftime(fecha, sizeof(fecha), "%d%m%Y:::%H%M", tlocal);
    
    while (1) {
        dir = opendir(args->PATH_FILES); // Abrir la carpeta
        if (dir == NULL) {
            // Control de errores
            printf("Error al abrir la carpeta\n");
        }
        else{
            ARCHIVO_CONSOLIDADO = fopen(args->INVENTORY_FILE, "a");

            if (ARCHIVO_CONSOLIDADO == NULL) {
                // Control de errores
                printf("No se puede abrir el archivo consolidado.\n");
            }
            else{
                // Leer los contenidos de la carpeta
                while ((ent = readdir(dir)) != NULL) {
                    if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                        continue;
                    }
                    if(ent->d_name[4] == '2'){
                        sprintf(ruta_archivo, "%s/%s", args->PATH_FILES, ent->d_name);
                        printf("\n");

                        // Abre el archivo
                        en_archivo = fopen(ruta_archivo, "r");

                        // Control de errores
                        if (en_archivo == NULL) {
                            // Control de errores
                            printf("No se puede abrir el archivo %s.\n", args->PATH_FILES);
                        }
                        else{
                            args->SU001++;
                            while (fgets(buffer, 1024, en_archivo) != NULL) {
                                fputs(buffer, ARCHIVO_CONSOLIDADO);
                            }

                            //Escritura en el log
                            ARCHIVO_LOG = fopen(args->LOG_FILE, "a");
                            if (ARCHIVO_LOG == NULL) {
                                // Control de errores
                                printf("No se puede abrir el archivo log.\n");
                            }
                            else{
                                strftime(fechaFin, sizeof(fechaFin), "%d%m%Y:::%H%M", tlocal);
                                fprintf(ARCHIVO_LOG, "%s:::%lu:::%s:::%s:::%d\n", fecha, (unsigned long)pthread_self(), fechaFin, ent->d_name, args->SU001 + args->SU002 + args->SU003 + args->SU004);
                                printf("%s:::%lu:::%s:::%s:::%d\n", fecha, (unsigned long)pthread_self(), fechaFin, ent->d_name, args->SU001 + args->SU002 + args->SU003 + args->SU004);
                            }

                            status = remove(ruta_archivo);
                        }
                    }
                }
            }   
        }
        closedir(dir); // Cerrar la carpeta
        fclose(ARCHIVO_CONSOLIDADO);
        fclose(ARCHIVO_LOG);
        sleep(args->SIMULATE_SLEEP);
    }
    pthread_exit(NULL);
}

void* funcion_hilo3(void *arg) {
    time_t tiempo = time(NULL);
    struct tm *tlocal = localtime(&tiempo);

    // Definición de variables
    ArgumentosHilosConfiguracion *args = (ArgumentosHilosConfiguracion*)arg;
    DIR *dir;
    struct dirent *ent;
    
    char ruta_archivo[1024];
    char buffer[1024];
    FILE *ARCHIVO_CONSOLIDADO;
    FILE *ARCHIVO_LOG;
    FILE *en_archivo;
    int status;

    char fecha[20] = "";
    char fechaFin[20] = "";

    strftime(fecha, sizeof(fecha), "%d%m%Y:::%H%M", tlocal);
    
    while (1) {
        dir = opendir(args->PATH_FILES); // Abrir la carpeta
        if (dir == NULL) {
            // Control de errores
            printf("Error al abrir la carpeta\n");
        }
        else{
            ARCHIVO_CONSOLIDADO = fopen(args->INVENTORY_FILE, "a");

            // Control de errores
            if (ARCHIVO_CONSOLIDADO == NULL) {
                // Control de errores
                printf("No se puede abrir el archivo consolidado.\n");
            }
            else{
                // Leer los contenidos de la carpeta
                while ((ent = readdir(dir)) != NULL) {
                    if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                        continue;
                    }
                    if(ent->d_name[4] == '3'){
                        sprintf(ruta_archivo, "%s/%s", args->PATH_FILES, ent->d_name);
                        printf("\n");

                        // Abre el archivo
                        en_archivo = fopen(ruta_archivo, "r");

                        if (en_archivo == NULL) {
                            // Control de errores
                            printf("No se puede abrir el archivo %s.\n", args->PATH_FILES);
                        }
                        else{
                            args->SU001++;
                            while (fgets(buffer, 1024, en_archivo) != NULL) {
                                fputs(buffer, ARCHIVO_CONSOLIDADO);
                            }

                            //Escritura en el log
                            ARCHIVO_LOG = fopen(args->LOG_FILE, "a");
                            if (ARCHIVO_LOG == NULL) {
                                // Control de errores
                                printf("No se puede abrir el archivo log.\n");
                            }
                            else{
                                strftime(fechaFin, sizeof(fechaFin), "%d%m%Y:::%H%M", tlocal);
                                fprintf(ARCHIVO_LOG, "%s:::%lu:::%s:::%s:::%d\n", fecha, (unsigned long)pthread_self(), fechaFin, ent->d_name, args->SU001 + args->SU002 + args->SU003 + args->SU004);
                                printf("%s:::%lu:::%s:::%s:::%d\n", fecha, (unsigned long)pthread_self(), fechaFin, ent->d_name, args->SU001 + args->SU002 + args->SU003 + args->SU004);
                            }

                            status = remove(ruta_archivo);
                        }
                    }
                }
            }   
        }
        closedir(dir); // Cerrar la carpeta
        fclose(ARCHIVO_CONSOLIDADO);
        fclose(ARCHIVO_LOG);
        sleep(args->SIMULATE_SLEEP);
    }
    pthread_exit(NULL);
}

void* funcion_hilo4(void *arg) {
    time_t tiempo = time(NULL);
    struct tm *tlocal = localtime(&tiempo);

    // Definición de variables
    ArgumentosHilosConfiguracion *args = (ArgumentosHilosConfiguracion*)arg;
    DIR *dir;
    struct dirent *ent;
    
    char ruta_archivo[1024];
    char buffer[1024];
    FILE *ARCHIVO_CONSOLIDADO;
    FILE *ARCHIVO_LOG;
    FILE *en_archivo;
    int status;

    char fecha[20] = "";
    char fechaFin[20] = "";

    strftime(fecha, sizeof(fecha), "%d%m%Y:::%H%M", tlocal);
    
    while (1) {
        dir = opendir(args->PATH_FILES); // Abrir la carpeta
        if (dir == NULL) {
            // Control de errores
            printf("Error al abrir la carpeta\n");
        }
        else{
            ARCHIVO_CONSOLIDADO = fopen(args->INVENTORY_FILE, "a");

            // Control de errores
            if (ARCHIVO_CONSOLIDADO == NULL) {
                // Control de errores
                printf("No se puede abrir el archivo consolidado.\n");
            }
            else{
                // Leer los contenidos de la carpeta
                while ((ent = readdir(dir)) != NULL) {
                    if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
                        continue;
                    }
                    if(ent->d_name[4] == '4'){
                        sprintf(ruta_archivo, "%s/%s", args->PATH_FILES, ent->d_name);
                        printf("\n");

                        // Abre el archivo
                        en_archivo = fopen(ruta_archivo, "r");

                        if (en_archivo == NULL) {
                            // Control de errores
                            printf("No se puede abrir el archivo %s.\n", args->PATH_FILES);
                        }
                        else{
                            args->SU001++;
                            while (fgets(buffer, 1024, en_archivo) != NULL) {
                                fputs(buffer, ARCHIVO_CONSOLIDADO);
                            }

                            //Escritura en el log
                            ARCHIVO_LOG = fopen(args->LOG_FILE, "a");
                            if (ARCHIVO_LOG == NULL) {
                                // Control de errores
                                printf("No se puede abrir el archivo log.\n");
                            }
                            else{
                                strftime(fechaFin, sizeof(fechaFin), "%d%m%Y:::%H%M", tlocal);
                                fprintf(ARCHIVO_LOG, "%s:::%lu:::%s:::%s:::%d\n", fecha, (unsigned long)pthread_self(), fechaFin, ent->d_name, args->SU001 + args->SU002 + args->SU003 + args->SU004);
                                printf("%s:::%lu:::%s:::%s:::%d\n", fecha, (unsigned long)pthread_self(), fechaFin, ent->d_name, args->SU001 + args->SU002 + args->SU003 + args->SU004);
                            }

                            status = remove(ruta_archivo);
                        }
                    }
                }
            } 
        }
        closedir(dir); // Cerrar la carpeta
        fclose(ARCHIVO_CONSOLIDADO);
        fclose(ARCHIVO_LOG);
        sleep(args->SIMULATE_SLEEP);
        
    }
    pthread_exit(NULL);
}

void* funcion_hilo5(void *arg) {
    pthread_exit(NULL);
}

void BorrarPantalla(){
    printf("*********************************************************************\n");
    printf("*                        Process Auditor                            *\n");
    printf("*********************************************************************\n\n");
}
