// USBKeyPersonalizer.cpp : Définit le point d'entrée de l'application.
// © Patrice Waechter-Ebling 2022

#include "framework.h"
#include "USBKeyPersonalizer.h"

HINSTANCE hInst;
CHAR szTitle[MAX_LOADSTRING];
CHAR szWindowClass[MAX_LOADSTRING];
HWND hLecteur,hTitre,hIcone,hConfig,hConfigTitre, imgIcon;
WNDCLASSEX wcex;
NOTIFYICONDATA nf;


INT APIENTRY wWinMain(_In_ HINSTANCE hInstance,_In_opt_ HINSTANCE hPrevInstance,_In_ LPWSTR    lpCmdLine,_In_ INT       nCmdShow){
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    LoadString(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadString(hInstance, IDC_USBKEYPERSONALIZER, szWindowClass, MAX_LOADSTRING);
    memset(&wcex, 0, sizeof(wcex));
    wcex.cbSize = sizeof(WNDCLASSEX);
    wcex.style          = CS_HREDRAW | CS_VREDRAW;
    wcex.lpfnWndProc    = WndProc;
    wcex.cbClsExtra     = 0;
    wcex.cbWndExtra     = 0;
    wcex.hInstance      = hInstance;
    wcex.hIcon          = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_USBKEYPERSONALIZER));
    wcex.hCursor        = LoadCursor(nullptr, IDC_ARROW);
    wcex.hbrBackground  = (HBRUSH)CreateSolidBrush(RGB(0, 0, 128));
    wcex.lpszMenuName = NULL;
    wcex.lpszClassName  = szWindowClass;
    wcex.hIconSm        = wcex.hIcon;
    if (!RegisterClassEx(&wcex)) { return FALSE; }
    hInst = hInstance;
    RECT rc;
    GetClientRect(GetDesktopWindow(), &rc);
    HWND hWnd = CreateWindow(szWindowClass, "Personnaliser sa clef USB", WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_THICKFRAME, (rc.right - 480) / 2, (rc.bottom - 320) / 2, 480, 320, nullptr, nullptr, hInstance, nullptr);
    if (!hWnd){ return FALSE; }
    CreateWindowExA(0, WC_STATIC, "Selectionner un lecteur compatible", WS_CHILD | WS_VISIBLE, 32, 32, 230, 19, hWnd, 0, hInstance, 0);
    hLecteur = CreateWindowExA(0, WC_COMBOBOX, 0x00, WS_CHILD | WS_VISIBLE | CBS_DROPDOWN | CBS_HASSTRINGS | CBS_SORT | WS_VSCROLL | WS_TABSTOP, 264, 30, 180, 19, hWnd, (HMENU)1000, hInstance, 0);
    CreateWindowExA(0, WC_STATIC, "Titre a appliquer", WS_CHILD | WS_VISIBLE, 32, 57, 230, 19, hWnd, 0, hInstance, 0);
    hTitre = CreateWindowExA(0, WC_EDIT, 0x00, WS_CHILD | WS_VISIBLE  | CBS_SORT | WS_TABSTOP|WS_BORDER, 32,72, 380, 19, hWnd, (HMENU)1001, hInstance, 0);
    ComboBox_SetText(hLecteur,"Lecteurs disponibles");
    CreateWindowExA(0, WC_STATIC, "Emplacement de l'icone existante a utiliser", WS_CHILD | WS_VISIBLE, 32, 97, 290, 19, hWnd, 0, hInstance, 0);
    hIcone = CreateWindowExA(0, WC_EDIT, 0x00, WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER|WS_DISABLED, 32, 112, 380, 19, hWnd, (HMENU)1002, hInstance, 0);
    CreateWindowExA(0, WC_STATIC, "Icone existante", WS_CHILD | WS_VISIBLE, 32, 134, 100, 19, hWnd, 0, hInstance, 0);
    hConfig = CreateWindowExA(0, WC_EDIT, 0x00, WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | WS_DISABLED, 132, 132, 280, 19, hWnd, (HMENU)1002, hInstance, 0);
    CreateWindowExA(0, WC_STATIC, "Nouvelle icone", WS_CHILD | WS_VISIBLE, 32, 154, 100, 19, hWnd, 0, hInstance, 0);
    hConfigTitre = CreateWindowExA(0, WC_EDIT, 0x00, WS_CHILD | WS_VISIBLE | WS_TABSTOP | WS_BORDER | WS_DISABLED, 132, 152, 280, 19, hWnd, (HMENU)1002, hInstance, 0);
    CreateWindowExA(0, WC_BUTTON, "...", WS_CHILD | WS_VISIBLE, 414, 111, 30, 21, hWnd, (HMENU)1003, hInstance, 0);
    CreateWindowExA(0, WC_BUTTON, "?", WS_CHILD | WS_VISIBLE, 332, 232, 30, 19, hWnd, (HMENU)IDM_ABOUT, hInstance, 0);
    CreateWindowExA(0, WC_BUTTON, "Quitter", WS_CHILD | WS_VISIBLE, 365, 232, 70, 19, hWnd, (HMENU)IDM_EXIT, hInstance, 0);
    imgIcon = CreateWindowEx(0, "STATIC", NULL, WS_VISIBLE | WS_CHILD | SS_ICON, 32, 182, 10, 10, hWnd, (HMENU)45000, hInstance, NULL);
    CreateStatusWindow(WS_CHILD | WS_VISIBLE | WS_BORDER, "© Patrice Waechter-Ebling 2022", hWnd, 0x1770);
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);
    HACCEL hAccelTable = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_USBKEYPERSONALIZER));
    MSG msg;
    ListerDisques();
    while (GetMessage(&msg, nullptr, 0, 0)) {
        if (!TranslateAccelerator(msg.hwnd, hAccelTable, &msg)) { TranslateMessage(&msg); DispatchMessage(&msg); }
    }
    return (INT) msg.wParam;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam){
    switch (message) {
        case WM_CREATE: {
            TBADDBITMAP tbab{};
            TBBUTTON tbb[5];
            HWND hToolBar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | WS_VISIBLE, 0, 0, 0, 0, hWnd, (HMENU)0x1386, hInst, NULL);
            SendMessage(hToolBar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0);
            tbab.hInst = HINST_COMMCTRL;
            tbab.nID = IDB_STD_SMALL_COLOR;
            SendMessage(hToolBar, TB_ADDBITMAP, 0, (LPARAM)&tbab);
            ZeroMemory(tbb, sizeof(tbb));
            tbb[0].iBitmap = STD_FILEOPEN;
            tbb[0].fsState = TBSTATE_ENABLED;
            tbb[0].fsStyle = TBSTYLE_BUTTON;
            tbb[0].idCommand = 1003;
            tbb[1].iBitmap = STD_FIND;
            tbb[1].fsState = TBSTATE_ENABLED;
            tbb[1].fsStyle = TBSTYLE_BUTTON;
            tbb[1].idCommand = 1004;
            tbb[2].fsStyle = TBSTYLE_SEP;
            tbb[3].iBitmap = STD_PROPERTIES;
            tbb[3].fsState = TBSTATE_ENABLED;
            tbb[4].iBitmap = STD_HELP;
            tbb[4].fsStyle = TBSTYLE_BUTTON;
            tbb[4].idCommand = IDM_ABOUT;
            tbb[4].fsState = TBSTATE_ENABLED;
            SendMessage(hToolBar, TB_ADDBUTTONS, 5, (LPARAM)&tbb);
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hToolBar, &ps);
            RECT hrc;
            GetClientRect(hToolBar, &hrc);
            FillRect(hdc,&hrc, CreateSolidBrush(RGB(128, 128, 128)));
            MoveWindow(hToolBar, hrc.left, hrc.top, hrc.right - 65, hrc.bottom, true);
            DrawIcon(hdc, hrc.right - 35, 0, LoadIcon(wcex.hInstance, (LPCTSTR)IDI_BOMBE));
            EndPaint(hToolBar, &ps);
            nf.cbSize = sizeof(nf);
            nf.hIcon = wcex.hIcon;
            nf.hWnd = hWnd;
            strcpy(nf.szTip, "USBKeyPersonalizer\n(C) Patrice Waechter-Ebling\nVersion 1.00\0");
            nf.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP | NIF_REALTIME | NIF_SHOWTIP;
            Shell_NotifyIcon(NIM_ADD, &nf);

        }return 1;
        case WM_COMMAND:
        {
            switch (LOWORD(wParam))
            {
            case IDM_ABOUT:
                DialogBox(hInst, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case 1000:
                if (HIWORD(wParam) == CBN_SELCHANGE) {
                    int ItemIndex = SendMessage(hLecteur, CB_GETCURSEL, 0, 0);
                    TCHAR  ListItem[0x24]{}, Lecteur[3], Temp[260];
                    SendMessage(hLecteur, CB_GETLBTEXT, (WPARAM)ItemIndex, (LPARAM)ListItem);
                    SetDlgItemText(hWnd, 0x1770, ListItem);
                    int b = SendMessage(hLecteur, CB_GETLBTEXTLEN, (WPARAM)ItemIndex, NULL);
                    strcpy(Temp, ListItem);
                    CString tmp = ListItem;
                    strcpy(Lecteur, tmp.Left(3));
                    strcpy(Temp, tmp.Right(tmp.GetLength()-4));
                    SetWindowText(hTitre, Temp);
                    MessageBox(0, Lecteur, ListItem, 0);
                    LireDonnees(Lecteur);


                }break;
            case 1003:
            {
                ChargerIconeExistante();
                
            }
            case 1004: {
                char tmp[0x40];//64 caractere de longeur
                GetWindowText(hTitre, tmp, sizeof(tmp));
                if (strlen(tmp) < 3) {
                    MsgBox(hWnd, (CHAR*)"La longeur du titre doit contenir au moins 3 caracteres", (CHAR*)"USBKeyPersonalizer", MB_OK, 0);
                }
                SetWindowText(hTitre, AjourerLeNomDuProjet(tmp));

            }break;
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        case WM_CTLCOLORDLG:return (LONG)(HBRUSH)CreateSolidBrush(RGB(0, 0, 128));
        case WM_CTLCOLORSTATIC:SetTextColor((HDC)wParam, RGB(255, 0, 255));SetBkMode((HDC)wParam, TRANSPARENT);return (LONG)wcex.hbrBackground;
        case WM_CTLCOLOREDIT: SetTextColor((HDC)wParam, RGB(0xA0, 0xFF, 0xAA)); SetBkMode((HDC)wParam, TRANSPARENT); return (LONG)(HBRUSH)CreateSolidBrush(RGB(0, 0, 128));
        case WM_CTLCOLORLISTBOX:SetTextColor((HDC)wParam, RGB(0, 255, 255));SetBkMode((HDC)wParam, TRANSPARENT); return (LONG)(HBRUSH)CreateSolidBrush(RGB(0, 128, 0));
        case WM_CTLCOLORBTN:SetTextColor((HDC)wParam, RGB(0, 255, 255)); SetBkMode((HDC)wParam, TRANSPARENT); return (LONG)(HBRUSH)CreateSolidBrush(RGB(128, 0, 0));
        case WM_CTLCOLORMSGBOX: return (long)(HBRUSH)CreateSolidBrush(RGB(255, 0, 128));
        case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hdc = BeginPaint(hWnd, &ps);
            RECT hrc;
            GetClientRect(hWnd, &hrc);
            DrawIcon(hdc, hrc.right - 50, 65, LoadIcon(wcex.hInstance, (LPCTSTR)IDI_USB));
            EndPaint(hWnd, &ps);
        }break;
        case WM_DESTROY:Shell_NotifyIcon(NIM_DELETE, &nf); PostQuitMessage(0); break;
        default: return DefWindowProc(hWnd, message, wParam, lParam);
    }return 0;
}

// Gestionnaire de messages pour la boîte de dialogue À propos de.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
        case WM_CTLCOLORDLG:return (LONG)(HBRUSH)CreateSolidBrush(RGB(0, 0, 128));
        case WM_CTLCOLORSTATIC:SetTextColor((HDC)wParam, RGB(255, 0, 255)); SetBkMode((HDC)wParam, TRANSPARENT); return (LONG)wcex.hbrBackground;
        case WM_CTLCOLOREDIT: SetTextColor((HDC)wParam, RGB(0xA0, 0xFF, 0xAA)); SetBkMode((HDC)wParam, TRANSPARENT); return (LONG)(HBRUSH)CreateSolidBrush(RGB(0, 0, 128));
        case WM_CTLCOLORLISTBOX:SetTextColor((HDC)wParam, RGB(0, 255, 255)); SetBkMode((HDC)wParam, TRANSPARENT); return (LONG)(HBRUSH)CreateSolidBrush(RGB(0, 128, 0));
        case WM_CTLCOLORBTN:SetTextColor((HDC)wParam, RGB(0, 255, 255)); SetBkMode((HDC)wParam, TRANSPARENT); return (LONG)(HBRUSH)CreateSolidBrush(RGB(128, 0, 0));
        case WM_CTLCOLORMSGBOX: return (long)(HBRUSH)CreateSolidBrush(RGB(255, 0, 128));
    }
    return (INT_PTR)FALSE;
}