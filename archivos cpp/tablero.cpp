#include "../dominios/tablero.h"   

Tablero::Tablero() { limpiar(); } 

void Tablero::limpiar() {
    for (auto& fila : m_celdas)        
        fila.fill(std::nullopt);       
}

void Tablero::inicializar() {
    limpiar();  

    for (int f = 0; f < 4; ++f) {            
        for (int c = 0; c < N; ++c) {        
            Pos p{ f, c };                  
            if (casillaOscura(p))            
                poner(p, Pieza{ Color::Negro, TipoPieza::Peon }); 
        }
    }
        for (int f = N - 4; f < N; ++f) {

        for (int c = 0; c < N; ++c) {         
            Pos p{ f, c };          

            if (casillaOscura(p))  

                poner(p, Pieza{ Color::Blanco, TipoPieza::Peon }); 
        }
    }
}
