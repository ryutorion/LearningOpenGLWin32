// 003_PrintSystemError.cpp
#include <Windows.h>
#include <tchar.h>
#include <iostream>
#include <cstdlib>
#include "UsingConsole.h"

using namespace std;

DWORD PrintSystemError(DWORD error = GetLastError());

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE, LPWSTR lpCmdLine, int nCmdShow)
{
    UsingConsole uc;

    // �G���[���N�������߂̏���
    WNDCLASSEX wcx {};
    if(!RegisterClassEx(&wcx))
    {
        PrintSystemError();
        return 0;
    }

    return 0;
}

DWORD PrintSystemError(DWORD error)
{
    // �G���[�R�[�h�ɑΉ����郁�b�Z�[�W�̎擾
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

        // FormatMessage�Ŋ��蓖�Ă�ꂽ�������̉��
        LocalFree(pBuffer);
    }

    return error;
}