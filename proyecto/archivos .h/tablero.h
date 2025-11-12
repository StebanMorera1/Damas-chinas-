#pragma once                          
#include <array>                         
#include <optional>                       
#include "tipos.h"                        

class Tablero {
public:
    static constexpr int N = 10;          

    Tablero();                           

    void limpiar();                       

    static bool dentro(const Pos& p) { return p.f>=0 && p.f<N && p.c>=0 && p.c<N; }

    static bool casillaOscura(const Pos& p) { return ((p.f + p.c) % 2) == 1; }

    const std::optional<Pieza>& get(const Pos& p) const { return m_celdas[p.f][p.c]; }

    std::optional<Pieza>& get(const Pos& p) { return m_celdas[p.f][p.c]; }

    void poner(const Pos& p, const Pieza& x) { m_celdas[p.f][p.c] = x; }

    void quitar(const Pos& p) { m_celdas[p.f][p.c] = std::nullopt; }

    void inicializar();

private:
    std::array<std::array<std::optional<Pieza>, N>, N> m_celdas{};  
};
