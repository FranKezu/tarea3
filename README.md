# 👾 Tarea 3: GRAPHQUEST

* **👨‍💻 Integrantes:** Ariel Leiva y Franco Bernal
* **📚 Asignatura:** Estructura de Datos (ICI2240-2)
* **📅 Fecha de entrega:** 25-05-2025

---

## 🚀 ¿Cómo ejecutar el programa?

### 🟢 Opción 1: Ejecutar el archivo

Puedes ejecutar el programa directamente haciendo doble clic en el archivo `run` que se encuentra en la carpeta del proyecto.
Esto abrirá automáticamente una consola con el programa corriendo.

---

### 🛠️ Opción 2: Compilar manualmente

1. Abre la consola de Visual Studio Code como por ejemplo.
2. Compila el programa con el siguiente comando:

```bash
gcc main.c tdas\list.c tdas\extra.c tdas\hashmap.c grafo.c -o main -lwinmm
```

3. Luego ejecútalo con:

```bash
./main
```

En caso de ver carácteres raros, es porque no se están mostrando los tildes correctamente, para solucionar esto basta con escribir lo siguiente la primera vez antes de ejecturar el programa.

```powershell
$OutputEncoding = [Console]::OutputEncoding = [Text.UTF8Encoding]::new()
```

---

### 📋 ¿Qué hace el programa?

Este programa implementa un juego de aventura por texto llamado GraphQuest, en el que uno o dos jugadores recorren escenarios conectados, recogiendo ítems, gestionando su inventario y tiempo restante, con el objetivo de alcanzar un escenario final. Incluye modos de juego individual y cooperativo, donde ambos jugadores deben completar el juego para ganar. El sistema utiliza estructuras como listas y mapas hash para manejar inventarios, escenarios y conexiones, y ofrece un menú interactivo que permite al jugador tomar decisiones en cada turno.

---

### 🔧 Requisitos previos (sólo si deseas compilarlo manualmente)

- Tener instalado **Visual Studio Code.**
- Instalar la extensión para **C/C++**.
- Tener instalado un compilador para C, como **MinGW.**

---

---

### 📂 Archivos requeridos (Incluídos en el repositorio)

- `grafo.c` y `grafo.h`: contiene funciones importantes del movimiento entre gráfos.
- `main.c`: contiene el menú del programa con sus respectivas funciones.
- `data/graphquest.csv`: base de datos que contiene el juego.
- `tdas/list.c` y `tdas/list.h`:
- `tdas/extra.c` y `tdas/extra.h`
- `tdas/hashmap.c` y `tdas/hashmap.h`

### 🚀 Ejemplo simple de uso

**Iniciar el programa**:

```bash
   Seleccione el modo de juego:
   1. 🎮 Un jugador
   2. 🤝 Cooperativo
   3. 🚪 Salir
```

1. **Seleccionamos un modo (ej: Un Jugador (1) )**:
   - Selecciona la opción `1`.

```
   >> Iniciando GRAPHQUEST para un jugador...
👤 Ingrese el nombre de usuario: Ariel
```

- Se muestra el menú.

```
   ════════════════════════════════════════════════════════════════════════════════════════
   📍 Estás en: Entrada principal
   📝 Una puerta rechinante abre paso a esta mansion olvidada por los dioses y los conserjes. El aire huele a humedad y a misterios sin resolver.
   🎒 Peso: 0 | 💎 Puntaje: 0 | ⏳ Tiempo restante: 30
   ════════════════════════════════════════════════════════════════════════════════════════
   📭 No hay ítems en este escenario.

   🎒 Tu inventario está vacío.

   ╔════════════════════════════════╗
   ║      OPCIONES DEL JUGADOR      ║
   ╚════════════════════════════════╝
   1.🎁 Recoger Ítem(s),
   2.🗑️  Descartar Ítem(s),
   3.🧭 Avanzar en una Dirección,
   4.🔄 Reiniciar Partida,
   5.🚪 Salir del Juego,
   Ingrese su opción: 3
```

- Selecciona la opción `3`(Avanzar).

```
   📍 Posibles direcciones desde Entrada principal:
  
   Abajo (s): Cocina,
   Ingrese su opción: s (Tu jugador se movera en esa dirección).
```

- Así el jugador se va desplazando en las distintas habitaciones y recogiendo como descartando items a su gusto, hasta que se acabe el tiempo o el jugador consiga salir (habitacion llamada "Salida").

```
   🎉 ¡GANASTE! 🎉

   📦 Ítems Obtenidos: (Se muestran los items recogidos por el jugador).
   
   💎 Puntaje Total Obtenido: (Puntaje total).

   "FIN PROGRAMA".
```

---

### 🔨 Comentario como grupo

- Como dupla nos fue muy fácil trabajar, programamos siempre juntos, ya sea presencialmente o por discord, por lo que el trabajo fue equitativamente bien distribuido. No tuvimos ningún tipo de inconveniente al realizar la tarea.

---

### 🐞 Posibles problemas encontrados

- **Soporte**: solo Windows, usuarios de MacOS podrían tener dificultades ejecutando el programa.
- **Entrada inválida**: si se ingresan caracteres cuando se solicita un número, el programa puede trabarse.
- **Funciones**: estas se encargan de que el juego funcione correctamente, por lo que si una de estas falla, el juego se corrompe, por lo que nos aseguramos de que todas las funciones cumplan con su tarea y así darle la mejor experiencia al jugador o los jugadores dependiendo de la opción de juego que desean.