#include "MainWindow.h"
#include <commctrl.h>
#include <windowsx.h>
#include "Drawing/WindowDrawer.h"

#include "PageIds.hpp"

#include "Windows/AppWindowCommon.h"
#include "Windows/Core/CoreWindow.h"
#include "Windows/Auth/AuthWindow.h"

#include "PBConnection.h"

#include "Components/PreloaderComponent.h"

MainWindow::MainWindow()
{
	hWnd = NULL;
	hInst = NULL;
	borderWidth = 5;

	RECT xy;
	SystemParametersInfo(SPI_GETWORKAREA, 0, &xy, 0);

	int x = xy.right - xy.left;
	int y = xy.bottom - xy.top;

	int iWidth = 500;
	int iHeight = 400;

	RECT WindowRect;
	WindowRect.top = (y - iHeight) / 2;
	WindowRect.left = (x - iWidth) / 2;
	WindowRect.right = WindowRect.left + iWidth;
	WindowRect.bottom = WindowRect.top + iHeight;

	DWORD dwExStyle = 0;
	DWORD dwStyle = 0;

	hInst = GetModuleHandle(NULL);

	LoadString(hInst, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
	LoadString(hInst, IDC_PARISHBOOK, szWindowClass, MAX_LOADSTRING);

	RegisterWindowClass();

	AdjustWindowRectEx(&WindowRect, dwStyle, FALSE, dwExStyle);

	hWnd = CreateWindowExW(dwExStyle, szWindowClass,
		szTitle,
		WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
		WindowRect.left, WindowRect.top, WindowRect.right - WindowRect.left,
		WindowRect.bottom - WindowRect.top,
		NULL, NULL,
		hInst,
		this);

	updater.ReplaceTempVersion();

	if (hWnd == NULL)
	{
		throw std::wstring(L"Cannot create the main window: " + GetLastErrorAsString());
	}

}


MainWindow::~MainWindow()
{
}

void MainWindow::OpenWindow(AppWindowTypes winType)
{
	if (currentWindow)
	{
		delete currentWindow;
		currentWindow = NULL;
	}

	currentWindowType = winType;

	switch (winType)
	{
	case AppWindowTypes::CORE:
		currentWindow = new CoreWindow(hWnd, hInst);
		break;
	case AppWindowTypes::AUTH:
		currentWindow = new AuthWindow(hWnd, hInst);
		break;
	}

	if (currentWindow)
	{
		currentWindow->OnCreated();
		currentWindow->OnWindowLoaded();
	}
}


LRESULT MainWindow::OnEvent(HWND Handle, UINT Message, WPARAM wParam, LPARAM lParam)
{
	if (Message == WM_NCCREATE)
	{
		CREATESTRUCT* pCreateStruct = reinterpret_cast<CREATESTRUCT*> (lParam);

		SetWindowLongPtr(Handle, GWLP_USERDATA,
			(LONG_PTR)(pCreateStruct->lpCreateParams));
	}

	MainWindow* pWindow = reinterpret_cast<MainWindow*> (GetWindowLongPtr(Handle, GWLP_USERDATA));

	if (pWindow)
	{
		LRESULT res = pWindow->ProcessEvent(Handle, Message, wParam, lParam);
		if (res) return res;
	}

	return 0;
}

LRESULT MainWindow::ProcessEvent(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	
	switch (message)
	{
	case MM_REQUEST_RESPONSE:
	{

		PBRequest* request = (PBRequest*)wParam;
		if (request->aborted) break;

		if (!request->response.Succeed())
		{
			MessageDlg(hWnd, L"Server error",
				request->response.GetError().c_str(),
				MD_OK, MD_ERR).OpenDlg();
			PreloaderComponent::Stop();
			request->NotifyRequestFailed();
			break;
		}

		(request->callback)(request->response);
		break;
	}
	case WM_TIMER:
	{
		KillTimer(hWnd, IDC_STATIC);
		break;
	}
	case WM_GETMINMAXINFO:
	{
		if (currentWindow)
		{
			LPMINMAXINFO lpMMI = (LPMINMAXINFO)lParam;

			currentWindow->OnGetMinMaxInfo(lpMMI);
		}

		break;
	}

	case WM_PAINT:
	{
		if(currentWindow) currentWindow->OnPaint();
		return TRUE;
	}


	case WM_ERASEBKGND:
	{
		return TRUE;
	}

	case WM_SIZE:
	{
		std::vector<Controls*> allControls = Controls::GetAllControls();

		for_each(allControls.begin(), allControls.end(), [](Controls* con)
			{
				con->OnResize();
				RedrawWindow(con->GetHWND(), NULL, NULL, RDW_ALLCHILDREN | RDW_INVALIDATE);
			});

		if (wParam == SIZE_MAXIMIZED) {}

		break;
	}

	case WM_NCACTIVATE:
		return TRUE;
	case WM_NCCALCSIZE:
	{
		WINDOWPLACEMENT place;
		memset(&place, 0, sizeof(WINDOWPLACEMENT));
		place.length = sizeof(WINDOWPLACEMENT);
		GetWindowPlacement(hWnd, &place);
		LPNCCALCSIZE_PARAMS pncc = (LPNCCALCSIZE_PARAMS)lParam;

		if (place.showCmd == SW_SHOWMAXIMIZED)
		{
			//pncc->rgrc[0] is the new rectangle
			//pncc->rgrc[1] is the old rectangle
			//pncc->rgrc[2] is the client rectangle
			pncc->rgrc[0].right -= pncc->rgrc[0].left * -1;
			pncc->rgrc[0].bottom -= pncc->rgrc[0].top * -1;
			pncc->rgrc[0].top = 0;
			pncc->rgrc[0].left = 0;
		}

		return 0;
	}
	case WM_NCHITTEST:
	{
		if (currentWindow) return currentWindow->OnNChitTest(lParam);
		return false;
	}
	case WM_DESTROY:
	{
		PostQuitMessage(0);
		break;
	}
	default:
		return DefWindowProc(hWnd, message, wParam, lParam);
	}

	return 0;
}

void MainWindow::RegisterWindowClass()
{
	WNDCLASSW WindowClass;
	WindowClass.style = 0;
	WindowClass.lpfnWndProc = &MainWindow::OnEvent;
	WindowClass.cbClsExtra = 0;
	WindowClass.cbWndExtra = 0;
	WindowClass.hInstance = hInst;
	WindowClass.hIcon = LoadIcon(hInst, MAKEINTRESOURCE(IDI_LOGO));
	WindowClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	WindowClass.hbrBackground = NULL;
	WindowClass.lpszMenuName = NULL;
	WindowClass.lpszClassName = (LPWSTR)szWindowClass;

	RegisterClassW(&WindowClass);
}