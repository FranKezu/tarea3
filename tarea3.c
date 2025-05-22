#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>
#include <math.h>

// ID,Nombre,Descripcion,Items,Arriba,Abajo,Izquierda,Derecha,EsFinal}

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

void *leer_escenarios() {
    FILE *archivo = fopen("graphquest.csv", "r");
    if (archivo == NULL) {
        perror("Error al abrir el archivo");
        return NULL;
    }

    HashMap *grafo = createMap(100); // Capacidad inicial
    char **campos;

    // Ignorar la primera línea (encabezados)
    campos = leer_linea_csv(archivo, ',');

    while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
        Escenario *esc = (Escenario *)malloc(sizeof(Escenario));
        esc->id = atoi(campos[0]);
        esc->nombre = strdup(campos[1]);
        esc->descripcion = strdup(campos[2]);

        // Parsear ítems
        esc->items = split_string(campos[3], ";");
        List *items_procesados = list_create();
        for (char *item = list_first(esc->items); item != NULL; item = list_next(esc->items)) {
            List *valores = split_string(item, ",");
            Item *nuevo_item = (Item *)malloc(sizeof(Item));
            nuevo_item->nombre = strdup((char *)list_first(valores));
            nuevo_item->valor = atoi((char *)list_next(valores));
            nuevo_item->peso = atoi((char *)list_next(valores));
            list_pushBack(items_procesados, nuevo_item);
            list_clean(valores);
            free(valores);
        }
        list_clean(esc->items);
        free(esc->items);
        esc->items = items_procesados;

        // Conexiones
        esc->conexiones[0] = atoi(campos[4]); // Arriba
        esc->conexiones[1] = atoi(campos[5]); // Abajo
        esc->conexiones[2] = atoi(campos[6]); // Izquierda
        esc->conexiones[3] = atoi(campos[7]); // Derecha
        esc->es_final = atoi(campos[8]);

        // Insertar en el HashMap (usar ID como clave)
        char id_str[10];
        sprintf(id_str, "%d", esc->id);
        insertMap(grafo, id_str, esc);

        // Liberar memoria de campos (no es necesario con la implementación actual de leer_linea_csv)
    }

    fclose(archivo);
    printf("Se ha guardado correctamente\n");
    return grafo;
}

//---------------------------------------------------------------------------------------------------------------------------
int seleccionar_modo_juego() {
    int modo;
    printf("=== BIENVENIDO A GRAPHQUEST ===\n");
    printf("Selecciona modo de juego:\n");
    printf("1. Un jugador\n");
    printf("2. Coperativo\n");
    printf("Opción: ");
    scanf("%d", &modo);

    if (modo != 1 && modo != 2) {
        printf("Opción inválida. Por favor intente de nuevo.\n");
        scanf("%d", &modo);
    }
    return modo;
}

Jugador *crear_jugador(Escenario *escenario_inicial, char *usuario) {
  Jugador *jugador = (Jugador *)malloc(sizeof(Jugador));
  jugador->inventario = list_create();
  jugador->peso_total = 0;
  jugador->puntaje_total = 0;
  jugador->tiempo_restante = 30; //REVISAR TIEMPO 
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


void mostrar_estado(Jugador * jugador) {
  limpiarPantalla();

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

  // Menú de opciones bonito
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

//--------------------------------------------------------------OPCION 1 (RECOGER ITEM)--------------------------------------------------------------
void recoger_item(Jugador *jugador) {
  if (list_size(jugador->escenario_actual->items) == 0) {
    printf("No hay ítems en este escenario.\n");
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
    printf("Tu inventario está vacío. No hay nada que descartar.\n");
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
  scanf(" %c", & direccion);

  int indice = -1;
  if (direccion == 'w') indice = 0;
  else if (direccion == 's') indice = 1;
  else if (direccion == 'a') indice = 2;
  else if (direccion == 'd') indice = 3;
  else {
    printf("Dirección inválida.\n");
    return;
  }

  int id_conexion = jugador -> escenario_actual -> conexiones[indice];
  if (id_conexion == -1) {
    printf("No hay conexión en esa dirección.\n");
    return;
  }

  char id_str[10];
  sprintf(id_str, "%d", id_conexion);
  Pair * par = searchMap(grafo, id_str);
  if (par == NULL) {
    printf("No se encontró el escenario al que quieres ir.\n");
    return;
  }

  jugador -> escenario_actual = (Escenario * ) par -> value;
  jugador -> tiempo_restante -= (jugador -> peso_total + 1) / 10;
  printf("Te has movido a: %s\n", jugador -> escenario_actual -> nombre);
}
//----------------------------------------------------------------------------------------------------------------------------------------------------

void mostrar_menu(Jugador *jugador, HashMap *grafo) {
  char opcion;
  do {
    mostrar_estado(jugador);
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
      //reiniciar_partida()
      break;
    case '5':
      puts("Saliendo del juego...");
      break;
    default:
      puts("Opción no válida. Por favor, intente de nuevo.");
    }

  } while (opcion != '5');
}

void iniciar_solo(Jugador *jugador, HashMap *grafo) {
  printf(">> Iniciando juego para un jugador...\n");
  mostrar_menu(jugador, grafo);
    // Aquí pondrás la lógica del juego con un solo jugador
}

void iniciar_cooperativo(Jugador *jugador1, Jugador *jugador2, HashMap *grafo) {
  printf(">> Iniciando juego para dos jugadores...\n");
    // Aquí irá la lógica del juego con turnos (la agregaremos en el siguiente paso)
}

int main() {
  setlocale(LC_ALL, "es_ES.UTF-8"); // Para que se puedan ver tildes, ñ, y carácteres especiales.
  HashMap *grafo = createMap(100);
  grafo = leer_escenarios(grafo);

  int modo = seleccionar_modo_juego();
  //---------------------------------------------------------------------------
  Pair *par = searchMap(grafo, "1"); // "0" es la clave como string
  
  Escenario *inicio = (Escenario *)par->value;
  
  Jugador *jugador1 = crear_jugador(inicio, NULL);
  Jugador *jugador2 = NULL;
  
  if (modo == 2) jugador2 = crear_jugador(inicio, jugador1->usuario);
  
  if (modo == 1) iniciar_solo(jugador1, grafo);

  else iniciar_cooperativo(jugador1, jugador2, grafo);

  return 0;
}