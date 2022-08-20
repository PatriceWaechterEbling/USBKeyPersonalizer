#include "framework.h"
#include "USBKeyPersonalizer.h"
extern "C" {
    HWND hLecteur,hConfig;
    HINSTANCE hInst;
};

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
                    BOOL bSucceeded = GetVolumeInformation(ptchDrive,szVolumeName,MAX_PATH,NULL,NULL,NULL,NULL,0);
                    char  lpszVolumePathName[0x200]; //512 caract
                    if (ptchDrive == "A:\\")return;
                    GetVolumePathName(ptchDrive, lpszVolumePathName,MAX_PATH);
                    wsprintf(buf, "%s %s",lpszVolumePathName , szVolumeName);
                    ComboBox_AddString(hLecteur,buf );
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
INT MsgBox(HWND hDlg,CHAR* lpszText, CHAR* lpszCaption, DWORD dwStyle, INT lpszIcon) { //MessageBox personalise
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
    char tmp[0x104],Reponse[0x104];
    if (Lecteur[3]== ' ') { MsgBox(NULL, (LPSTR)"Le format passe pour le lecteur est non conforme", (LPSTR)"ParseDrive", MB_APPLMODAL | MB_OK, IDI_BOMBE); return; }
    wsprintf(tmp, "%c%c%c\\autorun.inf", Lecteur[0], Lecteur[1], Lecteur[2]);
    GetPrivateProfileString((LPCTSTR)"AutoRun", (LPCTSTR)"icon", (LPCTSTR)"icone non definie",Reponse, sizeof(tmp), tmp);
    SetWindowText(hConfig, Reponse);
    if (strcmp(Reponse,"icone non definie")==0) {
        ChargerIconeExistante();
    }
}