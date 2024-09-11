#include <windows.h>
#include <shlobj.h>
#include <ctype.h>
#include "Setup.h"

#define ID_BUTTON_SELECT_FOLDER 1
#define ID_EDIT_FOLDER_PATH 2
#define ID_BUTTON_INSTALL 3
#define ID_CHECKBOX1 4
#define ID_CHECKBOX2 5

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    static HWND hButtonSelectFolder, hEditFolderPath, hButtonInstall, hEditMultipleAgreement, hCheckbox1, hCheckbox2;
    static int desktopShortcut, startMenuShortcut;

    switch (msg) {
        case WM_CREATE: {
            HFONT hFont = CreateFont(16, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, 
                                      ANSI_CHARSET, OUT_OUTLINE_PRECIS, CLIP_DEFAULT_PRECIS,
                                      CLEARTYPE_QUALITY, VARIABLE_PITCH, TEXT("Segoe UI"));

            hEditFolderPath = CreateWindow("EDIT", "",
                                           WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_READONLY,
                                           10, 50, 300, 30, hwnd, (HMENU)ID_EDIT_FOLDER_PATH, NULL, NULL);
            SendMessage(hEditFolderPath, WM_SETFONT, (WPARAM)hFont, TRUE);

            hEditMultipleAgreement = CreateWindow("EDIT", agreement,
                         WS_CHILD | WS_VISIBLE | WS_BORDER | ES_MULTILINE | WS_VSCROLL | ES_AUTOVSCROLL | ES_READONLY ,
                         10, 80, 300, 100, hwnd, NULL, NULL, NULL);
            SendMessage(hEditMultipleAgreement, WM_SETFONT, (WPARAM)hFont, TRUE);

            hCheckbox1 = CreateWindow("BUTTON", "Create Shortcut to Desktop",
                        WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
                        10, 180, 300, 30, hwnd, (HMENU)ID_CHECKBOX1, NULL, NULL);
            SendMessage(hCheckbox1, WM_SETFONT, (WPARAM)hFont, TRUE);

            hCheckbox2 = CreateWindow("BUTTON", "Create Shortcut to Start Menu",
                        WS_CHILD | WS_VISIBLE | BS_CHECKBOX,
                        10, 210, 300, 30, hwnd, (HMENU)ID_CHECKBOX2, NULL, NULL);
            SendMessage(hCheckbox2, WM_SETFONT, (WPARAM)hFont, TRUE);
            
            hButtonInstall = CreateWindow("BUTTON", "Agree&Install",
                        WS_CHILD | WS_VISIBLE,
                        210, 240, 100, 30, hwnd, (HMENU)ID_BUTTON_INSTALL, NULL, NULL);
            SendMessage(hButtonInstall, WM_SETFONT, (WPARAM)hFont, TRUE);
        }
        break;

        case WM_COMMAND:
            if (HIWORD(wParam) == EN_SETFOCUS && LOWORD(wParam) == ID_EDIT_FOLDER_PATH) {
                char folderPath[MAX_PATH] = "";
                BROWSEINFO bi = { 0 };
                bi.lpszTitle = "Select Installation Folder";
                LPITEMIDLIST pidl = SHBrowseForFolder(&bi);
                
                if (pidl != 0) {
                    SHGetPathFromIDList(pidl, folderPath);
                    
                    SetWindowText(hEditFolderPath, folderPath);
                    
                    CoTaskMemFree(pidl);
                }
            }
            if (LOWORD(wParam) == ID_BUTTON_INSTALL) {
                char folderPath[MAX_PATH];
                GetWindowText(hEditFolderPath, folderPath, MAX_PATH);
                sprintf(folderPath, "%s\\%s", folderPath, appName);

                int result = runSetup(folderPath, desktopShortcut, startMenuShortcut);
                system("del createShortcut.ps1");
                system("del createStartMenuIcon.ps1");
                if(!result)
                    MessageBox(hwnd, "kurulum tamamlandı", "Information", MB_OK | MB_ICONINFORMATION);
                else
                    MessageBox(hwnd, "kurulum tamamlanamadı", "Error", MB_OK | MB_ICONINFORMATION);
            }
            if (LOWORD(wParam) == ID_CHECKBOX1) {
                desktopShortcut = SendMessage(hCheckbox1, BM_GETCHECK, 0, 0);

                if (desktopShortcut){
                    SendMessage(hCheckbox1, BM_SETCHECK, BST_UNCHECKED, 0);
                    desktopShortcut = 0;
                }
                else{
                    SendMessage(hCheckbox1, BM_SETCHECK, BST_CHECKED, 0);
                    desktopShortcut = 1;
                }
            }            
            if (LOWORD(wParam) == ID_CHECKBOX2) {
                startMenuShortcut = SendMessage(hCheckbox2, BM_GETCHECK, 0, 0);

                if (startMenuShortcut){
                    SendMessage(hCheckbox2, BM_SETCHECK, BST_UNCHECKED, 0);
                    startMenuShortcut = 0;
                }
                else{
                    SendMessage(hCheckbox2, BM_SETCHECK, BST_CHECKED, 0);
                    startMenuShortcut = 1;
                }
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        case WM_SIZE: {
            int width = LOWORD(lParam);
            int height = HIWORD(lParam);

            MoveWindow(hEditFolderPath, 10, 10, width - 20, 30, TRUE);
            MoveWindow(hCheckbox1, 10, 50, width - 20, 30, TRUE);
            MoveWindow(hCheckbox2, 10, 90, width - 20, 30, TRUE);
            MoveWindow(hEditMultipleAgreement, 10, 130, width - 20, height - 180, TRUE);
            MoveWindow(hButtonInstall, width - 120, height - 60, 100, 30, TRUE);
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
    
    HWND hwnd = CreateWindow("MyWindowClass", "DYSetup", 
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