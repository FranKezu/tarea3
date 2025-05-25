#include "tdas/list.h"
#include "tdas/hashmap.h"
#include "tdas/extra.h"
#include "grafo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <mmsystem.h>
#pragma comment(lib, "winmm.lib")

void * leer_escenarios() {
  FILE * archivo = fopen("graphquest.csv", "r");
  if (archivo == NULL) {
    perror("Error al abrir el archivo");
    return NULL;
  }

  HashMap * grafo = createMap(100); // Capacidad inicial
  char ** campos;

  // Ignorar la primera línea (encabezados)
  campos = leer_linea_csv(archivo, ',');

  while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
    Escenario * esc = (Escenario * ) malloc(sizeof(Escenario));
    esc -> id = atoi(campos[0]);
    esc -> nombre = strdup(campos[1]);
    esc -> descripcion = strdup(campos[2]);

    // Parsear ítems
    esc -> items = split_string(campos[3], ";");
    List * items_procesados = list_create();
    for (char * item = list_first(esc -> items); item != NULL; item = list_next(esc -> items)) {
      List * valores = split_string(item, ",");
      Item * nuevo_item = (Item * ) malloc(sizeof(Item));
      nuevo_item -> nombre = strdup((char * ) list_first(valores));
      nuevo_item -> valor = atoi((char * ) list_next(valores));
      nuevo_item -> peso = atoi((char * ) list_next(valores));
      list_pushBack(items_procesados, nuevo_item);
      list_clean(valores);
      free(valores);
    }
    list_clean(esc -> items);
    free(esc -> items);
    esc -> items = items_procesados;

    // Conexiones
    esc -> conexiones[0] = atoi(campos[4]); // Arriba
    esc -> conexiones[1] = atoi(campos[5]); // Abajo
    esc -> conexiones[2] = atoi(campos[6]); // Izquierda
    esc -> conexiones[3] = atoi(campos[7]); // Derecha
    esc -> es_final = atoi(campos[8]);

    // Insertar en el HashMap (usar ID como clave)
    char id_str[10];
    sprintf(id_str, "%d", esc -> id);
    insertMap(grafo, id_str, esc);

    // Liberar memoria de campos (no es necesario con la implementación actual de leer_linea_csv)
  }

  fclose(archivo);
  return grafo;
}

void recoger_item(Jugador *jugador) {
  if (list_size(jugador->escenario_actual->items) == 0) {
    puts("No hay ítems disponibles para recoger.\n");
    presioneTeclaParaContinuar();
    return;
  }

  printf("Nombre del ítem que quieres recoger: ");
  char nombre_item[100];
  scanf(" %[^\n]s", nombre_item);

  for (Item *item = list_first(jugador->escenario_actual->items); item != NULL; item = list_next(jugador->escenario_actual->items)) {
    if (is_equal(item->nombre, nombre_item)) {

      list_pushBack(jugador->inventario, item);
      jugador->peso_total += item->peso;
      jugador->puntaje_total += item->valor;

      list_popCurrent(jugador->escenario_actual->items); // Eliminar del escenario
      printf("Has recogido el ítem: %s\n", item->nombre);
      jugador->tiempo_restante -= 1; //TIEMPO DESCONTADO POR AGARRAR ITEM
      return;
    }
  }

  printf("No se encontró el ítem con ese nombre.\n");
}
//----------------------------------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------OPCION 2 (DESCARTAR ITEM)--------------------------------------------------------------
void descartar_item(Jugador *jugador) {
  if (list_size(jugador -> inventario) == 0) {
    puts("Tu inventario está vacío. No hay nada que descartar.\n");
    presioneTeclaParaContinuar();
    return;
  }

  printf("\n🎒 Inventario actual:\n");
  for (Item * item = list_first(jugador -> inventario); item != NULL; item = list_next(jugador -> inventario)) {
    printf(" - %s (valor: %d, peso: %d)\n", item -> nombre, item -> valor, item -> peso);
  }

  printf("\nIngrese el nombre del ítem que desea descartar: ");
  char nombre_item[100];
  scanf(" %[^\n]s", nombre_item);

  for (Item * item = list_first(jugador -> inventario); item != NULL; item = list_next(jugador -> inventario)) {
    if (is_equal(item -> nombre, nombre_item)) {
      jugador -> peso_total -= item -> peso;
      jugador -> puntaje_total -= item -> valor;

      list_popCurrent(jugador -> inventario); // Elimina del inventario
      list_pushBack(jugador -> escenario_actual -> items, item); // Lo deja en el escenario

      jugador -> tiempo_restante -= 1;

      printf("Has descartado el ítem '%s'. Ahora está disponible en %s.\n", item -> nombre, jugador -> escenario_actual -> nombre);
      return;
    }
  }

  printf("No tienes ningún ítem con ese nombre.\n");
}
//----------------------------------------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------OPCION 3 (MOVER JUGADOR)--------------------------------------------------------------
void mover_jugador(Jugador * jugador, HashMap * grafo) {
  
  const char * direcciones[] = {"Arriba (w)", "Abajo (s)", "Izquierda (a)", "Derecha (d)"};
  char direccion_char[] = {'w','s','a','d'};

  printf("\n📍 Posibles direcciones desde %s:\n", jugador -> escenario_actual -> nombre);

  for (int i = 0; i < 4; i++) {
    int destino_id = jugador -> escenario_actual -> conexiones[i];
    if (destino_id != -1) {
      char id_str[10];
      sprintf(id_str, "%d", destino_id);
      Pair * par = searchMap(grafo, id_str);
      if (par && par -> value) {
        Escenario * destino = (Escenario * ) par -> value;
        printf("  - %s: %s\n", direcciones[i], destino -> nombre);
      }
    }
  }

  char direccion;
  printf("Ingrese su opción: ");
  scanf(" %c", & direccion);

  int indice = -1;
  if (direccion == 'w') indice = 0;
  else if (direccion == 's') indice = 1;
  else if (direccion == 'a') indice = 2;
  else if (direccion == 'd') indice = 3;
  else {
    printf("Opción inválida.\n");
    presioneTeclaParaContinuar();

    return;
  }

  int id_conexion = jugador -> escenario_actual -> conexiones[indice];
  if (id_conexion == -1) {
    puts("No puedes ir a esta dirección.\n");
    presioneTeclaParaContinuar();
    return;
  }

  char id_str[10];
  sprintf(id_str, "%d", id_conexion);
  Pair * par = searchMap(grafo, id_str);

  jugador -> escenario_actual = (Escenario * ) par -> value;
  jugador -> tiempo_restante -= (jugador -> peso_total + 1) / 10;
  printf("Te has movido a %s\n", jugador -> escenario_actual -> nombre);
  PlaySound(TEXT("audio/pasos.wav"), NULL, SND_FILENAME | SND_ASYNC);
  presioneTeclaParaContinuar();
}