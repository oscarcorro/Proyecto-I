#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>

#define NUM_FILAS 8
#define TAM_LINEA 16
#define TAM_CACHE 4096

//Definimos la estructura de cada linea
typedef struct {
    unsigned char ETQ;
    unsigned char Data[TAM_LINEA];
    } T_CACHE_LINE;

//Declaramos las variables globales
char texto[100];
int numCaracteres=0;

//Declaramos los prototipos de las funciones a utilizar
int comprobarFichero(char *fich);//Funcion para comprobar que existan los ficheros
void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]); //Funcion para inicializar la etiqueta a 0xFF y los datos de la cache a 0x23
void VolcarCACHE(T_CACHE_LINE *tbl);//Funcion para imprimir el contenido de la cahe por pantalla
void ParsearDireccion(unsigned int addr, int *ETQ, int*palabra, int *linea, int *bloque);//Funcion para obtener la etiqueta, linea, palabra y bloque a partir de la direccion
void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ,int linea, int bloque);//Funcion para actualizar la etiquete y los datos por los correspondientes
void volcarContenido(char *fich, T_CACHE_LINE *tbl);//Funcion para volcar el contenido de la cache en un fichero binario

void main (int argc, char *argv[]){

    //Declaramos las variables que necesitaremos
    FILE *fd;
    int globalTime=0;
    int numFallos=0;
    int numAccesos=0;
    unsigned int addr;
    int ETQ;
    int palabra;
    int linea;
    int bloque;
    unsigned char SIMUL_RAM[TAM_CACHE];
    T_CACHE_LINE tbl[NUM_FILAS];

    //tbl inicializa a xFF todos los campos Label y a x23 los campos de los datos de la caché
    LimpiarCACHE(tbl);

    //Comprobamos que los ficheros CONTENTS_RAM y dirs_memoria.txt existen
    if((comprobarFichero("CONTENTS_RAM.bin") != -1) && (comprobarFichero("accesos_memoria.txt") != -1)){

        //Leemos el contendido del fichero binario y lo guardamos en la variable SIMUL_RAM
        fd=fopen("CONTENTS_RAM.bin", "rb");
        fread(SIMUL_RAM, sizeof(unsigned char), sizeof(SIMUL_RAM), fd);
        fclose(fd);

        //Leemos el contenido del fichero con las direcciones de memoria
        fd=fopen("accesos_memoria.txt", "r");
        while((fscanf(fd, "%x", &addr)) && (!feof(fd))){
            
            ParsearDireccion(addr, &ETQ, &palabra, &linea, &bloque);

            if(tbl[linea].ETQ != ETQ){//fallo cache

                numFallos++;
                printf("\nT: %d, Fallo de caché %d, addr %04X etq %X linea %02X palabra %02X bloque %02X", globalTime, numFallos, addr, ETQ, linea, palabra, bloque);
                globalTime += 20;

                printf("\nCargando el bloque %02X en la línea %02X", bloque, linea);
                TratarFallo(tbl, SIMUL_RAM, ETQ, linea, bloque);
            }
            numAccesos++;
            printf("\nT: %d, Acierto de caché %d, addr %04X etq %X linea %02X dato %02X", globalTime, numAccesos, addr, ETQ, linea, tbl[linea].Data[(addr % TAM_LINEA)]);

            VolcarCACHE(tbl);
            sleep(1);
        }
        fclose(fd);

        printf("\nAccesos totales: %d; fallos: %d; tiempo medio: %f\n", numAccesos, numFallos, (globalTime/(float)numAccesos));
        printf("Texto leido: %s\n", texto);

        volcarContenido("CONTENTS_CACHE.bin", tbl);
    }
}

int comprobarFichero(char *fich){
    int existe = 0;
    FILE *fd=fopen(fich, "r");
    if(fd==NULL){
        printf("El fichero %s no existe\n", fich);
        existe = -1;
    }
    fclose(fd); 
    return existe;
}

void LimpiarCACHE(T_CACHE_LINE tbl[NUM_FILAS]) {

    for (int i = 0; i < NUM_FILAS; i++) {
        tbl[i].ETQ = 0xFF;
        for (int j = 0; j < TAM_LINEA; j++)
            tbl[i].Data[j] = 0x23;
    }
}

void VolcarCACHE(T_CACHE_LINE* tbl){

    for (int i = 0; i < NUM_FILAS; i++) {
        printf("\n%02X \t Datos:", tbl[i].ETQ);
        for (int j = 0; j < TAM_LINEA; j++) {
            printf("%02X ", tbl[i].Data[j]);
        }
    }
    printf("\n");
}

void ParsearDireccion(unsigned int addr, int *ETQ, int *palabra, int *linea, int *bloque){

    *ETQ = (addr & 0b111110000000)>>7;
    *palabra = (addr & 0b000000001111);
	*linea = (addr & 0b000001110000)>>4;
	*bloque = (addr & 0b111111110000)>>4;
}

void TratarFallo(T_CACHE_LINE *tbl, char *MRAM, int ETQ,int linea, int bloque){

    tbl[linea].ETQ=ETQ;

    for(int i=0; i<TAM_LINEA; i++){
        tbl[linea].Data[i]=MRAM[bloque*TAM_LINEA+15-i];
            texto[numCaracteres]=tbl[linea].Data[i];
            numCaracteres++;
    }
}

void volcarContenido(char *fich, T_CACHE_LINE *tbl){
    FILE *fd;
    fd=fopen(fich, "wb");
    for(int i=0; i<NUM_FILAS; i++){
        for(int j=0; j<TAM_LINEA; j++){
            fprintf(fd, "%c", tbl[i].Data[j]);
        }
    }
    fclose(fd);
}