//Librerías
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>

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
    archivo_config = fopen("/Users/alvaro/fp.txt", "r");
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
    printf("\nPATRÓN 2: Múltiples retiros en un mismo día\n");
    printf("\n//NO FUNCIONA\n\n");
    return NULL;
}



void* funcion_hilo3(void *arg) {
    printf("PATRÓN 3: Errores repetidos por un usuario.\n");
    printf("\n//NO FUNCIONA\n\n");
    return NULL;
}



void* funcion_hilo4(void *arg){
    printf("PATRÓN 4: Un usuario realiza una operación de cada tipo en un mismo día.\n");
    printf("\n//NO FUNCIONA\n\n");
    return NULL;
}



void* funcion_hilo5(void *arg) {
    printf("PATRÓN 5: La cantidad de dinero retirado (-) en un día concreto es mayor que el dinero ingresado (+) ese mismo día.\n");
    printf("\n//NO FUNCIONA\n\n");
    return NULL;
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
        printf("%d:%d\t\t",  listaMovimientos[i].horaFin, listaMovimientos[i].minutoFin);
        printf("%s\t\t",  listaMovimientos[i].idUsuario);
        printf("%s\t",  listaMovimientos[i].idTipoOperacion);
        printf("%d\t\t",  listaMovimientos[i].numOperacion);
        printf("%d\t\t",  listaMovimientos[i].importe);
        if (strcmp(listaMovimientos[i].estado, "Error") == 0) {
            printf("%s\t\t",  listaMovimientos[i].estado);  
        } else {
            printf("%s\t",  listaMovimientos[i].estado);  
        }
        
        printf("%02d:%02d\n",  listaMovimientos[i].horaProceso, listaMovimientos[i].minutoProceso);
        
    }
}
void OrdenarConsolidado(tSucursal listaMovimientos[]){
    int i,j,usuarioNumeroJ, usuarioNumeroI;
    char usuarioNumeroIChar[4];
    char usuarioNumeroJChar[4];

    tSucursal Aux;

    for(i=0;i<NUMERO_LINEAS;i++){
        sprintf(usuarioNumeroIChar, "%c%c%c", listaMovimientos[i].idUsuario[4], listaMovimientos[i].idUsuario[5], listaMovimientos[i].idUsuario[6]);
        usuarioNumeroI = atoi(usuarioNumeroIChar);
        for(j=0;j<i;j++){
            sprintf(usuarioNumeroJChar, "%c%c%c", listaMovimientos[j].idUsuario[4], listaMovimientos[j].idUsuario[5], listaMovimientos[j].idUsuario[6]);
            usuarioNumeroJ = atoi(usuarioNumeroJChar);

            if(usuarioNumeroI<usuarioNumeroJ){
                Aux=listaMovimientos[i];
                listaMovimientos[i]=listaMovimientos[j];
                listaMovimientos[j]=Aux;
            }
        }
    }

    //Ordenación por hora
    for(i=0;i<NUMERO_LINEAS;i++){
        for(j=0;j<i;j++){
            if(strcmp(listaMovimientos[i].idUsuario, listaMovimientos[j].idUsuario) == 0 && listaMovimientos[i].horaInicio<listaMovimientos[j].horaInicio){
                Aux=listaMovimientos[i];
                listaMovimientos[i]=listaMovimientos[j];
                listaMovimientos[j]=Aux;
            }
        }
    }

    //Ordenación por minutos
    for(i=0;i<NUMERO_LINEAS;i++){
        for(j=0;j<i;j++){
            if(strcmp(listaMovimientos[i].idUsuario, listaMovimientos[j].idUsuario) == 0 && listaMovimientos[i].horaInicio == listaMovimientos[j].horaInicio){
                if(listaMovimientos[i].minutoInicio < listaMovimientos[j].minutoInicio){
                    Aux=listaMovimientos[i];
                    listaMovimientos[i]=listaMovimientos[j];
                    listaMovimientos[j]=Aux;
                }
            }
        }
    }

}

//Función para contar las líneas del archivo
int ContarLineas(char nombreArchivo[]){
    FILE *archivo; //Creamos el puntere de tipo fichero
    int numeroLineas = 0; //Creamos una variable que igualamos a cero, que la utilizaremos para almacenar el número de líneas que tiene el archivo
    int ch = 0;
    archivo = fopen(nombreArchivo, "r"); //Abrimos el archivo modo lectura

    //Comprobamos si es posible abrir el archivo
    //En el caso de que no sea posible entrará por la primera rama del bucle
    if(archivo == NULL){
        // Control de errores
    	printf("Error al acceder archivo\n");
        return(1); //La función devolverá 1
    }
    //En el caso de que si sea posible abrir el archivo, entrará por esta rama del bucle
    else{
        //Siempre que el valor que devuelva la función fgetc sea distinta de EOF, quiere decir que no ha llegado al final del archivo
        while((ch=fgetc(archivo)) != EOF){
            //Si el valor el \n, quiere decir que hay un salto de línea
            if(ch == '\n')
                numeroLineas++; //Incrementamos la variable numeroLineas en 1
        }
        //Cuando llegue al final del archivo, lo cerramos
        fclose(archivo);
    }
    return(numeroLineas); //La función devuelve el número de líneas calculadas
}

void BorrarPantalla(){
    printf("*********************************************************************\n");
    printf("*                        Process Auditor                            *\n");
    printf("*********************************************************************\n\n");
}
