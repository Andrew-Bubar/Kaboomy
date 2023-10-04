
//test

// Varribles
static bool running = true;

//functions
bool platform_create_window(int w, int h, char* title);
void platform_update_window();

//Windows functions
#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

//Windows Globals
static HWND window;

// Platform implementations
LRESULT CALLBACK windows_window_callback(
    HWND win, UINT msg, WPARAM wParam, LPARAM lParam
    ){
        LRESULT result = 0;

        switch(msg){

            case WM_CLOSE: {

                running = false;
                break;
            }

            default:{

                result = DefWindowProcA(win, msg, wParam, lParam);
            }
        }

        return result;
}

bool platform_create_window(int w, int h, char* title){

    HINSTANCE intstance = GetModuleHandleA(0);

    WNDCLASSA wc {};
    wc.hInstance = intstance;
    wc.hIcon = LoadIcon(intstance, IDI_APPLICATION);
    wc.hCursor = LoadCursor(NULL, IDC_ARROW);
    wc.lpszClassName = title;
    wc.lpfnWndProc = windows_window_callback;

    if(!RegisterClassA(&wc)){ return false;}

    int dwStyle = WS_OVERLAPPEDWINDOW;

    window = CreateWindowExA(0,
                                title,
                                title,
                                dwStyle,
                                100,
                                100,
                                w,
                                h,
                                NULL,
                                NULL,
                                intstance,
                                NULL);

    if (window == NULL){
        return false;
    }                

    ShowWindow(window, SW_SHOW);
    return true;
}

void platform_update_window(){

    MSG msg;

    while(PeekMessageA(&msg, window, 0, 0, PM_REMOVE)){

        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }
}
#endif

int main(){

    platform_create_window(1280, 720, "kaboomy");

    while (running)
    {
        platform_update_window();
    }
    
    return 0;
}