#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <signal.h>
#include <pthread.h>

// Inclusión condicional de librerías específicas del sistema operativo para compatibilidad.
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
#define MAX_USUARIOS 100
#define MAX_ERRORES 3
#define DEFAULT_SIZE_FP 2 * 1024 * 1024 // 2 MB
#define SHM_NAME "/bank_operations_shm"
#define DUMP_FILE "memory_dump.txt"

typedef struct {
    char idUsuario[13];
    int conteoErrores;
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
    char estado[11];
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

void BorrarPantalla();
void ErrorAcceso();
int ContarLineas(char nombreArchivo[]);
void VisualizarArray(tSucursal listaMovimientos[], int num_movimientos);
void OrdenarConsolidado(tSucursal listaMovimientos[], int num_movimientos);
void* funcion_hilo1(void *arg);
void* funcion_hilo2(void *arg);
void* funcion_hilo3(void *arg);
void* funcion_hilo4(void *arg);
void* funcion_hilo5(void *arg);
void handle_signal(int signal);

int NUMERO_LINEAS = 0;
int shm_fd;
size_t shm_size = DEFAULT_SIZE_FP;
void *shm_ptr;

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
        fclose(archivo_config);

        token = strtok(CadenaLeida, ";");
        strcpy(args.PATH_FILES, token);
        token = strtok(NULL, ";");
        strcpy(args.INVENTORY_FILE, token);
        token = strtok(NULL, ";");
        strcpy(args.LOG_FILE, token);
        token = strtok(NULL, ";");
        args.NUM_PROCESOS = atoi(token);
        token = strtok(NULL, ";");
        args.SIMULATE_SLEEP = atoi(token);
        token = strtok(NULL, ";");
    }

    NUMERO_LINEAS = ContarLineas(args.INVENTORY_FILE);

    // Configurar memoria compartida
    shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("shm_open");
        exit(EXIT_FAILURE);
    }
    ftruncate(shm_fd, shm_size);
    shm_ptr = mmap(0, shm_size, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
    if (shm_ptr == MAP_FAILED) {
        perror("mmap");
        exit(EXIT_FAILURE);
    }

    tSucursal *listaMovimientos = (tSucursal *)shm_ptr;

    pFichEnt = fopen(args.INVENTORY_FILE, "r");
    if (pFichEnt == NULL) {
        printf("Error al abrir el fichero %s\n", args.INVENTORY_FILE);
        return 0;
    } else {
        FinFich = 0;
        j = 0;
        while (!FinFich) {
            Cadena[0] = '\0';
            fgets(Cadena, MAX_CARACTER, pFichEnt);

            if (Cadena[strlen(Cadena) - 1] == '\n')
                Cadena[strlen(Cadena) - 1] = '\0';

            cpToken = strtok(Cadena, ";");

            while (cpToken != NULL) {
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

                strcpy(listaMovimientos[j].estado, cpToken);
                cpToken = strtok(NULL, ";");

                listaMovimientos[j].horaProceso = listaMovimientos[j].horaFin - listaMovimientos[j].horaInicio;
                listaMovimientos[j].minutoProceso = listaMovimientos[j].minutoFin - listaMovimientos[j].minutoInicio;

                j++;
            }
            FinFich = feof(pFichEnt);
        }
        fclose(pFichEnt);
    }

    OrdenarConsolidado(listaMovimientos, NUMERO_LINEAS);
    VisualizarArray(listaMovimientos, NUMERO_LINEAS);
    printf("\n\n");

    pthread_t hilos[5];
    pthread_create(&hilos[0], NULL, funcion_hilo1, (void *)listaMovimientos);
    sleep(args.SIMULATE_SLEEP);
    pthread_create(&hilos[1], NULL, funcion_hilo2, (void *)listaMovimientos);
    sleep(args.SIMULATE_SLEEP);
    pthread_create(&hilos[2], NULL, funcion_hilo3, (void *)listaMovimientos);
    sleep(args.SIMULATE_SLEEP);
    pthread_create(&hilos[3], NULL, funcion_hilo4, (void *)listaMovimientos);
    sleep(args.SIMULATE_SLEEP);
    pthread_create(&hilos[4], NULL, funcion_hilo5, (void *)listaMovimientos);

    for (int i = 0; i < 5; i++) {
        pthread_join(hilos[i], NULL);
    }

    // Configurar manejador de señales
    struct sigaction sa;
    sa.sa_handler = handle_signal;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    // Mantener el programa en ejecución para permitir la captura de señales
    pause();

    return 1;
}

void handle_signal(int signal) {
    if (signal == SIGINT) {
        printf("\nInterrupción recibida. Volcando datos de memoria compartida a fichero...\n");

        FILE *file = fopen(DUMP_FILE, "w");
        if (!file) {
            perror("Error al abrir el fichero de volcado");
            exit(EXIT_FAILURE);
        }

        tSucursal *movimientos = (tSucursal *)shm_ptr;
        for (int i = 0; i < NUMERO_LINEAS; i++) {
            fprintf(file, "%s;%s;%02d:%02d;%s;%02d:%02d;%s;%s;%d;%d;%s;%02d:%02d\n",
                    movimientos[i].idOperacion,
                    movimientos[i].fechaInicio,
                    movimientos[i].horaInicio, movimientos[i].minutoInicio,
                    movimientos[i].fechaFin,
                    movimientos[i].horaFin, movimientos[i].minutoFin,
                    movimientos[i].idUsuario,
                    movimientos[i].idTipoOperacion,
                    movimientos[i].numOperacion,
                    movimientos[i].importe,
                    movimientos[i].estado,
                    movimientos[i].horaProceso, movimientos[i].minutoProceso);
        }

        fclose(file);
        printf("Datos volcados exitosamente a %s\n", DUMP_FILE);
        munmap(shm_ptr, shm_size);
        shm_unlink(SHM_NAME);
        exit(EXIT_SUCCESS);
    }
}

void VisualizarArray(tSucursal listaMovimientos[], int num_movimientos) {
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

    for (int i = 0; i < num_movimientos; ++i) {
        printf("%d\t", i);
        printf("%s\t\t", listaMovimientos[i].idOperacion);
        printf("%s  ", listaMovimientos[i].fechaInicio);
        printf("%02d:%02d\t\t", listaMovimientos[i].horaInicio, listaMovimientos[i].minutoInicio);
        printf("%s  ", listaMovimientos[i].fechaFin);
        printf("%02d:%02d\t\t", listaMovimientos[i].horaFin, listaMovimientos[i].minutoFin);
        printf("%s\t\t", listaMovimientos[i].idUsuario);
        printf("%s\t\t", listaMovimientos[i].idTipoOperacion);
        printf("%d\t\t", listaMovimientos[i].numOperacion);
        printf("%d\t\t", listaMovimientos[i].importe);
        printf("%s\t\t", listaMovimientos[i].estado);
        printf("%02d:%02d\n", listaMovimientos[i].horaProceso, listaMovimientos[i].minutoProceso);
    }
}

void OrdenarConsolidado(tSucursal listaMovimientos[], int num_movimientos) {
    tSucursal temp;
    for (int i = 0; i < num_movimientos - 1; i++) {
        for (int j = i + 1; j < num_movimientos; j++) {
            if (strcmp(listaMovimientos[i].fechaInicio, listaMovimientos[j].fechaInicio) > 0 ||
                (strcmp(listaMovimientos[i].fechaInicio, listaMovimientos[j].fechaInicio) == 0 &&
                 listaMovimientos[i].horaInicio > listaMovimientos[j].horaInicio) ||
                (strcmp(listaMovimientos[i].fechaInicio, listaMovimientos[j].fechaInicio) == 0 &&
                 listaMovimientos[i].horaInicio == listaMovimientos[j].horaInicio &&
                 listaMovimientos[i].minutoInicio > listaMovimientos[j].minutoInicio)) {
                temp = listaMovimientos[i];
                listaMovimientos[i] = listaMovimientos[j];
                listaMovimientos[j] = temp;
            }
        }
    }
}

int ContarLineas(char nombreArchivo[]) {
    FILE *archivo;
    int contador = 0;
    char caracter;

    archivo = fopen(nombreArchivo, "r");
    if (archivo == NULL) {
        printf("No se pudo abrir el archivo.\n");
        exit(1);
    }

    for (caracter = getc(archivo); caracter != EOF; caracter = getc(archivo)) {
        if (caracter == '\n') {
            contador++;
        }
    }

    fclose(archivo);
    return contador;
}

void BorrarPantalla() {
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

void* funcion_hilo1(void *arg) {
    printf("PATRÓN 1: Transacciones repetidas en una hora concreta por usuario.\n");
    tSucursal *movimientos = (tSucursal *) arg;
    int conteoTransacciones[24][MAX_USUARIOS] = {0};  // Arreglo 2D para contar transacciones por hora y por usuario.
    int usuarioIndex, hora;

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
    int conteoRetiros[MAX_USUARIOS] = {0};

    for (int i = 0; i < NUMERO_LINEAS; i++) {
        if (strcmp(movimientos[i].idTipoOperacion, "Retiro") == 0 && strcmp(movimientos[i].fechaInicio, movimientos[i].fechaFin) == 0) {
            int usuarioIndex = atoi(movimientos[i].idUsuario);
            conteoRetiros[usuarioIndex]++;
        }
    }

    int umbralRetiros = 3;

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
    int conteoErrores[MAX_USUARIOS] = {0};

    for (int i = 0; i < NUMERO_LINEAS; i++) {
        if (strcmp(movimientos[i].estado, "ERROR") == 0) {
            int usuarioIndex = atoi(movimientos[i].idUsuario);
            conteoErrores[usuarioIndex]++;
        }
    }

    for (int i = 0; i < MAX_USUARIOS; i++) {
        if (conteoErrores[i] > MAX_ERRORES) {
            printf("Usuario %d: %d errores\n", i, conteoErrores[i]);
        }
    }

    pthread_exit(NULL);
}

void* funcion_hilo4(void *arg) {
    printf("PATRÓN 4: Un usuario realiza una operación de cada tipo en un mismo día.\n");

    tSucursal *movimientos = (tSucursal *) arg;
    int conteoTiposOperacion[MAX_USUARIOS][MAX_OPERACIONES] = {0};
    int usuarioIndex, tipoOperacionIndex;

    for (int i = 0; i < NUMERO_LINEAS; i++) {
        usuarioIndex = atoi(movimientos[i].idUsuario);
        tipoOperacionIndex = atoi(movimientos[i].idTipoOperacion);
        conteoTiposOperacion[usuarioIndex][tipoOperacionIndex]++;
    }

    for (int i = 0; i < MAX_USUARIOS; i++) {
        for (int j = 0; j < MAX_OPERACIONES; j++) {
            if (conteoTiposOperacion[i][j] > 0) {
                for (int k = j + 1; k < MAX_OPERACIONES; k++) {
                    if (conteoTiposOperacion[i][k] == 0) {
                        printf("El usuario %d no ha realizado una operación de cada tipo en el mismo día.\n", i);
                        break;
                    }
                }
                break;
            }
        }
    }

    pthread_exit(NULL);
}

void* funcion_hilo5(void *arg) {
    printf("PATRÓN 5: La cantidad de dinero retirado (-) en un día concreto es mayor que el dinero ingresado (+) ese mismo día.\n");

    tSucursal *movimientos = (tSucursal *) arg;
    int saldoDia[MAX_USUARIOS][MAX_CARACTER] = {0};
    int usuarioIndex, diaIndex;

    for (int i = 0; i < NUMERO_LINEAS; i++) {
        usuarioIndex = atoi(movimientos[i].idUsuario);
        diaIndex = atoi(movimientos[i].fechaInicio);
        if (strcmp(movimientos[i].idTipoOperacion, "+") == 0) {
            saldoDia[usuarioIndex][diaIndex] += movimientos[i].importe;
        } else if (strcmp(movimientos[i].idTipoOperacion, "-") == 0) {
            saldoDia[usuarioIndex][diaIndex] -= movimientos[i].importe;
        }
    }

    for (int i = 0; i < MAX_USUARIOS; i++) {
        for (int j = 0; j < MAX_CARACTER; j++) {
            if (saldoDia[i][j] < 0) {
                for (int k = j + 1; k < MAX_CARACTER; k++) {
                    if (saldoDia[i][k] > 0) {
                        printf("El usuario %d tiene un saldo negativo mayor que el saldo positivo en algún día.\n", i);
                        break;
                    }
                }
                break;
            }
        }
    }

    pthread_exit(NULL);
}
