#pragma once

#include "resource.h"

LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK About(HWND, UINT, WPARAM, LPARAM);
VOID ListerDisques();
CHAR* AjourerLeNomDuProjet(CHAR* s);
INT MsgBox(HWND hDlg, CHAR* lpszText, CHAR* lpszCaption, DWORD dwStyle, INT lpszIcon);
VOID EcrireDonnees(CHAR* Lecteur);
VOID LireDonnees(CHAR* Lecteur);
VOID ChargerIconeExistante();

VOID ListerDisques();
CHAR* AjourerLeNomDuProjet(CHAR* s);
INT MsgBox(HWND hDlg, CHAR* lpszText, CHAR* lpszCaption, DWORD dwStyle, INT lpszIcon);
VOID EcrireDonnees(CHAR* Lecteur);
VOID LireDonnees(CHAR* Lecteur);

VOID ChargerIconeExistante();
HRESULT CreerIcone(HICON hIcon, CHAR* path);
UINT EcrireEnteteIcone(HANDLE hFile, int nImages);
UINT CalculerOctetImage(BITMAP* pBitmap);
UINT EcrireEnteteImageIcone(HANDLE hFile, BITMAP* pbmpColor, BITMAP* pbmpMask);
BOOL LireInfosImageIcone(HICON hIcon, ICONINFO* pIconInfo, BITMAP* pbmpColor, BITMAP* pbmpMask);
UINT EcrireRepertoireImageIcone(HANDLE hFile, int nIdx, HICON hIcon, UINT nImageOffset);
UINT EcrireDonneesIcone(HANDLE hFile, HBITMAP hBitmap);
BOOL EcrireIcone(TCHAR* szIconFile, HICON hIcon[], int nNumIcons);
int ExtraireIconeDuFichier(TCHAR* filename, TCHAR* iconFile);
