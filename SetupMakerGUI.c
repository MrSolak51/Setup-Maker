#include <windows.h>
#include <shlobj.h>
#include <ctype.h>
#include "SetupMaker.h"

#define ID_EDIT_FOLDER_PATH 1
#define ID_BUTTON_CREATE 2
#define ID_EDIT_FOLDER_SETUP 3
#define ID_EDIT_APP_NAME 4
#define ID_EDIT_AGREEMENT 5

char* getAgreement() {
    FILE* file = fopen("agreement.txt", "r");
    if (file == NULL) {
        MessageBox(NULL, "Dosya açılamadı.", "Hata", MB_OK | MB_ICONERROR);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    long fileSize = ftell(file);
    fseek(file, 0, SEEK_SET);

    char* buffer = (char*)malloc(fileSize + 1);
    if (buffer == NULL) {
        fclose(file);
        MessageBox(NULL, "Bellek tahsisi başarısız.", "Hata", MB_OK | MB_ICONERROR);
        return NULL;
    }

    fread(buffer, 1, fileSize, file);
    buffer[fileSize] = '\0'; 

    fclose(file);
    char* newBuffer = (char*)malloc(fileSize * 2 + 2); 
    if (newBuffer == NULL) {
        free(buffer);
        MessageBox(NULL, "Bellek tahsisi başarısız.", "Hata", MB_OK | MB_ICONERROR);
        return NULL;
    }

    char* p = buffer;
    char* q = newBuffer;
    while (*p) {
        if (*p == '\n') {
            *q++ = '\r';
        }
        *q++ = *p++;
    }
    *q = '\0';

    free(buffer);
    return newBuffer;
}

int isEmpty(const char *str) {
    while (*str) {
        if (!isspace((unsigned char)*str)) {
            return 0;
        }
        str++;
    }
    return 1;
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hButtonSelectFolder, hEditFolderPath, hButtonSelectSetup, hEditFolderSetup, hButtonCreate, hEditMultipleAgreement, hEditAppName;
    static char folderPath[MAX_PATH];
    static char setupPath[MAX_PATH];

    switch (msg) {
        case WM_CREATE: {
            HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
                                      ANSI_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                                      CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Segoe UI"));

            hEditAppName = CreateWindow("EDIT", "APP",
                        WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL,
                        10, 10, 300, 30, hwnd, (HMENU)ID_EDIT_APP_NAME, NULL, NULL);
            SendMessage(hEditAppName, WM_SETFONT, (WPARAM)hFont, TRUE);

            hEditFolderPath = CreateWindow("EDIT", "",
                                           WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_READONLY,
                                           10, 50, 300, 30, hwnd, (HMENU)ID_EDIT_FOLDER_PATH, NULL, NULL);
            SendMessage(hEditFolderPath, WM_SETFONT, (WPARAM)hFont, TRUE);

            hEditFolderSetup = CreateWindow("EDIT", "",
                                           WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_READONLY,
                                           10, 90, 300, 30, hwnd, (HMENU)ID_EDIT_FOLDER_SETUP, NULL, NULL);
            SendMessage(hEditFolderSetup, WM_SETFONT, (WPARAM)hFont, TRUE);

            hEditMultipleAgreement = CreateWindow("EDIT", getAgreement(),
                         WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | WS_VSCROLL | ES_AUTOVSCROLL | ES_READONLY,
                         10, 130, 350, 100, hwnd, NULL, NULL, NULL);
            SendMessage(hEditMultipleAgreement, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            hButtonCreate = CreateWindow("BUTTON", "Create",
                                         WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                                         210, 240, 100, 30, hwnd, (HMENU)ID_BUTTON_CREATE, NULL, NULL);
            SendMessage(hButtonCreate, WM_SETFONT, (WPARAM)hFont, TRUE);
        }
        break;

        case WM_COMMAND:
            if (HIWORD(wParam) == EN_SETFOCUS && LOWORD(wParam) == ID_EDIT_FOLDER_PATH) {
                BROWSEINFO bi = { 0 };
                bi.lpszTitle = "Select a Folder";
                LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
                
                if (pidl != 0) {
                    SHGetPathFromIDList(pidl, folderPath);
                    
                    SetWindowText(hEditFolderPath, folderPath);
                    
                    CoTaskMemFree(pidl);
                }
            }
            if (HIWORD(wParam) == EN_SETFOCUS && LOWORD(wParam) == ID_EDIT_FOLDER_SETUP) {
                BROWSEINFO bi = { 0 };
                bi.lpszTitle = "Select a Folder";
                LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
                
                if (pidl != 0) {
                    SHGetPathFromIDList(pidl, setupPath);
                    
                    SetWindowText(hEditFolderSetup, setupPath);
                    
                    CoTaskMemFree(pidl);
                }
            }
            if (LOWORD(wParam) == ID_BUTTON_CREATE) {
                char editPath[MAX_PATH];
                GetWindowText(hEditFolderPath, editPath, MAX_PATH);
                
                char appName[100];
                GetWindowText(hEditAppName, appName, sizeof(appName));

                char agreement[100000];
                GetWindowText(hEditMultipleAgreement, agreement, sizeof(agreement));

                if(!isEmpty(appName) && !isEmpty(editPath)){
                    int result = runSetupMaker(editPath, agreement, appName);
                    if(!result){
                        char command[256];
                        snprintf(command, sizeof(command), "gcc SetupGui.c -o %s\\Setup.exe -mwindows -lole32 -lshell32 -lshlwapi Setup.c", setupPath);
                        system(command);
                        MessageBox(hwnd, "Kurulum tamamlandı.", "Information", MB_OK | MB_ICONINFORMATION);
                    }
                    else{
                        MessageBox(hwnd, "Kurulum tamamlanamadı.", "Error", MB_OK | MB_ICONERROR);
                    }
                }
                else{
                    MessageBox(hwnd, "Lütfen tüm alanları doldurun.", "Warning", MB_OK | MB_ICONWARNING);
                }
            }
            break;

        case WM_DESTROY:
            CoUninitialize();
            PostQuitMessage(0);
            break;

        case WM_SIZE: {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);

            MoveWindow(hEditAppName, 10, 10, width - 20, 30, TRUE);
            MoveWindow(hEditFolderPath, 10, 50, width - 20, 30, TRUE);
            MoveWindow(hEditFolderSetup, 10, 90, width - 20, 30, TRUE);
            MoveWindow(hEditMultipleAgreement, 10, 130, width - 20, height - 180, TRUE);
            MoveWindow(hButtonCreate, width - 120, height - 60, 100, 30, TRUE);
        }
        break;

        case WM_GETMINMAXINFO: {
            MINMAXINFO *pInfo = (MINMAXINFO*)lParam;
            pInfo->ptMinTrackSize.x = 500;
            pInfo->ptMinTrackSize.y = 300;
            pInfo->ptMaxSize.x = 800;
            pInfo->ptMaxSize.y = 600;
            pInfo->ptMaxTrackSize.x = 800;
            pInfo->ptMaxTrackSize.y = 600;
        }
        break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProcedure;
    wc.hInstance = hInstance;
    wc.lpszClassName = "MyWindowClass";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW+1);
    
    RegisterClass(&wc);
    
    HWND hwnd = CreateWindow("MyWindowClass", "DYSetupMaker", 
                             WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                             CW_USEDEFAULT, CW_USEDEFAULT, 550, 250,
                             NULL, NULL, hInstance, NULL);
    
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
