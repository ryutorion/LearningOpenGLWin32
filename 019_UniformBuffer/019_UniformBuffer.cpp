// 019_UniformBuffer.cpp
#define _USE_MATH_DEFINES
#include <Windows.h>
#include <tchar.h>
#include <iostream>
#include <fstream>
#include <iterator>
#include <sstream>
#include <string>
#include <cstdlib>
#include <memory>
#include <cassert>
#include <cmath>
#include "UsingConsole.h"
#include <gl/GL.h>
#include "glcorearb.h"
#include "wglext.h"
#include "glLoadAddress.h"

#pragma comment(lib, "OpenGL32.lib")

using namespace std;

class Vector3
{
public:
    explicit Vector3(float v) : x(v), y(v), z(v) {}
    Vector3(float vx, float vy, float vz) : x(vx), y(vy), z(vz) {}
    Vector3(const Vector3 & v) : x(v.x), y(v.y), z(v.z) {}

    Vector3 & operator=(const Vector3 & v)
    {
        x = v.x;
        y = v.y;
        z = v.z;

        return *this;
    }

    Vector3 & operator+=(const Vector3 & v)
    {
        x += v.x;
        y += v.y;
        z += v.z;

        return *this;
    }

    const Vector3 operator+(const Vector3 & v) const
    {
        Vector3 result(*this);
        result += v;

        return result;
    }

    Vector3 & operator-=(const Vector3 & v)
    {
        x -= v.x;
        y -= v.y;
        z -= v.z;

        return *this;
    }

    const Vector3 operator-(const Vector3 & v) const
    {
        Vector3 result(*this);
        result -= v;

        return result;
    }

    Vector3 & operator*=(float s)
    {
        x *= s;
        y *= s;
        z *= s;

        return *this;
    }

    const Vector3 operator*(float s) const
    {
        Vector3 result(*this);
        result *= s;

        return result;
    }

    float dot(const Vector3 & v) const
    {
        return x * v.x + y * v.y + z * v.z;
    }

    float length() const
    {
        return sqrt(dot(*this));
    }

    const Vector3 normalize() const
    {
        return *this * (1.0f / length());
    }

    const Vector3 cross(const Vector3 & v) const
    {
        return Vector3(
            y * v.z - z * v.y,
            z * v.x - x * v.z,
            x * v.y - y * v.x
        );
    }

    float x;
    float y;
    float z;
};

class Matrix4x4
{
public:
    Matrix4x4() :
        Matrix4x4(
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 0.0f, 0.0f
        )
    {
    }

    Matrix4x4(
        float m00, float m01, float m02, float m03,
        float m10, float m11, float m12, float m13,
        float m20, float m21, float m22, float m23,
        float m30, float m31, float m32, float m33
    )
    {
        v[0][0] = m00;
        v[0][1] = m01;
        v[0][2] = m02;
        v[0][3] = m03;
        v[1][0] = m10;
        v[1][1] = m11;
        v[1][2] = m12;
        v[1][3] = m13;
        v[2][0] = m20;
        v[2][1] = m21;
        v[2][2] = m22;
        v[2][3] = m23;
        v[3][0] = m30;
        v[3][1] = m31;
        v[3][2] = m32;
        v[3][3] = m33;
    }

    Matrix4x4(const Matrix4x4 & m)
    {
        *this = m;
    }

    Matrix4x4 & operator=(const Matrix4x4 & m)
    {
        v[0][0] = m.v[0][0];
        v[0][1] = m.v[0][1];
        v[0][2] = m.v[0][2];
        v[0][3] = m.v[0][3];
        v[1][0] = m.v[1][0];
        v[1][1] = m.v[1][1];
        v[1][2] = m.v[1][2];
        v[1][3] = m.v[1][3];
        v[2][0] = m.v[2][0];
        v[2][1] = m.v[2][1];
        v[2][2] = m.v[2][2];
        v[2][3] = m.v[2][3];
        v[3][0] = m.v[3][0];
        v[3][1] = m.v[3][1];
        v[3][2] = m.v[3][2];
        v[3][3] = m.v[3][3];

        return *this;
    }

    static const Matrix4x4 Translation(const Vector3 & v)
    {
        return Matrix4x4(
            1.f, 0.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            v.x, v.y, v.z, 1.f
        );
    }

    static const Matrix4x4 Scaling(const Vector3 & v)
    {
        return Matrix4x4(
            v.x, 0.f, 0.f, 0.f,
            0.f, v.y, 0.f, 0.f,
            0.f, 0.f, v.z, 0.f,
            0.f, 0.f, 0.f, 1.f
        );
    }

    static const Matrix4x4 RotationX(float radian)
    {
        float s = sin(radian);
        float c = cos(radian);

        return Matrix4x4(
            1.f, 0.f, 0.f, 0.f,
            0.f,   c,   s, 0.f,
            0.f,  -s,   c, 0.f,
            0.f, 0.f, 0.f, 1.f
        );
    }

    static const Matrix4x4 RotationY(float radian)
    {
        float s = sin(radian);
        float c = cos(radian);

        return Matrix4x4(
              c, 0.f,  -s, 0.f,
            0.f, 1.f, 0.f, 0.f,
              s, 0.f,   c, 0.f,
            0.f, 0.f, 0.f, 1.f
        );
    }

    static const Matrix4x4 RotationZ(float radian)
    {
        float s = sin(radian);
        float c = cos(radian);

        return Matrix4x4(
              c,   s, 0.f, 0.f,
             -s,   c, 0.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 0.f, 1.f
        );
    }

    static const Matrix4x4 LookAtRightHand(const Vector3 & eye, const Vector3 & at, const Vector3 & up)
    {
        const Vector3 z = (eye - at).normalize();
        const Vector3 x = up.cross(z).normalize();
        const Vector3 y = z.cross(x);

        return Matrix4x4(
            x.x, y.x, z.x, 0.f,
            x.y, y.y, z.y, 0.f,
            x.z, y.z, z.z, 0.f,
            -eye.dot(x), -eye.dot(y), -eye.dot(z), 1.f
        );
    }

    static const Matrix4x4 PerspectiveFovRightHand(float fovRadian, float aspect, float nearPlane, float farPlane)
    {
        float xScale = 1.f / tan(fovRadian * 0.5f);
        float yScale = xScale / aspect;
        float zScale = (farPlane + nearPlane) / (farPlane - nearPlane);
        float zTrans = 2 * nearPlane * farPlane / (farPlane - nearPlane);

        return Matrix4x4(
            xScale, 0.f, 0.f, 0.f,
            0.f, yScale, 0.f, 0.f,
            0.f, 0.f, zScale, -1.f,
            0.f, 0.f, zTrans, 0.f
        );
    }

private:
    float v[4][4];
};

struct ModelViewProjection
{
    Matrix4x4 Model;
    Matrix4x4 View;
    Matrix4x4 Projection;
};

LRESULT CALLBACK WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
bool OnCreate(HWND hWnd);
void OnUpdate();
void OnDraw(HWND hWnd);
DWORD PrintSystemError(DWORD error = GetLastError());

GLuint Program = GL_INVALID_VALUE;
GLuint VertexArrayObject = GL_INVALID_VALUE;
GLuint VertexBuffer = GL_INVALID_VALUE;
GLuint IndexBuffer = GL_INVALID_VALUE;
GLuint UniformBuffer = GL_INVALID_VALUE;
GLint  Position = GL_INVALID_VALUE;
GLint  MVP = GL_INVALID_VALUE;
GLint  Model = GL_INVALID_VALUE;
GLint  View = GL_INVALID_VALUE;
GLint  Projection = GL_INVALID_VALUE;

float theta = 0.f;
float phi = 0.f;

Matrix4x4 ModelMatrix;
Matrix4x4 ViewMatrix;
Matrix4x4 ProjectionMatrix;

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
            OnUpdate();
            OnDraw(hWnd);
            // メッセージがなかった場合の処理
            Sleep(1);
        }
    }

    if(Program != GL_INVALID_VALUE)
    {
        glDeleteProgram(Program);
    }

    if(VertexArrayObject != GL_INVALID_VALUE)
    {
        glDeleteVertexArrays(1, &VertexArrayObject);
    }

    if(VertexBuffer != GL_INVALID_VALUE)
    {
        glDeleteBuffers(1, &VertexBuffer);
    }

    if(IndexBuffer != GL_INVALID_VALUE)
    {
        glDeleteBuffers(1, &IndexBuffer);
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

    auto wglCreateContextAttribsARB =
        reinterpret_cast<PFNWGLCREATECONTEXTATTRIBSARBPROC>(
            wglGetProcAddress("wglCreateContextAttribsARB")
        );
    if(wglCreateContextAttribsARB == nullptr)
    {
        PrintSystemError();
        return false;
    }

    const int attributes[] =
    {
        WGL_CONTEXT_MAJOR_VERSION_ARB, MajorVersion,
        WGL_CONTEXT_MINOR_VERSION_ARB, MinorVersion,
        WGL_CONTEXT_PROFILE_MASK_ARB, WGL_CONTEXT_CORE_PROFILE_BIT_ARB,
#ifdef _DEBUG
        WGL_CONTEXT_FLAGS_ARB, WGL_CONTEXT_DEBUG_BIT_ARB,
#endif
        0
    };

    HGLRC hCoreProfileGLRC = wglCreateContextAttribsARB(hDC, nullptr, attributes);
    if(hCoreProfileGLRC == nullptr)
    {
        PrintSystemError();
        return false;
    }

    if(!wglMakeCurrent(hDC, hCoreProfileGLRC))
    {
        wglDeleteContext(hCoreProfileGLRC);
        PrintSystemError();
        return false;
    }
    wglDeleteContext(hGLRC);

    glLoadAddress(MajorVersion, MinorVersion);

    ifstream fvert("..\\Shaders\\019_UniformBuffer.vert");
    string vsrc((istreambuf_iterator<char>(fvert)), istreambuf_iterator<char>());

    const char * vsources[] = { vsrc.c_str() };
    const int vlengths[] = { static_cast<const int>(vsrc.length()) };

    GLuint VertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(VertexShader, 1, vsources, vlengths);
    glCompileShader(VertexShader);

    GLint compiled;
    glGetShaderiv(VertexShader, GL_COMPILE_STATUS, &compiled);
    if(!compiled)
    {
        return false;
    }

    Program = glCreateProgram();
    glAttachShader(Program, VertexShader);
    glLinkProgram(Program);
    GLint linked;
    glGetProgramiv(Program, GL_LINK_STATUS, &linked);
    if(!linked)
    {
        GLint length = 0;
        glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &length);
        unique_ptr<char[]> pLogBuffer(new char[length]);
        glGetProgramInfoLog(Program, length, &length, pLogBuffer.get());

        cout << pLogBuffer.get() << endl;

        return false;
    }

    glUseProgram(Program);
    glDisable(GL_CULL_FACE);

    Position = glGetAttribLocation(Program, "position");
    Model = glGetUniformLocation(Program, "Model");
    View = glGetUniformLocation(Program, "View");
    Projection = glGetUniformLocation(Program, "Projection");
    MVP = glGetUniformBlockIndex(Program, "MVP");

    glGenVertexArrays(1, &VertexArrayObject);
    glGenBuffers(1, &VertexBuffer);
    glGenBuffers(1, &IndexBuffer);
    glGenBuffers(1, &UniformBuffer);

    glBindVertexArray(VertexArrayObject);
    glBindBuffer(GL_ARRAY_BUFFER, VertexBuffer);
    float vertices[] =
    {
        -0.5,  0.5,
         0.5,  0.5,
         0.5, -0.5,
        -0.5, -0.5,
    };
    GLint components = 2;
    
    glBufferData(GL_ARRAY_BUFFER, sizeof vertices, vertices, GL_STATIC_DRAW);
    glEnableVertexAttribArray(Position);
    glVertexAttribPointer(Position, components, GL_FLOAT, false, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexBuffer);
    uint16_t indices[] = 
    {
        0, 1, 2,
        0, 2, 3
    };
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof indices, indices, GL_STATIC_DRAW);

    glBindVertexArray(0);

    return true;
}

void OnUpdate()
{
    Vector3 eye(0.f, 0.f, 3.0f);
    Vector3 at(sin(phi), 0.f, 0.f);
    Vector3 up(0.f, 1.f, 0.f);

    ModelViewProjection MVPMatrices
    {
        Matrix4x4::RotationY(theta),
        Matrix4x4::LookAtRightHand(eye, at, up),
        Matrix4x4::PerspectiveFovRightHand(
            static_cast<float>(M_PI_4),
            static_cast<float>(Width) / Height,
            0.01f,
            10000.0f
        )
    };

    theta += 0.01f;
    phi += 0.02f;

    glBindBuffer(GL_UNIFORM_BUFFER, UniformBuffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof MVPMatrices, &MVPMatrices, GL_DYNAMIC_DRAW);
    glBindBuffer(GL_UNIFORM_BUFFER, 0);
}

void OnDraw(HWND hWnd)
{
    float clearColor[] = { 1.0f, 0.0f, 0.0f, 1.0f };
    glClearBufferfv(GL_COLOR, 0, clearColor);

    glBindVertexArray(VertexArrayObject);
    glBindBuffer(GL_UNIFORM_BUFFER, UniformBuffer);
    glBindBufferBase(GL_UNIFORM_BUFFER, MVP, UniformBuffer);

    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, 0);

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
