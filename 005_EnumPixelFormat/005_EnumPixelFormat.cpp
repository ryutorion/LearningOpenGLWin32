// 005_EnumPixelFormat.cpp
#include <Windows.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstdlib>
#include "UsingConsole.h"

using namespace std;

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool OnCreate(HWND hWnd);
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
            // メッセージがなかった場合の処理
            Sleep(1);
        }
    }

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

    const int PixelFormatNum = DescribePixelFormat(
        hDC,
        0,
        sizeof(PIXELFORMATDESCRIPTOR),
        nullptr
    );

    ofstream fout("result.txt");
    for(int i = 1; i <= PixelFormatNum; ++i)
    {
        PIXELFORMATDESCRIPTOR PixelFormatDescriptor;
        if(!DescribePixelFormat(hDC, i, sizeof PixelFormatDescriptor, &PixelFormatDescriptor))
        {
            PrintSystemError();
            continue;
        }

        fout << "Pixel Format[" << i << "]" << endl;
        fout << "\tnSize           : " << PixelFormatDescriptor.nSize << endl;
        fout << "\tnVersion        : " << PixelFormatDescriptor.nVersion << endl;
        fout << "\tdwFlags         : ";
        char * delim = "";
#define OUTPUT_FLAG(stream, flag) \
        if(PixelFormatDescriptor.dwFlags & flag)\
        {\
            stream << delim << #flag;\
            delim = "|";\
        }

        OUTPUT_FLAG(fout, PFD_DRAW_TO_WINDOW);
        OUTPUT_FLAG(fout, PFD_DRAW_TO_BITMAP);
        OUTPUT_FLAG(fout, PFD_SUPPORT_GDI);
        OUTPUT_FLAG(fout, PFD_SUPPORT_OPENGL);
        OUTPUT_FLAG(fout, PFD_GENERIC_ACCELERATED);
        OUTPUT_FLAG(fout, PFD_GENERIC_FORMAT);
        OUTPUT_FLAG(fout, PFD_NEED_SYSTEM_PALETTE);
        OUTPUT_FLAG(fout, PFD_DOUBLEBUFFER);
        OUTPUT_FLAG(fout, PFD_STEREO);
        OUTPUT_FLAG(fout, PFD_SWAP_LAYER_BUFFERS);
#undef OUTPUT_FLAG
        fout << endl;

        fout << "\tiPixelType      : ";
        if(PixelFormatDescriptor.iPixelType & PFD_TYPE_RGBA)
        {
            fout << "PFD_TYPE_RGBA" << endl;
        }
        else if(PixelFormatDescriptor.iPixelType & PFD_TYPE_COLORINDEX)
        {
            fout << "PFD_TYPE_COLORINDEX" << endl;
        }
        else
        {
            fout << hex << static_cast<uint32_t>(PixelFormatDescriptor.iPixelType) << endl;
        }

        fout << "\tcColorBits      : " << static_cast<uint32_t>(PixelFormatDescriptor.cColorBits) << endl;
        fout << "\tcRedBits        : " << static_cast<uint32_t>(PixelFormatDescriptor.cRedBits) << endl;
        fout << "\tcRedShift       : " << static_cast<uint32_t>(PixelFormatDescriptor.cRedShift) << endl;
        fout << "\tcGreenBits      : " << static_cast<uint32_t>(PixelFormatDescriptor.cGreenBits) << endl;
        fout << "\tcGreenShift     : " << static_cast<uint32_t>(PixelFormatDescriptor.cGreenShift) << endl;
        fout << "\tcBlueBits       : " << static_cast<uint32_t>(PixelFormatDescriptor.cBlueBits) << endl;
        fout << "\tcBlueShift      : " << static_cast<uint32_t>(PixelFormatDescriptor.cBlueShift) << endl;
        fout << "\tcAlphaBits      : " << static_cast<uint32_t>(PixelFormatDescriptor.cAlphaBits) << endl;
        fout << "\tcAlphaShift     : " << static_cast<uint32_t>(PixelFormatDescriptor.cAlphaShift) << endl;
        fout << "\tcAccumBits      : " << static_cast<uint32_t>(PixelFormatDescriptor.cAccumBits) << endl;
        fout << "\tcAccumRedBits   : " << static_cast<uint32_t>(PixelFormatDescriptor.cAccumRedBits) << endl;
        fout << "\tcAccumGreenBits : " << static_cast<uint32_t>(PixelFormatDescriptor.cAccumGreenBits) << endl;
        fout << "\tcAccumBlueBits  : " << static_cast<uint32_t>(PixelFormatDescriptor.cAccumBlueBits) << endl;
        fout << "\tcAccumAlphaBits : " << static_cast<uint32_t>(PixelFormatDescriptor.cAccumAlphaBits) << endl;
        fout << "\tcDepthBits      : " << static_cast<uint32_t>(PixelFormatDescriptor.cDepthBits) << endl;
        fout << "\tcStencilBits    : " << static_cast<uint32_t>(PixelFormatDescriptor.cStencilBits) << endl;
        fout << "\tcAuxBuffers     : " << static_cast<uint32_t>(PixelFormatDescriptor.cAuxBuffers) << endl;
        fout << "\tiLayerType      : " << static_cast<uint32_t>(PixelFormatDescriptor.iLayerType) << endl;
        fout << "\tbReserved       : " << static_cast<uint32_t>(PixelFormatDescriptor.bReserved) << endl;
        fout << "\tdwLayerMask     : " << PixelFormatDescriptor.dwLayerMask << endl;
        fout << "\tdwVisibleMask   : " << PixelFormatDescriptor.dwVisibleMask << endl;
        fout << "\tdwDamageMask    : " << PixelFormatDescriptor.dwDamageMask << endl;
    }

    return true;
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
