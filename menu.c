#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <windows.h>
#include <assert.h>
#include <time.h>
#include "list.h"


char * selectFile (int * limit, int num) {
    //Variable del Nombre del Archivo a tomar.
    char * name = NULL;
    switch (num)
    {
    case 1:
        name = "SpaceBG./space.txt";
        (*limit) = 64;
        return name;
        break;
    }

    return NULL;
}

char ** createFrameList (char ** nList, int num) {
    
    //Definir Constantes para Importar.;
    int limit = 0;
    //Utilizar un switch para ver que archivo importar.
    char * fileName = selectFile(&limit, num);
    
    //Reservar Memoria
    nList = (char**) malloc (limit * sizeof(char*));
    for (int i = 0; i < limit; i++) {
        nList[i] = (char*) malloc (20 * sizeof(char));
    }
    
    //Abrir Archivo de las imagenes.
    FILE * f = fopen(fileName, "r");
    if (f == NULL) exit(1);

    //Crear char para almacenar.
    char aux[1024];
    int cont = 0;

    //Ciclo de lectura del archivo .txt
    while (fgets(aux, 1023, f) != NULL) {
        for (int i = 0; i < strlen(aux); i++) {
            if (aux[i] == '\n') {
                aux[i] = '\0';
            }
        }
        strcpy(nList[cont], aux);
        cont++;
        printf("%s\n", aux);  //ACÁ ESTÁ EL PRINT
        printf("sdkadf\n"); //IFNEWIONFEWIOGJMEWOIGNEWIOGNRWEOIGNRWOGNW
    }
    fclose(f);
    return nList;
}

 SDL_Surface * getFrame(char ** nList, SDL_Surface * current, int * nFrame, int limit) {
    //Crear Dato tipo char para cargar el Surface.
    char * name = NULL;

    //Condiciones para avanzar de Frame.
    if (current == NULL || (*nFrame) == limit) {
        name = nList[0];
        (*nFrame) = 0;
    }
    name = nList[(*nFrame)];
    (*nFrame)++;

    //Obtener el Frame
    current = SDL_LoadBMP(name);
    if (current == NULL) {
        printf("%s", SDL_GetError());
    }
    printf("%s\n", name);
    printf("Wsdkmslkf\n"); //AMKFLDSNFKOSENJIDANFJODASFNMDKSOGFMKODSFMOASK
    return current;
}

void createMenu () {
    //Iniciar SDL
    SDL_Init (SDL_INIT_VIDEO);

    //Crear Ventana
    SDL_Window *window = SDL_CreateWindow("MainMenu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 580, SDL_WINDOW_SHOWN);

    //Crear Surfaces.
    SDL_Surface * mainSurf = NULL;
    
    //Crear Arreglo de Frames.
    char ** nameList = NULL;
    int numberFile = 1;
    nameList = createFrameList(nameList, numberFile); //Inicializar.
    SDL_Surface * current = NULL; //Crear Imagen de Ventana.
    int nFrame = 64; //Numero de Frame actual.
    current = getFrame(nameList, current, &nFrame, 64);
    printf("msdklana"); // OIFEWDNGIJNGIOFEMGERMGOIPERMGPOERGNPERIONGO

    //Actualizar imagenes.
    mainSurf = SDL_GetWindowSurface(window);
    
    //Establecer que la aplicación está corriendo
    bool isRunning = true;
    //Crear un evento. (Cuando algo ocurre de forma externa).
    SDL_Event mainEv;
    printf("msdklana");
    //Ciclo de Ejecución del programa principal (Ventana). Ejecuta la ventana mientras isRunning sea True.
    while (isRunning) {
        while (SDL_PollEvent(&mainEv) != 0) { // Marca si el usuario pulsó una tecla o click.
            if(mainEv.type == SDL_QUIT) { //Marca si se pulsó el ícono de salir.
                isRunning = false;
            }
            // else if (mainEv.type == SDL_KEYDOWN) { //keyDown es para pulsar, keyup es para mantener y soltar.
            //     switch (mainEv.key.keysym.sym)
            //     {
            //     case SDLK_DOWN:
            //         current = surfOne;
            //         break;
            //     case SDLK_RIGHT:
            //         current = surfTwo;
            //         break;
            //     case SDLK_LEFT:
            //         current = surfThree;
            //         break;
            //     }
            // }
        }
        printf("msdklana");
        //Programar Animación Inicial.
        SDL_Delay(80);
        SDL_FreeSurface(current);
        current = getFrame(nameList, current, &nFrame, 64);
        SDL_BlitSurface(current, NULL, mainSurf, NULL);
        //Mostrar la Ventana
        SDL_UpdateWindowSurface(window);

    }
    
    SDL_DestroyWindow(window);

    current = NULL;
    window = NULL;

    SDL_Quit();
}

int main(int argc, char** argv)
{
    createMenu();
    //Menu(argc,argv);
    return EXIT_SUCCESS;
}
