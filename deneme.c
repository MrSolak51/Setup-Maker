#include <windows.h>
#include <shobjidl_core.h> // Modern FileDialog için gerekli
#include <shlobj.h>        // Diğer Shell fonksiyonları için

// Function to create a File Open Dialog and get the selected file path
void ShowFileOpenDialog(HWND hwnd) {
    IFileDialog *pfd = NULL;
    HRESULT hr = CoCreateInstance(CLSID_FileOpenDialog, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&pfd));

    if (SUCCEEDED(hr)) {
        hr = pfd->Show(hwnd);

        if (SUCCEEDED(hr)) {
            IShellItem *pItem;
            hr = pfd->GetResult(&pItem);

            if (SUCCEEDED(hr)) {
                PWSTR pszFilePath = NULL;
                hr = pItem->GetDisplayName(SIGDN_FILESYSPATH, &pszFilePath);

                if (SUCCEEDED(hr)) {
                    // Use pszFilePath to get the selected file path
                    MessageBox(hwnd, pszFilePath, L"Selected File", MB_OK);

                    CoTaskMemFree(pszFilePath);
                }

                pItem->Release();
            }
        }

        pfd->Release();
    }
}

LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_COMMAND:
            if (LOWORD(wParam) == ID_BUTTON_SELECT_FOLDER) {
                ShowFileOpenDialog(hwnd);
            }
            break;

        case WM_DESTROY:
            PostQuitMessage(0);
            break;

        default:
            return DefWindowProc(hwnd, msg, wParam, lParam);
    }
    return 0;
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Initialize COM library
    CoInitialize(NULL);

    WNDCLASS wc = {0};
    wc.lpfnWndProc = WindowProcedure;
    wc.hInstance = hInstance;
    wc.lpszClassName = "MyWindowClass";
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    
    RegisterClass(&wc);
    
    HWND hwnd = CreateWindow("MyWindowClass", "Modern File Dialog Example", 
                             WS_OVERLAPPEDWINDOW | WS_VISIBLE,
                             CW_USEDEFAULT, CW_USEDEFAULT, 550, 250,
                             NULL, NULL, hInstance, NULL);
    
    MSG msg = {0};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Uninitialize COM library
    CoUninitialize();

    return 0;
}
