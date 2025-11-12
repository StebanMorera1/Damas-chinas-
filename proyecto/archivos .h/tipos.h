#pragma once  o
#include <cstdint>   

struct Pos { int f; int c; };  

enum class Color : uint8_t { Blanco = 0, Negro = 1 }; 

enum class TipoPieza : uint8_t { Peon = 0, Dama = 1 };

struct Pieza {
    Color color;            
    TipoPieza tipo;        
};  
