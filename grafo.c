#include "tdas/list.h"
#include "tdas/hashmap.h"
#include "tdas/extra.h"
#include "grafo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib") // Para poder utilizar sonidos en el programa.

// Colores ANSI
#define RESET         "\033[0m"
#define RED           "\033[1;31m"
#define GREEN         "\033[1;32m"
#define YELLOW        "\033[1;33m"
#define BLUE          "\033[1;34m"
#define MAGENTA       "\033[1;35m"
#define CYAN          "\033[1;36m"
#define BOLD          "\033[1m"

// Esta función se encarga de leer los escenarios desde un archivo CSV
// Crea los escenarios, lee sus datos y los inserta en un HashMap que representa el grafo del juego
void * leer_escenarios() {
  FILE * archivo = fopen("data/graphquest.csv", "r");
  if (archivo == NULL) {
    perror("Error al abrir el archivo");
    return NULL;
  }

  HashMap * grafo = createMap(100); // Crear el mapa con una capacidad inicial
  char ** campos;

  // Leer y descartar la primera línea del archivo (los encabezados)
  campos = leer_linea_csv(archivo, ',');

  // Leer línea por línea el resto del archivo
  while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
    Escenario * esc = (Escenario * ) malloc(sizeof(Escenario));
    esc -> id = atoi(campos[0]);
    esc -> nombre = strdup(campos[1]);
    esc -> descripcion = strdup(campos[2]);

    // Dividir el campo de ítems por el separador ";"
    esc -> items = split_string(campos[3], ";");
    List * items_procesados = list_create();

    // Para cada ítem en la lista, se divide en nombre, valor y peso
    for (char * item = list_first(esc -> items); item != NULL; item = list_next(esc -> items)) {
      List * valores = split_string(item, ",");
      Item * nuevo_item = (Item * ) malloc(sizeof(Item));
      nuevo_item -> nombre = strdup((char * ) list_first(valores));
      nuevo_item -> valor = atoi((char * ) list_next(valores));
      nuevo_item -> peso = atoi((char * ) list_next(valores));
      list_pushBack(items_procesados, nuevo_item);
      list_clean(valores); // Limpiar la lista temporal
      free(valores);
    }
    list_clean(esc -> items);
    free(esc -> items);
    esc -> items = items_procesados;

    // Guardar las conexiones con otros escenarios (arriba, abajo, izquierda, derecha)
    esc -> conexiones[0] = atoi(campos[4]);
    esc -> conexiones[1] = atoi(campos[5]);
    esc -> conexiones[2] = atoi(campos[6]);
    esc -> conexiones[3] = atoi(campos[7]);
    esc -> es_final = atoi(campos[8]); // Si es el escenario final

    // Insertar el escenario en el mapa usando su ID como clave
    char id_str[10];
    sprintf(id_str, "%d", esc -> id);
    insertMap(grafo, id_str, esc);
  }

  fclose(archivo);
  return grafo; // Devolver el mapa con todos los escenarios
}

//--------------------------------------------------------------OPCION 1 (RECOGER ITEM)--------------------------------------------------------------
// Esta función permite al jugador recoger un ítem del escenario actual
// Si el ítem existe, se agrega al inventario y se actualizan el peso, puntaje y tiempo restante
int recoger_item(Jugador *jugador) {
  if (list_size(jugador->escenario_actual->items) == 0) {
    puts("\nNo hay ítems disponibles para recoger.\n");
    presioneTeclaParaContinuar();
    return 0;
  }

  printf("\nNombre del ítem que quieres recoger: ");
  char nombre_item[100];
  scanf(" %[^\n]s", nombre_item);
  printf("\n");

  for (Item *item = list_first(jugador->escenario_actual->items); item != NULL; item = list_next(jugador->escenario_actual->items)) {
    if (is_equal(item->nombre, nombre_item)) {

      list_pushBack(jugador->inventario, item); // Se añade al inventario
      jugador->peso_total += item->peso;
      jugador->puntaje_total += item->valor;

      list_popCurrent(jugador->escenario_actual->items); // Se elimina del escenario
      printf("Has recogido el ítem: %s\n", item->nombre);
      jugador->tiempo_restante -= 1; // Se descuenta tiempo por la acción
      presioneTeclaParaContinuar();
      return 1;
    }
  }

  printf(RED "❌ No se encontró el ítem con ese nombre.\n" RESET);
  presioneTeclaParaContinuar();
  return 0;
}

//--------------------------------------------------------------OPCION 2 (DESCARTAR ITEM)--------------------------------------------------------------
// Esta función permite al jugador descartar un ítem de su inventario
// El ítem se elimina del inventario y vuelve al escenario actual
int descartar_item(Jugador *jugador) {
  if (list_size(jugador -> inventario) == 0) {
    puts("\nTu inventario está vacío. No hay nada que descartar.\n");
    presioneTeclaParaContinuar();
    return 0;
  }

  // Mostramos el inventario actual, recorriendo la lista de items que tenemos.
  printf("\n🎒 Inventario actual:\n");
  for (Item * item = list_first(jugador -> inventario); item != NULL; item = list_next(jugador -> inventario)) {
    printf(" - %s (valor: %d, peso: %d)\n", item -> nombre, item -> valor, item -> peso);
  }

  printf("\nIngrese el nombre del ítem que desea descartar: ");
  char nombre_item[100];
  scanf(" %[^\n]s", nombre_item);
  printf("\n");

  for (Item * item = list_first(jugador -> inventario); item != NULL; item = list_next(jugador -> inventario)) {
    if (is_equal(item -> nombre, nombre_item)) {
      jugador -> peso_total -= item -> peso;
      jugador -> puntaje_total -= item -> valor;

      list_popCurrent(jugador -> inventario); // Elimina del inventario
      list_pushBack(jugador -> escenario_actual -> items, item); // Lo deja en el escenario

      jugador -> tiempo_restante -= 1; // Se descuenta tiempo por la acción

      printf("Has descartado el ítem '%s'. Ahora está disponible en %s.\n", item ->nombre, jugador->escenario_actual->nombre);
      presioneTeclaParaContinuar();
      return 1;
    }
  }

  printf(RED "❌No tienes ningún ítem con ese nombre.\n" RESET);
  presioneTeclaParaContinuar();
  return 0;
}

//--------------------------------------------------------------OPCION 3 (MOVER JUGADOR)--------------------------------------------------------------
// Esta función permite al jugador moverse a otro escenario según las conexiones disponibles
// El movimiento depende del peso total que lleva el jugador (afecta al tiempo restante)
int mover_jugador(Jugador * jugador, HashMap * grafo) {
  
  const char * direcciones[] = {"Arriba (w)", "Abajo (s)", "Izquierda (a)", "Derecha (d)"};
  char direccion_char[] = {'w','s','a','d'};

  // Mostramos al jugador las direcciones posibles a las que puede ir
  printf("\n📍 Posibles direcciones desde %s:\n", jugador -> escenario_actual -> nombre);

  //Iteramos 4 veces (por la cantidad de direcciones posibles) para comprobar cuáles son válidas.
  for (int i = 0; i < 4; i++) {
    int destino_id = jugador->escenario_actual->conexiones[i];
    if (destino_id != -1) {
      char id_str[10];
      sprintf(id_str, "%d", destino_id);
      Pair * par = searchMap(grafo, id_str);
      // Si la dirección es correcta, se muestra.
      if (par && par -> value) {
        Escenario * destino = (Escenario * ) par -> value;
        printf("  - %s: %s\n", direcciones[i], destino -> nombre);
      }
    }
  }

  char direccion;
  printf("Ingrese su opción: ");
  scanf(" %c", & direccion);
  printf("\n");

  // Mapeamos la tecla ingresada al índice de la conexión correspondiente
  int indice = -1;
  if (direccion == 'w' || direccion == 'W') indice = 0;
  else if (direccion == 's' || direccion == 'S') indice = 1;
  else if (direccion == 'a' || direccion == 'A') indice = 2;
  else if (direccion == 'd' || direccion == 'D') indice = 3;
  else {
    printf(RED "❌ Opción inválida.\n" RESET);
    presioneTeclaParaContinuar();
    return 0;
  }

  // Revisar si la dirección ingresada por el ususario es posible
  int id_conexion = jugador -> escenario_actual -> conexiones[indice];
  if (id_conexion == -1) {
    puts(RED "❌ No puedes ir a esta dirección.\n" RESET);
    presioneTeclaParaContinuar();
    return 0;
  }

  // Mover al jugador al nuevo escenario
  char id_str[10];
  sprintf(id_str, "%d", id_conexion);
  Pair * par = searchMap(grafo, id_str);
  jugador -> escenario_actual = (Escenario * ) par -> value;

  // Calculamos la penalización de tiempo por el peso total que lleva
  jugador -> tiempo_restante -= (int)ceil((double)(jugador -> peso_total + 1) / 10);
  printf("🚶‍♂️ Te has movido a %s\n", jugador -> escenario_actual -> nombre);

  // Ejecutamos el sonido de bob esponja WAAAA
  PlaySound(TEXT("data/pasos.wav"), NULL, SND_FILENAME | SND_ASYNC);
  presioneTeclaParaContinuar();
  PlaySound(NULL, 0, 0);
  return 1;
}