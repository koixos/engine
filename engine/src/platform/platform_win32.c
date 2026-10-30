#include "platform.h"

//Windows platform layer
#if KPLATFORM_WINDOWS

#include <core/logger.h>

#include <windows.h>
#include <windowsx.h>
#include <stdlib.h>

typedef struct internal_state {
    HINSTANCE h_inst;
    HWND hwnd;
} internal_state;

//clock
static f64 clock_freq;
static LARGE_INTEGER start_time;

LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param);

b8 platform_startup(
    platform_state *st,
    const char *app_name,
    i32 x,
    i32 y,
    i32 width,
    i32 height) {
    
    //create internal st
    st->internal_st = malloc(sizeof(internal_state));
    internal_state *state = (internal_state *)st->internal_st;

    state->h_inst = GetModuleHandleA(0);

    //setup and register window class
    HICON icon = LoadIcon(state->h_inst, IDI_APPLICATION);
    WNDCLASSA wc;
    memset(&wc, 0, sizeof(wc));
    wc.style = CS_DBLCLKS;  //get double clicks
    wc.lpfnWndProc = win32_process_message;
    wc.cbClsExtra = 0;
    wc.cbWndExtra = 0;
    wc.hInstance = state->h_inst;
    wc.hIcon = icon;
    wc.hCursor = LoadCursor(NULL, IDC_ARROW); //null, manage cursor manually
    wc.hbrBackground = NULL;
    wc.lpszClassName = "engine_window_class";

    if (!RegisterClassA(&wc)) {
        MessageBoxA(0, "Window registration failed", "Error", MB_ICONEXCLAMATION | MB_OK);
        return FALSE;
    }

    //create window
    u32 cli_x = x;
    u32 cli_y = y;
    u32 cli_width = width;
    u32 cli_height = height;

    u32 window_x = cli_x;
    u32 window_x = cli_y;
    u32 window_width = cli_width;
    u32 window_height = cli_height;

    u32 window_style = WS_OVERLAPPED | WS_SYSMENU | WS_CAPTION;
    u32 window_ex_style = WS_EX_APPWINDOW;
    
    window_style |= WS_MAXIMIZEBOX;
    window_style |= WS_MINIMIZEBOX;
    window_style |= WS_THICKFRAME;

    //obtain size of border
    RECT border_rect = { 0, 0, 0, 0 };
    AdjustWindowRectEx(&border_rect, window_style, 0, window_ex_style);

    //in this case, border rectangle is negative
    window_x += border_rect.left;
    window_y += border_rect.top;

    //grow by the size of OS border
    window_width += border_rect.right - border_rect.left;
    window_height += border_rect.bottom - border_rect.top;

    HWND handle = CreateWindowExA(
        window_ex_style, "engine_window_class", app_name,
        window_style, window_x, window_y, window_width, window_height,
        0, 0, state->h_inst, 0
    );

    if (handle == 0) {
        MessageBoxA(NULL, "Window creation failed", "Error", MB_ICONEXCLAMATION | MB_OK);
        KFATAL("Window creation failed");
        return FALSE;
    }

    state->hwnd = handle;

    //show window
    b32 should_activate = 1; //TODO: if window should not accept input, this should be false
    i32 show_window_command_flags = should_activate ? SW_SHOW : SW_SHOWNOACTIVATE;
    //if initially minimized, use SW_MINIMIZE : SW_SHOWMINNOACTIVE;
    //if initially maximized, use SW_SHOWMAXIMIZED : SW_MAXIMIZE;
    ShowWindow(state->hwnd, show_window_command_flags);

    //clock setup
    LARGE_INTEGER freq;
    QueryPerformanceFrequency(&freq);
    clock_freq = 1.0 / (f64)freq.QuadPart;
    QueryPerformanceCounter(&start_time);

    return TRUE;
}

void platform_shutdown(platform_state *st) {
    //cold-cast to the known type
    internal_state *state = (internal_state *)st->internal_st;

    if (state->hwnd) {
        DestroyWindow(state->hwnd);
        state->hwnd = 0;
    }
}

b8 platform_pump_msgs(platform_state *st) {
    MSG msg;
    while (PeekMessageA(&msg, NULL, 0, 0, PM_REMOVE)) {
        TranslateMessage(&msg);
        DispatchMessageA(&msg);
    }

    return TRUE;
}

void* platform_allocate(u64 size, b8 aligned) {
    return malloc(size);
}

void platform_free(void *block, b8 aligned) {
    free(block);
} 

void* platform_zero_mem(void *block, u64 size) {
    return memset(block, 0, size);
}

void* platform_copy_mem(void *dest, const void *src, u64 size) {
    return memcpy(dest, src, size);
}

void* platform_set_mem(void *dest, i32 val, u64 size) {
    return memset(dest, val, size);
}

void platform_console_write(const char *msg, u8 clr) {
    HANDLE console_handle = GetStdHandle(STD_OUTPUT_HANDLE);
    //FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    static u8 levels[6] = {64, 4, 6, 2, 1, 8};
    SetConsoleTextAttribute(console_handle, levels[clr]); 
    OutputDebugStringA(msg);
    u64 len = strlen(msg);
    LPDWORD num_written = 0;
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), msg, (DWORD)len, num_written, 0);
}

void platform_console_write_error(const char *msg, u8 clr) {
    HANDLE console_handle = GetStdHandle(STD_ERROR_HANDLE);
    //FATAL, ERROR, WARN, INFO, DEBUG, TRACE
    static u8 levels[6] = {64, 4, 6, 2, 1, 8};
    SetConsoleTextAttribute(console_handle, levels[clr]); 
    OutputDebugStringA(msg);
    u64 len = strlen(msg);
    LPDWORD num_written = 0;
    WriteConsoleA(GetStdHandle(STD_ERROR_HANDLE), msg, (DWORD)len, num_written, 0);
}

f64 platform_get_absolute_time() {
    LARGE_INTEGER curr_time;
    QueryPerformanceCounter(&curr_time);
    return (f64)curr_time.QuadPart * clock_freq;
}

//sleep on the thread for provided ms - this blocks main thread
// should only be used to give time back to OS for unused update power
// thus it is not exported
void platform_sleep(u64 ms) {
    Sleep(ms);
}

LRESULT CALLBACK win32_process_message(HWND hwnd, u32 msg, WPARAM w_param, LPARAM l_param) {
    switch (msg) {
        case WM_ERASEBKGND:
            //notify OS that erasing will be handled by app to prevent flicker
            return 1;
        case WM_CLOSE:
            //TODO: fire an event for app to quit
            return 0;
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
        case WM_SIZE: {
            //get updated size
            RECT r;
            GetClientRect(hwnd, &r);
            u32 width = r.right - r.left;
            u32 height = r.bottom - r.top;

            //TODO: fire an event for window resize
        } break;
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        case WM_KEYUP:
        case WM_SYSKEYUP: {
            //key pressed/released
            //b8 pressed = (msg == WM_KEYDOWN || msg == WM_SYSKEYDOWN);
            //TODO: inp processing
        } break;
        case WM_MOUSEMOVE: {
            //mouse move
            i32 x_pos = GET_X_LPARAM(l_param);
            i32 y_pos = GET_X_LPARAM(l_param);
            //TODO: inp processing
        } break;
        case WM_MOUSEWHEEL: {
            i32 z_delta = GET_WHEEL_DELTA_WPARAM(w_param);
            if (z_delta != 0) {
                //TODO: inp processing
            }
        } break;
        case WM_LBUTTONDOWN:
        case WM_MBUTTONDOWN:
        case WM_RBUTTONDOWN:
        case WM_LBUTTONUP:
        case WM_MBUTTONUP:
        case WM_RBUTTONUP:{
            //key pressed/released
            //b8 pressed = (msg == WM_LBUTTONDOWN || msg == WM_MBUTTONDOWN || mag == WM_RBUTTONDOWN);
            //TODO: inp processing
        } break;
    }

    return DefWindowProcA(hwnd, msg, w_param, l_param);
}

#endif //KPLATFORM_WINDOWS