#include<stdio.h>
#include<stdlib.h>

int main(int argc, char *argv[]){
        //direccion de 16 bits
        //4 bits para indentificar la palabra
        unsigned int address=0b0000110111000011;
        unsigned int word=address&0b0000000000001111;
        unsigned int block=address>>4;
        unsigned int set=block&0b001111111111;
        unsigned int label_from_block=block>>10;
        printf("Direccion %x(h) con palabra %d(d), %X(H)\n", address, word, word);
        printf("Nº de bloque %d(d), %X(H)\n", block, block);
        printf("Nº de conjunto %d(d), %X(H)\n", set, set);
        printf("Etiquieta %d(d), %X(H)", label_from_block, label_from_block);
        return 0;
}