#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

// Inclusión condicional de librerías específicas del sistema operativo para compatibilidad. Se ejecuta lo primero para comprobar su compat>
#if defined(_WIN32)
    #include <windows.h>
#elif defined(_WIN64)
    #include <windows.h>
#elif defined(CYGWIN) && !defined(_WIN32)
    #include <windows.h>
#else
    #include <sys/param.h>
    #if defined(BSD)
       #include <unistd.h>
    #endif
#endif

#define MAX_CARACTER 100
#define MAX_OPERACIONES 1000
#define MAX_USUARIOS 100  // Número máximo de usuarios a rastrear
#define MAX_ERRORES 3     // Número máximo de errores permitidos antes de reportar

typedef struct {
    char idUsuario[13];  // ID del usuario
    int conteoErrores;   // Conteo de errores por usuario
} UsuarioError;

typedef struct {
    char idOperacion[13];
    char fechaInicio[13];
    int horaInicio;
    int minutoInicio;
    char fechaFin[13];
    int horaFin;
    int minutoFin;
    char idUsuario[13];
    char idTipoOperacion[13];
    int numOperacion;
    int importe;
    char estado[11]; // Para soportar strings como "ERROR", "CORRECTO", "FINALIZADO"
    int horaProceso;
    int minutoProceso;
} tSucursal;

typedef struct {
    int NUM_PROCESOS;
    int SIMULATE_SLEEP;
    char INVENTORY_FILE[20];    
    char LOG_FILE[20];
    char PATH_FILES[45];
} ArgumentosHilosConfiguracion;

// Declaración de funciones
void BorrarPantalla();
void ErrorAcceso();
int ContarLineas(char nombreArchivo[]); // Función implementada para contar las líneas del archivo
void VisualizarArray(tSucursal listaMovimientos[]);
void OrdenarConsolidado(tSucursal listaMovimientos[]);

int NUMERO_LINEAS = 0;

void* funcion_hilo1(void *arg);
void* funcion_hilo2(void *arg);
void* funcion_hilo3(void *arg);
void* funcion_hilo4(void *arg);
void* funcion_hilo5(void *arg);

int main() {
    // Variables
    FILE *pFichEnt;
    char* cpToken;
    char* token;
    char Cadena[MAX_CARACTER];
    char CadenaLeida[MAX_CARACTER] = "";
    int j, FinFich, i;
    FILE *archivo_config;

    int numeroLineas;
    char horarioIn[6], horarioFin[6], horaIn[3], horaFin[3], minutoIn[3], minutoFin[3];
    BorrarPantalla();

    // Creamos una estructura para almacenar los valores del archivo de configuración
    ArgumentosHilosConfiguracion args;

    // Abrimos el archivo de configuración
    archivo_config = fopen("fp.txt", "r");
    // Si no se puede abrir el archivo se mostrará un mensaje por pantalla
    if (archivo_config == NULL) {
        printf("Error: No se abrir el archivo de configuración.\n");
        exit(EXIT_FAILURE);
    } else {
        while (!FinFich) {
            Cadena[0] = '\0';
            fgets(Cadena, MAX_CARACTER, archivo_config);

            if (Cadena[strlen(Cadena) - 1] == '\n')
                Cadena[strlen(Cadena) - 1] = '\0';

            cpToken = strtok(Cadena, "=");

            while (cpToken != NULL) {
                cpToken = strtok(NULL, "=");
                strcat(CadenaLeida, cpToken);
                if (i != 4)
                    strcat(CadenaLeida, ";");
                cpToken = strtok(NULL, "=");
            }
            FinFich = feof(archivo_config);
            i++;
        }
        fclose(archivo_config); // Se cierra el fichero de texto
        
        token = strtok(CadenaLeida, ";"); // Obtener el primer token
        strcpy(args.PATH_FILES, token);
        token = strtok(NULL, ";"); // Obtener el siguiente token
        strcpy(args.INVENTORY_FILE, token);
        token = strtok(NULL, ";"); // Obtener el siguiente token
        strcpy(args.LOG_FILE, token);
        token = strtok(NULL, ";"); // Obtener el siguiente token
        args.NUM_PROCESOS = atoi(token);
        token = strtok(NULL, ";"); // Obtener el siguiente token
        args.SIMULATE_SLEEP = atoi(token);
        token = strtok(NULL, ";"); // Obtener el siguiente token
    }

    NUMERO_LINEAS = ContarLineas(args.INVENTORY_FILE);

    tSucursal listaMovimientos[NUMERO_LINEAS];

    tSucursal mov;
    pFichEnt = fopen(args.INVENTORY_FILE, "r");

    // Si el fichero no se puede abrir, el programa entrará en esta parte del bucle
    if (pFichEnt == NULL) {
        // Control de errores
        printf("Error al abrir el fichero %s\n", args.INVENTORY_FILE); // Se mostrará el mensaje de error por pantalla
        return 0; // Como valor de la función se devuelve un cero
    }
    // Si se puede abrir el archivo, el programa entrará en esta parte del bucle
    else {
        FinFich = 0;
        printf("\n");
        j = 0;
        // Se realiza la lectura del fichero y se guarda la información en el programa.
        while (!FinFich) {
            Cadena[0] = '\0';
            fgets(Cadena, MAX_CARACTER, pFichEnt);

            if (Cadena[strlen(Cadena) - 1] == '\n')
                Cadena[strlen(Cadena) - 1] = '\0';

            cpToken = strtok(Cadena, ";");

            while (cpToken != NULL) { // Lee cada campo hasta el ;
                strcpy(listaMovimientos[j].idOperacion, cpToken);
                cpToken = strtok(NULL, ";");

                strcpy(listaMovimientos[j].fechaInicio, cpToken);
                cpToken = strtok(NULL, ";");

                strcpy(horarioIn, cpToken);

                sprintf(horaIn, "%c%c", horarioIn[0], horarioIn[1]);
                sprintf(minutoIn, "%c%c", horarioIn[3], horarioIn[4]);

                listaMovimientos[j].horaInicio = atoi(horaIn);
                listaMovimientos[j].minutoInicio = atoi(minutoIn);
                cpToken = strtok(NULL, ";");

                strcpy(listaMovimientos[j].fechaFin, cpToken);
                cpToken = strtok(NULL, ";");

                strcpy(horarioFin, cpToken);

                sprintf(horaFin, "%c%c", horarioFin[0], horarioFin[1]);
                sprintf(minutoFin, "%c%c", horarioFin[3], horarioFin[4]);

                listaMovimientos[j].horaFin = atoi(horaFin);
                listaMovimientos[j].minutoFin = atoi(minutoFin);
                cpToken = strtok(NULL, ";");

                strcpy(listaMovimientos[j].idUsuario, cpToken);
                cpToken = strtok(NULL, ";");

                strcpy(listaMovimientos[j].idTipoOperacion, cpToken);
                cpToken = strtok(NULL, ";");

                listaMovimientos[j].numOperacion = atoi(cpToken);
                cpToken = strtok(NULL, ";");

                listaMovimientos[j].importe = atoi(cpToken);
                cpToken = strtok(NULL, ";");

                // Cambiado para asignar una cadena a estado
                strcpy(listaMovimientos[j].estado, cpToken);
                cpToken = strtok(NULL, ";");

                listaMovimientos[j].horaProceso = listaMovimientos[j].horaFin - listaMovimientos[j].horaInicio;
                listaMovimientos[j].minutoProceso = listaMovimientos[j].minutoFin - listaMovimientos[j].minutoInicio;

                j++;
            }
            FinFich = feof(pFichEnt);
        }
        fclose(pFichEnt); // Se cierra el fichero de texto
    }

    OrdenarConsolidado(listaMovimientos);
    VisualizarArray(listaMovimientos);
    printf("\n\n");

    // Creamos los hilos
    pthread_t hilos[5];

    pthread_create(&hilos[0], NULL, funcion_hilo1, (void*)&listaMovimientos);
    sleep(args.SIMULATE_SLEEP);
    pthread_create(&hilos[1], NULL, funcion_hilo2, (void*)&listaMovimientos);
    sleep(args.SIMULATE_SLEEP);
    pthread_create(&hilos[2], NULL, funcion_hilo3, (void*)&listaMovimientos);
    sleep(args.SIMULATE_SLEEP);
    pthread_create(&hilos[3], NULL, funcion_hilo4, (void*)&listaMovimientos);
    sleep(args.SIMULATE_SLEEP);
    pthread_create(&hilos[4], NULL, funcion_hilo5, (void*)&listaMovimientos);

    // Esperamos a que terminen los hilos de ser creados
    for (int i = 0; i < 5; i++) {
        pthread_join(hilos[i], NULL);
    }

    return 1;
}

//Hilos con los patrones

void* funcion_hilo1(void *arg) {
    printf("PATRÓN 1: Transacciones repetidas en una hora concreta por usuario.\n");
    tSucursal *movimientos = (tSucursal *) arg;
    int conteoTransacciones[24][MAX_USUARIOS] = {0};  // Arreglo 2D para contar transacciones por hora y por usuario.
    int usuarioIndex, hora;

    // Inicializar conteos a cero para todos los usuarios y todas las horas
    for (int i = 0; i < NUMERO_LINEAS; i++) {
        usuarioIndex = atoi(movimientos[i].idUsuario); // Convierte el ID del usuario a un índice numérico si es posible.
        hora = movimientos[i].horaInicio;
        conteoTransacciones[hora][usuarioIndex]++;

        if (conteoTransacciones[hora][usuarioIndex] > 5) {
            printf("El usuario %s tiene más de 5 transacciones en la hora %d.\n", movimientos[i].idUsuario, hora);
        }
    }

    pthread_exit(NULL);
}



void* funcion_hilo2(void *arg) {
    printf("PATRÓN 2: Múltiples retiros en un mismo día\n");

    tSucursal *movimientos = (tSucursal *) arg;
    int conteoRetiros[MAX_USUARIOS] = {0};  // Arreglo para contar retiros por usuario.

    // Iterar sobre los movimientos para contar los retiros por usuario en un día específico
    for (int i = 0; i < NUMERO_LINEAS; i++) {
        // Verificar si la operación es un retiro y está dentro de un mismo día
        if (strcmp(movimientos[i].idTipoOperacion, "Retiro") == 0 && strcmp(movimientos[i].fechaInicio, movimientos[i].fechaFin) == 0) {
            // Incrementar el conteo de retiros para el usuario correspondiente
            int usuarioIndex = atoi(movimientos[i].idUsuario); // Convierte el ID del usuario a un índice numérico si es posible.
            conteoRetiros[usuarioIndex]++;
        }
    }

    // Definir el umbral para considerar múltiples retiros en un día
    int umbralRetiros = 3;

    // Mostrar usuarios que han realizado múltiples retiros en un día
    for (int i = 0; i < MAX_USUARIOS; i++) {
        if (conteoRetiros[i] > umbralRetiros) {
            printf("El usuario %d ha realizado %d retiros en un día.\n", i, conteoRetiros[i]);
        }
    }

    pthread_exit(NULL);
}


void* funcion_hilo3(void *arg) {
    printf("PATRÓN 3: Errores repetidos por un usuario.\n");

    tSucursal *movimientos = (tSucursal *) arg;
    int conteoErrores[MAX_USUARIOS] = {0};  // Arreglo para contar errores por usuario.

    // Contar la cantidad de errores por usuario
    for (int i = 0; i < NUMERO_LINEAS; i++) {
        if (strcmp(movimientos[i].estado, "ERROR") == 0) {
            int usuarioIndex = atoi(movimientos[i].idUsuario); // Convertir el ID de usuario a un índice numérico si es posible.
            conteoErrores[usuarioIndex]++;
        }
    }

    // Imprimir los usuarios con errores repetidos
    for (int i = 0; i < MAX_USUARIOS; i++) {
        if (conteoErrores[i] > MAX_ERRORES) { // Si un usuario ha cometido más errores que el umbral definido, imprimir su ID.
            printf("Usuario %d: %d errores\n", i, conteoErrores[i]);
        }
    }

    pthread_exit(NULL);
}



void* funcion_hilo4(void *arg){
    printf("PATRÓN 4: Un usuario realiza una operación de cada tipo en un mismo día.\n");

    tSucursal *movimientos = (tSucursal *) arg;
    int conteoTiposOperacion[MAX_USUARIOS][MAX_OPERACIONES] = {0};  // Arreglo 2D para contar tipos de operaciones por usuario.
    int usuarioIndex, tipoOperacionIndex;

    // Contar la cantidad de cada tipo de operación por usuario
    for (int i = 0; i < NUMERO_LINEAS; i++) {
        usuarioIndex = atoi(movimientos[i].idUsuario); // Convertir el ID de usuario a un índice numérico si es posible.
        tipoOperacionIndex = atoi(movimientos[i].idTipoOperacion); // Convertir el ID de tipo de operación a un índice numérico si es posible.
        conteoTiposOperacion[usuarioIndex][tipoOperacionIndex]++;
    }

    // Comprobar si un usuario ha realizado una operación de cada tipo en el mismo día
    for (int i = 0; i < MAX_USUARIOS; i++) {
        for (int j = 0; j < MAX_OPERACIONES; j++) {
            if (conteoTiposOperacion[i][j] > 0) { // Comprobamos las operaciones realizadas de un tipo
                for (int k = j + 1; k < MAX_OPERACIONES; k++) {
                    if (conteoTiposOperacion[i][k] == 0) { // Comprobamos si se han realizado o no varias y diferentes operaciones
                        printf("El usuario %d no ha realizado una operación de cada tipo en el mismo día.\n", i);
                        break; // No es necesario seguir comprobando para este usuario.
                    }
                }
                break; // No es necesario seguir comprobando para este usuario.
            }
        }
    }

    pthread_exit(NULL);
}



void* funcion_hilo5(void *arg) {
    printf("PATRÓN 5: La cantidad de dinero retirado (-) en un día concreto es mayor que el dinero ingresado (+) ese mismo día.\n");

    tSucursal *movimientos = (tSucursal *) arg;
    int saldoDia[MAX_USUARIOS][MAX_CARACTER] = {0};  // Arreglo 2D para almacenar el saldo de cada usuario por día.
    int usuarioIndex, diaIndex;

    // Calcular el saldo
    for (int i = 0; i < NUMERO_LINEAS; i++) {
        usuarioIndex = atoi(movimientos[i].idUsuario); // Convertir el ID de usuario a un índice numérico si es posible.
        diaIndex = atoi(movimientos[i].fechaInicio); // Convertir la fecha de inicio a un índice numérico si es posible.
        if (strcmp(movimientos[i].idTipoOperacion, "+") == 0) {
            saldoDia[usuarioIndex][diaIndex] += movimientos[i].importe; // Sumar al saldo la cantidad ingresada
        } else if (strcmp(movimientos[i].idTipoOperacion, "-") == 0) {
            saldoDia[usuarioIndex][diaIndex] -= movimientos[i].importe; // Restar al saldo la cantidad retirada
        }
    }

    // Comprobar si el saldo negativo es mayor que el saldo positivo
    for (int i = 0; i < MAX_USUARIOS; i++) {
        for (int j = 0; j < MAX_CARACTER; j++) {
            if (saldoDia[i][j] < 0) { // Saldo negativo?
                for (int k = j + 1; k < MAX_CARACTER; k++) {
                    if (saldoDia[i][k] > 0) { // Saldo restante es positivo?
                        printf("El usuario %d tiene un saldo negativo mayor que el saldo positivo en algún día.\n", i);
                        break; // No es necesario seguir comprobando para este usuario.
                    }
                }
                break; // No es necesario seguir comprobando para este usuario.
            }
        }
    }

    pthread_exit(NULL);
}



//-------------------Funciones---------------------
void VisualizarArray(tSucursal listaMovimientos[]){
    printf("REGISTRO DE SESIONES\n\n");
    printf("#\t");
    printf("idOpera\t\t");
    printf("fInicio\t");
    printf("hInicio\t\t");
    printf("fFinali\t");
    printf("hFinali\t\t");
    printf("Usuario\t\t");
    printf("tipOper\t\t");
    printf("numOper\t\t");
    printf("Importe\t\t");
    printf("aEstado\t\t");
    printf("tiempoT\n");
    printf("-------------------------------------------------------------------------------------------------------------------------------------------------------------------\n");

    for (int i = 0; i < NUMERO_LINEAS; ++i){
        printf("%d\t", i);
        printf("%s\t\t", listaMovimientos[i].idOperacion);
        printf("%s  ",  listaMovimientos[i].fechaInicio);
        printf("%02d:%02d\t\t",  listaMovimientos[i].horaInicio, listaMovimientos[i].minutoInicio);
        printf("%s  ",  listaMovimientos[i].fechaFin);
        printf("%02d:%02d\t\t",  listaMovimientos[i].horaFin, listaMovimientos[i].minutoFin);
        printf("%s\t\t",  listaMovimientos[i].idUsuario);
        printf("%s\t\t",  listaMovimientos[i].idTipoOperacion);
        printf("%d\t\t",  listaMovimientos[i].numOperacion);
        printf("%d\t\t",  listaMovimientos[i].importe);
        printf("%s\t\t",  listaMovimientos[i].estado);
        printf("%02d:%02d\n", listaMovimientos[i].horaProceso, listaMovimientos[i].minutoProceso);
    }
}

void OrdenarConsolidado(tSucursal listaMovimientos[]) {
    tSucursal temp;
    for (int i = 0; i < NUMERO_LINEAS - 1; i++) {
        for (int j = i + 1; j < NUMERO_LINEAS; j++) {
            // Comparar fechas y horas de inicio para ordenar
            if (strcmp(listaMovimientos[i].fechaInicio, listaMovimientos[j].fechaInicio) > 0 ||
                (strcmp(listaMovimientos[i].fechaInicio, listaMovimientos[j].fechaInicio) == 0 &&
                 listaMovimientos[i].horaInicio > listaMovimientos[j].horaInicio) ||
                (strcmp(listaMovimientos[i].fechaInicio, listaMovimientos[j].fechaInicio) == 0 &&
                 listaMovimientos[i].horaInicio == listaMovimientos[j].horaInicio &&
                 listaMovimientos[i].minutoInicio > listaMovimientos[j].minutoInicio)) {
                // Intercambiar elementos si el primero es mayor que el segundo
                temp = listaMovimientos[i];
                listaMovimientos[i] = listaMovimientos[j];
                listaMovimientos[j] = temp;
            }
        }
    }
}

// Implementación de la función para contar líneas del archivo
int ContarLineas(char nombreArchivo[]){
    FILE *archivo;
    int contador = 0;
    char caracter;

    // Se abre el archivo
    archivo = fopen(nombreArchivo, "r");

    // Si el archivo no se pudo abrir
    if (archivo == NULL){
        printf("No se pudo abrir el archivo.\n");
        exit(1);
    }

    // Se cuenta el número de líneas
    for (caracter = getc(archivo); caracter != EOF; caracter = getc(archivo)){
        if (caracter == '\n'){
            contador++;
        }
    }

    // Se cierra el archivo
    fclose(archivo);

    // Se devuelve el número de líneas
    return contador;
}

void BorrarPantalla() {
    // Comando para limpiar la pantalla dependiendo del sistema operativo
    #if defined(WIN32) || defined(WIN64) || defined(CYGWIN) && !defined(_WIN32)
        system("cls");
    #else
        system("clear");
    #endif
}

void ErrorAcceso() {
    printf("\n\tError de acceso\n");
    printf("\tPuede que no tengas permisos de lectura para este archivo.\n\n");
}
