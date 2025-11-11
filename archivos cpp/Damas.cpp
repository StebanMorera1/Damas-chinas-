#include <windows.h>            // Cabecera principal de Win32
#include <windowsx.h>           // Macros GET_X/Y_LPARAM (si se usan en WndProc)
#include "ui/ui_win32.h"        // Interfaz de dibujo y control de clicks
#define IDM_NUEVA_PARTIDA 1001
#define IDM_EXPORTAR_HIST 1002
#define IDM_EXPORTAR_BMP 1003
#define IDM_AYUDA_ACERCA 2001

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);  // Firma del procedimiento de ventana
UIWin32 g_ui;                                          // Instancia global de la UI

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow) {
    const wchar_t* cls = L"DamasWin32Class";           // Nombre de la clase de ventana

    // --- Registrar clase de ventana ---
    WNDCLASSW wc{};                                   // Estructura de clase
    wc.lpszClassName = cls;                           // Nombre
    wc.hInstance     = hInstance;                     // Instancia del programa
    wc.lpfnWndProc   = WndProc;                       // Procedimiento de ventana
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);   // Cursor por defecto
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);      // Brocha de fondo
    RegisterClassW(&wc);                              // Registrar la clase

    // --- Crear ventana principal ---
    HWND hWnd = CreateWindowW(
        cls,                                          // Clase
        L"Damas Internacionales (Parcial \u2013 Corte 3) - Estudiante: Steban Felipe Morera Busaquillo", // Título de la ventana
        WS_OVERLAPPEDWINDOW,                          // Estilo de ventana
        CW_USEDEFAULT, 0, 900, 700,                   // Posición y tamaño inicial
        nullptr, nullptr, hInstance, nullptr          // Padre, menú, instancia, datos
    );

    g_ui.asociar(hWnd);                               // Asociar la ventana a la UI

    ShowWindow(hWnd, nCmdShow);

    // Timer de 1s para reloj
    SetTimer(hWnd, 1, 1000, nullptr);                       // Mostrar
    UpdateWindow(hWnd);                               // Forzar repintado inicial

    // --- Bucle de mensajes ---
    MSG msg;                                          // Estructura de mensaje
    while (GetMessage(&msg, nullptr, 0, 0)) {         // Extraer mensajes
        TranslateMessage(&msg);                       // Traducir teclas
        DispatchMessage(&msg);                        // Enviar a WndProc
    }
    return (int)msg.wParam;                           // Código de salida
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {                                    // Seleccionamos por tipo de mensaje
    case WM_CREATE: {                                 // Al crear la ventana
        // Crear menú "Juego" con opciones básicas
        HMENU hMenu  = CreateMenu();                  // Menú raíz
        HMENU hJuego = CreatePopupMenu();             // Submenú
        AppendMenuW(hJuego, MF_STRING, IDM_NUEVA_PARTIDA, L"Nueva partida\tCtrl+N");
    AppendMenuW(hJuego, MF_STRING, IDM_EXPORTAR_HIST, L"Exportar historial...");
    AppendMenuW(hJuego, MF_STRING, IDM_EXPORTAR_BMP, L"Exportar Tablero (BMP)...");
    AppendMenuW(hJuego, MF_STRING, 2, L"Reglas");      // ID=2
    AppendMenuW(hJuego, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hJuego, MF_STRING, 3, L"Salir");       // ID=3
        AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hJuego, L"Juego"); // Agregar submenú
        HMENU hAyuda = CreateMenu();
        AppendMenuW(hAyuda, MF_STRING, IDM_AYUDA_ACERCA, L"Acerca de...");
        AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hAyuda, L"Ayuda");
        SetMenu(hWnd, hMenu);                         // Asociar a la ventana
        break; }
    case WM_COMMAND: {
        if (LOWORD(wParam)==IDM_EXPORTAR_HIST) { g_ui.exportarHistorial(); return 0; }
        if (LOWORD(wParam)==IDM_EXPORTAR_BMP) { g_ui.exportarTableroBMP(); return 0; }
        if (LOWORD(wParam)==IDM_AYUDA_ACERCA) { MessageBoxW(hWnd,
            L"Proyecto: Damas Internacionales\nProfesor: Andres Ducuara\nMateria: Pensamiento Algoritmico",
            L"Acerca de", MB_OK | MB_ICONINFORMATION); return 0; }
        if (LOWORD(wParam)==IDM_NUEVA_PARTIDA) { g_ui.reiniciar(); return 0; }                                // Al seleccionar una opción del menú
        switch (LOWORD(wParam)) {                     // Mirar ID de comando
        case 1: g_ui.nuevoJuego(); break;             // Nuevo juego
        case 2: MessageBoxW(hWnd,
            L"Reglas b\u00E1sicas:\n"
            L"- Tablero 10x10 en casillas oscuras.\n"
            L"- Pe\u00F3n se mueve adelante 1 diagonal; captura en 4 diagonales.\n"
            L"- Dama se desliza por diagonales y captura a distancia.\n"
            L"- Captura obligatoria si existe.\n"
            L"- Coronaci\u00F3n al llegar a la fila opuesta.",
            L"Reglas", MB_OK | MB_ICONINFORMATION); break;
        case 3: PostQuitMessage(0); break;            // Salir
        }
        break; }
    case WM_RBUTTONDOWN: { g_ui.cancelarSeleccion(); break; }
    case WM_KEYDOWN: {
        if (wParam==VK_ESCAPE) { g_ui.cancelarSeleccion(); }
        else if ((GetKeyState(VK_CONTROL)&0x8000) && wParam=='Z') {
            g_ui.deshacer();
        }
        break; }
    case WM_LBUTTONDOWN: {                            // Click izquierdo del mouse
        POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        if (PtInRect(&g_ui.botonUndoRect(), pt)) {
            g_ui.deshacer();
            break;
        }
        int x = GET_X_LPARAM(lParam);                 // Extraer X del lParam
        int y = GET_Y_LPARAM(lParam);                 // Extraer Y del lParam
        g_ui.click(x,y);                              // Delegar a la UI
        break; }
    case WM_TIMER: { g_ui.tick(); InvalidateRect(hWnd, nullptr, FALSE); break; }
    case WM_ERASEBKGND: return 1;
    case WM_PAINT: {                                  // Cuando hay que repintar
        PAINTSTRUCT ps; HDC hdc = BeginPaint(hWnd, &ps); // Iniciar pintura
        RECT rc; GetClientRect(hWnd, &rc);            // Rectángulo del cliente
        g_ui.pintar(hdc, rc);                         // Dibuja todo
        EndPaint(hWnd, &ps);                          // Finaliza pintura
        break; }
    case WM_DESTROY: PostQuitMessage(0); break;       // Cerrar aplicación
    default: return DefWindowProc(hWnd, msg, wParam, lParam); // Por defecto
    }
    return 0;                                         // Fin de procesamiento
}
