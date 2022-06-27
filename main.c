#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <windows.h>
#include <SDL2/SDL_image.h>
#include <assert.h>
#include <time.h>
#include "list.h"

#define MenuInicio 1
#define MenuFin 5
#define LineaDeInicio 1

#define Largo 580
#define Ancho 640

#define MAX 10000
#define SIZE 100
#define NUMS_TO_GENERATE 1

typedef struct Nave Nave;
typedef struct Misil Misil;
typedef struct Punto Punto;
typedef struct Entidad Entidad;
typedef struct Usuario Usuario;
void navePintar(Nave*,SDL_Renderer*,Entidad**);
void naveIzquierdaDerecha(Nave*);
void naveDispara(Nave*);
void misilAvanza(Misil *);
void borrarMisiles(Misil *);
void dibujarlineaInferior(Punto *,SDL_Renderer *);
void moverEntidades(SDL_Renderer *,SDL_Texture *,SDL_Texture *,SDL_Rect* ,Entidad**,int*,int*,int*,int*);
void entidadMover();
void moverEntidadDerecha();
void goy(int);
void disparoEntidades(Entidad **,SDL_Renderer*);

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

    SDL_Rect rectangulo;
    int vida;
    bool permitidoDisparar;
    int razonDisparo;

    Misil *misiles;
};

int BORRARCONTADOR = 0;

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

    //CARGAR IMAGENES INVADER1 INVADER2

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

    SDL_Texture *imagen1 = SDL_CreateTextureFromSurface(renderer,image);
    SDL_Texture *imagen2 = SDL_CreateTextureFromSurface(renderer,image2);
    SDL_Rect *texture_destination;


    if (window == NULL){
        printf("ERROR");
        return EXIT_FAILURE;
    }

    startTime = SDL_GetTicks64();
    SDL_Rect screenRectangle = {0,0,580,640};
    //SDL_Rect imageRectangle = {100,150,30,30};

    int cont = 0;

    Entidad **matriz = (Entidad**)malloc(5 * sizeof(Entidad*));

    for (int i = 0 ; i < 5 ; i++){
        matriz[i] = (Entidad*)malloc(11 * sizeof(Entidad));
    }



    int eje_x = 110;
    int eje_y = 100;

    int direccion_x = 1;
    int direccion_y = 1;
    int inicio = 1;

    //Inicializar
    for (int i = 0 ; i < 5 ; i++){
        for (int j = 0 ; j < 11 ; j++){
            matriz[i][j].rectangulo.x = eje_x;
            matriz[i][j].rectangulo.y = eje_y;
            matriz[i][j].rectangulo.h = 25;
            matriz[i][j].rectangulo.w = 25;
            matriz[i][j].vida = 1;
            matriz[i][j].permitidoDisparar = false;
            if (i == 4){
                matriz[i][j].permitidoDisparar = true;
            }
            eje_x += 40;
        }
        eje_x = 110;
        eje_y += 50;
    }

    while (running){
        while (SDL_PollEvent(&event)){
                if (event.type == SDL_QUIT){
                    running = false;
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

                    //PONER COULDOWN
                    naveDispara(&nave);
                }
            }
            borrarMisiles(nave.misiles);

            SDL_UpdateWindowSurface(window);

            SDL_SetRenderDrawColor(renderer,0,0,0,0);

            SDL_RenderClear(renderer);

            moverEntidades(renderer,imagen1,imagen2,&screenRectangle,matriz,&cont,&direccion_x,&direccion_y,&inicio);

            SDL_SetRenderDrawColor(renderer,0,255,12,0);

            navePintar(&nave,renderer,matriz);

            disparoEntidades(matriz,renderer);


            //dibujarlineaInferior(&linea,renderer);
            SDL_RenderPresent(renderer);

            SDL_Delay(10);
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_FreeSurface(image);
    SDL_FreeSurface(image2);
    SDL_DestroyTexture(imagen1);
    SDL_Quit();
    IMG_Quit();
}

void navePintar(Nave *nave,SDL_Renderer *renderer,Entidad**matriz){
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

    int coordenadaEjeX,coordenadaEjeY;
    //Pintar misiles
    Misil *auxMisil = nave->misiles;

    while (auxMisil != NULL){

        misilAvanza(auxMisil);

        SDL_RenderDrawLine(renderer,auxMisil->x1,auxMisil->y1,auxMisil->x2,auxMisil->y2);
        SDL_RenderDrawLine(renderer,auxMisil->x1,auxMisil->y1,auxMisil->x2,auxMisil->y2);

        //Colisiones
        for (int i = 0 ; i < 5 ; i++){
            for (int j = 0 ; j < 11 ; j++){

                coordenadaEjeX = matriz[i][j].rectangulo.x;
                coordenadaEjeY = matriz[i][j].rectangulo.y;

                if (matriz[i][j].vida != 0){
                    if (coordenadaEjeX <= auxMisil->x1 && coordenadaEjeX+25 >= auxMisil->x1){
                        if (coordenadaEjeY <= auxMisil->y1 && coordenadaEjeY+25 >= auxMisil->y1){

                            //ELIMINAR
                            auxMisil->x1 = 0;
                            auxMisil->y1 = 0;
                            auxMisil->x2 = 0;
                            auxMisil->y2 = 0;

                            matriz[i][j].vida = 0;

                            if (auxMisil->siguiente != NULL){
                                auxMisil->siguiente->x1 = 0;
                                auxMisil->siguiente->y1 = 0;
                                auxMisil->siguiente->x2 = 0;
                                auxMisil->siguiente->y2 = 0;
                            }
                        }
                    }
                }
            }
        }
        auxMisil = auxMisil->siguiente;
    }
}

void naveIzquierdaDerecha(Nave *nave){

    if (nave->x5 > 5 && nave->x3 < 640){
        nave->x1 += nave->velocidad_x;
        nave->x2 += nave->velocidad_x;
        nave->x3 += nave->velocidad_x;
        nave->x4 += nave->velocidad_x;
        nave->x5 += nave->velocidad_x;
        nave->x6 += nave->velocidad_x;
        nave->x7 += nave->velocidad_x;
    }
    //BORDES
    if (nave->x5 == 5){
        nave->x1 -= nave->velocidad_x;
        nave->x2 -= nave->velocidad_x;
        nave->x3 -= nave->velocidad_x;
        nave->x4 -= nave->velocidad_x;
        nave->x5 -= nave->velocidad_x;
        nave->x6 -= nave->velocidad_x;
        nave->x7 -= nave->velocidad_x;
    }
    if (nave->x3 == 635){
        nave->x1 -= nave->velocidad_x;
        nave->x2 -= nave->velocidad_x;
        nave->x3 -= nave->velocidad_x;
        nave->x4 -= nave->velocidad_x;
        nave->x5 -= nave->velocidad_x;
        nave->x6 -= nave->velocidad_x;
        nave->x7 -= nave->velocidad_x;
    }
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
void mover(SDL_Renderer *renderer,SDL_Texture *imagen1,SDL_Texture *imagen2,SDL_Rect *screenRectangle,Entidad **Matriz,int*cont){


}

void mover_ejeX(Entidad **matriz,int *direccion_x){
    for(int i = 0 ; i < 5 ; i++){
        for (int j = 0 ; j < 11; j++){
            if ((*direccion_x) == 1){
                matriz[i][j].rectangulo.x += 8;
            }
            else{
                matriz[i][j].rectangulo.x -= 8;
            }
        }
    }
}

void mover_ejeY(Entidad **matriz,int *direccion_x){
    for (int i = 0 ; i < 5 ; i++){
        for (int j = 0 ; j < 11 ; j++){
            if ((*direccion_x) == 1){
                matriz[i][j].rectangulo.x -= 8;
                matriz[i][j].rectangulo.y += 10;
            }
            else{
                matriz[i][j].rectangulo.x += 8;
                matriz[i][j].rectangulo.y += 10;
            }
        }
    }
}

float velocidadEntidades(){
    float time = Time() / 1000.f;
    //AUMENTAR VELOCIDAD
    if ((int)time > 10){
        time *= 1.2;
    }
    if ((int)time > 20){
        time *= 1.2;
    }
    if ((int)time > 30){
        time *= 1.2;
    }
    if ((int)time > 40){
        time *= 1.2;
    }
    if ((int)time > 50){
        time *= 1.2;
    }
    if ((int)time > 60){
        time *= 1.2;
    }
    if ((int)time > 120){
        time *= 1.6;
    }
    if ((int)time > 210){
        time *= 1.9;
    }
    if ((int)time > 500){
        time *= 2.2;
    }
    return time;
}


void moverEntidades(SDL_Renderer *renderer,SDL_Texture *imagen1,SDL_Texture *imagen2,SDL_Rect *screenRectangle,Entidad **matriz,int*cont,int *direccion_x,int *direccion_y,int *inicio){

    float time = velocidadEntidades();
    for (int i = 0 ; i < 5 ; i++){
        for (int j = 0 ; j < 11 ; j++){
            if ((int)time % 2 == 0){
                if((*cont) % 2 != 0){
                    mover_ejeX(matriz,direccion_x);
                    (*cont)++;
                }
                //CASO INICIO
                if ((*cont) == 12 && (*inicio) == 1){
                    mover_ejeY(matriz,direccion_x);
                    (*direccion_x) *= -1;
                    (*cont) = 0;
                    (*inicio) = 0;
                }
                //CASO PROMEDIO
                if ((*cont) == 24){
                    mover_ejeY(matriz,direccion_x);
                    (*direccion_x) *= -1;
                    (*cont) = 0;
                }
                if (matriz[i][j].vida != 0){
                    SDL_RenderCopy(renderer,imagen1,screenRectangle,&matriz[i][j].rectangulo);
                }
            }
            else
            {
                if((*cont) % 2 == 0){
                    mover_ejeX(matriz,direccion_x);
                    (*cont)++;
                }
                //CASO INICIO
                if ((*cont) == 12 && (*inicio) == 1){
                    mover_ejeY(matriz,direccion_x);
                    (*direccion_x) *= -1;
                    (*cont) = 0;
                    (*inicio) = 0;
                }
                //CASO PROMEDIO
                if ((*cont) == 24){
                    mover_ejeY(matriz,direccion_x);
                    (*direccion_x) *= -1;
                    (*cont) = 0;
                }
                if (matriz[i][j].vida != 0){
                    SDL_RenderCopy(renderer,imagen2,screenRectangle,&matriz[i][j].rectangulo);
                }
            }
        }
    }
    if ((int)time > 2){

    }
}

void crearDisparoEntidad(Entidad *naveEnemiga){
    if (naveEnemiga->misiles == NULL){
        naveEnemiga->misiles = (Misil*)SDL_malloc(sizeof(Misil));
        naveEnemiga->misiles->x1 = naveEnemiga->rectangulo.x;
        naveEnemiga->misiles->y1 = naveEnemiga->rectangulo.y;
        naveEnemiga->misiles->x2 = naveEnemiga->rectangulo.x;
        naveEnemiga->misiles->y2 = naveEnemiga->rectangulo.y - 5;
        naveEnemiga->misiles->velocidad_x = 0;
        naveEnemiga->misiles->velocidad_y = -5;
        naveEnemiga->misiles->siguiente = NULL;
    }
    else{
        Misil *auxMisil = naveEnemiga->misiles;
        while (auxMisil->siguiente != NULL){
            auxMisil = auxMisil->siguiente;
        }
        auxMisil->siguiente = (Misil*)SDL_malloc(sizeof(Misil));
        auxMisil = auxMisil->siguiente;
        auxMisil->x1 = naveEnemiga->rectangulo.x;
        auxMisil->y1 = naveEnemiga->rectangulo.y;
        auxMisil->x2 = naveEnemiga->rectangulo.x;
        auxMisil->y2 = naveEnemiga->rectangulo.y - 5;
        auxMisil->velocidad_x = 0;
        auxMisil->velocidad_y = -5;
        auxMisil->siguiente = NULL;
    }
}

void disparoEntidades(Entidad **matriz,SDL_Renderer *renderer){
    for (int i = 0 ; i < 5 ; i++){
        for (int j = 0 ; j < 11 ; j++){
            if (matriz[i][j].permitidoDisparar == true){
                matriz[i][j].razonDisparo = rand() % MAX;

                //CREAR DISPAROS
                if (matriz[i][j].razonDisparo < 4){
                    printf("[DISPARO ENEMIGO]\n");
                    //crearDisparoEntidad(&matriz[i][j]);
                }
            }
        }
    }
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
