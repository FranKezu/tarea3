#ifndef GRAFO_H
#define GRAFO_H

#include "tdas/list.h"
#include "tdas/hashmap.h"
#include <stdio.h>

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
  int termino;
  Escenario *escenario_actual;
} Jugador;

char **leer_linea_csv(FILE *archivo, char separador);
List *split_string(const char *str, const char *delimitador);
void *leer_escenarios();
int recoger_item(Jugador *jugador);
int descartar_item(Jugador *jugador);
int mover_jugador(Jugador *jugador, HashMap *grafo);

#endif