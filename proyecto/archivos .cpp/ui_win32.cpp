#include "../ui/ui_win32.h"
#include <sstream>
#include <string>
#include <vector>
#include <ShlObj.h> 


static void DrawFancyHeader(HDC ctx, RECT rcClient, RECT rtBoard, const wchar_t* turno);
static bool SaveHBITMAPToBMP(HBITMAP hbm, const wchar_t* path);

static constexpr int PANEL_W = 280;

static std::wstring PosATexto(const Pos& p) {
    wchar_t col = L'A' + p.c;
    wchar_t buf[16];
    swprintf(buf, 16, L"%c%d", col, p.f + 1);
    return std::wstring(buf);
}

UIWin32::UIWin32() {}

void UIWin32::nuevoJuego() {
    m_juego.reiniciar();
    InvalidateRect(m_hWnd, nullptr, TRUE);
}

void UIWin32::deshacer() {
    if (m_juego.deshacer()) {
        InvalidateRect(m_hWnd, nullptr, TRUE);
    }
}

RECT UIWin32::rectTablero(RECT rc) const {
   
    const int TOP_OFFSET = 72; 
    RECT zona{ rc.left, rc.top + TOP_OFFSET, rc.right - PANEL_W, rc.bottom };

    int w = zona.right - zona.left;
    int h = zona.bottom - zona.top;
    int lado = ((w < h) ? w : h) - 40; 
    if (lado < 200) lado = (w < h ? w : h);
    int x = zona.left + (w - lado) / 2;
    int y = zona.top + (h - lado) / 2;
    RECT r{ x, y, x + lado, y + lado };
    return r;
}

Pos UIWin32::puntoACelda(int x, int y, RECT rt) const {
    int lado = rt.right - rt.left;
    int tam = lado / Tablero::N;
    int c = (x - rt.left) / tam;
    int f = (y - rt.top) / tam;
    return Pos{ f, c };
}


void UIWin32::pintar(HDC hdc, RECT rc) {
   
    HDC memDC = CreateCompatibleDC(hdc);
    HBITMAP memBmp = CreateCompatibleBitmap(hdc, rc.right - rc.left, rc.bottom - rc.top);
    HGDIOBJ oldBmp = SelectObject(memDC, memBmp);
    HDC ctx = memDC;

    HBRUSH bg = (HBRUSH)GetStockObject(LTGRAY_BRUSH);
    FillRect(ctx, &rc, bg);

    RECT rt = rectTablero(rc);

    const wchar_t* turnoTxt = (m_juego.turno() == Color::Blanco) ? L"Blancas" : L"Negras";
    DrawFancyHeader(ctx, rc, rt, turnoTxt);

    int lado = rt.right - rt.left;
    int tam = lado / Tablero::N;

    
    for (int f = 0; f < Tablero::N; ++f) {
        for (int c = 0; c < Tablero::N; ++c) {
            RECT cel{ rt.left + c * tam, rt.top + f * tam,
                      rt.left + (c + 1) * tam, rt.top + (f + 1) * tam };
            bool oscura = ((f + c) % 2) == 1;
            HBRUSH br = CreateSolidBrush(oscura ? RGB(90, 98, 112) : RGB(235, 238, 243));
            FillRect(ctx, &cel, br);
            DeleteObject(br);
            FrameRect(ctx, &cel, (HBRUSH)GetStockObject(BLACK_BRUSH));
        }
    }

    
    for (int f = 0; f < Tablero::N; ++f) {
        for (int c = 0; c < Tablero::N; ++c) {
            RECT cel{ rt.left + c * tam, rt.top + f * tam,
                      rt.left + (c + 1) * tam, rt.top + (f + 1) * tam };
            auto p = m_juego.tablero().get(Pos{ f,c });
            if (!p) continue;

            HBRUSH piezaBr = CreateSolidBrush((p->color == Color::Blanco) ? RGB(250, 250, 252) : RGB(45, 47, 52));
            HBRUSH old = (HBRUSH)SelectObject(ctx, piezaBr);
            Ellipse(ctx, cel.left + 6, cel.top + 6, cel.right - 6, cel.bottom - 6);

    
            HPEN penPiece = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));
            HPEN oldPenPiece = (HPEN)SelectObject(ctx, penPiece);
            HBRUSH oldBrushHollow = (HBRUSH)SelectObject(ctx, GetStockObject(HOLLOW_BRUSH));
            Ellipse(ctx, cel.left + 6, cel.top + 6, cel.right - 6, cel.bottom - 6);
            SelectObject(ctx, oldBrushHollow);
            SelectObject(ctx, oldPenPiece);
            DeleteObject(penPiece);

            HPEN penGloss = CreatePen(PS_SOLID, 1, RGB(255, 255, 255));
            HPEN oldPenGloss = (HPEN)SelectObject(ctx, penGloss);
            Arc(ctx, cel.left + 10, cel.top + 10, cel.right - 10, cel.bottom - 20, 0, 0, 0, 0);
            SelectObject(ctx, oldPenGloss);
            DeleteObject(penGloss);

            SelectObject(ctx, old);
            DeleteObject(piezaBr);

            if (p->tipo == TipoPieza::Dama) {
                HPEN pen = CreatePen(PS_SOLID, 3, RGB(200, 180, 0));
                HPEN oldp = (HPEN)SelectObject(ctx, pen);
                HBRUSH oldh = (HBRUSH)SelectObject(ctx, GetStockObject(HOLLOW_BRUSH));
                Ellipse(ctx, cel.left + 10, cel.top + 10, cel.right - 10, cel.bottom - 10);
                SelectObject(ctx, oldh);
                SelectObject(ctx, oldp);
                DeleteObject(pen);
            }
        }
    }

    
    if (m_mostrarCoords) {
        SetBkMode(ctx, TRANSPARENT);
        HFONT hFont = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
        HFONT oldF = (HFONT)SelectObject(ctx, hFont);
        for (int i = 0; i < Tablero::N; ++i) {
            wchar_t col = L'A' + i; wchar_t cbuf[4]{ col,0,0,0 };
            RECT rcCol{ rt.left + i * tam, rt.top - 20, rt.left + (i + 1) * tam, rt.top };
            DrawTextW(ctx, cbuf, -1, &rcCol, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            RECT rcCol2{ rt.left + i * tam, rt.bottom, rt.left + (i + 1) * tam, rt.bottom + 20 };
            DrawTextW(ctx, cbuf, -1, &rcCol2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);

            wchar_t nbuf[8]; wsprintfW(nbuf, L"%d", i + 1);
            RECT rcRow{ rt.left - 20, rt.top + i * tam, rt.left, rt.top + (i + 1) * tam };
            DrawTextW(ctx, nbuf, -1, &rcRow, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
            RECT rcRow2{ rt.right, rt.top + i * tam, rt.right + 20, rt.top + (i + 1) * tam };
            DrawTextW(ctx, nbuf, -1, &rcRow2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
        }
        SelectObject(ctx, oldF);
    }

        if (m_juego.seleccion()) {
        Pos s = *m_juego.seleccion();
        RECT cel{ rt.left + s.c * tam, rt.top + s.f * tam,
                  rt.left + (s.c + 1) * tam, rt.top + (s.f + 1) * tam };
        HPEN pen = CreatePen(PS_SOLID, 3, RGB(0, 120, 215));
        HPEN oldp = (HPEN)SelectObject(ctx, pen);
        HBRUSH oldb = (HBRUSH)SelectObject(ctx, GetStockObject(HOLLOW_BRUSH));
        Rectangle(ctx, cel.left + 2, cel.top + 2, cel.right - 2, cel.bottom - 2);
        SelectObject(ctx, oldp);
        SelectObject(ctx, oldb);
        DeleteObject(pen);

        auto movs = m_juego.movimientosValidosDesde(s);
        for (auto& m : movs) {
            RECT c2{ rt.left + m.hasta.c * tam, rt.top + m.hasta.f * tam,
                     rt.left + (m.hasta.c + 1) * tam, rt.top + (m.hasta.f + 1) * tam };
            int cx = (c2.left + c2.right) / 2;
            int cy = (c2.top + c2.bottom) / 2;
            int r = tam / 6;
            HBRUSH br = CreateSolidBrush(m.esCaptura ? RGB(220, 60, 50) : RGB(40, 160, 90));
            HBRUSH oldh = (HBRUSH)SelectObject(ctx, br);
            Ellipse(ctx, cx - r, cy - r, cx + r, cy + r);
            SelectObject(ctx, oldh);
            DeleteObject(br);
        }
    }

    
    if (m_tieneUltimo) {
        RECT c1{ rt.left + m_lastDesde.c * tam, rt.top + m_lastDesde.f * tam, rt.left + (m_lastDesde.c + 1) * tam, rt.top + (m_lastDesde.f + 1) * tam };
        RECT c2{ rt.left + m_lastHasta.c * tam, rt.top + m_lastHasta.f * tam, rt.left + (m_lastHasta.c + 1) * tam, rt.top + (m_lastHasta.f + 1) * tam };
        HPEN pen = CreatePen(PS_SOLID, 3, RGB(255, 170, 0));
        HPEN oldp = (HPEN)SelectObject(ctx, pen);
        HBRUSH oldb = (HBRUSH)SelectObject(ctx, GetStockObject(HOLLOW_BRUSH));
        Rectangle(ctx, c1.left + 1, c1.top + 1, c1.right - 1, c1.bottom - 1);
        Rectangle(ctx, c2.left + 1, c2.top + 1, c2.right - 1, c2.bottom - 1);
        SelectObject(ctx, oldp); SelectObject(ctx, oldb); DeleteObject(pen);
    }

    
    dibujarPanelInfo(ctx, rc, rt);

    BitBlt(hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top, ctx, 0, 0, SRCCOPY);
    SelectObject(memDC, oldBmp);
    DeleteObject(memBmp);
    DeleteDC(memDC);
}


void UIWin32::click(int x, int y) {
    auto guardFinTiempo = [&]()->bool {
        if (m_gameOver) { MessageBoxW(m_hWnd, L"La partida ha finalizado.", L"Aviso", MB_OK | MB_ICONINFORMATION); return true; }
        if ((m_juego.turno() == Color::Blanco && m_timeLeftBlancas == 0) ||
            (m_juego.turno() == Color::Negro && m_timeLeftNegras == 0)) {
            MessageBoxW(m_hWnd, L"Tiempo agotado para el jugador en turno.", L"Aviso", MB_OK | MB_ICONWARNING);
            return true;
        }
        return false;
        };

    POINT p{ x, y };
    if (PtInRect(&m_btnNew, p)) { reiniciar(); return; }
    if (PtInRect(&m_btnUndo, p)) { deshacer();  return; }
    if (PtInRect(&m_chkCoords, p)) { m_mostrarCoords = !m_mostrarCoords; InvalidateRect(m_hWnd, nullptr, TRUE); return; }
    if (guardFinTiempo()) return;

    RECT rc; GetClientRect(m_hWnd, &rc);
    RECT rt = rectTablero(rc);

    if (x < rt.left || x >= rt.right || y < rt.top || y >= rt.bottom) {
        cancelarSeleccion();
        return;
    }

    Pos cel = puntoACelda(x, y, rt);
    std::string msg;

    
    if (m_juego.seleccion() &&
        m_juego.seleccion()->f == cel.f &&
        m_juego.seleccion()->c == cel.c) {
        cancelarSeleccion();
        return;
    }

    if (!m_juego.haySeleccion()) {
        if (!m_juego.seleccionar(cel, msg)) {
            MessageBoxW(m_hWnd, L"No se puede seleccionar esa casilla.",
                L"Aviso", MB_OK | MB_ICONINFORMATION);
        }
    }
    else {
    
        Pos desde = *m_juego.seleccion();
        auto res = m_juego.moverA(cel, msg);
        if (res == ResultadoMovimiento::Ok) {
            m_lastDesde = desde;
            m_lastHasta = cel;
            m_tieneUltimo = true;

            Color oponente = (m_juego.turno() == Color::Blanco) ? Color::Negro : Color::Blanco;
            if (m_juego.sinPiezas(oponente) || m_juego.sinMovimientos(oponente)) {
                m_gameOver = true;
                m_ganador = (m_juego.turno() == Color::Blanco) ? L"Blancas" : L"Negras";
                MessageBoxW(m_hWnd, (std::wstring(L"Juego terminado. Ganador: ") + m_ganador).c_str(),
                    L"Partida finalizada", MB_OK | MB_ICONINFORMATION);
            }
        }
        else {
            int wlen = MultiByteToWideChar(CP_UTF8, 0, msg.c_str(), -1, nullptr, 0);
            std::wstring wmsg(wlen, L'\0');
            MultiByteToWideChar(CP_UTF8, 0, msg.c_str(), -1, &wmsg[0], wlen);
            MessageBoxW(m_hWnd, wmsg.c_str(), L"Movimiento inválido", MB_OK | MB_ICONWARNING);
        }

        if (m_juego.sinPiezas(Color::Blanco) || m_juego.sinPiezas(Color::Negro)) {
            const wchar_t* g = m_juego.sinPiezas(Color::Blanco) ? L"Negras" : L"Blancas";
            std::wstring fin = std::wstring(L"Ganador: ") + g;
            MessageBoxW(m_hWnd, fin.c_str(), L"Fin del juego",
                MB_OK | MB_ICONINFORMATION);
        }

        if (m_juego.sinMovimientos(m_juego.turno())) {
            const wchar_t* g = (m_juego.turno() == Color::Blanco) ? L"Negras" : L"Blancas";
            std::wstring fin = std::wstring(L"Ganador por bloqueo: ") + g;
            MessageBoxW(m_hWnd, fin.c_str(), L"Fin del juego",
                MB_OK | MB_ICONINFORMATION);
        }
    }

    InvalidateRect(m_hWnd, nullptr, FALSE);
}


void UIWin32::dibujarPanelInfo(HDC hdc, RECT rc, RECT rt) const {
    RECT pan{ rc.right - PANEL_W, rc.top, rc.right, rc.bottom };

    HBRUSH bg = CreateSolidBrush(RGB(245, 245, 247));
    FillRect(hdc, &pan, bg); DeleteObject(bg);
    FrameRect(hdc, &pan, (HBRUSH)GetStockObject(GRAY_BRUSH));

    
    HFONT hBody = CreateFontW(14, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe UI");
    HFONT oldBody = (HFONT)SelectObject(hdc, hBody);

    HFONT hHeader = CreateFontW(14, 0, 0, 0, FW_SEMIBOLD, FALSE, FALSE, FALSE, DEFAULT_CHARSET, 0, 0, 0, 0, L"Segoe UI");
    auto header = [&](const wchar_t* txt, int& y) {
        RECT r{ pan.left + 12, y, pan.right - 12, y + 22 };
        HFONT oldH = (HFONT)SelectObject(hdc, hHeader);
        DrawTextW(hdc, txt, -1, &r, DT_LEFT | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
        SelectObject(hdc, oldH);
        y += 22;
        HPEN pen = CreatePen(PS_SOLID, 1, RGB(210, 210, 210));
        HPEN old = (HPEN)SelectObject(hdc, pen);
        MoveToEx(hdc, pan.left + 12, y, nullptr); LineTo(hdc, pan.right - 12, y);
        SelectObject(hdc, old); DeleteObject(pen);
        y += 6;
        };
    auto put = [&](const std::wstring& label, const std::wstring& value, int& y) {
        RECT r{ pan.left + 12, y, pan.right - 12, y + 20 };
        std::wstring line = label + L": " + value;
        DrawTextW(hdc, line.c_str(), -1, &r, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);
        y += 20;
        };

    int y = pan.top + 10;

    
    header(L"Estado", y);
    
    m_chkCoords = RECT{ pan.left + 12, y, pan.left + 180, y + 20 };
    Rectangle(hdc, m_chkCoords.left, m_chkCoords.top, m_chkCoords.right, m_chkCoords.bottom);
    RECT tchk{ m_chkCoords.left + 24, m_chkCoords.top, m_chkCoords.right, m_chkCoords.bottom };
    DrawTextW(hdc, L"Mostrar coordenadas", -1, &tchk, DT_LEFT | DT_VCENTER | DT_SINGLELINE);
    if (m_mostrarCoords) {
        MoveToEx(hdc, m_chkCoords.left + 6, m_chkCoords.top + 10, nullptr);
        LineTo(hdc, m_chkCoords.left + 10, m_chkCoords.top + 14);
        LineTo(hdc, m_chkCoords.left + 18, m_chkCoords.top + 6);
    }
    y += 26;
    if (m_gameOver) {
        put(L"\u26A0 Estado", L"partida finalizada", y);
        put(L"Ganador", m_ganador, y);
    }

    const std::wstring nombreBlancas = L"Blancas";
    const std::wstring nombreNegras = L"Negras";
    std::wstring turnoTxt = (m_juego.turno() == Color::Blanco) ? nombreBlancas : nombreNegras;
    put(L"Turno", turnoTxt, y);
    bool capObl = m_juego.hayAlgunaCaptura(m_juego.turno());
    put(L"Captura obligatoria", capObl ? L"S\u00ED" : L"No", y);

    
    header(L"Conteo y puntaje", y);
    int pezB = m_juego.contarPiezas(Color::Blanco);
    int pezN = m_juego.contarPiezas(Color::Negro);
    int damB = m_juego.contarDamas(Color::Blanco);
    int damN = m_juego.contarDamas(Color::Negro);
    int ptsB = m_juego.puntaje(Color::Blanco);
    int ptsN = m_juego.puntaje(Color::Negro);

    put(L"Blancas", std::to_wstring(pezB) + L" (Damas: " + std::to_wstring(damB) + L")  Pts: " + std::to_wstring(ptsB), y);
    put(L"Negras", std::to_wstring(pezN) + L" (Damas: " + std::to_wstring(damN) + L")  Pts: " + std::to_wstring(ptsN), y);

    std::wstring ventaja;
    if (ptsB > ptsN) ventaja = L"Blancas (+" + std::to_wstring(ptsB - ptsN) + L")";
    else if (ptsN > ptsB) ventaja = L"Negras (+" + std::to_wstring(ptsN - ptsB) + L")";
    else ventaja = L"Empate";
    put(L"Ventaja", ventaja, y);

    auto fmt = [](int s) { wchar_t b[32]; if (s < 0) s = 0; wsprintfW(b, L"%02d:%02d:%02d", s / 3600, (s / 60) % 60, s % 60); return std::wstring(b); };
    put(L"Tiempo Blancas (restante)", fmt(m_timeLeftBlancas), y);
    put(L"Tiempo Negras (restante)", fmt(m_timeLeftNegras), y);

    
    header(L"Selecci\u00F3n y jugadas", y);
    if (m_juego.seleccion()) {
        Pos s = *m_juego.seleccion();
        put(L"Seleccionada", PosATexto(s), y);
        auto movs = m_juego.movimientosValidosDesde(s);
        RECT rlist{ pan.left + 12, y, pan.right - 12, pan.bottom - 80 };
        DrawTextW(hdc, L"Movimientos posibles:", -1, &rlist, DT_LEFT | DT_SINGLELINE);
        y += 18;
        int mostrados = 0;
        for (auto& m : movs) {
            std::wstring linea = L" - " + PosATexto(m.desde) + L" \u2192 " + PosATexto(m.hasta);
            if (m.esCaptura) linea += L"  (\u00D7)";
            RECT li{ pan.left + 20, y, pan.right - 12, y + 18 };
            DrawTextW(hdc, linea.c_str(), -1, &li, DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);
            y += 18; ++mostrados;
            if (y > pan.bottom - 160) break;
        }
        if (mostrados == 0) {
            RECT li{ pan.left + 20, y, pan.right - 12, y + 18 };
            DrawTextW(hdc, L"(sin movimientos)", -1, &li, DT_LEFT | DT_SINGLELINE);
            y += 18;
        }
    }
    else {
        put(L"Seleccionada", L"(ninguna)", y);
    }

    
    int btnH = 26;
    header(L"Historial", y);

    const auto& hist = m_juego.historialJugadas();
    put(L"Jugadas totales", std::to_wstring(hist.size()), y);
    std::wstring ult = hist.empty() ? L"\u2014" : hist.back();
    put(L"\u00DAltima jugada", ult, y);

    int maxLines = 12;
    int start = (int)hist.size() - maxLines; if (start < 0) start = 0;
    for (int i = start; i < (int)hist.size(); ++i) {
        RECT r{ pan.left + 20, y, pan.right - 12, y + 18 };
        DrawTextW(hdc, hist[i].c_str(), -1, &r, DT_LEFT | DT_SINGLELINE | DT_END_ELLIPSIS);
        y += 18;
        if (y > pan.bottom - 70) break;
    }

    
    y = pan.bottom - (btnH + 14);
    m_btnUndo = RECT{ pan.left + 12, y, pan.right - 12, y + btnH };
    {
        HBRUSH btn = CreateSolidBrush(RGB(230, 230, 230));
        FillRect(hdc, &m_btnUndo, btn); DeleteObject(btn);
        FrameRect(hdc, &m_btnUndo, (HBRUSH)GetStockObject(BLACK_BRUSH));
        RECT lb{ m_btnUndo.left, m_btnUndo.top, m_btnUndo.right, m_btnUndo.bottom };
        DrawTextW(hdc, L"Deshacer (Ctrl+Z)", -1, &lb, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }
    int y2 = m_btnUndo.top - 10 - btnH;
    if (y2 < pan.top + 10) y2 = pan.top + 10;
    m_btnNew = RECT{ pan.left + 12, y2, pan.right - 12, y2 + btnH };
    {
        HBRUSH b2 = CreateSolidBrush(RGB(230, 230, 230));
        FillRect(hdc, &m_btnNew, b2); DeleteObject(b2);
        FrameRect(hdc, &m_btnNew, (HBRUSH)GetStockObject(BLACK_BRUSH));
        RECT lb2{ m_btnNew.left, m_btnNew.top, m_btnNew.right, m_btnNew.bottom };
        DrawTextW(hdc, L"Nueva partida", -1, &lb2, DT_CENTER | DT_VCENTER | DT_SINGLELINE);
    }

    
    SelectObject(hdc, oldBody);
    DeleteObject(hBody);
    DeleteObject(hHeader);
}


void UIWin32::tick() {

    DWORD now = GetTickCount();
    if (m_lastTick == 0) { m_lastTick = now; m_lastTurn = m_juego.turno(); return; }
    DWORD delta = now - m_lastTick;
    if (delta >= 1000) {
        int secs = int(delta / 1000);

        if (m_lastTurn == Color::Blanco) {
            m_timeLeftBlancas = (m_timeLeftBlancas > secs) ? (m_timeLeftBlancas - secs) : 0;
        }
        else {
            m_timeLeftNegras = (m_timeLeftNegras > secs) ? (m_timeLeftNegras - secs) : 0;
        }

        m_lastTick += secs * 1000;
    }

    if (m_lastTurn != m_juego.turno()) {
        m_lastTurn = m_juego.turno();
        m_lastTick = now;
    }


    if (!m_gameOver && (m_timeLeftBlancas == 0 || m_timeLeftNegras == 0)) {
        m_gameOver = true;
        m_ganador = (m_timeLeftBlancas == 0) ? L"Negras" : L"Blancas";
        MessageBoxW(m_hWnd, (std::wstring(L"Tiempo agotado. Ganador: ") + m_ganador).c_str(),
            L"Partida finalizada", MB_OK | MB_ICONINFORMATION);
        return;
    }
}

void UIWin32::reiniciar() {
    m_juego.reiniciar();
    m_timeLeftBlancas = 200; 
    m_timeLeftNegras = 200; 
    m_lastTick = 0;
    m_lastTurn = Color::Blanco;
    m_gameOver = false;
    m_ganador.clear();
    m_tieneUltimo = false;
    InvalidateRect(m_hWnd, nullptr, FALSE);
}


void UIWin32::exportarHistorial() const {
    
    auto hist = m_juego.historialJugadas();
    wchar_t path[MAX_PATH];
    if (SUCCEEDED(SHGetFolderPathW(nullptr, CSIDL_DESKTOP, nullptr, 0, path))) {
        std::wstring file = std::wstring(path) + L"\\Historial_Damas.txt";
        HANDLE h = CreateFileW(file.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (h != INVALID_HANDLE_VALUE) {
            
            std::wstring header = L"Historial de jugadas - Damas Internacionales\r\n\r\n";
            DWORD wr = 0;
            WriteFile(h, header.c_str(), (DWORD)(header.size() * sizeof(wchar_t)), &wr, nullptr);
            
            for (size_t i = 0; i < hist.size(); ++i) {
                std::wstring linea = std::to_wstring(i + 1) + L". " + hist[i] + L"\r\n";
                WriteFile(h, linea.c_str(), (DWORD)(linea.size() * sizeof(wchar_t)), &wr, nullptr);
            }
            CloseHandle(h);
            MessageBoxW(m_hWnd, L"Historial exportado en el Escritorio (Historial_Damas.txt).", L"Exportar historial", MB_OK | MB_ICONINFORMATION);
        }
        else {
            MessageBoxW(m_hWnd, L"No se pudo crear el archivo de salida.", L"Exportar historial", MB_OK | MB_ICONERROR);
        }
    }
}



static void DrawFancyHeader(HDC ctx, RECT rcClient, RECT rtBoard, const wchar_t* turno) {

    int badgeW = (rtBoard.right - rtBoard.left) - 80;
    if (badgeW < 360) badgeW = (rtBoard.right - rtBoard.left) - 40;
    int badgeH = 44;

    int cx = rtBoard.left + (rtBoard.right - rtBoard.left) / 2;
    RECT badge{ cx - badgeW / 2, rcClient.top + 8, cx + badgeW / 2, rcClient.top + 8 + badgeH };


    RECT shadow = badge; OffsetRect(&shadow, 2, 2);
    HBRUSH brShadow = CreateSolidBrush(RGB(180, 180, 180));
    HBRUSH oldBr = (HBRUSH)SelectObject(ctx, brShadow);
    HPEN oldPen = (HPEN)SelectObject(ctx, GetStockObject(NULL_PEN));
    RoundRect(ctx, shadow.left, shadow.top, shadow.right, shadow.bottom, 16, 16);
    SelectObject(ctx, oldPen);
    SelectObject(ctx, oldBr);
    DeleteObject(brShadow);


    HBRUSH brBg = CreateSolidBrush(RGB(245, 246, 248));
    oldBr = (HBRUSH)SelectObject(ctx, brBg);
    HPEN pen = CreatePen(PS_SOLID, 1, RGB(205, 205, 205));
    oldPen = (HPEN)SelectObject(ctx, pen);
    RoundRect(ctx, badge.left, badge.top, badge.right, badge.bottom, 16, 16);
    SelectObject(ctx, oldPen); DeleteObject(pen);
    SelectObject(ctx, oldBr);  DeleteObject(brBg);

    HFONT base = (HFONT)GetStockObject(DEFAULT_GUI_FONT);
    LOGFONTW lf{}; GetObject(base, sizeof(LOGFONTW), &lf);
    lf.lfHeight = -14; lf.lfWeight = FW_BOLD;
    HFONT hBold = CreateFontIndirectW(&lf);
    lf.lfHeight = -12; lf.lfWeight = FW_NORMAL;
    HFONT hReg = CreateFontIndirectW(&lf);

    RECT r1{ badge.left + 14, badge.top + 4, badge.right - 14, badge.top + 22 };
    HFONT oldF = (HFONT)SelectObject(ctx, hBold);
    SetBkMode(ctx, TRANSPARENT);
    DrawTextW(ctx, L"Profesor: Andr\u00E9s Ducuara   \u2013   Materia: Pensamiento Algor\u00EDtmico", -1, &r1,
        DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);

    RECT r2{ badge.left + 14, badge.top + 22, badge.right - 14, badge.bottom - 4 };
    SelectObject(ctx, hReg);
    std::wstring l2 = std::wstring(L"Damas Internacionales \u2013 Turno: ") + turno;
    DrawTextW(ctx, l2.c_str(), -1, &r2, DT_CENTER | DT_SINGLELINE | DT_VCENTER | DT_END_ELLIPSIS);

    SelectObject(ctx, oldF);
    DeleteObject(hBold); DeleteObject(hReg);
}

static bool SaveHBITMAPToBMP(HBITMAP hbm, const wchar_t* path) {
    BITMAP bm; GetObject(hbm, sizeof(bm), &bm);
    BITMAPINFOHEADER bi = { 0 };
    bi.biSize = sizeof(BITMAPINFOHEADER);
    bi.biWidth = bm.bmWidth;
    bi.biHeight = bm.bmHeight;
    bi.biPlanes = 1;
    bi.biBitCount = 24;
    bi.biCompression = BI_RGB;
    DWORD dwBmpSize = ((bm.bmWidth * bi.biBitCount + 31) / 32) * 4 * bm.bmHeight;

    HANDLE hDIB = GlobalAlloc(GHND, sizeof(BITMAPINFOHEADER) + dwBmpSize);
    if (!hDIB) return false;
    BITMAPINFO* biAll = (BITMAPINFO*)GlobalLock(hDIB);
    memcpy(&biAll->bmiHeader, &bi, sizeof(BITMAPINFOHEADER));

    HDC hdc = GetDC(nullptr);
    GetDIBits(hdc, hbm, 0, (UINT)bm.bmHeight, (LPVOID)((BYTE*)biAll + sizeof(BITMAPINFOHEADER)), biAll, DIB_RGB_COLORS);
    ReleaseDC(nullptr, hdc);

    HANDLE hFile = CreateFileW(path, GENERIC_WRITE, 0, nullptr, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (hFile == INVALID_HANDLE_VALUE) { GlobalUnlock(hDIB); GlobalFree(hDIB); return false; }

    BITMAPFILEHEADER bfh = { 0 };
    bfh.bfType = 0x4D42; 
    bfh.bfOffBits = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER);
    bfh.bfSize = bfh.bfOffBits + dwBmpSize;

    DWORD written = 0;
    WriteFile(hFile, &bfh, sizeof(bfh), &written, nullptr);
    WriteFile(hFile, &biAll->bmiHeader, sizeof(BITMAPINFOHEADER), &written, nullptr);
    WriteFile(hFile, (BYTE*)biAll + sizeof(BITMAPINFOHEADER), dwBmpSize, &written, nullptr);

    CloseHandle(hFile);
    GlobalUnlock(hDIB);
    GlobalFree(hDIB);
    return true;
}

void UIWin32::exportarTableroBMP() {
    RECT rc; GetClientRect(m_hWnd, &rc);
    RECT rt = rectTablero(rc);
    int w = rt.right - rt.left;
    int h = rt.bottom - rt.top;

    HDC hdc = GetDC(m_hWnd);
    HDC mem = CreateCompatibleDC(hdc);
    HBITMAP bmp = CreateCompatibleBitmap(hdc, w, h);
    HGDIOBJ old = SelectObject(mem, bmp);
    BitBlt(mem, 0, 0, w, h, hdc, rt.left, rt.top, SRCCOPY);
    SelectObject(mem, old);
    ReleaseDC(m_hWnd, hdc);

    wchar_t path[MAX_PATH];
    SYSTEMTIME st; GetLocalTime(&st);
    swprintf(path, MAX_PATH, L"Tablero_%04d%02d%02d_%02d%02d%02d.bmp", st.wYear, st.wMonth, st.wDay, st.wHour, st.wMinute, st.wSecond);
    if (SaveHBITMAPToBMP(bmp, path)) {
        MessageBoxW(m_hWnd, (std::wstring(L"Guardado: ") + path).c_str(), L"Exportar tablero", MB_OK | MB_ICONINFORMATION);
    }
    else {
        MessageBoxW(m_hWnd, L"No se pudo guardar la imagen.", L"Exportar tablero", MB_OK | MB_ICONERROR);
    }
    DeleteObject(bmp);
}
