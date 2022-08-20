// header.h : fichier Include pour les fichiers Include système standard,
// ou les fichiers Include spécifiques aux projets
//

#pragma once

#include "targetver.h"
#define WIN32_LEAN_AND_MEAN             // Exclure les en-têtes Windows rarement utilisés
#define MAX_LOADSTRING 100

using namespace std;

// Fichiers d'en-tête Windows
#include <windows.h>
#include <windowsx.h>
#include <CommCtrl.h>
#include <Winuser.h>
#include <commdlg.h>
#include <stdlib.h>
#include <malloc.h>
#include <memory.h>
#include <tchar.h>
#include <shellapi.h>
#include <stdio.h>
#include <conio.h>
#include <string>
#include <atlstr.h>
#include <olectl.h>

#pragma warning(disable:4996)
#pragma warning(disable:6054)
#pragma warning(disable:4244)
#pragma warning(disable:6054)
#pragma warning(disable:4311)
#pragma warning(disable:4302)
#pragma warning(disable:4172)

#pragma comment(lib,"comctl32")
#pragma comment(lib, "oleaut32.lib")
