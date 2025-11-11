#pragma once  // Evita inclusiones múltiples del mismo archivo
#include <cstdint>   // Tipos de tamaño fijo como uint8_t


// Estructura que representa una posición en el tablero.
// f = fila (0..9), c = columna (0..9)
struct Pos { int f; int c; };  // Estructura muy simple para direccionar celdas

// Enumeración para el color de jugador/pieza.
// Usamos uint8_t debajo para compilar más compacto.
enum class Color : uint8_t { Blanco = 0, Negro = 1 };  // Dos colores posibles

// Enumeración para el tipo de pieza.
enum class TipoPieza : uint8_t { Peon = 0, Dama = 1 }; // Peón o Dama (coronada)

// Estructura que representa una pieza en el tablero.
struct Pieza {
    Color color;            // A qué jugador pertenece
    TipoPieza tipo;         // Peón o Dama
};  // 'Pieza' se guarda dentro de celdas opcionales del tablero
