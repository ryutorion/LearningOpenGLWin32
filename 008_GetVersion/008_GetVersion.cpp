// 008_GetVersion.cpp
#include <Windows.h>
#include <tchar.h>
#include <iostream>
#include <sstream>
#include <cstdlib>
#include "UsingConsole.h"
#include <gl/GL.h>

#pragma comment(lib, "OpenGL32.lib")

using namespace std;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool OnCreate(HWND hWnd);
void OnDraw(HWND hWnd);
DWORD PrintSystemError(DWORD error = GetLastError());

// 画面サイズ
const int Width = 640;
const int Height = 480;

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR lpCmdLine, int nCmdShow)
{
    UsingConsole uc;

    // ウィンドウクラスの初期化
    WNDCLASSEX wcx {};
    wcx.cbSize = sizeof wcx;
    wcx.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
    wcx.lpfnWndProc = WindowProc;
    wcx.cbClsExtra = 0;
    wcx.cbWndExtra = 0;
    wcx.hInstance = hInstance;
    wcx.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    wcx.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wcx.hbrBackground = reinterpret_cast<HBRUSH>(COLOR_WINDOW + 1);
    wcx.lpszMenuName = nullptr;
    wcx.lpszClassName = L"LearningOpenGLWin32";

    // ウィンドウクラスの登録
    if(!RegisterClassEx(&wcx))
    {
        PrintSystemError();
        return 0;
    }

    // スクリーンサイズの取得
    const int ScreenWidth = GetSystemMetrics(SM_CXSCREEN);
    const int ScreenHeight = GetSystemMetrics(SM_CYSCREEN);

    // ウィンドウサイズの決定 (画面中心に配置)
    DWORD WindowStyle = WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME;
    RECT WindowRect {
        (ScreenWidth - Width) / 2,
        (ScreenHeight - Height) / 2,
        (ScreenWidth + Width) / 2,
        (ScreenHeight + Height) / 2
    };
    AdjustWindowRect(&WindowRect, WindowStyle, FALSE);

    // ウィンドウの生成
    HWND hWnd = CreateWindow(
        wcx.lpszClassName,
        wcx.lpszClassName,
        WindowStyle,
        WindowRect.left,
        WindowRect.top,
        WindowRect.right - WindowRect.left,
        WindowRect.bottom - WindowRect.top,
        nullptr,
        nullptr,
        wcx.hInstance,
        nullptr
    );
    if(!hWnd)
    {
        PrintSystemError();
        return 0;
    }

    // ウィンドウの表示
    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    // メインループ
    MSG msg {};
    while(msg.message != WM_QUIT)
    {
        // メッセージの確認
        if(PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            // メッセージがあった場合の処理
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            OnDraw(hWnd);
            // メッセージがなかった場合の処理
            Sleep(1);
        }
    }

    HGLRC hGLRC = wglGetCurrentContext();
    wglMakeCurrent(nullptr, nullptr);
    wglDeleteContext(hGLRC);

    return static_cast<int>(msg.wParam);
}

// メッセージ処理関数
LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    switch(msg)
    {
    case WM_CREATE:
        if(!OnCreate(hWnd))
        {
            return -1;
        }
        break;
    case WM_DESTROY:
        // WM_QUITメッセージを送る
        PostQuitMessage(0);
        break;
    default:
        // 未処理のメッセージを処理する
        return DefWindowProc(hWnd, msg, wParam, lParam);
    }

    return 0;
}

bool OnCreate(HWND hWnd)
{
    HDC hDC = GetDC(hWnd);
    if(hDC == nullptr)
    {
        PrintSystemError();
        return false;
    }

    PIXELFORMATDESCRIPTOR PixelFormatDescriptor {};
    PixelFormatDescriptor.nSize = sizeof PixelFormatDescriptor;
    PixelFormatDescriptor.nVersion = 1;
    PixelFormatDescriptor.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    PixelFormatDescriptor.iPixelType = PFD_TYPE_RGBA;
    PixelFormatDescriptor.cColorBits = 24;
    // PixelFormatDescriptor.cRedBits;
    // PixelFormatDescriptor.cRedShift;
    // PixelFormatDescriptor.cGreenBits;
    // PixelFormatDescriptor.cGreenShift;
    // PixelFormatDescriptor.cBlueBits;
    // PixelFormatDescriptor.cBlueShift;
    PixelFormatDescriptor.cAlphaBits = 8;
    // PixelFormatDescriptor.cAlphaShift;
    PixelFormatDescriptor.cAccumBits = 0;
    // PixelFormatDescriptor.cAccumRedBits;
    // PixelFormatDescriptor.cAccumGreenBits;
    // PixelFormatDescriptor.cAccumBlueBits;
    // PixelFormatDescriptor.cAccumAlphaBits;
    PixelFormatDescriptor.cDepthBits = 24;
    PixelFormatDescriptor.cStencilBits = 8;
    PixelFormatDescriptor.cAuxBuffers = 0;
    PixelFormatDescriptor.iLayerType = PFD_MAIN_PLANE;
    // PixelFormatDescriptor.bReserved;
    // PixelFormatDescriptor.dwLayerMask;
    // PixelFormatDescriptor.dwVisibleMask;
    // PixelFormatDescriptor.dwDamageMask;

    int PixelFormatIndex = ChoosePixelFormat(hDC, &PixelFormatDescriptor);
    if(!PixelFormatIndex)
    {
        PrintSystemError();
        return false;
    }

    if(!SetPixelFormat(hDC, PixelFormatIndex, &PixelFormatDescriptor))
    {
        PrintSystemError();
        return false;
    }

    HGLRC hGLRC = wglCreateContext(hDC);
    if(hGLRC == nullptr)
    {
        PrintSystemError();
        return false;
    }

    if(!wglMakeCurrent(hDC, hGLRC))
    {
        wglDeleteContext(hGLRC);
        PrintSystemError();
        return false;
    }

    auto * pVersionString = reinterpret_cast<const char *>(glGetString(GL_VERSION));
    if(pVersionString == nullptr)
    {
        return false;
    }
    istringstream version(pVersionString);
    int MajorVersion = 1;
    version >> MajorVersion;
    version.get();
    int MinorVersion = 1;
    version >> MinorVersion;

    cout << "OpenGL version : " << MajorVersion << "." << MinorVersion << endl;

    return true;
}

void OnDraw(HWND hWnd)
{
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glEnableClientState(GL_VERTEX_ARRAY);

    float vertices[] =
    {
         0.0f,  0.5f,
         0.5f, -0.5f,
        -0.5f, -0.5f
    };
    int components = 2;

    glVertexPointer(components, GL_FLOAT, 0, vertices);
    glDrawArrays(GL_TRIANGLES, 0, 3);

    SwapBuffers(GetDC(hWnd));
}

DWORD PrintSystemError(DWORD error)
{
    WCHAR * pBuffer = nullptr;
    DWORD length = FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr,
        error,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPWSTR>(&pBuffer),
        0,
        nullptr
    );

    if(length > 0 && pBuffer != nullptr)
    {
        wcout << pBuffer << endl;
        LocalFree(pBuffer);
    }

    return error;
}
