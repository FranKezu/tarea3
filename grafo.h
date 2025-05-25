#ifndef GRAFO_H
#define GRAFO_H

#include "tdas/list.h"
#include "tdas/hashmap.h"
#include <stdio.h>


// ID,Nombre,Descripcion,Items,Arriba,Abajo,Izquierda,Derecha,EsFinal

typedef struct {
  char *nombre;
  int valor;
  int peso;
} Item;

typedef struct {
  int id;
  char *nombre;
  char *descripcion;
  List *items;
  int conexiones[4];
  int es_final;
} Escenario;

typedef struct {
  char *usuario;
  List *inventario;
  int peso_total;
  int puntaje_total;
  int tiempo_restante;
  Escenario *escenario_actual;
} Jugador;

char **leer_linea_csv(FILE *archivo, char separador);
List *split_string(const char *str, const char *delimitador);
void *leer_escenarios();
void recoger_item(Jugador *jugador);
void descartar_item(Jugador *jugador);
void mover_jugador(Jugador *jugador, HashMap *grafo);

#endif