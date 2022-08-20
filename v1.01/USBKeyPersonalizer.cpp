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
HICON newIcone = 0;

typedef struct
{
    WORD idReserved; // doit etre 0
    WORD idType; // 1 = ICONE, 2 = CURSEUR
    WORD idCount; // nombre d'images
} ICONHEADER;
typedef struct
{
    BYTE bWidth;
    BYTE bHeight;
    BYTE bColorCount;
    BYTE bReserved;
    WORD wPlanes;
    WORD wBitCount;
    DWORD dwBytesInRes;
    DWORD dwImageOffset;
} ICONDIR;
typedef struct { BITMAPINFOHEADER biHeader; } ICONIMAGE;

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
HRESULT CreerIcone(HICON hIcon, CHAR* path) {
    PICTDESC desc = { sizeof(PICTDESC) };
    desc.picType = PICTYPE_ICON;
    desc.icon.hicon = hIcon;
    IPicture* pPicture = 0;
    HRESULT hr = OleCreatePictureIndirect(&desc, IID_IPicture, FALSE, (void**)&pPicture);
    if (FAILED(hr)) return hr;
    IStream* pStream = 0;
    CreateStreamOnHGlobal(0, TRUE, &pStream);
    LONG cbSize = 0;
    hr = pPicture->SaveAsFile(pStream, TRUE, &cbSize);
    if (!FAILED(hr)) {
        HGLOBAL hBuf = 0;
        GetHGlobalFromStream(pStream, &hBuf);
        void* buffer = GlobalLock(hBuf);
        HANDLE hFile = CreateFile(path, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
        if (!hFile) hr = HRESULT_FROM_WIN32(GetLastError());
        else {
            DWORD written = 0;
            WriteFile(hFile, buffer, cbSize, &written, 0);
            CloseHandle(hFile);
        }
        GlobalUnlock(buffer);
    }
    pStream->Release();
    pPicture->Release();
    return hr;
}
UINT EcrireEnteteIcone(HANDLE hFile, int nImages)
{
    ICONHEADER iconheader{};
    DWORD nWritten;
    iconheader.idReserved = 0;
    iconheader.idType = 1;
    iconheader.idCount = nImages;
    WriteFile(hFile, &iconheader, sizeof(iconheader), &nWritten, 0);
    return nWritten;
}
UINT CalculerOctetImage(BITMAP* pBitmap)
{
    int nWidthBytes = pBitmap->bmWidthBytes;
    if (nWidthBytes & 3) nWidthBytes = (nWidthBytes + 4) & ~3;
    return nWidthBytes * pBitmap->bmHeight;
}
UINT EcrireEnteteImageIcone(HANDLE hFile, BITMAP* pbmpColor, BITMAP* pbmpMask)
{
    BITMAPINFOHEADER biHeader;
    DWORD nWritten;
    UINT nImageBytes;
    nImageBytes = CalculerOctetImage(pbmpColor) + CalculerOctetImage(pbmpMask);
    ZeroMemory(&biHeader, sizeof(biHeader));
    biHeader.biSize = sizeof(biHeader);
    biHeader.biWidth = pbmpColor->bmWidth;
    biHeader.biHeight = pbmpColor->bmHeight * 2; // pour  coleur + monochrome
    biHeader.biPlanes = pbmpColor->bmPlanes;
    biHeader.biBitCount = pbmpColor->bmBitsPixel;
    biHeader.biSizeImage = nImageBytes;
    WriteFile(hFile, &biHeader, sizeof(biHeader), &nWritten, 0);
    if (pbmpColor->bmBitsPixel == 2 || pbmpColor->bmBitsPixel == 8)
    {

    }
    return nWritten;
}

BOOL LireInfosImageIcone(HICON hIcon, ICONINFO* pIconInfo, BITMAP* pbmpColor, BITMAP* pbmpMask)
{
    if (!GetIconInfo(hIcon, pIconInfo))return FALSE;
    if (!GetObject(pIconInfo->hbmColor, sizeof(BITMAP), pbmpColor))return FALSE;
    if (!GetObject(pIconInfo->hbmMask, sizeof(BITMAP), pbmpMask)) return FALSE;
    return TRUE;
}
UINT EcrireRepertoireImageIcone(HANDLE hFile, int nIdx, HICON hIcon, UINT nImageOffset)
{
    ICONINFO iconInfo;
    ICONDIR iconDir{};
    BITMAP bmpColor;
    BITMAP bmpMask;
    DWORD nWritten;
    UINT nColorCount;
    UINT nImageBytes;
    LireInfosImageIcone(hIcon, &iconInfo, &bmpColor, &bmpMask);
    nImageBytes = CalculerOctetImage(&bmpColor) + CalculerOctetImage(&bmpMask);
    if (bmpColor.bmBitsPixel >= 8)        nColorCount = 0;
    else
        nColorCount = 1 << (bmpColor.bmBitsPixel * bmpColor.bmPlanes);
    iconDir.bWidth = (BYTE)bmpColor.bmWidth;
    iconDir.bHeight = (BYTE)bmpColor.bmHeight;
    iconDir.bColorCount = nColorCount;
    iconDir.bReserved = 0;
    iconDir.wPlanes = bmpColor.bmPlanes;
    iconDir.wBitCount = bmpColor.bmBitsPixel;
    iconDir.dwBytesInRes = sizeof(BITMAPINFOHEADER) + nImageBytes;
    iconDir.dwImageOffset = nImageOffset;
    WriteFile(hFile, &iconDir, sizeof(iconDir), &nWritten, 0);
    DeleteObject(iconInfo.hbmColor);
    DeleteObject(iconInfo.hbmMask);
    return nWritten;
}

UINT EcrireDonneesIcone(HANDLE hFile, HBITMAP hBitmap)
{
    BITMAP bmp{};
    int i;
    BYTE* pIconData;
    UINT nBitmapBytes;
    DWORD nWritten;
    GetObject(hBitmap, sizeof(BITMAP), &bmp);
    nBitmapBytes = CalculerOctetImage(&bmp);
    pIconData = (BYTE*)malloc(nBitmapBytes);
    GetBitmapBits(hBitmap, nBitmapBytes, pIconData);
    for (i = bmp.bmHeight - 1; i >= 0; i--) {
        WriteFile(hFile, pIconData + (i * bmp.bmWidthBytes), bmp.bmWidthBytes, &nWritten, 0);
        if (bmp.bmWidthBytes & 3) {
            DWORD padding = 0;
            WriteFile(hFile, &padding, 4 - bmp.bmWidthBytes, &nWritten, 0);
        }
    }
    free(pIconData);
    return nBitmapBytes;
}
BOOL EcrireIcone(TCHAR* szIconFile, HICON hIcon[], int nNumIcons)
{
    HANDLE hFile;
    int i;
    int* pImageOffset;

    if (hIcon == 0 || nNumIcons < 1)return FALSE;
    hFile = CreateFile(szIconFile, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, 0, 0);
    if (hFile == INVALID_HANDLE_VALUE)        return FALSE;
    EcrireEnteteIcone(hFile, nNumIcons);
    SetFilePointer(hFile, sizeof(ICONDIR) * nNumIcons, 0, FILE_CURRENT);
    pImageOffset = (int*)malloc(nNumIcons * sizeof(int));
    for (i = 0; i < nNumIcons; i++) {
        ICONINFO iconInfo;
        BITMAP bmpColor, bmpMask;
        LireInfosImageIcone(hIcon[i], &iconInfo, &bmpColor, &bmpMask);
        pImageOffset[i] = SetFilePointer(hFile, 0, 0, FILE_CURRENT);
        EcrireEnteteImageIcone(hFile, &bmpColor, &bmpMask);
        EcrireDonneesIcone(hFile, iconInfo.hbmColor);
        EcrireDonneesIcone(hFile, iconInfo.hbmMask);
        DeleteObject(iconInfo.hbmColor);
        DeleteObject(iconInfo.hbmMask);
    }
    SetFilePointer(hFile, sizeof(ICONHEADER), 0, FILE_BEGIN);
    for (i = 0; i < nNumIcons; i++) {
        EcrireRepertoireImageIcone(hFile, i, hIcon[i], pImageOffset[i]);
    }
    free(pImageOffset);
    CloseHandle(hFile);
    return TRUE;
}
int ExtraireIconeDuFichier(TCHAR* filename, TCHAR* iconFile) {
    HICON hIconLarge;
    HICON hIconSmall;
    BOOL ret;
    if (ExtractIconEx(filename, 0, &hIconLarge, &hIconSmall, 1) == 0) { return 1; }
    ret = EcrireIcone(iconFile, &hIconSmall, 1);
    if (ret) { return 0; }
    return -1;
}
VOID ListerDisques() {
    char atchDrives[150];
    CHAR* ptchDrive = NULL;
    char buf[256];
    if (GetLogicalDriveStringsA(sizeof(atchDrives), atchDrives))
    {
        if (strlen(atchDrives) > 0)
        {
            ptchDrive = atchDrives;
            do
            {
                if ((GetDriveTypeA(ptchDrive)) == 0x02) //Verifie que c'est bien un Disque ejectable
                {
                    CHAR szVolumeName[MAX_PATH];
                    BOOL bSucceeded = GetVolumeInformation(ptchDrive, szVolumeName, MAX_PATH, NULL, NULL, NULL, NULL, 0);
                    char  lpszVolumePathName[0x200]; //512 caract
                    if (ptchDrive == "A:\\")return;
                    GetVolumePathName(ptchDrive, lpszVolumePathName, MAX_PATH);
                    wsprintf(buf, "%s %s", lpszVolumePathName, szVolumeName);
                    ComboBox_AddString(hLecteur, buf);
                }
                do ++ptchDrive;
                while (*ptchDrive);
                ++ptchDrive;
            } while (strlen(ptchDrive));
        }
    }
}
CHAR* AjourerLeNomDuProjet(CHAR* s)
{
    char tt[35];
    std::string t = s;
    for (INT i = 0; i < t.size(); i++) {
        if (t[i] = ' ') {
            t[i] = '_';
        }
    }
    wsprintfA(tt, "%s", t.c_str());
    return tt;
}
INT MsgBox(HWND hDlg, CHAR* lpszText, CHAR* lpszCaption, DWORD dwStyle, INT lpszIcon) { //MessageBox personalise
    MSGBOXPARAMS lpmbp{};
    lpmbp.hInstance = hInst;
    lpmbp.cbSize = sizeof(MSGBOXPARAMS);
    lpmbp.hwndOwner = hDlg;
    lpmbp.dwLanguageId = MAKELANGID(0x0800, 0x0800); //par defaut celui du systeme
    lpmbp.lpszText = lpszText;
    lpmbp.lpszCaption = lpszCaption;
    lpmbp.dwStyle = dwStyle | 0x00000080L;
    if (lpszIcon == 0) {
        lpmbp.lpszIcon = (LPCSTR)IDI_USBKEYPERSONALIZER;
    }
    else {
        lpmbp.lpszIcon = MAKEINTRESOURCE(lpszIcon);
    }
    lpmbp.lpfnMsgBoxCallback = 0;
    return  MessageBoxIndirect(&lpmbp);
}

VOID EcrireDonnees(CHAR* Lecteur)
{
    char tmp[0x104], Reponse[0x104];
    if (Lecteur[3] == ' ') { MsgBox(NULL, (LPSTR)"Le format passe pour le lecteur est non conforme", (LPSTR)"ParseDrive", MB_APPLMODAL | MB_OK, IDI_BOMBE); return; }
    wsprintf(tmp, "%c%c%c\\autorun.inf", Lecteur[0], Lecteur[1], Lecteur[2]);
    GetPrivateProfileString((LPCTSTR)"AutoRun", (LPCTSTR)"icon", (LPCTSTR)"icone non definie", Reponse, sizeof(tmp), tmp);
    SetWindowText(hConfig, Reponse);

}
VOID LireDonnees(CHAR* Lecteur) {
    char tmp[0x104], Reponse[0x104];
    if (Lecteur[3] == ' ') { MsgBox(NULL, (LPSTR)"Le format passe pour le lecteur est non conforme", (LPSTR)"ParseDrive", MB_APPLMODAL | MB_OK, IDI_BOMBE); return; }
    wsprintf(tmp, "%c%c%c\\autorun.inf", Lecteur[0], Lecteur[1], Lecteur[2]);
    GetPrivateProfileString((LPCTSTR)"AutoRun", (LPCTSTR)"icon", (LPCTSTR)"icone non definie", Reponse, sizeof(tmp), tmp);
    SetWindowText(hConfig, Reponse);
    if (strcmp(Reponse, "icone non definie") == 0) {
        ChargerIconeExistante();
    }
}
VOID ChargerIconeExistante() {
    OPENFILENAME ofn;
    char szFile[100]{};
    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize = sizeof(ofn);
    ofn.hwndOwner = NULL;
    ofn.lpstrFile = szFile;
    ofn.lpstrFile[0] = '\0';
    ofn.nMaxFile = sizeof(szFile);
    ofn.lpstrFilter = "Fichiers icones\0*.ico\0\0";
    ofn.nFilterIndex = 1;
    ofn.lpstrFileTitle = NULL;
    ofn.nMaxFileTitle = 0;
    ofn.lpstrInitialDir = NULL;
    ofn.lpstrDefExt = "ico";
    ofn.Flags = OFN_PATHMUSTEXIST | OFN_EXPLORER | OFN_FILEMUSTEXIST | OFN_HIDEREADONLY;
    if (!GetOpenFileName(&ofn)) { return; }
    SetWindowText(hIcone, ofn.lpstrFile);
    newIcone = (HICON)LoadImage(wcex.hInstance, ofn.lpstrFile, IMAGE_ICON, 0x30, 0x30, LR_LOADFROMFILE);//creer une icone au format 48x48 24bits
    if (!newIcone) { return; }
    SendMessage(imgIcon, STM_SETIMAGE, IMAGE_ICON, (LPARAM)newIcone);
    wsprintf(szFile, "%sautorun.bin", hLecteur);
}
