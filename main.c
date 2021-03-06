#include <time.h>
#include "list.h"

#define MenuInicio 1
#define MenuFin 5
#define LineaDeInicio 1

#define Largo 580
#define Ancho 640

#define MAX 10000
#define SIZE 100
#define NUMS_TO_GENERATE 2
#define MAXVALORFILAS 4
#define MAXVALORCOLUMNAS 10

#define SIZE_X 650
#define SIZE_Y 850

typedef struct Nave Nave;
typedef struct Misil Misil;
typedef struct Punto Punto;
typedef struct Entidad Entidad;
typedef struct Usuario Usuario;
typedef struct Nivel Nivel;
void navePintar(Nave*,SDL_Renderer*,Entidad**,Usuario*,int,int,int*,Entidad *,int *,int*);
void naveIzquierdaDerecha(Nave*);
void naveDispara(Nave*);
void misilAvanza(Misil *);
void borrarMisiles(Nave *);
void dibujarlineaInferior(Punto *,SDL_Renderer *);
void moverEntidades(SDL_Renderer *,SDL_Texture *,SDL_Texture *,SDL_Rect* ,Entidad**,int,int,int*,int*,int*,int*,SDL_Window*);
void entidadMover();
void moverEntidadDerecha();
void goy(int);
Entidad dameEnemigo(Entidad**, int, int);
void iniciarMisiles();
void dibujarMisilesEntidades(Entidad*,SDL_Renderer*,int*,Nave*,Usuario*,SDL_Rect*,SDL_Window*);
void asignarPuntaje(Usuario*,int);
void misilAvanzaEntidad(Misil*);
void colisionDisparo_Usuario(Entidad *,Nave *,Usuario *,SDL_Rect *,SDL_Renderer *,SDL_Window *);
UINT64 Time();
Entidad **crearMatrizEntidad(int, int);
Nivel *createLevel(int ,int *);
List * GenerarNiveles(void);
void mostrarInformacionNiveles(List *);
void startGame(Entidad **, int, int,int,List*);
void gameover(SDL_Rect *,SDL_Renderer*,SDL_Window*);
void createMenu();


struct Nivel{
    Entidad **enemigo;
    int x,y;
    int nivel;
    int enemigos;
};

struct Usuario{
    char user[4];
    int puntaje;
    int nivel;
    int dificultad;
    int vidas;
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

Usuario* crearUsuario(){
    Usuario *user = (Usuario*)malloc(sizeof(Usuario));
    user->dificultad = 0;
    user->nivel = 0;
    user->puntaje = 0;
    user->vidas = 3;
    return user;
}

Entidad ** memoriaMatriz(){
    Entidad **matriz = (Entidad**)malloc(5 * sizeof(Entidad*));
    for (int i = 0 ; i < 5 ; i++){
        matriz[i] = (Entidad*)malloc(11 * sizeof(Entidad));
    }
    return matriz;
}

Entidad *memoriaBoss(){
    Entidad *boss = (Entidad*)malloc(sizeof(Entidad));
    return boss;
}

void inicializarBoss(Entidad *boss){
    boss->rectangulo.x = 280;
    boss->rectangulo.y = 10;
    boss->rectangulo.h = 80;
    boss->rectangulo.w = 80;
    boss->vida = 3;
    boss->permitidoDisparar = true;
}

void inicializarEntidades(Entidad **matriz){
    int eje_x = 110;
    int eje_y = 100;
    for (int i = 0 ; i < 5 ; i++){
        for (int j = 0 ; j < 11 ; j++){
            matriz[i][j].rectangulo.x = eje_x;
            matriz[i][j].rectangulo.y = eje_y;
            matriz[i][j].rectangulo.h = 25;
            matriz[i][j].rectangulo.w = 25;
            matriz[i][j].vida = 1;
            matriz[i][j].permitidoDisparar = true;
            eje_x += 40;
        }
        eje_x = 110;
        eje_y += 50;
    }
}

bool cooldown (Nave *nave){
    if (nave->misiles == NULL){
        return true;
    }
    else{
        if (nave->misiles->siguiente == 0){
            return true;
        }
    }
    return false;
}

void parametrosBoss(Entidad *nave){
    nave->misiles = NULL;
    if (nave->permitidoDisparar == true){
        if (nave->misiles == NULL){
            nave->misiles = (Misil*)SDL_malloc(sizeof(Misil));
            nave->misiles->x1 = nave->rectangulo.x + 40;
            nave->misiles->y1 = nave->rectangulo.y;
            nave->misiles->x2 = nave->rectangulo.x + 40;
            nave->misiles->y2 = nave->rectangulo.y - 5;
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
            auxMisil->x1 = nave->rectangulo.x + 40;
            auxMisil->y1 = nave->rectangulo.y;
            auxMisil->x2 = nave->rectangulo.x + 40;
            auxMisil->y2 = nave->rectangulo.y - 5;
            auxMisil->velocidad_x = 0;
            auxMisil->velocidad_y = -5;
            auxMisil->siguiente = NULL;
        }
    }
}

float velocidadBoss(){
    float time = Time() / 1000.f;
    //AUMENTAR VELOCIDAD
    if ((int)time > 5){
        time *= 1.8;
    }
    if ((int)time > 10){
        time *= 2.0;
    }
    if ((int)time > 15){
        time *= 2.3;
    }
    if ((int)time > 30){
        time *= 2.8;
    }

    return time;
}

void moverBossEjeX(Entidad*boss,int *direccionX){

    if ((*direccionX) == 1){
        boss->rectangulo.x += 8;
    }
    else{
        boss->rectangulo.x -= 8;
    }

}

void moverBossEjeY(Entidad*boss,int *direccionY){

    if ((*direccionY) == 1){
        boss->rectangulo.x -= 8;
        //boss->rectangulo.y += 20;
        if (boss->rectangulo.y >= 445 && boss->vida >= 0){
            printf("[GAMEOVER]\n");
        }
    }
    else{
        boss->rectangulo.x += 8;
        //boss->rectangulo.y += 20;
        if (boss->rectangulo.y >= 445 && boss->vida >= 0){
            printf("[GAMEOVER]\n");
        }
    }

}

void moverBoss(SDL_Rect *screenRectangle,Entidad *boss,int *cont,int *direccion_x,int *direccionY,int*inicio,SDL_Renderer *renderer,SDL_Texture*imagen1,SDL_Texture*imagen2){
    float time = velocidadBoss();
    if ((int)time % 2 == 0){
            if ((*cont) % 2 != 0){
                moverBossEjeX(boss,direccion_x);
                (*cont)++;
            }
            //CASO INICIO
            if ((*cont) == 36 && (*inicio) == 1){
                moverBossEjeY(boss,direccionY);
                (*direccion_x) *= -1;
                (*cont) = 0;
                (*inicio) = 0;
            }
            //CASO PROMEDIO
            if ((*cont) == 70){
                moverBossEjeY(boss,direccion_x);
                (*direccion_x) *= -1;
                (*cont) = 0;
            }
            if (boss->vida != 0){
                SDL_RenderCopy(renderer,imagen1,screenRectangle,&boss->rectangulo);
            }
        }
        else
        {
            if((*cont) % 2 == 0){
                moverBossEjeX(boss,direccion_x);
                (*cont)++;
            }
            //CASO INICIO
            if ((*cont) == 36 && (*inicio) == 1){
                moverBossEjeY(boss,direccionY);
                (*direccion_x) *= -1;
                (*cont) = 0;
                (*inicio) = 0;
            }
            //CASO PROMEDIO
            if ((*cont) == 70){
                moverBossEjeY(boss,direccionY);
                (*direccion_x) *= -1;
                (*cont) = 0;
            }
            if (boss->vida != 0){
                SDL_RenderCopy(renderer,imagen2,screenRectangle,&boss->rectangulo);
            }
        }
}

void dibujarMisilesBoss(Entidad *boss,Nave*naveUsuario,SDL_Renderer *renderer,Usuario *user,int *verificador,SDL_Rect*screenRectangule, SDL_Window*window){
    Misil *auxMisil = boss->misiles;

    while (auxMisil != NULL){
        if (auxMisil->y1 >= 640){
            (*verificador) = 0;
        }
        misilAvanzaEntidad(auxMisil);
        SDL_RenderDrawLine(renderer,auxMisil->x1,auxMisil->y1,auxMisil->x2,auxMisil->y2);
        SDL_RenderDrawLine(renderer,auxMisil->x1,auxMisil->y1,auxMisil->x2,auxMisil->y2);
        colisionDisparo_Usuario(boss,naveUsuario,user,screenRectangule, renderer, window);

        auxMisil = auxMisil->siguiente;

    }
}

void gameover(SDL_Rect *screenRectangle,SDL_Renderer *renderer,SDL_Window*window){
    SDL_Surface *image;
    image = IMG_Load("npc./gameover.jpg");
    SDL_Texture *imagen = SDL_CreateTextureFromSurface(renderer,image);
    SDL_Rect rect = {10,0,640,580};
    SDL_RenderCopy(renderer,imagen,screenRectangle,&rect);
    SDL_RenderPresent(renderer);
    SDL_Delay(2000);
    SDL_DestroyWindow(window);
    createMenu();
}

void startGame(Entidad **matriz, int largo_x, int largo_y, int nivel, List * levelsList){
    // //Revisar el nivel actual y Actualizarlo.
    // if (nivel != 1) {
    //     Nivel * aux = nextList(levelsList);
    //     matriz = aux->enemigo;
    //     largo_x = aux->x;
    //     largo_y = aux->y;
    //     nivel = aux->nivel;
    // }

    //Crear Usuario y Render para Usuario.
    Usuario *user = crearUsuario();
    SDL_Window * window = NULL;
    SDL_Renderer *renderer = NULL;

    //Definir L??mites de Pantalla.
    int largo = 580;
    int ancho = 640;
    
    //Definir L??mites de Nave.
    int size_x = 650;
    int size_y = 850;

    window = SDL_CreateWindow("Galactic Madness",SDL_WINDOWPOS_UNDEFINED,SDL_WINDOWPOS_UNDEFINED,ancho,largo,SDL_WINDOW_SHOWN);
    renderer = SDL_CreateRenderer(window,-1,SDL_RENDERER_ACCELERATED);
    Punto linea = {size_x/2+360,size_y/2+50,size_x/2-360,size_y/2+50};

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
                size_x/2-5,size_y/2+80,10,10,NULL};

    
    SDL_Surface *image;
    image = IMG_Load("npc./invader.jpg");
    if (!image){
        printf("IMG_LOAD: %s",IMG_GetError());
    }
    SDL_Surface *image2;
    image2 = IMG_Load("npc./invader2.jpg");
    if (!image2){
        printf("IMG_LOAD: %s",IMG_GetError());
    }

    SDL_Surface *image3;
    image3 = IMG_Load("npc./slave00.jpg");
    if (!image3){
        printf("IMG_LOAD: %s",IMG_GetError());
    }

    SDL_Surface *image4;
    image4 = IMG_Load("npc./slave01.jpg");
    if (!image4){
        printf("IMG_LOAD: %s",IMG_GetError());
    }



    SDL_Texture *imagen1 = SDL_CreateTextureFromSurface(renderer,image);
    SDL_Texture *imagen2 = SDL_CreateTextureFromSurface(renderer,image2);
    SDL_Texture *imagen3 = SDL_CreateTextureFromSurface(renderer,image3);
    SDL_Texture *imagen4 = SDL_CreateTextureFromSurface(renderer,image4);

    if (window == NULL){
        printf("ERROR");
        return;
    }

    SDL_Rect screenRectangle = {0,0,580,640};

    //MEMORIA ENTIDADES
    // Entidad **matriz = memoriaMatriz();

    // //Inicializar Entidades
    // inicializarEntidades(matriz);


    //Memoria Boss
    Entidad *boss = memoriaBoss();

    inicializarBoss(boss);

    int cont = 0;
    int direccion_x = 1;
    int direccion_y = 1;
    int inicio = 1;
    int direccion_xBOSS = 1;
    int direccion_yBOSS = 1;
    int inicioBoss = 1;

    int contador = 0;
    int frecuenciaAparicionBoss = 0;
    int bossTiempo = 0;
    int contadorBoss = 0;
    int verificador = 0;

    int frecuenciaDisparo;
    Entidad enemigo;

    int numeroentidades = largo_x*largo_y;

    float timeGlobal = Time() / 1000.f;

    int confirmacion = 0;

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
                    if (cooldown(&nave)){
                        naveDispara(&nave);
                    }
                }
            }
            frecuenciaDisparo = rand() % MAX;
            frecuenciaAparicionBoss = rand() % MAX;


            borrarMisiles(&nave);

            SDL_UpdateWindowSurface(window);

            SDL_SetRenderDrawColor(renderer,0,0,0,0);

            SDL_RenderClear(renderer);

            if (numeroentidades != 0){
                moverEntidades(renderer,imagen1,imagen2,&screenRectangle,matriz,largo_x,largo_y,&cont,&direccion_x,&direccion_y,&inicio,window);
            }
            SDL_SetRenderDrawColor(renderer,0,255,12,0);

            navePintar(&nave,renderer,matriz,user,largo_x,largo_y,&contadorBoss,boss,&numeroentidades,&confirmacion);
            if (contador == 0){
                if (frecuenciaDisparo < 200 && numeroentidades != 0){
                    enemigo = dameEnemigo(matriz, largo_x, largo_y);
                    contador++;
                }
            }

            if (contador > 0){
                dibujarMisilesEntidades(&enemigo,renderer,&contador,&nave,user,&screenRectangle,window);
            }

            //BOSS
            if (frecuenciaAparicionBoss < 50 && contadorBoss == 0){
                contadorBoss++;
            }

            if (contadorBoss != 0){
                moverBoss(&screenRectangle,boss,&bossTiempo,&direccion_xBOSS,&direccion_yBOSS,&inicioBoss,renderer,imagen3,imagen4);
                if (rand() % 1000 < 10){
                    if (verificador == 0){
                        parametrosBoss(boss);
                    }
                    verificador++;
                }
                if (verificador != 0){
                    dibujarMisilesBoss(boss,&nave,renderer,user,&verificador,&screenRectangle,window);
                }
            }

            dibujarlineaInferior(&linea,renderer);

            SDL_RenderPresent(renderer);

            SDL_Delay(10);

            if(confirmacion == 1) {
                break;
            }
    }
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_FreeSurface(image);
    SDL_FreeSurface(image2);
    SDL_DestroyTexture(imagen1);
    SDL_DestroyTexture(imagen2);
    SDL_DestroyTexture(imagen3);
    SDL_DestroyTexture(imagen4);
}

void misilAvanzaEntidad(Misil *misil){
    misil->x1 -= misil->velocidad_x;
    misil->y1 -= misil->velocidad_y;
    misil->x2 -= misil->velocidad_x;
    misil->y2 -= misil->velocidad_y;
}

void dejarEnCeroUsuario(Nave*naveUsuario,Misil *auxMisil){
    naveUsuario->x1 = SIZE_X/2;
    naveUsuario->x2 = SIZE_X/2+5;
    naveUsuario->x3 = SIZE_X/2+20;
    naveUsuario->x4 = SIZE_X/2+20;
    naveUsuario->x5 = SIZE_X/2-20;
    naveUsuario->x6 = SIZE_X/2-20;
    naveUsuario->x7 = SIZE_X/2-5;
    naveUsuario->y1 = SIZE_Y/2+70;
    naveUsuario->y2 = SIZE_Y/2+80;
    naveUsuario->y3 = SIZE_Y/2+80;
    naveUsuario->y4 = SIZE_Y/2+90;
    naveUsuario->y5 = SIZE_Y/2+90;
    naveUsuario->y6 = SIZE_Y/2+80;
    naveUsuario->y7 = SIZE_Y/2+80;

    auxMisil->x1 = 0;
    auxMisil->x2 = 0;
    auxMisil->y1 = 0;
    auxMisil->y2 = 0;
}

void colisionDisparo_Usuario(Entidad *nave,Nave *naveUsuario,Usuario *user, SDL_Rect *screenRectangule, SDL_Renderer*renderer,SDL_Window * window){
    int coordenadasEjeX = naveUsuario->x1;
    int coordenadasEjeY = naveUsuario->y1 + 12;
    Misil *auxMisil = nave->misiles;
    if (coordenadasEjeX-20 <= auxMisil->x1 && coordenadasEjeX+20 >= auxMisil->x1){
        if (coordenadasEjeY <= auxMisil->y1 && coordenadasEjeY+20 >= auxMisil->y1){
            printf("[IMPACTO]\n");
            user->vidas -= 1;

            SDL_Delay(1500);
            dejarEnCeroUsuario(naveUsuario,auxMisil);

            if (user->vidas == 0){
                printf("[GAMEOVER]\n");
                gameover(screenRectangule,renderer,window);
            }
        }
    }
}

void dibujarMisilesEntidades(Entidad *nave,SDL_Renderer *renderer,int *cont, Nave *naveUsuario,Usuario *user,SDL_Rect*screenRectangule,SDL_Window * window){
    Misil *auxMisil = nave->misiles;
    while (auxMisil != NULL){

        misilAvanzaEntidad(auxMisil);

        if (auxMisil->y1 >= 640){
            (*cont) = 0;
        }
        SDL_RenderDrawLine(renderer,auxMisil->x1,auxMisil->y1,auxMisil->x2,auxMisil->y2);
        SDL_RenderDrawLine(renderer,auxMisil->x1,auxMisil->y1,auxMisil->x2,auxMisil->y2);
        colisionDisparo_Usuario(nave,naveUsuario,user,screenRectangule,renderer,window);

        auxMisil = auxMisil->siguiente;
    }

}

void iniciarMisiles(Entidad *nave){
    nave->misiles = NULL;
    if (nave->misiles == NULL){
        nave->misiles = (Misil*)SDL_malloc(sizeof(Misil));
        nave->misiles->x1 = nave->rectangulo.x;
        nave->misiles->y1 = nave->rectangulo.y;
        nave->misiles->x2 = nave->rectangulo.x;
        nave->misiles->y2 = nave->rectangulo.y - 5;
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
        auxMisil->x1 = nave->rectangulo.x;
        auxMisil->y1 = nave->rectangulo.y;
        auxMisil->x2 = nave->rectangulo.x;
        auxMisil->y2 = nave->rectangulo.y - 5;
        auxMisil->velocidad_x = 0;
        auxMisil->velocidad_y = -5;
        auxMisil->siguiente = NULL;
    }
}

Entidad dameEnemigo(Entidad **matriz, int largo_x, int largo_y){
    int a,b;
    //iniciar misiles
    while(1){
        a = rand() % largo_x;
        b = rand() % largo_y;
        if (matriz[a][b].permitidoDisparar == true){
            break;
        }
    }
    Entidad *nave = &matriz[a][b];
    iniciarMisiles(nave);
    return matriz[a][b];
}

void crearDisparoEntidad(Entidad *naveEnemiga){
    if (naveEnemiga->misiles == NULL){
        naveEnemiga->misiles = (Misil*)SDL_malloc(sizeof(Misil));
        naveEnemiga->misiles->x1 = naveEnemiga->rectangulo.x;
        naveEnemiga->misiles->y1 = naveEnemiga->rectangulo.y;
        naveEnemiga->misiles->x2 = naveEnemiga->rectangulo.x;
        naveEnemiga->misiles->y2 = naveEnemiga->rectangulo.y + 5;
        naveEnemiga->misiles->velocidad_x = 0;
        naveEnemiga->misiles->velocidad_y = 5;
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
        auxMisil->y2 = naveEnemiga->rectangulo.y + 5;
        auxMisil->velocidad_x = 0;
        auxMisil->velocidad_y = 5;
        auxMisil->siguiente = NULL;
    }
}

void asignarPuntaje(Usuario *user,int i){
    user->puntaje += abs(5-i) * 10;
    printf("Puntaje: %d\n",user->puntaje);
}

void asignarPuntajeBoss(Usuario *user){
    user->puntaje += 1500;
    printf("Puntaje: %d\n",user->puntaje);
}

void colisionDisparo_Entidad(Misil *auxMisil,Entidad **matriz,Usuario *user,int largo_x, int largo_y,Nave *nave,int *numeroentidades, int*confirmacion){
    int coordenadaEjeX,coordenadaEjeY;

    for (int i = 0 ; i < largo_x ; i++){
            for (int j = 0 ; j < largo_y ; j++){

                coordenadaEjeX = matriz[i][j].rectangulo.x;
                coordenadaEjeY = matriz[i][j].rectangulo.y;

                if (matriz[i][j].vida != 0){
                    if (coordenadaEjeX <= auxMisil->x1 && coordenadaEjeX+40 >= auxMisil->x1){
                        if (coordenadaEjeY <= auxMisil->y1 && coordenadaEjeY+40 >= auxMisil->y1){
                             //ASIGNAR PUNTAJE
                             asignarPuntaje(user,i);

                            //ELIMINAR
                            auxMisil->x1 = 0;
                            auxMisil->y1 = 0;
                            auxMisil->x2 = 0;
                            auxMisil->y2 = 0;

                            matriz[i][j].vida = 0;
                            matriz[i][j].permitidoDisparar = false;
                            (*numeroentidades)--;
                            printf("Numero entidades: %d\n",(*numeroentidades));
                            if((*numeroentidades) == 0) {
                                (*confirmacion) = 1;
                            }
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
}

void colisionDisparo_Boss(int *contadorBoss,Entidad *boss,Misil *auxMisil,Usuario *user){
    int coordenadaEjeX,coordenadaEjeY;
    coordenadaEjeX = boss->rectangulo.x;
    coordenadaEjeY = boss->rectangulo.y;

    if (boss->vida != 0){
        if (coordenadaEjeX <= auxMisil->x1 && coordenadaEjeX+70 >= auxMisil->x1){
            if (coordenadaEjeY <= auxMisil->y1 && coordenadaEjeY+50 >= auxMisil->y1){
                printf("[IMPACTO BOSS]\n");
                //ELIMINAR
                auxMisil->x1 = 0;
                auxMisil->y1 = 0;
                auxMisil->x2 = 0;
                auxMisil->y2 = 0;

                boss->vida -= 1;
                if (boss->vida == 0){
                    contadorBoss = 0;
                    boss->permitidoDisparar = false;
                    boss->rectangulo.x = 0;
                    boss->rectangulo.y = 0;
                    boss->rectangulo.h = 0;
                    boss->rectangulo.w = 0;
                    //ASIGNAR PUNTAJE
                    asignarPuntajeBoss(user);
                    //ANIMACION DESTRUCCION (?)

                }

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

void navePintar(Nave *nave,SDL_Renderer *renderer,Entidad**matriz,Usuario *user, int largo_x, int largo_y, int *contadorBoss,Entidad *boss,int *numeroentidades, int * confirmacion){
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
        SDL_RenderDrawLine(renderer,auxMisil->x1,auxMisil->y1,auxMisil->x2,auxMisil->y2);

        //Colisiones
        colisionDisparo_Entidad(auxMisil,matriz,user,largo_x,largo_y,nave,numeroentidades,confirmacion);

        //COLISION BOSS
        if ((*contadorBoss) != 0){
            colisionDisparo_Boss(contadorBoss,boss,auxMisil,user);
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

void borrarMisiles(Nave *nave){
    Misil *auxMisil = nave->misiles;
    Misil *auxiliar_misiles;

    if (auxMisil == NULL){
        return;
    }

    while (auxMisil->siguiente != NULL){
        if (auxMisil->siguiente->y1 < 0){
            auxiliar_misiles = auxMisil->siguiente->siguiente;
            SDL_free(auxMisil->siguiente);
            auxMisil->siguiente = auxiliar_misiles;
        }
        else
        {
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

void mover_ejeX(Entidad **matriz,int *direccion_x, int largo_x, int largo_y){
    for(int i = 0 ; i < largo_x ; i++){
        for (int j = 0 ; j < largo_y; j++){
            if ((*direccion_x) == 1){
                matriz[i][j].rectangulo.x += 8;
            }
            else{
                matriz[i][j].rectangulo.x -= 8;
            }
        }
    }
}

void mover_ejeY(Entidad **matriz,int *direccion_x, int largo_x, int largo_y, SDL_Rect * screenRectangule, SDL_Renderer*renderer, SDL_Window*window){
    for (int i = 0 ; i < largo_x ; i++){
        for (int j = 0 ; j < largo_y ; j++){
            if ((*direccion_x) == 1){
                matriz[i][j].rectangulo.x -= 8;
                matriz[i][j].rectangulo.y += 10;
                if (matriz[i][j].rectangulo.y >= 445 && matriz[i][j].vida == 1){
                    printf("[GAMEOVER]\n");
                    gameover(screenRectangule, renderer, window);
                }
            }
            else{
                matriz[i][j].rectangulo.x += 8;
                matriz[i][j].rectangulo.y += 10;
                if (matriz[i][j].rectangulo.y >= 445 && matriz[i][j].vida == 1){
                    printf("[GAMEOVER]\n");
                    gameover(screenRectangule, renderer, window);
                }
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
        time *= 1.3;
    }
    if ((int)time > 30){
        time *= 1.3;
    }
    if ((int)time > 40){
        time *= 1.3;
    }
    if ((int)time > 50){
        time *= 1.3;
    }
    if ((int)time > 60){
        time *= 1.3;
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

void moverEntidades(SDL_Renderer *renderer,SDL_Texture *imagen1,SDL_Texture *imagen2,SDL_Rect *screenRectangle,Entidad **matriz,int largo_x,int largo_y,int*cont,int *direccion_x,int *direccion_y,int *inicio, SDL_Window * window){

    float time = velocidadEntidades();
    for (int i = 0 ; i < largo_x ; i++){
        for (int j = 0 ; j < largo_y ; j++){
            if ((int)time % 2 == 0){
                if((*cont) % 2 != 0){
                    mover_ejeX(matriz,direccion_x,largo_x,largo_y);
                    (*cont)++;
                }
                //CASO INICIO
                if ((*cont) == 12 && (*inicio) == 1){
                    mover_ejeY(matriz,direccion_x,largo_x,largo_y,screenRectangle,renderer,window);
                    (*direccion_x) *= -1;
                    (*cont) = 0;
                    (*inicio) = 0;
                }
                //CASO PROMEDIO
                if ((*cont) == 24){
                    mover_ejeY(matriz,direccion_x,largo_x,largo_y,screenRectangle,renderer,window);
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
                    mover_ejeX(matriz,direccion_x,largo_x,largo_y);
                    (*cont)++;
                }
                //CASO INICIO
                if ((*cont) == 12 && (*inicio) == 1){
                    mover_ejeY(matriz,direccion_x,largo_x,largo_y,screenRectangle,renderer,window);
                    (*direccion_x) *= -1;
                    (*cont) = 0;
                    (*inicio) = 0;
                }
                //CASO PROMEDIO
                if ((*cont) == 24){
                    mover_ejeY(matriz,direccion_x,largo_x,largo_y,screenRectangle,renderer,window);
                    (*direccion_x) *= -1;
                    (*cont) = 0;
                }
                if (matriz[i][j].vida != 0){
                    SDL_RenderCopy(renderer,imagen2,screenRectangle,&matriz[i][j].rectangulo);
                }
            }
        }
    }
}

void goy(int y){
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos;
    pos.X = 0;
    pos.Y = y;
    SetConsoleCursorPosition(hConsole, pos);
}

UINT64 Time(){
    return SDL_GetTicks64();
}

Entidad **crearMatrizEntidad(int x, int y){
    Entidad **matriz = (Entidad**)malloc(x * sizeof(Entidad*));
    int i,j;
    int eje_x;
    int eje_y;
    for (int i = 0 ; i < x ; i++){
        matriz[i] = (Entidad*)malloc(y * sizeof(Entidad));
    }
    
    if(y = 9){
        eje_x = 150;
        eje_y = 100;
        for(i = 0 ; i < x ; i++){
            for (int j = 0 ; j < y ; j++){
                matriz[i][j].rectangulo.x = eje_x;
                matriz[i][j].rectangulo.y = eje_y;
                matriz[i][j].rectangulo.h = 25;
                matriz[i][j].rectangulo.w = 25;
                matriz[i][j].vida = 1;
                matriz[i][j].permitidoDisparar = true;
                eje_x += 40;
            }
        }
        eje_x = 150;
        eje_y += 50;
    }
    if(y = 11){
        eje_x = 110;
        eje_y = 100;
        for(i = 0 ; i < x ; i++){
            for (int j = 0 ; j < y ; j++){
                matriz[i][j].rectangulo.x = eje_x;
                matriz[i][j].rectangulo.y = eje_y;
                matriz[i][j].rectangulo.h = 25;
                matriz[i][j].rectangulo.w = 25;
                matriz[i][j].vida = 1;
                matriz[i][j].permitidoDisparar = true;
                eje_x += 40;
            }
            eje_x = 110;
            eje_y += 50;
        }
    }

    return matriz;
}

Nivel *createLevel(int nivel,int *cont){
    Nivel *newLevel = (Nivel*) malloc(sizeof(Nivel));
    newLevel->nivel = nivel+1;

    if(nivel % 2 == 0){
        (*cont)++;
        newLevel->enemigo = crearMatrizEntidad(*cont,9);
        newLevel->x = *cont;
        newLevel->y = 9;
        newLevel->enemigos = *cont * 9;
        return newLevel;
    }
    else{
        newLevel->enemigo = crearMatrizEntidad(*cont,11);
        newLevel->x = *cont;
        newLevel->y = 11;
        newLevel->enemigos = *cont * 11;
        return newLevel;
    }
    
    return NULL;
}

List * GenerarNiveles(){
    List *NivelesTotales = createList();
    int cont = 1; //para verificar un aumeto par de la matriz
    int i;
    int k = 7;
    for(i = 0 ; i < k ; i++){
        pushBack(NivelesTotales,createLevel(i,&cont));
    }
    return NivelesTotales;
}

void mostrarInformacionNiveles(List *lista){
    Nivel *aux = firstList(lista);
    while(aux){
        printf("------------------------------\n");
        printf("Nivel: %i\n",aux->nivel);
        printf("X: %i\n",aux->x);
        printf("y: %i\n",aux->y);
        printf("cantidad de enemigos: %i\n",aux->enemigos);
        printf("------------------------------\n");
        aux = nextList(lista);
    }
    system("pause");
}
