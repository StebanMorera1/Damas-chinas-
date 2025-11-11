#include "../dominios/tablero.h"   // Header correspondiente

// ============================================================
/* Archivo: dominios/tablero.cpp
   Autor: Steban Felipe Morera Busaquillo
   Descripción: Implementación de operaciones del tablero. */
// ============================================================

Tablero::Tablero() { limpiar(); }  // Al construir, se limpia el tablero

void Tablero::limpiar() {
    for (auto& fila : m_celdas)        // Recorremos cada fila
        fila.fill(std::nullopt);       // y la llenamos con 'vacío'
}

void Tablero::inicializar() {
    limpiar();  // Partimos de tablero vacío

    // --- Negras arriba (filas 0..3) ---
    for (int f = 0; f < 4; ++f) {             // Primeras 4 filas
        for (int c = 0; c < N; ++c) {         // Todas las columnas
            Pos p{ f, c };                    // Posición actual
            if (casillaOscura(p))             // Sólo en casillas oscuras
                poner(p, Pieza{ Color::Negro, TipoPieza::Peon }); // Colocar peón negro
        }
    }

    // --- Blancas abajo (filas 6..9) ---
    for (int f = N - 4; f < N; ++f) {         // Últimas 4 filas
        for (int c = 0; c < N; ++c) {         // Todas las columnas
            Pos p{ f, c };                    // Posición actual
            if (casillaOscura(p))             // Sólo en casillas oscuras
                poner(p, Pieza{ Color::Blanco, TipoPieza::Peon }); // Colocar peón blanco
        }
    }
}
