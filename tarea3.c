#include "tdas/extra.h"
#include "tdas/list.h"
#include "tdas/hashmap.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// ID,Nombre,Descripcion,Items,Arriba,Abajo,Izquierda,Derecha,EsFinal

void leer_escenarios() {
  FILE *archivo = fopen("graphquest.csv", "r");
  if (archivo == NULL) {
    perror(
        "Error al abrir el archivo");
    return;
  }

  char **campos;
  campos = leer_linea_csv(archivo, ',');

  while ((campos = leer_linea_csv(archivo, ',')) != NULL) {
    printf("ID: %d\n", atoi(campos[0]));
    printf("Nombre: %s\n", campos[1]);
    printf("Descripción: %s\n", campos[2]);

    List* items = split_string(campos[3], ";");

    printf("Items: \n");
    for(char *item = list_first(items); item != NULL; 
          item = list_next(items)){

        List* values = split_string(item, ",");
        char* item_name = list_first(values);
        int item_value = atoi(list_next(values));
        int item_weight = atoi(list_next(values));
        printf("  - %s (%d pts, %d kg)\n", item_name, item_value, item_weight);
        list_clean(values);
        free(values);
    }

    int arriba = atoi(campos[4]);
    int abajo = atoi(campos[5]);
    int izquierda = atoi(campos[6]);
    int derecha = atoi(campos[7]);

    if (arriba != -1) printf("Arriba: %d\n", arriba);
    if (abajo != -1) printf("Abajo: %d\n", abajo);
    if (izquierda != -1) printf("Izquierda: %d\n", izquierda);
    if (derecha != -1) printf("Derecha: %d\n", derecha);

    
    int is_final = atoi(campos[8]);
    if (is_final) printf("Es final\n");
    printf("\n");

    list_clean(items);
    free(items);
    
  }
  fclose(archivo);

}

int main() {
  leer_escenarios();

  return 0;
}