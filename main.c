#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/hashmap.h"
#include "grafo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

// Colores ANSI
#define RESET         "\033[0m"
#define RED           "\033[1;31m"
#define GREEN         "\033[1;32m"
#define YELLOW        "\033[1;33m"
#define BLUE          "\033[1;34m"
#define MAGENTA       "\033[1;35m"
#define CYAN          "\033[1;36m"
#define BOLD          "\033[1m"

void mostrar_menu(Jugador *, HashMap *, int, int*);
void empezar_juego();
Jugador *crear_jugador(Escenario* , char*);
void finalizar_partida(Jugador *, HashMap *, int );
void mostrar_estado(Jugador *, HashMap *, int);

//--------------------------------------------------------------CREAR JUGADOR(ES)--------------------------------------------------------------
Jugador *crear_jugador(Escenario *escenario_inicial, char *usuario) {
  Jugador *jugador = (Jugador *)malloc(sizeof(Jugador));
  jugador->inventario = list_create();
  jugador->peso_total = 0;
  jugador->puntaje_total = 0;
  jugador->tiempo_restante = 10; //REVISAR TIEMPO 
  jugador->escenario_actual = escenario_inicial;

  jugador->termino = 0; //PARA MANEJAR EL COOP

  char name[200];
  if(!usuario) printf("👤 Ingrese el nombre de usuario: ");
  else printf("👥 Ingrese el nombre del segundo usuario: ");
  scanf(" %[^\n]s", name);
  while(is_equal(name, usuario)){
    printf("\n⚠️ El nombre ingresado es igual al otro jugador, por favor ingresa otro: ");
    scanf(" %[^\n]s", name);
  }

  jugador->usuario = strdup(name);

  return jugador;
}
//-------------------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------FINALIZAR SOLO--------------------------------------------------------------
void finalizar_partida(Jugador *jugador, HashMap *grafo, int modo) {
  if (jugador -> tiempo_restante <= 0 && modo == 1) {
    puts(RED "\n═════════════════════════════════════");
    puts("⏳ El tiempo se ha agotado. ¡PERDISTE!");
    puts("═════════════════════════════════════" RESET);
    puts("\n1. 🔁 Empezar otra partida");
    puts("2. ❌ Salir del juego");

    char opcion;
    do {
      printf(YELLOW "\nIngrese su opción: " RESET);
      scanf(" %c", & opcion);

      switch (opcion) {
      case '1':
        empezar_juego();
        break;
      case '2':
        puts("🚪 Saliendo del juego...");
        exit(0);
        break;
      default:
        puts(RED "❌ Opción no válida. Por favor, intente de nuevo." RESET);
      }
    } while (opcion != '2');
    return;
  }
  else if(jugador->tiempo_restante <= 0) jugador->termino = 1; //PARA DECIR QUE TERMINO POR TIEMPO

  Pair *par = searchMap(grafo, "16");
  Escenario *final = (Escenario * ) par -> value;

  if(jugador->escenario_actual == final && modo == 1) {
    printf(GREEN "\n🎉 ¡GANASTE! 🎉\n" RESET);
    printf("📦 Ítems Obtenidos:\n");
    for (Item * item = list_first(jugador -> inventario); item != NULL; item = list_next(jugador -> inventario)) {
      printf("  - %s ", item->nombre);
    }
    printf("\n\n💎 Puntaje Total Obtenido: %d\n\n", jugador->puntaje_total);
    exit(0);
  }
  else if(jugador->escenario_actual == final) jugador->termino = 1; // MODO COOP PARA GUARDAR QUE ESTE YA TERMINO

}
//-------------------------------------------------------------------------------------------------------------------------------------------


//--------------------------------------------------------------ESTADO DURANTE PARTIDA--------------------------------------------------------------
void mostrar_estado(Jugador *jugador, HashMap *grafo, int modo) {
  limpiarPantalla();
  finalizar_partida(jugador, grafo, modo);
  if(modo == 2 && jugador->termino)return; // PARA QUE NO PUEDA JUGAR YA SI ES QUE TERMINO Y SIGA CON EL OTRO NOMAS

  // Mostrar estado del jugador y escenario
  printf(BLUE "\n╔════════════════════════════════════════════╗\n" RESET);
  printf(BOLD "📍 Estás en: %s\n" RESET, jugador->escenario_actual->nombre);
  printf("📝 %s\n", jugador -> escenario_actual -> descripcion);
  printf("🎒 Peso: %d | 💎 Puntaje: %d | ⏳ Tiempo restante: %d\n", jugador -> peso_total, jugador -> puntaje_total, jugador -> tiempo_restante);
  printf(BLUE "╚════════════════════════════════════════════╝\n" RESET);

  // Mostrar ítems del escenario
  if (list_size(jugador -> escenario_actual -> items) == 0) {
    printf("📭 No hay ítems en este escenario.\n");
  } else {
    printf("📦 Ítems disponibles:\n");
    for (Item * item = list_first(jugador -> escenario_actual -> items); item != NULL; item = list_next(jugador -> escenario_actual -> items)) {
      printf("  - %s (valor: %d, peso: %d)\n", item -> nombre, item -> valor, item -> peso);
    }
  }

  // Mostrar inventario del jugador
  if (list_size(jugador -> inventario) == 0) {
    printf("\n🎒 Tu inventario está vacío.\n");
  } else {
    printf("\n🎒 Inventario de %s:\n", jugador -> usuario);
    int total_valor = 0;
    int total_peso = 0;
    int i = 1;
    for (Item * item = list_first(jugador -> inventario); item != NULL; item = list_next(jugador -> inventario)) {
      printf("%d. %s (valor: %d, peso: %d)\n", i++, item -> nombre, item -> valor, item -> peso);
      total_valor += item -> valor;
      total_peso += item -> peso;
    }
  }

  puts(BLUE "\n╔════════════════════════════════╗");
  puts("║      OPCIONES DEL JUGADOR      ║");
  puts("╚════════════════════════════════╝" RESET);
  puts("1. 🎁 Recoger Ítem(s)");
  puts("2. 🗑️  Descartar Ítem(s)");
  puts("3. 🧭 Avanzar en una Dirección");
  puts("4. 🔄 Reiniciar Partida");
  puts("5. 🚪 Salir del Juego");
}
//-------------------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------INICIAR PARTIDAS SOLO Y COOP--------------------------------------------------------------
void iniciar_solo(Jugador *jugador, HashMap *grafo) {
  printf("\n" CYAN ">> Iniciando juego para un jugador...\n" RESET);
  grafo = leer_escenarios();
  Pair *par = searchMap(grafo, "1"); // "0" es la clave como string
  Escenario *inicio = (Escenario *)par->value;
  jugador = crear_jugador(inicio, NULL);
  int contador = -1;
  mostrar_menu(jugador, grafo, 1, &contador);
}

//COOP

void iniciar_cooperativo(Jugador *jugador1, Jugador *jugador2, HashMap *grafo) {
  printf("\n" CYAN ">> Iniciando juego para dos jugadores...\n" RESET);
  grafo = leer_escenarios();
  Pair *par = searchMap(grafo, "1"); // "0" es la clave como string
  Escenario *inicio = (Escenario *)par->value;
  jugador1 = crear_jugador(inicio, NULL);
  jugador2 = crear_jugador(inicio, jugador1->usuario);
  while (1) {
    int contador = 2;
    if(jugador1->termino == 0){
      printf("\n" MAGENTA "=== Turno de %s ===\n" RESET, jugador1->usuario);
      mostrar_menu(jugador1, grafo, 2, &contador);
    }

    contador = 2;
    if(jugador2->termino == 0){
      printf("\n" MAGENTA "=== Turno de %s ===\n" RESET, jugador2->usuario);
      mostrar_menu(jugador2, grafo, 2, &contador);
    }
    if(jugador1->tiempo_restante <= 0 || jugador2->tiempo_restante <= 0){
      limpiarPantalla();
      printf("Uno de los jugadores se le ha acabado el tiempo y no ha alcanzado a salir, los dos pierden\n");
      presioneTeclaParaContinuar();
      empezar_juego();
    }
    if(jugador1->termino == 1 && jugador2->termino == 1) break;
  }
  limpiarPantalla();
  printf("LOS DOS JUGADORES HAN CONSEGUIDO SALIR, HAN GANADO\n");
  printf("📦 Ítems Obtenidos por %s:\n",jugador1->usuario);
    for (Item * item = list_first(jugador1 -> inventario); item != NULL; item = list_next(jugador1 -> inventario)) {
      printf("  - %s ", item->nombre);
    }
    printf("\n\n💎 Puntaje Total Obtenido: %d\n\n\n", jugador1->puntaje_total);

  printf("📦 Ítems Obtenidos por %s:\n",jugador2->usuario);
    for (Item * item = list_first(jugador2 -> inventario); item != NULL; item = list_next(jugador2 -> inventario)) {
      printf("  - %s ", item->nombre);
    }
    printf("\n\n💎 Puntaje Total Obtenido: %d\n\n", jugador2->puntaje_total);  
  presioneTeclaParaContinuar();
  empezar_juego();
}
//-------------------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------MENU PRINCIPAL--------------------------------------------------------------
void empezar_juego() {
  limpiarPantalla();
  printf(BOLD "\nSeleccione el modo de juego:\n" RESET);
  printf("1. 🎮 Un jugador\n");
  printf("2. 🤝 Cooperativo\n");
  printf("3. 🚪 Salir\n");

  HashMap *grafo = createMap(100);
  Jugador *jugador1 = NULL;
  Jugador *jugador2 = NULL;
  
  char opcion;
  do {
    printf(YELLOW "Ingrese su opción: " RESET);
    scanf(" %c", &opcion);

    switch (opcion) {
    case '1':
      iniciar_solo(jugador1, grafo);
      break;
    case '2':
      iniciar_cooperativo(jugador1, jugador2, grafo);
      break;
    case '3':
      puts("👋 Saliendo del programa...");
      exit(0);
      break;
    default:
      puts(RED "❌ Opción no válida. Por favor, intente de nuevo." RESET);
    }

  } while (opcion != '3');
}
//---------------------------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------MENU DE JUEGO (ACCIONES DURANTE PARTIDA)---------------------------------------------------
void mostrar_menu(Jugador *jugador, HashMap *grafo, int modo, int *contador) {
  char opcion;
  do {
    mostrar_estado(jugador, grafo, modo);
    if (modo == 2 && jugador->termino){ 
      printf("%s ha llegado al final. Marcado como terminado.\n", jugador->usuario);
      presioneTeclaParaContinuar();
      return; // PARA SALIR DE ESTE MENU TAMBIEN
    }  
    if(modo == 2){
      printf(BOLD "\nJugador actual: %s\n" RESET, jugador->usuario);
      printf("🧮 Acciones restantes: %d\n\n", *contador);
    }
    printf(YELLOW "Ingrese su opción: " RESET);
    scanf(" %c", &opcion);

    switch (opcion) {
    case '1':
      if (recoger_item(jugador)) (*contador)--;
      break;
    case '2':
      if (descartar_item(jugador)) (*contador)--;
      break;
    case '3':
      if (mover_jugador(jugador, grafo)) (*contador)--;
      break;
    case '4':
      empezar_juego();
      break;
    case '5':
      puts("👋 Saliendo del juego...");
      exit(0);
      break;
    default:
      puts(RED "❌ Opción no válida. Por favor, intente de nuevo." RESET);
    }
    if(*contador == 0) break;
  } while (opcion != '5');
}
//------------------------------------------------------------------------------------------------------------------------------------------------------


int main() {
  setlocale(LC_ALL, "es_ES.UTF-8"); // Para que se puedan ver tildes, ñ, y carácteres especiales.
  
  printf(BOLD GREEN "\n═══════════════════════════════════════\n" RESET);
  printf(BOLD "       BIENVENIDO A GRAPHQUEST       \n" RESET);
  printf(GREEN "═══════════════════════════════════════\n" RESET);
  empezar_juego();

  return 0;
}