#include <windows.h>           
#include <windowsx.h>          
#include "ui/ui_win32.h"       
#define IDM_NUEVA_PARTIDA 1001
#define IDM_EXPORTAR_HIST 1002
#define IDM_EXPORTAR_BMP 1003
#define IDM_AYUDA_ACERCA 2001

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);  
UIWin32 g_ui;                                          

int APIENTRY wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR, int nCmdShow) {
    const wchar_t* cls = L"DamasWin32Class";           

    WNDCLASSW wc{};                                   
    wc.lpszClassName = cls;                           
    wc.hInstance     = hInstance;                     
    wc.lpfnWndProc   = WndProc;                       
    wc.hCursor       = LoadCursor(NULL, IDC_ARROW);   
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);     
    RegisterClassW(&wc);                              

    HWND hWnd = CreateWindowW(
        cls,                                          
        L"Damas Internacionales (Parcial \u2013 Corte 3) - Estudiante: Steban Felipe Morera Busaquillo", 
        WS_OVERLAPPEDWINDOW,                          
        CW_USEDEFAULT, 0, 900, 700,                   
        nullptr, nullptr, hInstance, nullptr          
    );

    g_ui.asociar(hWnd);                               

    ShowWindow(hWnd, nCmdShow);

    SetTimer(hWnd, 1, 1000, nullptr);                       
    UpdateWindow(hWnd);                               

    MSG msg;                                         
    while (GetMessage(&msg, nullptr, 0, 0)) {        
        TranslateMessage(&msg);                       
        DispatchMessage(&msg);                        
    }
    return (int)msg.wParam;                           
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {                                    
    case WM_CREATE: {                                 

        HMENU hMenu  = CreateMenu();                  
        HMENU hJuego = CreatePopupMenu();            
        AppendMenuW(hJuego, MF_STRING, IDM_NUEVA_PARTIDA, L"Nueva partida\tCtrl+N");
    AppendMenuW(hJuego, MF_STRING, IDM_EXPORTAR_HIST, L"Exportar historial...");
    AppendMenuW(hJuego, MF_STRING, IDM_EXPORTAR_BMP, L"Exportar Tablero (BMP)...");
    AppendMenuW(hJuego, MF_STRING, 2, L"Reglas");      
    AppendMenuW(hJuego, MF_SEPARATOR, 0, NULL);
    AppendMenuW(hJuego, MF_STRING, 3, L"Salir");      
        AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hJuego, L"Juego"); 
        HMENU hAyuda = CreateMenu();
        AppendMenuW(hAyuda, MF_STRING, IDM_AYUDA_ACERCA, L"Acerca de...");
        AppendMenuW(hMenu, MF_POPUP, (UINT_PTR)hAyuda, L"Ayuda");
        SetMenu(hWnd, hMenu);                       
        break; }
    case WM_COMMAND: {
        if (LOWORD(wParam)==IDM_EXPORTAR_HIST) { g_ui.exportarHistorial(); return 0; }
        if (LOWORD(wParam)==IDM_EXPORTAR_BMP) { g_ui.exportarTableroBMP(); return 0; }
        if (LOWORD(wParam)==IDM_AYUDA_ACERCA) { MessageBoxW(hWnd,
            L"Proyecto: Damas Internacionales\nProfesor: Andres Ducuara\nMateria: Pensamiento Algoritmico",
            L"Acerca de", MB_OK | MB_ICONINFORMATION); return 0; }
        if (LOWORD(wParam)==IDM_NUEVA_PARTIDA) { g_ui.reiniciar(); return 0; }                              
        switch (LOWORD(wParam)) {                     
        case 1: g_ui.nuevoJuego(); break;             
        case 2: MessageBoxW(hWnd,
            L"Reglas b\u00E1sicas:\n"
            L"- Tablero 10x10 en casillas oscuras.\n"
            L"- Pe\u00F3n se mueve adelante 1 diagonal; captura en 4 diagonales.\n"
            L"- Dama se desliza por diagonales y captura a distancia.\n"
            L"- Captura obligatoria si existe.\n"
            L"- Coronaci\u00F3n al llegar a la fila opuesta.",
            L"Reglas", MB_OK | MB_ICONINFORMATION); break;
        case 3: PostQuitMessage(0); break;            
        }
        break; }
    case WM_RBUTTONDOWN: { g_ui.cancelarSeleccion(); break; }
    case WM_KEYDOWN: {
        if (wParam==VK_ESCAPE) { g_ui.cancelarSeleccion(); }
        else if ((GetKeyState(VK_CONTROL)&0x8000) && wParam=='Z') {
            g_ui.deshacer();
        }
        break; }
    case WM_LBUTTONDOWN: {                            
        POINT pt{ GET_X_LPARAM(lParam), GET_Y_LPARAM(lParam) };
        if (PtInRect(&g_ui.botonUndoRect(), pt)) {
            g_ui.deshacer();
            break;
        }
        int x = GET_X_LPARAM(lParam);                 
        int y = GET_Y_LPARAM(lParam);                 
        g_ui.click(x,y);                              
        break; }
    case WM_TIMER: { g_ui.tick(); InvalidateRect(hWnd, nullptr, FALSE); break; }
    case WM_ERASEBKGND: return 1;
    case WM_PAINT: {                                  
        PAINTSTRUCT ps; HDC hdc = BeginPaint(hWnd, &ps);
        RECT rc; GetClientRect(hWnd, &rc);            
        g_ui.pintar(hdc, rc);                         
        EndPaint(hWnd, &ps);                          
        break; }
    case WM_DESTROY: PostQuitMessage(0); break;       
    default: return DefWindowProc(hWnd, msg, wParam, lParam); 
    }
    return 0;                                         
}
