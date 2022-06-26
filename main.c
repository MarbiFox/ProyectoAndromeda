#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <windows.h>
// #include <SDL2/SDL_image.h>
#include <assert.h>
#include <time.h>
#include "list.h"

#define MenuInicio 1
#define MenuFin 5
#define LineaDeInicio 1

typedef struct Nave Nave;
typedef struct Misil Misil;
typedef struct Punto Punto;
typedef struct Entidad Entidad;
typedef struct Usuario Usuario;
void navePintar(Nave*,SDL_Renderer*);
void naveIzquierdaDerecha(Nave*);
void naveDispara(Nave*);
void misilAvanza(Misil *);
void borrarMisiles(Misil *);
void dibujarlineaInferior(Punto *,SDL_Renderer *);
void moverEntidades(SDL_Renderer *,SDL_Texture *,SDL_Texture *,SDL_Rect *);
void entidadMover();
void moverEntidadDerecha();
void goy(int);

UINT64 Time();
UINT64 startTime;

struct Usuario{
    char user[4];
    int puntaje;
};


struct Misil{
    int x1,y1;
    int x2,y2;
    int velocidad_x,velocidad_y;
    Misil *siguiente;
};

struct Nave{
    int x1,y1;
    int x2,y2;
    int x3,y3;
    int x4,y4;
    int x5,y5;
    int x6,y6;
    int x7,y7;
    int velocidad_x, velocidad_y;
    Misil *misiles;
};

struct Punto{
    int x1,y1;
    int x2,y2;
};

struct Entidad{
    int vida;
    int x1,y1;
    int velocidad_x,velocidad_y;
    Misil *misiles;
};



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
        return 1;
    }


    char* alias=buscarPalabra(rank);
    int score=0;


    while (alias) {

        Usuario *d=(Usuario*)malloc(sizeof(Usuario));

        strcpy(d->user,alias);
        d->puntaje=convertNum(buscarPalabra(rank));

        insertarListaRanking(l,d);

        alias=buscarPalabra(rank);
    }
    printf("[FUNCION OBTENER RANKINGS]\n");
    system("pause");
    fclose(rank);
    return l;

}

void SDL(){
    SDL_Window *window = NULL;
    SDL_Renderer *renderer = NULL;

    int largo = 580;
    int ancho = 640;
    int size_x = 650;
    int size_y = 850;

    if (SDL_Init(SDL_INIT_VIDEO) < 0){
        printf("ERROR\n");
        return EXIT_FAILURE;
    }

    window = SDL_CreateWindow("Ventana",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,ancho,largo,SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    //windowSurface = SDL_GetWindowSurface(window);
    //TTF_Font*sans = TTF_OpenFont("sans.ttf",24);

    Punto linea = {size_x/2+360,size_y/2-350,size_x/2-360,size_y/2-350};

    SDL_Event event;
    bool running = true;
    const unsigned char *keys = SDL_GetKeyboardState(NULL);
    int typeEvent;


    Nave nave = {size_x/2,size_y/2+70,
                size_x/2+5,size_y/2+80,
                size_x/2+20,size_y/2+80,
                size_x/2+20,size_y/2+90,
                size_x/2-20,size_y/2+90,
                size_x/2-20,size_y/2+80,
                size_x/2-5,size_y/2+80,5,5,NULL};

    int flag = IMG_Init(IMG_INIT_JPG);
    int initStatus = IMG_Init(flag);
    if ((initStatus && flag) != flag){
        printf("ERROR\n");
    }
    SDL_Surface *image;
    image = IMG_Load("invader.jpg");
    if (!image){
        printf("IMG_LOAD: %s",IMG_GetError());
    }
    SDL_Surface *image2;
    image2 = IMG_Load("invader2.jpg");
    if (!image2){
        printf("IMG_LOAD: %s",IMG_GetError());
    }

    SDL_Texture *imagen2 = SDL_CreateTextureFromSurface(renderer,image2);
    SDL_Texture *imagen1 = SDL_CreateTextureFromSurface(renderer,image);
    SDL_Rect *texture_destination;


    if (window == NULL){
        printf("ERROR");
        return EXIT_FAILURE;
    }

    startTime = SDL_GetTicks64();
    SDL_Rect screenRectangle = {0,0,580,640};
    SDL_Rect imageRectangle = {100,150,30,30};

    int cont = 0;

    Entidad **matriz = (Entidad**)malloc(5 * sizeof(Entidad*));

    for (int i = 0 ; i < 5 ; i++){
        matriz[i] = (Entidad*)malloc(11 * sizeof(Entidad));
    }
    Entidad naveEnemiga;

    for (int i = 0 ; i < 5 ; i++){
        for (int j = 0 ; j < 11 ; j++){
            matriz[i][j] = naveEnemiga;
        }
    }

    while (running){
        while (SDL_PollEvent(&event)){
                if (event.type == SDL_QUIT){
                    running = false;
                }
                else if(typeEvent == SDL_SCANCODE_SPACE){
                    if (GetAsyncKeyState(VK_SPACE)){
                        naveDispara(&nave);
                    }
                }
                else if (keys[SDL_SCANCODE_LEFT]){
                    nave.velocidad_x = -abs(nave.velocidad_x);
                    naveIzquierdaDerecha(&nave);
                }
                else if(keys[SDL_SCANCODE_RIGHT]){
                    nave.velocidad_x = +abs(nave.velocidad_x);
                    naveIzquierdaDerecha(&nave);
                }
                else if (keys[SDL_SCANCODE_SPACE]){
                    naveDispara(&nave);
                }
            }
            borrarMisiles(nave.misiles);

            float time = Time() / 1000.f;

            //imageRectangle.x = Time() * 0.1f;

            SDL_UpdateWindowSurface(window);

            SDL_SetRenderDrawColor(renderer,0,0,0,0);

            SDL_RenderClear(renderer);


            if ((int)time % 2 == 0){
                if(cont % 2 != 0){
                    imageRectangle.x += 5;
                    cont++;
                }
                SDL_RenderCopy(renderer,imagen1,&screenRectangle,&imageRectangle);
            }
            else
            {
                if(cont % 2 == 0){
                    imageRectangle.x += 5;
                    cont++;
                }
                SDL_RenderCopy(renderer,imagen2,&screenRectangle,&imageRectangle);
            }

            //moverEntidades(renderer,imagen1,imagen2,texture_destination);

            //SDL_RenderCopy(renderer,imagen1,NULL,&texture_destination);

            SDL_SetRenderDrawColor(renderer,0,255,12,0);

            navePintar(&nave,renderer);


            //dibujarlineaInferior(&linea,renderer);
            SDL_RenderPresent(renderer);


            SDL_Delay(10);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_FreeSurface(image);
    SDL_DestroyTexture(imagen1);
    SDL_Quit();
    IMG_Quit();
}

void navePintar(Nave *nave,SDL_Renderer *renderer){
    SDL_Point points[8] = {
        {nave->x1,nave->y1},
        {nave->x2,nave->y2},
        {nave->x3,nave->y3},
        {nave->x4,nave->y4},
        {nave->x5,nave->y5},
        {nave->x6,nave->y6},
        {nave->x7,nave->y7},
    };
    points[7] = points[0];
    SDL_RenderDrawLines(renderer,points,8);

    //Pintar misiles
    Misil *auxMisil = nave->misiles;
    while (auxMisil != NULL){
        misilAvanza(auxMisil);
        SDL_RenderDrawLine(renderer,auxMisil->x1,auxMisil->y1,auxMisil->x2,auxMisil->y2);
        auxMisil = auxMisil->siguiente;
    }
}

void naveIzquierdaDerecha(Nave *nave){
    nave->x1 += nave->velocidad_x;
    nave->x2 += nave->velocidad_x;
    nave->x3 += nave->velocidad_x;
    nave->x4 += nave->velocidad_x;
    nave->x5 += nave->velocidad_x;
    nave->x6 += nave->velocidad_x;
    nave->x7 += nave->velocidad_x;

}

void naveDispara(Nave *nave){
    if (nave->misiles == NULL){
        nave->misiles = (Misil*)SDL_malloc(sizeof(Misil));
        nave->misiles->x1 = nave->x1;
        nave->misiles->y1 = nave->y1;
        nave->misiles->x2 = nave->x1;
        nave->misiles->y2 = nave->y1 - 5;
        nave->misiles->velocidad_x = 0;
        nave->misiles->velocidad_y = -5;
        nave->misiles->siguiente = NULL;
    }
    else{
        Misil *auxMisil = nave->misiles;
        while (auxMisil->siguiente != NULL){
            auxMisil = auxMisil->siguiente;
        }
        auxMisil->siguiente = (Misil*)SDL_malloc(sizeof(Misil));
        auxMisil = auxMisil->siguiente;
        auxMisil->x1 = nave->x1;
        auxMisil->y1 = nave->y1;
        auxMisil->x2 = nave->x1;
        auxMisil->y2 = auxMisil->y1 - 5;
        auxMisil->velocidad_x = 0;
        auxMisil->velocidad_y = -5;
        auxMisil->siguiente = NULL;
    }
}

void misilAvanza(Misil *misil){
    misil->x1 += misil->velocidad_x;
    misil->y1 += misil->velocidad_y;
    misil->x2 += misil->velocidad_x;
    misil->y2 += misil->velocidad_y;
}

void borrarMisiles(Misil *misiles){
    Misil *auxMisil = misiles;
    Misil *auxiliar_misiles;

    if (auxMisil == NULL){
        return;
    }
    while (auxMisil->siguiente != NULL){
        if (auxMisil->siguiente->y1 < 0){
            auxiliar_misiles = auxMisil->siguiente->siguiente;
            SDL_free(auxMisil->siguiente);
            auxMisil->siguiente = auxiliar_misiles;
        }else{
                auxMisil = auxMisil->siguiente;
            }
        }
}

void dibujarlineaInferior(Punto *punto,SDL_Renderer *renderer){
    SDL_Point points[3] = {
        {punto->x1,punto->y1},
        {punto->x2,punto->y2},
    };
    points[2] = points[0];
    SDL_RenderDrawLines(renderer,points,3);
}

void moverEntidades(SDL_Renderer *renderer,SDL_Texture *imagen1,SDL_Texture *imagen2,SDL_Rect *texture_destination){

    /*
    int posicion_x = 100;
    int posicion_y = 140;
    int width = 25;
    int length = 25;

    int posicion2_x = 100;
    int posicion2_y = 140;
    int width2 = 25;
    int length2 = 25;

    texture_destination->x = posicion_x;
    texture_destination->y = posicion_y;
    texture_destination->w = width;
    texture_destination->h = length;


    for (int i = 0 ; i < 11 ; i++){
        for (int j = 0 ; j < 5 ; j++){
            posicion_y += 35;
            texture_destination->x = posicion_x;
            texture_destination->y = posicion_y;
            SDL_RenderCopy(renderer,imagen1,NULL,texture_destination);

        }

        posicion_y = 140;
        posicion_x += 40;
    }
    */

    //Necesito crear para cada nave un cuadrado, luego en cada cuadrado sobreescribir la imagen 1
    SDL_Rect rect;
    rect.x = 100;
    rect.y = 140;
    rect.h = 25;
    rect.w = 25;
    SDL_SetRenderDrawColor( renderer, 0, 0, 255, 255 );
    SDL_RenderFillRect( renderer, &rect);
    SDL_RenderPresent(renderer);


}

void ententidadMover(){

}

//FUNCION MENU PRINCIPAL
void Menu(){
    system("COLOR 9");
    int menu;
    printf("--------PROTOTIPO MENU BASICO--------v1.0\n");
    goy(LineaDeInicio);
    printf("\t[1]Jugar\n");
    printf("\t[2]Dificultad\n");
    printf("\t[3]Tutorial\n");
    printf("\t[4]Rankings\n");
    printf("\t[5]Salir\n");
    menu = 1;
    goy(LineaDeInicio);
    printf("---->");



    while (1){
        Sleep(200);
        if(GetAsyncKeyState(VK_UP)){
            menu == MenuInicio ? MenuFin: --menu;
            printf("\r     ");
            goy(LineaDeInicio + menu-1);
            printf("---->");
        }
        else{
            if (GetAsyncKeyState(VK_DOWN)){
                menu == MenuFin ? MenuInicio: ++menu;
                printf("\r     ");
                goy(LineaDeInicio + menu-1);
                printf("---->");
            }
            else{
                if (GetAsyncKeyState(VK_RETURN)){break;}
            }
        }
    }
    printf("\n\n\n\n");
    if (menu == 1){
        SDL();
    }
    if (menu == 2){
        system("pause");

        printf("[FUNCION DIFICULTAD]");
    }
    if (menu == 3){
        system("pause");
        //printf("[FUNCION DIFICULTAD]");
    }
    if (menu == 4){
        obtenerRanking();
        printf("[FUNCION MOSTRAR RANKINGS]");
    }
    if (menu == 5){
        printf("Opcion 5");
    }

    //CAMBIAR MENU A UNO MAS BONITO
    /*
    switch (menu){
    case 1:
        Ventana(argc,argv);
    case 2:
    */
}
void goy(int y){
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos;
    pos.X = 0;
    pos.Y = y;
    SetConsoleCursorPosition(hConsole, pos);
}

UINT64 Time(){
    return SDL_GetTicks64() - startTime;
}

void moverEntidadDerecha(){

}

int main(int argc, char **argv){
    Menu();
    return EXIT_SUCCESS;
}

