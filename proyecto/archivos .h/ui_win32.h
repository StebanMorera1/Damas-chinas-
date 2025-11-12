#pragma once                         
#include <windows.h>                    
#include <windowsx.h>                   
#include <string>                       
#include "../logica/juego.h"            

class UIWin32 {
public:
    UIWin32();
    const RECT& botonUndoRect() const { return m_btnUndo; }                         

    void asociar(HWND hWnd) { m_hWnd = hWnd; }    
    void pintar(HDC hdc, RECT rc);                
    void click(int x, int y);                     
    void nuevoJuego();                            
    void deshacer();                              
    void cancelarSeleccion() { m_juego.cancelarSeleccion(); InvalidateRect(m_hWnd, nullptr, FALSE); }

public:
    void tick();
    void reiniciar();
    void exportarHistorial() const;
    void exportarTableroBMP();

private:
    HWND m_hWnd{nullptr};             
    Juego m_juego;                    

    mutable RECT m_btnUndo{0,0,0,0};  
    mutable RECT m_btnNew{0,0,0,0}; 
    mutable RECT m_chkCoords{0,0,0,0};

    bool m_mostrarCoords{true};
    DWORD m_lastTick{0};
    int m_secsBlancas{0};
    int m_secsNegras{0};
    Color m_lastTurn{Color::Blanco};
    
    int m_timeLeftBlancas{300};
    int m_timeLeftNegras{300};
    bool m_gameOver{false};
    std::wstring m_ganador;
  
    bool m_tieneUltimo{false};
    Pos m_lastDesde{0,0};
    Pos m_lastHasta{0,0};

    Pos  puntoACelda(int x, int y, RECT rcTab) const;  
    RECT rectTablero(RECT rcCliente) const;            
    void dibujarPanelInfo(HDC hdc, RECT rcCliente, RECT rtTab) const; 

};
