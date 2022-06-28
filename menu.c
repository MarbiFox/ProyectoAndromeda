#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <windows.h>
#include <assert.h>
#include <time.h>
#include "list.h"


SDL_Surface * loadImage (char * filePath) {

    SDL_Surface * surface = IMG_Load(filePath);
    if (surface == NULL) {
        printf("Error: %s\n", SDL_GetError());
    }

    return surface;
}

SDL_Surface * optimizeSurface (char * filePath, SDL_Surface * mSurface) {

    SDL_Surface * optimizedSurface = NULL;
    SDL_Surface * surface = IMG_Load(filePath);
    if (surface == NULL) {
        printf("Error: %s\n", SDL_GetError());
    }
    else {
        optimizedSurface = SDL_ConvertSurface(surface, mSurface->format, 0);
        if (optimizedSurface == NULL) {
            printf("Error: %s\n", SDL_GetError());
        }
    }

    SDL_FreeSurface(surface);
    return optimizedSurface;
}

void startOption (int option, bool * runGame) {
    switch (option)
    {
    case 1:
        // startGame();
        break;
    case 2:
        // showRankings();
        break;
    case 3:
        // showOptions();
        break;
    default:
        (*runGame) = false;
        break;
    }
}

void createMenu () {
    //Iniciar SDL
    SDL_Init (SDL_INIT_VIDEO);
    //Iniciar SDL_Image
    int imgFlag = IMG_INIT_PNG | IMG_INIT_JPG;
    if (!(IMG_Init(imgFlag) & imgFlag)) {
        printf("Error: %s\n", IMG_GetError());
    }

    //Crear Ventana, Textura Principal y Render Principal.
    SDL_Window * window = SDL_CreateWindow("MainMenu", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 580, SDL_WINDOW_SHOWN);
    SDL_Surface * mainSurface = SDL_GetWindowSurface(window);
    SDL_Renderer * renderer = NULL;
    SDL_Surface * button = NULL;

    //Declarar Timer.
    float frameTime = 0; //Tiempo Transcurrido.
    int prevTime = 0;    //Tiempo del Frame Pasado.
    int currentTime = 0; //Tiempo del Frame Actual.
    float deltaTime = 0; //Diferencia de Tiempo entre el Frame Actual y el Pasado.
    int milisec = 0; //Contador entero para eventos. (Cuenta Milisegundos).

    //Definir Límites de la Ventana.
    SDL_Rect windowLimit;
    windowLimit.x = windowLimit.y = 0;
    windowLimit.w = 650;
    windowLimit.h = 580;

    //Colocar Pantalla Principal
    char * filePath = "ScreenMenu./OP0.jpg";
    button = optimizeSurface(filePath, mainSurface);
    SDL_BlitScaled(button, NULL, mainSurface, &windowLimit);
    SDL_UpdateWindowSurface(window);

    //Crear Arreglo de Opciones:
    char ** optionMenu;
    optionMenu = (char**) malloc (4*sizeof(char*));
    for (int i = 0; i < 4; i++) {
        optionMenu[i] = (char*) malloc (20*sizeof(char));
    }


    //Llenar Arreglo:
    char * fileName = "ScreenMenu./OP0.jpg";
    printf("%s\n", fileName);
    for (int k = 1; k < 5; k++) {
        switch (k) {
        case 1:
            fileName = "ScreenMenu./OP1.jpg";
            printf("%s\n", fileName);
            break;
        case 2:
            fileName = "ScreenMenu./OP2.jpg";
            printf("%s\n", fileName);
            break;
        case 3:
            fileName = "ScreenMenu./OP3.jpg";
            printf("%s\n", fileName);
            break;
        case 4:
            fileName = "ScreenMenu./OP4.jpg";
            printf("%s\n", fileName);
            break;
        
        }
        optionMenu[k-1] = fileName;
    }

    //Establecer que la aplicación está corriendo
    bool isRunning = true;
    //Crear un evento. (Cuando algo ocurre de forma externa).
    SDL_Event mainEv;
    printf("msdklana");
    
    //Iniciar el Arreglo de opciones para el menú.
    int currentScreen = -1;

    //Ciclo de Ejecución del programa principal (Ventana). Ejecuta la ventana mientras isRunning sea True.
    while (isRunning) {

        while (SDL_PollEvent(&mainEv) != 0) { // Marca si el usuario pulsó una tecla o click.
            if(mainEv.type == SDL_QUIT) { //Marca si se pulsó el ícono de salir.
                isRunning = false;
            }
            else if (mainEv.type == SDL_KEYDOWN) { //keyDown es para pulsar, keyup es para mantener y soltar.
                switch (mainEv.key.keysym.sym)
                {
                case SDLK_DOWN:
                    if (currentScreen < 2) {
                        currentScreen += 2;
                    }
                    break;
                case SDLK_RIGHT:
                    if (currentScreen != 1 && currentScreen < 3) {
                        currentScreen += 1;
                    }
                    break;
                case SDLK_LEFT:
                    if (currentScreen > 0 && currentScreen != 2) {
                        currentScreen -= 1;
                    }
                    break;
                case SDLK_UP:
                    if (currentScreen > 1) {
                        currentScreen -= 2;
                    }
                    break;
                case SDLK_RETURN:
                    printf("potato");
                    if (currentScreen == -1) {
                        currentScreen += 1;
                    }
                    else {
                        startOption(currentScreen+1, &isRunning);
                    }
                    break;
                default:
                    printf("default");
                }
                
                if (currentScreen != -1) {
                    button = optimizeSurface(optionMenu[currentScreen], mainSurface);
                    SDL_BlitScaled(button, NULL, mainSurface, &windowLimit);
                    SDL_UpdateWindowSurface(window);
                }
                
            }
        }
        
        // printf("%d\n", currentScreen);
    }
    
    //Liberar memoria de los componentes.
    SDL_DestroyWindow(window);
    window = NULL;
    SDL_FreeSurface(button);
    button = mainSurface = NULL;

    //Cerrar SDL.
    SDL_Quit();
}

int main(int argc, char** argv){
    //Iniciar el Menú
    createMenu();
    return 0;
}