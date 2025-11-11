#pragma once                               // Protección de inclusión
#include <vector>                               // Listas de movimientos
#include <optional>                             // Selección opcional
#include <string>
#include <deque>                               // Mensajes de usuario
#include "../dominios/tablero.h"                // Acceso al tablero

// ============================================================
/* Archivo: logica/juego.h
   Autor: Steban Felipe Morera Busaquillo
   Descripción: Reglas del juego (movimientos, capturas, turnos,
                coronación y verificación de victoria). */
// ============================================================

// Posibles resultados al intentar mover
enum class ResultadoMovimiento {
    Ok,                                 // Todo bien
    Invalido,                           // Movimiento no permitido
    NoEsTuPieza,                        // Intentó mover pieza del rival
    CasillaOcupada,                     // Destino no está libre
    FueraDeTablero,                     // Coordenadas inválidas
    MovimientoNoDiagonal,               // (reservado)
    MovimientoRetrocesoPeon,            // (reservado)
    CapturaObligatoriaNoRealizada       // Había captura y no la hizo
};

// Estructura de movimiento calculado por las reglas
struct Movimiento {
    Pos desde;                          // Origen
    Pos hasta;                          // Destino
    bool esCaptura{false};              // ¿Es captura?
    std::optional<Pos> capturada;       // Pos. de la pieza capturada (si aplica)
};

// Clase principal de la lógica del juego
class Juego {
public:
    Juego();                            // Constructor: inicia una nueva partida

    // Acceso de sólo lectura al tablero
    const Tablero& tablero() const { return m_tablero; }

    // Color del turno actual
    Color turno() const { return m_turno; }

    // ¿Hay selección activa?
    bool haySeleccion() const { return m_sel.has_value(); }

    // Obtener la casilla seleccionada (si la hay)
    std::optional<Pos> seleccion() const { return m_sel; }

    // Intenta seleccionar una pieza en 'p'
    bool seleccionar(const Pos& p, std::string& msg);

    // Intenta mover la pieza seleccionada al 'destino'
    ResultadoMovimiento moverA(const Pos& destino, std::string& msg);

    // Lista de movimientos válidos para la pieza en 'p'
    std::vector<Movimiento> movimientosValidosDesde(const Pos& p) const;

    // ¿Alguna pieza del color 'c' tiene captura disponible?
    bool hayAlgunaCaptura(Color c) const;

    // ¿Ese color ya no tiene piezas?
    bool sinPiezas(Color c) const;

    // Reinicia completamente la partida
    void reiniciar();

    // --- Extensiones: capturas encadenadas / bloqueo ---
    bool puedeSeguirCapturandoDesde(const Pos& p) const;  // Continúa cadena de capturas
    bool sinMovimientos(Color c) const;                   // Victoria por bloqueo

    // Conteos y puntaje
    int contarPiezas(Color c) const;
    int contarDamas(Color c) const;
    int puntaje(Color c) const;

    // --- Historial de jugadas (para panel y deshacer) ---
    const std::deque<std::wstring>& historialJugadas() const { return m_historialTexto; }
    bool deshacer();                                       // Revierte la última jugada (si existe)

    // Limpia la selección actual (si existe)
    void cancelarSeleccion() { m_sel.reset(); }

private:
    Tablero m_tablero;                 // Estado del tablero
    Color m_turno{ Color::Blanco };    // Inician blancas (convención)
    std::optional<Pos> m_sel;          // Casilla actualmente seleccionada

    // Cálculo para peón y dama
    std::vector<Movimiento> calcularMovimientosPeon(const Pos& p) const;
    std::vector<Movimiento> calcularMovimientosDama(const Pos& p) const;

    // Dirección de avance de peón según color
    static int dirAvance(Color c) { return (c==Color::Blanco) ? -1 : +1; }

    // Convierte peón a dama si llegó al extremo
    void coronarSiAplica(const Pos& p);

    // Cambia de turno
    void pasarTurno();

    // --- Soporte de historial/deshacer ---
    struct Snapshot { Tablero tablero; Color turno; };     // Estado mínimo para deshacer
    void pushSnapshot();                                   // Guardar estado previo a una jugada
    static std::wstring posTexto(const Pos& p);            // Formato A1..J10 (1-10 desde arriba)
    std::deque<Snapshot> m_stack;                          // Pila de undo
    std::deque<std::wstring> m_historialTexto;             // Texto de jugadas para UI (limitar tamaño)
    void addHistorial(const std::wstring& w);              // Añadir entrada (con límite)
};
