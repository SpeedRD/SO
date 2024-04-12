//Librerías
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>

// Inclusión condicional de librerías específicas del sistema operativo para compatibilidad. Se ejecuta lo primero para comprobar su compatibilidad
#if defined(_WIN32)
    #include <windows.h>
#elif defined(_WIN64)
    #include <windows.h>
#elif defined(__CYGWIN__) && !defined(_WIN32)
    #include <windows.h>
// En el caso contrario, la maquina tendrá un sistema operativo Linux o MacOS e incluiremos las librerías compatibles compatibles como estos sistemas operativos
#else
    #include <sys/param.h>
    #if defined(BSD)
       #include <unistd.h>
    #endif
#endif

//Definiciones de constantes
#define MAX_NOMBRE_ARCHIVO 80
#define MAX_CARACTER 150 // para manejar el tamaño máximo de caracteres en operaciones de cadena.

//Variables globales
int SU001_numero = 1, SU002_numero = 1, SU003_numero = 1, SU004_numero = 1;
int SU001_numero_sesion = 1, SU002_numero_sesion = 1, SU003_numero_sesion = 1, SU004_numero_sesion = 1;

//Estructura para almacenar la configuración
typedef struct {
    int NUM_PROCESOS;
    int SIMULATE_SLEEP;
    char INVENTORY_FILE[20];
    char LOG_FILE[20];
    char PATH_FILES[45];
} ArgumentosHilosConfiguracion;

// Declaración de funciones
void BorrarPantalla();
int generarNumeroAleatorio(int min, int max);
int generarArchivo_SU001(char ruta[]);
int generarArchivo_SU002(char ruta[]);
int generarArchivo_SU003(char ruta[]);
int generarArchivo_SU004(char ruta[]);


int main(){

    // Inicialización de la semilla para la generación de números aleatorios
    srand(time(NULL));
    int opcion, numeroAle;

    // Declaración de variables
    char* token;
    int FinFich = 0, i = 0;
    FILE *archivo_config;
    char* cpToken;
    char Cadena[MAX_CARACTER];
    char CadenaLeida[MAX_CARACTER] = "";

    // Creamos una estructura para almacenar los valores del archivo de configuración
    ArgumentosHilosConfiguracion args;

    //Abrimos el archivo de configuración
    archivo_config = fopen("/Users/alvaro/fp.txt", "r");
    // Manejo de errores si el archivo no se puede abrir
    // Si no se puede abrir el archivo se mostrará un mensaje por pantalla
    if (archivo_config == NULL) {
        // Control de errores
        printf("No se puede abrir el archivo de configuración.\n");
        exit(EXIT_FAILURE);
    }
    else{
        // Lectura del archivo de configuración
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
        fclose(archivo_config); // Se cierra el fichero de texto
        
        // División de la cadena leída para obtener los valores de configuración
        token = strtok(CadenaLeida, ";"); // Obtener el primer token
        strcpy (args.PATH_FILES, token);
        token = strtok(NULL, ";"); // Obtener el siguiente 
        strcpy(args.INVENTORY_FILE,token );
        token = strtok(NULL, ";"); // Obtener el siguiente 
        strcpy(args.LOG_FILE, token);
        token = strtok(NULL, ";"); // Obtener el siguiente 
        args.NUM_PROCESOS = atoi(token);
        token = strtok(NULL, ";"); // Obtener el siguiente 
        args.SIMULATE_SLEEP = atoi(token);
        token = strtok(NULL, ";"); // Obtener el siguiente 
    }

    //Menú principal
    do{
        BorrarPantalla();
        printf("MENU: \n\n");
        printf("1. Generar 5 archivos\n");
        printf("2. Salir\n\n");
        printf("Introduce una opcion (1 o 2)? ");
        scanf("%d", &opcion);

        switch(opcion){
            case 1:
                BorrarPantalla();
                printf("NIVEL 1. Generar 5 archivos\n");
                for (int i = 0; i < 5; ++i)
                {
                    numeroAle = generarNumeroAleatorio(1, 4);
                    switch(numeroAle){
                        case 1:
                            generarArchivo_SU001(args.PATH_FILES);
                            break;

                        case 2:
                            generarArchivo_SU002(args.PATH_FILES);
                            break;

                        case 3:
                            generarArchivo_SU003(args.PATH_FILES);
                            break;

                        case 4:
                            generarArchivo_SU004(args.PATH_FILES);
                            break;
                    }
                }
                break;
            
            case 2:
                BorrarPantalla();
                break;
            default:
                BorrarPantalla();
                printf("La opción introducida no es válida\n");
                break;
        }
    }while(opcion != 2);
}

//-------------------Funciones---------------------
// Función para generar un número aleatorio dentro de un rango dado
int generarNumeroAleatorio(int min, int max) {
    return (rand() % (max - min + 1)) + min;
}
int generarArchivo_SU001(char ruta[]){
    // Obtención de la hora y fecha actual
    time_t tiempo = time(NULL);
    struct tm *tlocal = localtime(&tiempo);

    // Definición de variables
    char fechaIn[12], fechaFin[12], horaFin[3], minutoFin[3], estado[11];
    float importe;
    char SU001_NOMBRE[MAX_NOMBRE_ARCHIVO];
    FILE *f;

    // Obtener la fecha y hora actual
    strftime(fechaIn, sizeof(fechaIn), "%d%m%Y", tlocal);
    strftime(fechaFin, sizeof(fechaFin), "%d%m%Y", tlocal);
    strftime(horaFin, sizeof(horaFin), "%H", tlocal);
    strftime(minutoFin, sizeof(minutoFin), "%M", tlocal);
    int horaFinEntero = atoi(horaFin);
    int minutoFinEntero = atoi(minutoFin);

    // Crear el nombre del archivo
    sprintf(SU001_NOMBRE, "%s/SU001_OPE001_%d_%s.data", ruta, SU001_numero, fechaIn);
    printf("%s - ", SU001_NOMBRE);

    // Abrir el archivo en modo escritura
    f = fopen(SU001_NOMBRE, "w");

    // Comprobar si hay errores al abrir el archivo
    if(f == NULL){
        // Control de errores
        printf("No se pudo abrir el archivo\n");
        return 0;
    } else {
        printf("Archivo creado correctamente\n");

        // Generación de un número aleatorio de líneas para el archivo
        int lineas = generarNumeroAleatorio(1, 20);
        for(int i = 1; i <= lineas; i++){

            // Generación de datos simulados para cada línea
            importe = generarNumeroAleatorio(-50, 50);
            int random = generarNumeroAleatorio(1, 100);
            if (random <= 20) {
                strcpy(estado, "Error");
            } else if (random <= 60) {
                strcpy(estado, "Correcto");
            } else {
                strcpy(estado, "Finalizado");
            }
            int horaInEntero = generarNumeroAleatorio(0, horaFinEntero);
            int minutoInEntero = generarNumeroAleatorio(0, minutoFinEntero);
            fprintf(f, "OPE%02d;%s;%02d:%02d;%s;%s:%s;USER%03d;COMPRA%02d;%d;%.2f;%s\n", SU001_numero_sesion, fechaIn, horaInEntero, minutoInEntero, fechaFin, horaFin, minutoFin, generarNumeroAleatorio(1, 999), generarNumeroAleatorio(1, 99), generarNumeroAleatorio(1, 9), importe, estado);
            SU001_numero_sesion++;
        }
        fclose(f);
        SU001_numero++;
        return 1;
    }
}

int generarArchivo_SU002(char ruta[]){
    // Obtención de la hora y fecha actual
    time_t tiempo = time(NULL);
    struct tm *tlocal = localtime(&tiempo);

    // Definición de variables
    char fechaIn[12], fechaFin[12], horaFin[3], minutoFin[3], estado[11];
    float importe;
    char SU002_NOMBRE[MAX_NOMBRE_ARCHIVO];
    FILE *f;

    // Obtener la fecha y hora actual
    strftime(fechaIn, sizeof(fechaIn), "%d%m%Y", tlocal);
    strftime(fechaFin, sizeof(fechaFin), "%d%m%Y", tlocal);
    strftime(horaFin, sizeof(horaFin), "%H", tlocal);
    strftime(minutoFin, sizeof(minutoFin), "%M", tlocal);

    // Crear el nombre del archivo
    sprintf(SU002_NOMBRE, "%s/SU002_OPE002_%d_%s.data", ruta, SU002_numero, fechaIn);
    printf("%s - ", SU002_NOMBRE);

    // Abrir el archivo en modo escritura
    f = fopen(SU002_NOMBRE, "w+");

    // Comprobar si hay errores al abrir el archivo
    if(f == NULL){
        // Control de errores
        printf("No se pudo abrir el archivo\n");
        return 0;
    } else {
        printf("Archivo creado correctamente\n");

        int lineas = generarNumeroAleatorio(1, 20);
        for(int i = 1; i <= lineas; i++){
            importe = generarNumeroAleatorio(-50, 50);
            int random = generarNumeroAleatorio(1, 100);
            if (random <= 20) {
                strcpy(estado, "Error");
            } else if (random <= 60) {
                strcpy(estado, "Correcto");
            } else {
                strcpy(estado, "Finalizado");
            }
            int horaInEntero = generarNumeroAleatorio(0, atoi(horaFin));
            int minutoInEntero = generarNumeroAleatorio(0, atoi(minutoFin));
            fprintf(f, "OPE%02d;%s;%02d:%02d;%s;%s:%s;USER%03d;COMPRA%02d;%d;%.2f;%s\n", SU002_numero_sesion, fechaIn, horaInEntero, minutoInEntero, fechaFin, horaFin, minutoFin, generarNumeroAleatorio(1, 999), generarNumeroAleatorio(1, 99), generarNumeroAleatorio(1, 9), importe, estado);
            SU002_numero_sesion++;
        }
        fclose(f);
        SU002_numero++;
        return 1;
    }
}

int generarArchivo_SU003(char ruta[]){
    // Obtención de la hora y fecha actual
    time_t tiempo = time(NULL);
    struct tm *tlocal = localtime(&tiempo);

    // Definición de variables
    char fechaIn[12], fechaFin[12], horaFin[3], minutoFin[3], estado[11];
    float importe;
    char SU003_NOMBRE[MAX_NOMBRE_ARCHIVO];
    FILE *f;

    // Obtener la fecha y hora actual
    strftime(fechaIn, sizeof(fechaIn), "%d%m%Y", tlocal);
    strftime(fechaFin, sizeof(fechaFin), "%d%m%Y", tlocal);
    strftime(horaFin, sizeof(horaFin), "%H", tlocal);
    strftime(minutoFin, sizeof(minutoFin), "%M", tlocal);

    // Crear el nombre del archivo
    sprintf(SU003_NOMBRE, "%s/SU003_OPE003_%d_%s.data", ruta, SU003_numero, fechaIn);
    printf("%s - ", SU003_NOMBRE);

    // Abrir el archivo en modo escritura
    f = fopen(SU003_NOMBRE, "w+");

    // Comprobar si hay errores al abrir el archivo
    if(f == NULL){
        // Control de errores
        printf("No se pudo abrir el archivo\n");
        return 0;
    } else {
        printf("Archivo creado correctamente\n");

        int lineas = generarNumeroAleatorio(1, 20);
        for(int i = 1; i <= lineas; i++){
            importe = generarNumeroAleatorio(-50, 50);
            int random = generarNumeroAleatorio(1, 100);
            if (random <= 20) {
                strcpy(estado, "Error");
            } else if (random <= 60) {
                strcpy(estado, "Correcto");
            } else {
                strcpy(estado, "Finalizado");
            }
            int horaInEntero = generarNumeroAleatorio(0, atoi(horaFin));
            int minutoInEntero = generarNumeroAleatorio(0, atoi(minutoFin));
            fprintf(f, "OPE%02d;%s;%02d:%02d;%s;%s:%s;USER%03d;COMPRA%02d;%d;%.2f;%s\n", SU003_numero_sesion, fechaIn, horaInEntero, minutoInEntero, fechaFin, horaFin, minutoFin, generarNumeroAleatorio(1, 999), generarNumeroAleatorio(1, 99), generarNumeroAleatorio(1, 9), importe, estado);
            SU003_numero_sesion++;
        }
        fclose(f);
        SU003_numero++;
        return 1;
    }
}

int generarArchivo_SU004(char ruta[]){
    // Obtención de la hora y fecha actual
    time_t tiempo = time(NULL);
    struct tm *tlocal = localtime(&tiempo);

    // Definición de variables
    char fechaIn[12], fechaFin[12], horaFin[3], minutoFin[3], estado[11];
    float importe;
    char SU004_NOMBRE[MAX_NOMBRE_ARCHIVO];
    FILE *f;

    // Obtener la fecha y hora actual
    strftime(fechaIn, sizeof(fechaIn), "%d%m%Y", tlocal);
    strftime(fechaFin, sizeof(fechaFin), "%d%m%Y", tlocal);
    strftime(horaFin, sizeof(horaFin), "%H", tlocal);
    strftime(minutoFin, sizeof(minutoFin), "%M", tlocal);
    sprintf(SU004_NOMBRE, "%s/SU004_OPE004_%d_%s.data", ruta, SU004_numero, fechaIn);
    printf("%s - ", SU004_NOMBRE);

    // Abrir el archivo en modo escritura
    f = fopen(SU004_NOMBRE, "w+");

    // Comprobar si hay errores al abrir el archivo
    if(f == NULL){
        // Control de errores
        printf("No se pudo abrir el archivo\n");
        return 0;
    } else {
        printf("Archivo creado correctamente\n");

        int lineas = generarNumeroAleatorio(1, 20);
        for(int i = 1; i <= lineas; i++){
            importe = generarNumeroAleatorio(-50, 50);
            int random = generarNumeroAleatorio(1, 100);
            if (random <= 20) {
                strcpy(estado, "Error");
            } else if (random <= 60) {
                strcpy(estado, "Correcto");
            } else {
                strcpy(estado, "Finalizado");
            }
            int horaInEntero = generarNumeroAleatorio(0, atoi(horaFin));
            int minutoInEntero = generarNumeroAleatorio(0, atoi(minutoFin));
            fprintf(f, "OPE%02d;%s;%02d:%02d;%s;%s:%s;USER%03d;COMPRA%02d;%d;%.2f;%s\n", SU004_numero_sesion, fechaIn, horaInEntero, minutoInEntero, fechaFin, horaFin, minutoFin, generarNumeroAleatorio(1, 999), generarNumeroAleatorio(1, 99), generarNumeroAleatorio(1, 9), importe, estado);
            SU004_numero_sesion++;
        }
        fclose(f);
        SU004_numero++;
        return 1;
    }
}

void BorrarPantalla(){
    printf("*********************************************************************\n")
    printf("*                        Process Auditor                            *\n");
    printf("*********************************************************************\n\n");
}