#include "framework.h"
#include "USBKeyPersonalizer.h"

static HWND hLecteur, hTitre, hIcone, hConfig, hConfigTitre, imgIcon;
static WNDCLASSEX wcex;
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
typedef struct{ BITMAPINFOHEADER biHeader;} ICONIMAGE;

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
    newIcone =(HICON) LoadImage(wcex.hInstance, ofn.lpstrFile, IMAGE_ICON, 0x30, 0x30, LR_LOADFROMFILE);//creer une icone au format 48x48 24bits
    if (!newIcone) { return; }
    SendMessage(imgIcon, STM_SETIMAGE, IMAGE_ICON, (LPARAM)newIcone);
    wsprintf(szFile, "%sautorun.bin", hLecteur);
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
static UINT EcrireEnteteIcone(HANDLE hFile, int nImages)
{
    ICONHEADER iconheader{};
    DWORD nWritten;
    iconheader.idReserved = 0;
    iconheader.idType = 1; 
    iconheader.idCount = nImages; 
    WriteFile(hFile, &iconheader, sizeof(iconheader), &nWritten, 0);
    return nWritten;
}
static UINT CalculerOctetImage(BITMAP* pBitmap)
{
    int nWidthBytes = pBitmap->bmWidthBytes;
    if (nWidthBytes & 3) nWidthBytes = (nWidthBytes + 4) & ~3;
    return nWidthBytes * pBitmap->bmHeight;
}
static UINT EcrireEnteteImageIcone(HANDLE hFile, BITMAP* pbmpColor, BITMAP* pbmpMask)
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

static BOOL LireInfosImageIcone(HICON hIcon, ICONINFO* pIconInfo, BITMAP* pbmpColor, BITMAP* pbmpMask)
{
    if (!GetIconInfo(hIcon, pIconInfo))return FALSE;
    if (!GetObject(pIconInfo->hbmColor, sizeof(BITMAP), pbmpColor))return FALSE;
    if (!GetObject(pIconInfo->hbmMask, sizeof(BITMAP), pbmpMask)) return FALSE;
    return TRUE;
}
static UINT EcrireRepertoireImageIcone(HANDLE hFile, int nIdx, HICON hIcon, UINT nImageOffset)
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

static UINT EcrireDonneesIcone(HANDLE hFile, HBITMAP hBitmap)
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
    for (i = bmp.bmHeight - 1; i >= 0; i--){
        WriteFile(hFile,pIconData + (i * bmp.bmWidthBytes),bmp.bmWidthBytes,&nWritten,0);
        if (bmp.bmWidthBytes & 3){
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
    for (i = 0; i < nNumIcons; i++){
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
    for (i = 0; i < nNumIcons; i++){
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
    if (ret) {return 0;}
    return -1;
}