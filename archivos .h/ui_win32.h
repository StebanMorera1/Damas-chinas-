#pragma once                         // Protección de inclusión
#include <windows.h>                    // API base de Windows
#include <windowsx.h>                   // Macros GET_X/Y_LPARAM para mouse
#include <string>                       // Texto para la UI
#include "../logica/juego.h"            // Acceso a la lógica del juego

class UIWin32 {
public:
    UIWin32();
    const RECT& botonUndoRect() const { return m_btnUndo; }                          // Constructor por defecto

    void asociar(HWND hWnd) { m_hWnd = hWnd; }    // Enlaza la ventana
    void pintar(HDC hdc, RECT rc);                // Dibuja el tablero y piezas
    void click(int x, int y);                     // Maneja un clic del mouse
    void nuevoJuego();                            // Reinicia la partida
    void deshacer();                              // Deshacer última jugada
    void cancelarSeleccion() { m_juego.cancelarSeleccion(); InvalidateRect(m_hWnd, nullptr, FALSE); }

public:
    void tick();
    void reiniciar();
    void exportarHistorial() const;
    void exportarTableroBMP();

private:
    HWND m_hWnd{nullptr};             // Handle de ventana asociado
    Juego m_juego;                    // Instancia de la lógica

    mutable RECT m_btnUndo{0,0,0,0};  // Área del botón Deshacer (se actualiza al pintar)
    mutable RECT m_btnNew{0,0,0,0}; // Área del botón Nueva partida
    mutable RECT m_chkCoords{0,0,0,0}; // Área del checkbox Coordenadas

    // Reloj
    bool m_mostrarCoords{true};
    DWORD m_lastTick{0};
    int m_secsBlancas{0};
    int m_secsNegras{0};
    Color m_lastTurn{Color::Blanco};
    // Cuenta regresiva (por defecto 5 min por jugador)
    int m_timeLeftBlancas{300};
    int m_timeLeftNegras{300};
    bool m_gameOver{false};
    std::wstring m_ganador;
    // Último movimiento para resaltado
    bool m_tieneUltimo{false};
    Pos m_lastDesde{0,0};
    Pos m_lastHasta{0,0};

    // Utilidades internas de dibujo/posicionamiento
    Pos  puntoACelda(int x, int y, RECT rcTab) const;  // Traducir (x,y) a (fila,col)
    RECT rectTablero(RECT rcCliente) const;            // Rectángulo cuadrado centrado del tablero
    void dibujarPanelInfo(HDC hdc, RECT rcCliente, RECT rtTab) const; // Panel lateral

};
