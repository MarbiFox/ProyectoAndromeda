#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <windows.h>
#include <assert.h>
#include "list.h"



unsigned int convertNum(char *val){

    //convierte los characteres en numeros

    int aux = 0;
    int digit = 0;
    int i = 0;

    while(*(val + i) != '\0'){
        digit *= 10;
        aux = (int) (*(val + i) - '0');
        digit += aux;
        i++;
    }
    return digit;
}

char* buscarPalabra (FILE *f) {

    //busca una palabra en el archivo

    char x[1024];
    if (fscanf(f, " %1023s", x) == 1)
        return strdup(x);
    else
    return NULL;
}

void insertarListaRanking(List* list,Usuario* data){

    /*inserta una varible usario(nombre y puntaje)  a la lista de manera que quede ordenada de mayor a menor
    ESTA FUNCION SOLO QUEDARA ORDENADA SI LA LISTA ESTA VACIA POR LO CUAL SE DEBEN INSERTAR LOS ELEMENTOS UNO POR UNO*/
    
    assert(list != NULL);
    Node * new = createNode(data);

    if(list->head==NULL){
        pushFront(list,data);
        return;
    }

    firstList(list);
    Usuario* current=list->current->data;

    while(current){

        if (current->puntaje<data->puntaje)
        {
            if(list->current==list->head){
                pushFront(list,data);
                break;
            }else{
                prevList(list);
                pushCurrent(list,data);
                break;
            }
        }

        if (list->current->next==NULL)
        {
            pushBack(list,data);
            return;
        }

        nextList(list);

        current=list->current->data; 
    }

}

List* obtenerRanking(){

    /*obtiene los datos globales de los resultados de quienes han jugado el juego y sus puntajes 
    para esto se lee el archivo ranking.txt el cual esta ordenada de esta manera: 

        [ALIAS][PUNTAJE]
            ABC 123
            BCA 321
            BAC 213

    CABE RECALCAR QUE EL ORDEN EN QUE SE GUARDEN LOS USUARIOS NO IMPORTA YA QUE UTILISA LA FUNCION insertarListaRanking() la cual ordena de manera automatica cuando se ingresa*/
    
    List* l=createList();

    FILE *rank;
    rank=fopen("rankings.txt","r");

    if(rank==NULL){
        printf("error al abrir el archivo\n");
        exit(1);
    }

    
    char* alias=buscarPalabra(rank);
    

    while (alias) {

        Usuario *d=(Usuario*)malloc(sizeof(Usuario));

        strcpy(d->user,alias);
        d->puntaje=convertNum(buscarPalabra(rank));
        d->nivel=convertNum(buscarPalabra(rank));

        insertarListaRanking(l,d);

        alias=buscarPalabra(rank);
    }

    fclose(rank);
    return l;

}

void exportarRanking(List *l){

    FILE * salida;
    salida = fopen("rankings.txt", "w");

    if (salida == NULL) {
        printf("Error al abrir archivo.txt\n");
        exit(1);
    }

    Usuario *val= firstList(l);
    
    while(val != NULL){
        fprintf(salida,"%s %i %i\n",val->user,val->puntaje,val->nivel);
        val = nextList(l);
    }

    fclose(salida);
}