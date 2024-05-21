#include <windows.h>
#include <string>

//#include <vld.h> // for leak detections

#include "App.h"

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
    _In_opt_ HINSTANCE hPrevInstance,
    _In_ LPWSTR    lpCmdLine,
    _In_ int       nCmdShow)
{
    try
    {
        App app;
        app.Run();
    }
    catch (std::wstring& e)
    {
        MessageBox(NULL, e.c_str(), L"Error", MB_OK | MB_ICONEXCLAMATION);
    }

    return 0;
}