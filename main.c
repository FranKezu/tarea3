#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/hashmap.h"
#include "grafo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

void mostrar_menu(Jugador *, HashMap *, int, int*);
void empezar_juego();
Jugador *crear_jugador(Escenario* , char*);
void finalizar_partida(Jugador *, HashMap *);
void mostrar_estado(Jugador *, HashMap *);


Jugador *crear_jugador(Escenario *escenario_inicial, char *usuario) {
  Jugador *jugador = (Jugador *)malloc(sizeof(Jugador));
  jugador->inventario = list_create();
  jugador->peso_total = 0;
  jugador->puntaje_total = 0;
  jugador->tiempo_restante = 10; //REVISAR TIEMPO 
  jugador->escenario_actual = escenario_inicial;

  char name[200];
  if(!usuario) printf("Ingrese el nombre de usuario: ");
  else printf("Ingrese el nombre del segundo usuario: ");
  scanf(" %[^\n]s", name);
  while(is_equal(name, usuario)){
    printf("\nEl nombre ingresado es igual al otro jugador, por favor ingresa otro: ");
    scanf(" %[^\n]s", name);
  }

  jugador->usuario = strdup(name);

  return jugador;
}

void finalizar_partida(Jugador *jugador, HashMap *grafo) {
  if (jugador -> tiempo_restante <= 0) {
    puts("El tiempo se ha agotado. ¡PERDISTE!");
    puts("1. Empezar otra partida");
    puts("2. Salir del juego");

    char opcion;
    do {
      printf("Ingrese su opción: ");
      scanf(" %c", & opcion);

      switch (opcion) {
      case '1':
        empezar_juego();
        break;
      case '2':
        puts("Saliendo del juego...");
        exit(0);
        break;
      default:
        puts("Opción no válida. Por favor, intente de nuevo.");
      }
    } while (opcion != '2');
    return;
  }

  Pair *par = searchMap(grafo, "16");
  Escenario *final = (Escenario * ) par -> value;

  if(jugador->escenario_actual == final) {
    printf("GANASTE");
  }

}

void mostrar_estado(Jugador *jugador, HashMap *grafo) {
  limpiarPantalla();
  finalizar_partida(jugador, grafo);

  // Mostrar estado del jugador y escenario

  printf("\n📍 Estás en %s\n", jugador -> escenario_actual -> nombre);
  printf("📝 %s\n", jugador -> escenario_actual -> descripcion);
  printf("🎒 Peso: %d | 💎 Puntaje: %d | ⏳ Tiempo restante: %d\n", jugador -> peso_total, jugador -> puntaje_total, jugador -> tiempo_restante);

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

  puts("\n===============================");
  puts("      OPCIONES DEL JUGADOR     ");
  puts("===============================");
  puts("1. Recoger Ítem(s)");
  puts("2. Descartar Ítem(s)");
  puts("3. Avanzar en una Dirección");
  puts("4. Reiniciar Partida");
  puts("5. Salir del Juego");
  puts("===============================\n");
}

void iniciar_solo(Jugador *jugador, HashMap *grafo) {
  printf(">> Iniciando juego para un jugador...\n");
  grafo = leer_escenarios();
  Pair *par = searchMap(grafo, "1"); // "0" es la clave como string
  Escenario *inicio = (Escenario *)par->value;
  jugador = crear_jugador(inicio, NULL);
  int contador = -1;
  mostrar_menu(jugador, grafo, 1, &contador);
}

void iniciar_cooperativo(Jugador *jugador1, Jugador *jugador2, HashMap *grafo) {
  printf(">> Iniciando juego para dos jugadores...\n");
  grafo = leer_escenarios();
  Pair *par = searchMap(grafo, "1"); // "0" es la clave como string
  Escenario *inicio = (Escenario *)par->value;
  jugador1 = crear_jugador(inicio, NULL);
  jugador2 = crear_jugador(inicio, jugador1->usuario);
  while (1) {
    int contador = 2;
    printf("\n=== Turno de %s ===", jugador1->usuario);
    mostrar_menu(jugador1, grafo, 2, &contador);
    contador = 2;
    printf("\n=== Turno de %s ===", jugador2->usuario);
    mostrar_menu(jugador2, grafo, 2, &contador);
  }
}

void empezar_juego() {
  printf("Selecciona el modo de juego:\n");
  printf("1. Un jugador\n");
  printf("2. Cooperativo\n");
  printf("3. Salir.\n");

  HashMap *grafo = createMap(100);
  Jugador *jugador1 = NULL;
  Jugador *jugador2 = NULL;
  
  char opcion;
  do {
    printf("Ingrese su opción: ");
    scanf(" %c", &opcion);

    switch (opcion) {
    case '1':
      iniciar_solo(jugador1, grafo);
      break;
    case '2':
      iniciar_cooperativo(jugador1, jugador2, grafo);
      break;
    case '3':
      puts("Saliendo del programa...");
      break;
    default:
      puts("Opción no válida. Por favor, intente de nuevo.");
    }

  } while (opcion != '3');
}

void mostrar_menu(Jugador *jugador, HashMap *grafo, int modo, int *contador) {
  char opcion;
  do {
    mostrar_estado(jugador, grafo);
    if(modo == 2){
      printf("Jugador actual: %s\n", jugador->usuario);
      printf("Acciones restantes: %d\n\n", *contador);
    }
    printf("Ingrese su opción: ");
    scanf(" %c", &opcion);

    switch (opcion) {
    case '1':
      recoger_item(jugador);
      break;
    case '2':
      descartar_item(jugador);
      break;
    case '3':
      mover_jugador(jugador, grafo);
      break;
    case '4':
      empezar_juego();
      break;
    case '5':
      puts("Saliendo del juego...");
      break;
    default:
      puts("Opción no válida. Por favor, intente de nuevo.");
    }
    (*contador)--;
    if(*contador == 0) break;
  } while (opcion != '5');
}

int main() {
  setlocale(LC_ALL, "es_ES.UTF-8"); // Para que se puedan ver tildes, ñ, y carácteres especiales.

  printf("=== BIENVENIDO A GRAPHQUEST ===\n");
  empezar_juego();

  return 0;
}