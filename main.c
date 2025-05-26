#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/hashmap.h"
#include "grafo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

//gcc main.c tdas\list.c tdas\extra.c tdas\hashmap.c grafo.c -o main -lwinmm

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
/*
 * Esta función se encarga de crear un jugador nuevo.
 * Recibe el escenario inicial y un nombre de usuario (puede ser NULL).
 * Inicializa todos los valores base del jugador, como inventario, puntaje, tiempo, etc.
 */
Jugador *crear_jugador(Escenario *escenario_inicial, char *usuario) {
  Jugador *jugador = (Jugador *)malloc(sizeof(Jugador));
  jugador->inventario = list_create(); // Crea el inventario como lista vacía
  jugador->peso_total = 0;
  jugador->puntaje_total = 0;
  jugador->tiempo_restante = 30; // Tiempo inicial que tiene para jugar

  jugador->escenario_actual = escenario_inicial;
  jugador->termino = 0; // En modo coop indica si ya terminó su partida

  char name[200];
  if(!usuario) printf("👤 Ingrese el nombre de usuario: ");
  else printf("👥 Ingrese el nombre del segundo usuario: ");
  scanf(" %[^\n]s", name);

  // Evita que los dos jugadores tengan el mismo nombre
  while(is_equal(name, usuario)){
    printf("\n⚠️ El nombre ingresado es igual al otro jugador, por favor ingresa otro: ");
    scanf(" %[^\n]s", name);
  }

  jugador->usuario = strdup(name); // Se guarda el nombre

  return jugador;
}

//--------------------------------------------------------------FINALIZAR SOLO--------------------------------------------------------------
/*
 * Esta función revisa si el jugador ha perdido por tiempo o ha llegado al escenario final.
 * Si está en modo individual, muestra mensajes y finaliza el juego.
 * En modo cooperativo, solo marca que terminó.
 */
void finalizar_partida(Jugador *jugador, HashMap *grafo, int modo) {
  // Caso: el jugador se quedó sin tiempo y está en modo individual
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
        empezar_juego(); // Reinicia el juego
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
  // En modo coop, si se queda sin tiempo, solo se marca que terminó
  else if(jugador->tiempo_restante <= 0) jugador->termino = 1;

  // Se verifica si el jugador llegó al escenario final
  Pair *par = searchMap(grafo, "16");
  Escenario *final = (Escenario * ) par -> value;

  // Si llega al final y está en modo individual, ganó
  if(jugador->escenario_actual == final && modo == 1) {
    printf(BOLD "¡Felicidades %s!" RESET, jugador->usuario);
    printf(GREEN "\n🎉 ¡GANASTE! 🎉\n\n" RESET);
    printf("📦 Ítems Obtenidos:\n");
    if(list_size(jugador->inventario) == 0) printf("No conseguiste ningún item :(");
    else{
      for (Item * item = list_first(jugador -> inventario); item != NULL; item = list_next(jugador -> inventario)) {
        printf("  - %s ", item->nombre);
      }
    }
    printf("\n\n💎 Puntaje Total Obtenido: %d\n\n", jugador->puntaje_total);
    exit(0);
  }
  // En modo coop, solo se marca que llegó al final
  else if(jugador->escenario_actual == final) jugador->termino = 1;
}

//--------------------------------------------------------------ESTADO DURANTE PARTIDA--------------------------------------------------------------
/*
 * Muestra en pantalla toda la información del jugador durante la partida.
 * Incluye: escenario actual, descripción, ítems en el escenario, inventario, etc.
 * También llama a finalizar_partida por si ya ganó o perdió.
 */
void mostrar_estado(Jugador *jugador, HashMap *grafo, int modo) {
  limpiarPantalla(); // Limpia consola antes de mostrar info
  finalizar_partida(jugador, grafo, modo); // Verifica si ya terminó
  if(modo == 2 && jugador->termino)return; // Si está en coop y ya terminó, no sigue mostrando estado

  // Mostrar datos del jugador y escenario
  printf(BLUE "\n════════════════════════════════════════════════════════════════════════════════════════════\n" RESET);
  printf(BOLD "📍 Estás en: %s\n" RESET, jugador->escenario_actual->nombre);
  printf("📝 %s\n", jugador -> escenario_actual -> descripcion);
  printf("🎒 Peso: %d | 💎 Puntaje: %d | ⏳ Tiempo restante: %d\n", jugador -> peso_total, jugador -> puntaje_total, jugador -> tiempo_restante);
  printf(BLUE "════════════════════════════════════════════════════════════════════════════════════════════\n" RESET);

  // Mostrar ítems disponibles en el escenario actual
  if (list_size(jugador -> escenario_actual -> items) == 0) {
    printf("📭 No hay ítems en este escenario.\n");
  } else {
    printf("📦 Ítems disponibles:\n");
    // Nuestros items están almacenados en una lista, por lo tanto la recorremos para mostrar cada uno de estos.
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
    // Nuestro inventario también está en una lista de items, se repite el mismo proceso, se recorre e imprime.
    for (Item * item = list_first(jugador -> inventario); item != NULL; item = list_next(jugador -> inventario)) {
      printf("%d. %s (valor: %d, peso: %d)\n", i++, item -> nombre, item -> valor, item -> peso);
      total_valor += item -> valor;
      total_peso += item -> peso;
    }
  }

  // Menú de opciones para el jugador
  puts(BLUE "\n╔════════════════════════════════╗");
  puts("║      OPCIONES DEL JUGADOR      ║");
  puts("╚════════════════════════════════╝" RESET);
  puts("1. 🎁 Recoger Ítem(s)");
  puts("2. 🗑️  Descartar Ítem(s)");
  puts("3. 🧭 Avanzar en una Dirección");
  puts("4. 🔄 Reiniciar Partida");
  puts("5. 🚪 Salir del Juego");
}

//--------------------------------------------------------------INICIAR PARTIDAS SOLO Y COOP--------------------------------------------------------------
/*
 * Inicia el juego en modo individual.
 * Carga el grafo, crea al jugador y muestra el menú principal.
 */
void iniciar_solo(Jugador *jugador, HashMap *grafo) {
  printf("\n" CYAN ">> Iniciando GRAPHQUEST para un jugador...\n" RESET);
  grafo = leer_escenarios(); // Carga escenarios desde CSV
  Pair *par = searchMap(grafo, "1"); // Se empieza desde el nodo 1
  Escenario *inicio = (Escenario *)par->value;
  jugador = crear_jugador(inicio, NULL); // Crea jugador sin comparar nombre
  int contador = -1;
  mostrar_menu(jugador, grafo, 1, &contador);
}

/*
 * Inicia el juego en modo cooperativo para dos jugadores.
 * Ambos deben llegar al final para ganar, si uno pierde por tiempo, pierden ambos.
 */
void iniciar_cooperativo(Jugador *jugador1, Jugador *jugador2, HashMap *grafo) {
  printf("\n" CYAN ">> Iniciando GRAPHQUEST para dos jugadores...\n" RESET);
  grafo = leer_escenarios(); // Carga escenarios desde CSV
  Pair *par = searchMap(grafo, "1");
  Escenario *inicio = (Escenario *)par->value;

  jugador1 = crear_jugador(inicio, NULL);
  jugador2 = crear_jugador(inicio, jugador1->usuario); // Verifica que no usen el mismo nombre

  while (1) {
    int contador = 2;

    // Turno jugador 1
    if(jugador1->termino == 0){
      printf("\n" MAGENTA "=== Turno de %s ===\n" RESET, jugador1->usuario);
      mostrar_menu(jugador1, grafo, 2, &contador);
    }

    contador = 2;
    // Turno jugador 2
    if(jugador2->termino == 0){
      printf("\n" MAGENTA "=== Turno de %s ===\n" RESET, jugador2->usuario);
      mostrar_menu(jugador2, grafo, 2, &contador);
    }

    // Si uno pierde por tiempo, pierden ambos
    if(jugador1->tiempo_restante <= 0 || jugador2->tiempo_restante <= 0){
      limpiarPantalla();
      printf("Uno de los jugadores se le ha acabado el tiempo y no ha alcanzado a salir, los dos pierden\n");
      presioneTeclaParaContinuar();
      empezar_juego();
    }

    // Si ambos llegaron al final, ganan
    if(jugador1->termino == 1 && jugador2->termino == 1) break;
  }

  // Mostrar resultados finales
  limpiarPantalla();
  printf(BOLD "¡Felicidades %s y %s!" RESET, jugador1->usuario, jugador2->usuario);
  printf(GREEN "\n🎉 ¡GANARON! 🎉\n\n" RESET);

  // Recorremos las listas de items e inventario para mostrar sus elementos, además del puntaje obtenido por los jugadores.

  printf("📦 Ítems Obtenidos por %s:\n",jugador1->usuario);
  if(list_size(jugador1->inventario) == 0) printf("%s no conseguiste ningún item :(", jugador1->usuario);
  else{
    for (Item * item = list_first(jugador1->inventario); item != NULL; item = list_next(jugador1->inventario)) {
    printf("  - %s ", item->nombre);
    }
  }
  printf("\n\n💎 Puntaje Total Obtenido: %d\n\n", jugador1->puntaje_total);

  printf("📦 Ítems Obtenidos por %s:\n",jugador2->usuario);
  if(list_size(jugador2->inventario) == 0) printf("%s no conseguiste ningún item :(", jugador2->usuario);
  else{
    for (Item * item = list_first(jugador2->inventario); item != NULL; item = list_next(jugador2->inventario)) {
    printf("  - %s ", item->nombre);
    }
  }
  printf("\n\n💎 Puntaje Total Obtenido: %d\n\n", jugador2->puntaje_total);

  presioneTeclaParaContinuar();
  empezar_juego(); // Reinicia el juego
}

//--------------------------------------------------------------MENU PRINCIPAL--------------------------------------------------------------
void empezar_juego() {
  limpiarPantalla();
  puts(BLUE "╔════════════════════════════════════════════╗");
  puts(      "║        SELECCIONE EL MODO DE JUEGO         ║");
  puts(      "╚════════════════════════════════════════════╝" RESET);
  puts(" 1. 🎮  Un jugador");
  puts(" 2. 🤝  Cooperativo");
  puts(" 3. 🚪  Salir");

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
      puts("👋 Saliendo del juego...");
      exit(0);
      break;
    default:
      puts(RED "\n❌ Opción no válida. Por favor, intente de nuevo." RESET);
    }
    
  } while (opcion != '3');
}

//--------------------------------------------------------------MENU DE JUEGO (ACCIONES DURANTE PARTIDA)---------------------------------------------------
void mostrar_menu(Jugador *jugador, HashMap *grafo, int modo, int *contador) {
  char opcion;
  do {
    mostrar_estado(jugador, grafo, modo);

    if (modo == 2 && jugador->termino){ 
      printf(BOLD "¡%s ha escapado! Marcado como terminado.\n" RESET, jugador->usuario);
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
      puts(RED "\n❌ Opción no válida. Por favor, intente de nuevo." RESET);
      presioneTeclaParaContinuar();
    }
    if(*contador == 0) break;
  } while (opcion != '5');
}

int main() {
  setlocale(LC_ALL, "es_ES.UTF-8"); // Para que se puedan ver tildes, ñ, y carácteres especiales.
  
  printf(BOLD GREEN "\n═══════════════════════════════════════\n" RESET);
  printf(BOLD "       BIENVENIDO A GRAPHQUEST       \n" RESET);
  printf(GREEN "═══════════════════════════════════════\n\n" RESET);
  presioneTeclaParaContinuar();
  empezar_juego();

  return 0;
}