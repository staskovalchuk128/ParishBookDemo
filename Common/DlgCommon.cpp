#include "DlgCommon.h"
#include "..//Components/PreloaderComponent.h"
#include <thread>

DlgCommon::DlgCommon(HWND hWnd) : hWnd(hWnd), hInst(NULL), Controls(NULL)
{
	fixedDlgWidth = fixedDlgHeight = 0;
	minDlgWidth = minDlgHeight = 0;
	dlgPaddingX = 10;
	dlgTitle = "Dialog";
}

DlgCommon::~DlgCommon() {}

void DlgCommon::OnDlgResized() {}

void DlgCommon::OnDlgInit() 
{

}

void DlgCommon::OnDlgClose() 
{

}

bool DlgCommon::OnBeforeDlgClose()
{
	return false;
}

bool DlgCommon::OnDlgCommand(WPARAM wParam)
{
	return false;
}

void DlgCommon::DoneLoadingDlg(bool removeLoadingPlaceholder)
{
	SetWindowTextA(hWnd, dlgTitle);

	int extraHeight = GetExtraWindowHeight(hWnd);
	int extraWidth = GetExtraWindowWidth(hWnd);
	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int dlgWidth = 0;
	int dlgHeight = 0;

	if (fixedDlgWidth > 0 && fixedDlgHeight > 0)
	{
		dlgWidth = fixedDlgWidth;
		dlgHeight = fixedDlgHeight;
	}
	else
	{
		dlgWidth = GetTotalParentWidth() + extraWidth + dlgPaddingX;
		dlgHeight = GetTotalParentHeight() + extraHeight + dlgPaddingX;

		dlgWidth = minDlgWidth > 0 && dlgWidth < minDlgWidth ? minDlgWidth : dlgWidth;
		dlgHeight = minDlgHeight > 0 && dlgHeight < minDlgHeight ? minDlgHeight : dlgHeight;

		int top = (screenHeight - dlgHeight) / 2, left = (screenWidth - dlgWidth) / 2;

		SetWindowPos(GetHWND(), NULL, left, top, dlgWidth, dlgHeight, NULL);
	}




	OnResize();

	OnDlgResized();

	if(removeLoadingPlaceholder) PreloaderComponent::Stop();

	UpdateWindow(hWnd);

}

int DlgCommon::OpenDlg(HWND hWnd, const char* title)
{
	dlgTitle = title;
	HINSTANCE hInst = GetWindowInstance(hWnd);
	return static_cast<int> (DialogBoxThis<DlgCommon, &DlgCommon::DlgProc >(this, hInst,
		MAKEINTRESOURCEA(IDD_COMMONDLG), hWnd));

}

template < typename T, INT_PTR(T::* P)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) >
INT_PTR DlgCommon::DialogBoxThis(T* pThis, HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent)
{
	return ::DialogBoxParamA(hInstance, lpTemplateName, hWndParent, [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)->INT_PTR
		{
			if (uMsg == WM_INITDIALOG) SetWindowLongPtr(hWnd, DWLP_USER, lParam);
			T* pThis = reinterpret_cast<T*> (GetWindowLongPtr(hWnd, DWLP_USER));
			return pThis ? (pThis->*P)(hWnd, uMsg, wParam, lParam) : FALSE;
		}, reinterpret_cast<LPARAM> (pThis));
}

INT_PTR CALLBACK DlgCommon::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_SETFOCUS:
	{
		SetFocus(GetNextDlgTabItem(GetParent((HWND)wParam), (HWND)wParam, FALSE));
		return true;
	}
	case WM_ENTERIDLE:
	{

		break;
	}
	case WM_INITDIALOG:
	{
		this->hWnd = hWnd;

		SetWindowTextA(hWnd, "Loading data...");


		int extraHeight = GetExtraWindowHeight(hWnd);
		int extraWidth = GetExtraWindowWidth(hWnd);
		int screenWidth = GetSystemMetrics(SM_CXSCREEN);
		int screenHeight = GetSystemMetrics(SM_CYSCREEN);

		int dlgWidth = 500;
		int dlgHeight = 400;

		if (fixedDlgWidth > 0 && fixedDlgHeight > 0)
		{
			dlgWidth = fixedDlgWidth;
			dlgHeight = fixedDlgHeight;
		}


		int top = (screenHeight - dlgHeight) / 2, left = (screenWidth - dlgWidth) / 2;

		MoveWindow(hWnd, left, top, dlgWidth, dlgHeight, FALSE);

		PreloaderComponent::Start(hWnd);
	
		ReinitControls(hWnd);	// reinit controls
		OnDlgInit();

		return (INT_PTR)TRUE;
	}

	case WM_CTLCOLORDLG:
	{
		return (INT_PTR)GetStockObject(WHITE_BRUSH);
	}
	case WM_DESTROY:
	{
		OnDlgClose();
		break;
	}

	case WM_COMMAND:
	{
		UINT wId = LOWORD(wParam);
		if (wId == IDCANCEL)
		{
			if (OnBeforeDlgClose()) return (INT_PTR)TRUE;

			EndDialog(hWnd, wId);
			return (INT_PTR)TRUE;
		}

		if (OnDlgCommand(wParam)) return (INT_PTR)FALSE;

		break;
	}

	}

	return (INT_PTR)FALSE;
}