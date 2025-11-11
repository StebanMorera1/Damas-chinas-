#pragma once                          // Protección de inclusión
#include <array>                          // Matriz fija 10x10
#include <optional>                       // Para celdas vacías o con pieza
#include "tipos.h"                        // Tipos básicos del dominio

// ============================================================
/* Archivo: dominios/tablero.h
   Autor: Steban Felipe Morera Busaquillo
   Descripción: Representa el tablero 10x10 y operaciones básicas
                de lectura/escritura sobre sus celdas. */
// ============================================================
class Tablero {
public:
    static constexpr int N = 10;          // Tamaño del tablero (10x10)

    Tablero();                            // Constructor: deja el tablero limpio

    void limpiar();                        // Pone todas las celdas vacías

    // Verifica que una posición esté dentro de los límites 0..9
    static bool dentro(const Pos& p) { return p.f>=0 && p.f<N && p.c>=0 && p.c<N; }

    // Determina si una casilla es "oscura" (donde se juegan las piezas)
    static bool casillaOscura(const Pos& p) { return ((p.f + p.c) % 2) == 1; }

    // Acceso de sólo lectura a una celda
    const std::optional<Pieza>& get(const Pos& p) const { return m_celdas[p.f][p.c]; }

    // Acceso de lectura/escritura a una celda
    std::optional<Pieza>& get(const Pos& p) { return m_celdas[p.f][p.c]; }

    // Coloca una pieza en una posición
    void poner(const Pos& p, const Pieza& x) { m_celdas[p.f][p.c] = x; }

    // Quita (vacía) una posición
    void quitar(const Pos& p) { m_celdas[p.f][p.c] = std::nullopt; }

    // Inicializa 20 piezas por jugador en casillas oscuras
    void inicializar();

private:
    // Matriz 10x10 de celdas que pueden estar vacías (nullopt) o tener 'Pieza'
    std::array<std::array<std::optional<Pieza>, N>, N> m_celdas{};  // Se inicializa vacía
};
