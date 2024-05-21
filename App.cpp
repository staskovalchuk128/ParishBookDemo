#include "App.h"
#include <string>
#include <gdiplus.h>
#include <thread>

#include "Common\CFunctions.hpp"
#include "PBConnection.h"

App* App::selfPtr = NULL;

App::App()
{
    selfPtr = this;
}

App::~App()
{
    delete mainWindow;
}


void App::Run()
{
    UpdateTimeZone();

    shouldStopActiveWindow = false;
    shouldOpenAnotherWindow = false;

    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    SetProcessDPIAware();

    PBConnection::SetMainThreadId(GetCurrentThreadId());

    mainWindow = new MainWindow();
    PBConnection::SetMainHWND(mainWindow->GetHwnd());

    std::thread t([&]()
        {
            PBConnection connection;
            connection.ConnectToServer([&](bool connected, std::wstring err)
                {
                    if (!connected)
                    {
                        MessageBox(NULL, err.c_str(), L"Connection error", MB_OK);
                    }
                }
            );
        }
    );
    t.detach();
   
    
  
    MSG msg;

    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (msg.message == MM_ON_SERVER_CONNECTED)
        {
            mainWindow->OpenWindow(AppWindowTypes::AUTH);
            RunWindow();
            break;
        }
    }
  


    Gdiplus::GdiplusShutdown(gdiplusToken);

}

void App::OpenWindow(AppWindowTypes type)
{
    if (!selfPtr) return;
    selfPtr->Open(type);
}

void App::Open(AppWindowTypes type)
{
    mainWindow->OpenWindow(type);
}

void App::RunWindow()
{
    MSG msg;
    
    while (GetMessage(&msg, nullptr, 0, 0))
    {
        if (!IsDialogMessage(mainWindow->GetHwnd(), &msg))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

    }

}
