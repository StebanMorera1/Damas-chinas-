#include "../logica/juego.h"      // Declaración de la clase Juego
#include <algorithm>              // std::find_if

Juego::Juego() { reiniciar(); }  // Al construir, empezamos nueva partida

void Juego::reiniciar() {
    m_tablero.inicializar();        // Colocar 20 piezas por lado en casillas oscuras
    m_turno = Color::Blanco;        // Convención: blancas mueven primero
    m_sel.reset();                  // Nada seleccionado
}

bool Juego::seleccionar(const Pos& p, std::string& msg) {
    // Validación de límites
    if (!Tablero::dentro(p)) { msg = u8"Fuera del tablero."; return false; }

    auto& celda = m_tablero.get(p); // Acceso a la celda
    if (!celda) { msg = u8"No hay pieza en esa casilla."; return false; }
    if (celda->color != m_turno) { msg = u8"No es tu pieza."; return false; }

    // Guardamos selección
    m_sel = p;
    msg = u8"Pieza seleccionada.";
    return true;
}

ResultadoMovimiento Juego::moverA(const Pos& destino, std::string& msg) {
    if (!m_sel) { msg = u8"Primero selecciona una pieza."; return ResultadoMovimiento::Invalido; }

    Pos origen = *m_sel;
    if (!Tablero::dentro(destino)) {
        msg = u8"El destino está fuera del tablero.";
        return ResultadoMovimiento::FueraDeTablero;
    }

    // Calculamos opciones
    auto opciones = movimientosValidosDesde(origen);
    bool capturaGlobal = hayAlgunaCaptura(m_turno); // ¿Existe captura obligatoria?

    // ¿El destino está entre las opciones?
    auto it = std::find_if(opciones.begin(), opciones.end(),
        [&](const Movimiento& m) { return m.hasta.f == destino.f && m.hasta.c == destino.c; });

    if (it == opciones.end()) {
        msg = u8"Movimiento no válido.";
        return ResultadoMovimiento::Invalido;
    }

    if (capturaGlobal && !it->esCaptura) {
        msg = u8"Debes capturar (captura obligatoria).";
        return ResultadoMovimiento::CapturaObligatoriaNoRealizada;
    }

    // --- Ejecutar el movimiento ---
    pushSnapshot(); // Para permitir "Deshacer"

    auto pieza = m_tablero.get(origen);
    if (!pieza) {
        msg = u8"No hay pieza en la casilla de origen.";
        return ResultadoMovimiento::Invalido;
    }
    if (m_tablero.get(destino)) {
        msg = u8"La casilla de destino está ocupada.";
        return ResultadoMovimiento::CasillaOcupada;
    }

    // Si era captura, eliminamos la pieza rival
    bool fueCaptura = it->esCaptura;
    if (fueCaptura && it->capturada.has_value()) {
        m_tablero.quitar(*it->capturada);
    }

    m_tablero.quitar(origen);
    m_tablero.poner(destino, *pieza);

    // Coronación si aplica
    coronarSiAplica(destino);

    // Historial: 'Blancas: B6 × C7' (× si captura, → si movimiento simple)
    {
        wchar_t flecha = fueCaptura ? L'\u00D7' : L'\u2192'; // × / →
        std::wstring who = (m_turno == Color::Blanco) ? L"Blancas" : L"Negras";
        std::wstring linea = who + L": " + posTexto(origen) + L" " + std::wstring(1, flecha) + L" " + posTexto(destino);
        addHistorial(linea);
    }

    // --- Capturas encadenadas ---
    if (fueCaptura) {
        if (puedeSeguirCapturandoDesde(destino)) {
            m_sel = destino; // Mantiene la misma pieza seleccionada
            msg = u8"Captura realizada. Continúa capturando.";
            return ResultadoMovimiento::Ok; // No cambiamos de turno aún
        }
    }

    // Sin más capturas en cadena, pasa el turno
    m_sel.reset();
    pasarTurno();
    msg = fueCaptura ? u8"Captura realizada." : u8"Movimiento realizado.";
    return ResultadoMovimiento::Ok;
}

std::vector<Movimiento> Juego::movimientosValidosDesde(const Pos& p) const {
    std::vector<Movimiento> r;

    auto celda = m_tablero.get(p);
    if (!celda) return r; // Sin pieza, sin movimientos

    if (celda->tipo == TipoPieza::Peon) r = calcularMovimientosPeon(p);
    else                                r = calcularMovimientosDama(p);

    // Regla de captura obligatoria: si hay alguna captura, filtrar solo capturas
    bool hayCap = false;
    for (auto& m : r) if (m.esCaptura) { hayCap = true; break; }
    if (hayCap) {
        std::vector<Movimiento> solo;
        for (auto& m : r) if (m.esCaptura) solo.push_back(m);
        return solo;
    }
    return r;
}

bool Juego::hayAlgunaCaptura(Color c) const {
    // Recorremos todo el tablero buscando alguna pieza del color 'c' con capturas disponibles
    for (int f = 0; f < Tablero::N; ++f) for (int k = 0; k < Tablero::N; ++k) {
        Pos p{ f,k };
        auto celda = m_tablero.get(p);
        if (celda && celda->color == c) {
            auto movs = (celda->tipo == TipoPieza::Peon) ? calcularMovimientosPeon(p)
                : calcularMovimientosDama(p);
            for (auto& m : movs) if (m.esCaptura) return true; // Basta con una
        }
    }
    return false;
}

bool Juego::sinPiezas(Color c) const {
    // True si no queda ninguna pieza del color 'c'
    for (int f = 0; f < Tablero::N; ++f) for (int k = 0; k < Tablero::N; ++k) {
        auto celda = m_tablero.get(Pos{ f,k });
        if (celda && celda->color == c) return false;
    }
    return true;
}

bool Juego::sinMovimientos(Color c) const {
    // True si ninguna pieza del color 'c' tiene movimientos válidos
    for (int f = 0; f < Tablero::N; ++f) for (int k = 0; k < Tablero::N; ++k) {
        Pos p{ f,k };
        auto celda = m_tablero.get(p);
        if (!celda || celda->color != c) continue;
        auto movs = (celda->tipo == TipoPieza::Peon) ? calcularMovimientosPeon(p)
            : calcularMovimientosDama(p);
        if (!movs.empty()) return false;
    }
    return true;
}

std::vector<Movimiento> Juego::calcularMovimientosPeon(const Pos& p) const {
    std::vector<Movimiento> r;
    auto pieza = m_tablero.get(p); if (!pieza) return r;

    int dfAv = dirAvance(pieza->color); // Dirección de avance (Blanco -1, Negro +1)

    // --- Movimientos simples (un paso en diagonal hacia adelante) ---
    for (int dc : {-1, +1}) {
        Pos d{ p.f + dfAv, p.c + dc }; // Diagonal adyacente
        if (Tablero::dentro(d) && Tablero::casillaOscura(d) && !m_tablero.get(d)) {
            r.push_back({ p, d, false, std::nullopt });
        }
    }

    // --- Capturas (el peón puede capturar en las 4 diagonales en Internacional) ---
    for (int df : {-1, +1}) for (int dc : {-1, +1}) {
        Pos medio{ p.f + df, p.c + dc };     // Posición de la pieza rival
        Pos fin{ p.f + 2 * df, p.c + 2 * dc }; // Casilla después del salto
        if (!Tablero::dentro(fin) || !Tablero::casillaOscura(fin)) continue;
        if (m_tablero.get(fin)) continue;    // Debe estar libre
        auto mcelda = m_tablero.get(medio);
        if (mcelda && mcelda->color != pieza->color) {
            r.push_back({ p, fin, true, medio });
        }
    }
    return r;
}

std::vector<Movimiento> Juego::calcularMovimientosDama(const Pos& p) const {
    std::vector<Movimiento> r;
    auto pieza = m_tablero.get(p); if (!pieza) return r;

    const int dirs[4][2] = { {-1,-1}, {-1,+1}, {+1,-1}, {+1,+1} };
    for (auto& d : dirs) {
        int f = p.f + d[0], c = p.c + d[1];
        bool encontroEnemigo = false; Pos posEnem{};
        // Recorremos la diagonal hasta que algo bloquee
        while (Tablero::dentro(Pos{ f,c }) && Tablero::casillaOscura(Pos{ f,c })) {
            auto ocupado = m_tablero.get(Pos{ f,c });
            if (!ocupado) {
                if (!encontroEnemigo) {
                    r.push_back({ p, Pos{f,c}, false, std::nullopt });  // movimiento simple
                }
                else {
                    r.push_back({ p, Pos{f,c}, true, posEnem });        // caída de captura
                }
            }
            else {
                if (ocupado->color == pieza->color) break; // nuestra pieza bloquea
                if (encontroEnemigo) break;                // dos enemigos seguidos bloquean
                encontroEnemigo = true; posEnem = Pos{ f,c };
            }
            f += d[0]; c += d[1];
        }
    }
    return r;
}

void Juego::coronarSiAplica(const Pos& p) {
    auto& celda = m_tablero.get(p); if (!celda) return;
    if (celda->tipo == TipoPieza::Dama) return;

    // Si blanco llega a fila 0 o negro a fila N-1 → coronación
    if ((celda->color == Color::Blanco && p.f == 0) ||
        (celda->color == Color::Negro && p.f == Tablero::N - 1)) {
        celda->tipo = TipoPieza::Dama;
    }
}

bool Juego::puedeSeguirCapturandoDesde(const Pos& p) const {
    auto celda = m_tablero.get(p); if (!celda) return false;
    auto movs = (celda->tipo == TipoPieza::Peon) ? calcularMovimientosPeon(p)
        : calcularMovimientosDama(p);
    for (auto& m : movs) if (m.esCaptura) return true;
    return false;
}

void Juego::pasarTurno() {
    m_turno = (m_turno == Color::Blanco) ? Color::Negro : Color::Blanco;
}

// ============================================================
// Conteo y puntaje
// ============================================================
int Juego::contarPiezas(Color c) const {
    int n = 0;
    for (int f = 0; f < Tablero::N; ++f)
        for (int k = 0; k < Tablero::N; ++k) {
            auto cel = m_tablero.get(Pos{ f,k });
            if (cel && cel->color == c) ++n;
        }
    return n;
}

int Juego::contarDamas(Color c) const {
    int n = 0;
    for (int f = 0; f < Tablero::N; ++f)
        for (int k = 0; k < Tablero::N; ++k) {
            auto cel = m_tablero.get(Pos{ f,k });
            if (cel && cel->color == c && cel->tipo == TipoPieza::Dama) ++n;
        }
    return n;
}

int Juego::puntaje(Color c) const {
    int p = 0;
    for (int f = 0; f < Tablero::N; ++f)
        for (int k = 0; k < Tablero::N; ++k) {
            auto cel = m_tablero.get(Pos{ f,k });
            if (cel && cel->color == c) p += (cel->tipo == TipoPieza::Dama) ? 3 : 1;
        }
    return p;
}

// ============================================================
// Historial y deshacer
// ============================================================
void Juego::pushSnapshot() {
    m_stack.push_back(Snapshot{ m_tablero, m_turno });
    if (m_stack.size() > 200) m_stack.pop_front();
}

std::wstring Juego::posTexto(const Pos& p) {
    wchar_t col = L'A' + p.c; // A..J
    int row = p.f + 1;        // 1..10
    wchar_t buf[16]; swprintf(buf, 16, L"%c%d", col, row);
    return std::wstring(buf);
}

void Juego::addHistorial(const std::wstring& w) {
    m_historialTexto.push_back(w);
    if (m_historialTexto.size() > 60) m_historialTexto.pop_front();
}

bool Juego::deshacer() {
    if (m_stack.empty()) return false;
    Snapshot s = m_stack.back(); m_stack.pop_back();
    m_tablero = s.tablero;
    m_turno = s.turno;
    if (!m_historialTexto.empty()) m_historialTexto.pop_back();
    m_sel.reset();
    return true;
}
