#pragma once                             
#include <vector>                              
#include <optional>                             
#include <string>
#include <deque>                               
#include "../dominios/tablero.h"               


enum class ResultadoMovimiento {
    Ok,                                 
    Invalido,                           
    NoEsTuPieza,                        
    CasillaOcupada,                   
    FueraDeTablero,                   
    MovimientoNoDiagonal,              
    MovimientoRetrocesoPeon,           
    CapturaObligatoriaNoRealizada      
};

struct Movimiento {
    Pos desde;                          
    Pos hasta;                          
    bool esCaptura{false};              
    std::optional<Pos> capturada;       
};

class Juego {
public:
    Juego();                           

    const Tablero& tablero() const { return m_tablero; }

    Color turno() const { return m_turno; }

    bool haySeleccion() const { return m_sel.has_value(); }

    std::optional<Pos> seleccion() const { return m_sel; }

    bool seleccionar(const Pos& p, std::string& msg);

    ResultadoMovimiento moverA(const Pos& destino, std::string& msg);

    std::vector<Movimiento> movimientosValidosDesde(const Pos& p) const;

    bool hayAlgunaCaptura(Color c) const;

    bool sinPiezas(Color c) const;

    void reiniciar();

    bool puedeSeguirCapturandoDesde(const Pos& p) const;  
    bool sinMovimientos(Color c) const;                   

    int contarPiezas(Color c) const;
    int contarDamas(Color c) const;
    int puntaje(Color c) const;

    const std::deque<std::wstring>& historialJugadas() const { return m_historialTexto; }
    bool deshacer();                                       

    void cancelarSeleccion() { m_sel.reset(); }

private:
    Tablero m_tablero;                 
    Color m_turno{ Color::Blanco };    
    std::optional<Pos> m_sel;          

    std::vector<Movimiento> calcularMovimientosPeon(const Pos& p) const;
    std::vector<Movimiento> calcularMovimientosDama(const Pos& p) const;

    static int dirAvance(Color c) { return (c==Color::Blanco) ? -1 : +1; }

    void coronarSiAplica(const Pos& p);

    void pasarTurno();

    struct Snapshot { Tablero tablero; Color turno; };     
    void pushSnapshot();                                   
    static std::wstring posTexto(const Pos& p);            
    std::deque<Snapshot> m_stack;                          
    std::deque<std::wstring> m_historialTexto;             
    void addHistorial(const std::wstring& w);              
};


