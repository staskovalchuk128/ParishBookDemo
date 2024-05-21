#include "MessageDlg.h"
#include <string.h>

MessageDlg::MessageDlg(HWND hWnd, const wchar_t* title, const wchar_t* content, int btnsType, int dlgType) : Controls(NULL)
{
	textBoxStr = NULL;
	promptStr = L"";
	scroll = NULL;
	dlgHwnd = NULL;
	hParent = hWnd;

	dlgFont = CreateFont(18, 0, 0, 0, FW_BOLD, FALSE, FALSE, FALSE, ANSI_CHARSET,
		OUT_TT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, L"Verdana");

	this->title = title;
	this->content = content;
	this->btnsType = btnsType;
	this->dlgType = dlgType;

	iconWidth = 60;
	bodyWidth = 400;
	bodyHeight = 0;
	dialogWidth = 500;	// can be changed

	createSaveBtn = false;
}

int MessageDlg::OpenDlg(bool btnSave)
{
	HINSTANCE hInst = GetWindowInstance(hParent);

	createSaveBtn = btnSave;
	return static_cast<int> (DialogBoxThis<MessageDlg, &MessageDlg::DlgProc >(this, hInst,
		MAKEINTRESOURCEA(IDD_MSGDLG), hParent));
}

void MessageDlg::OnDlgInit()
{
	SetWindowText(dlgHwnd, title);
	ReinitControls(dlgHwnd);

	CreateDlgBody();

	if (btnsType == MD_OK)
	{
		CreateBtn("btnOk", 10, 10, 120, 30, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"OK", WS_STICK_RIGHT | WS_STICK_BOTTOM);
		SetFocus(GetControlHWND("btnOk"));
	}
	else if (btnsType == MD_OKCALNCEL)
	{
		CreateBtn("btnCancel", 10, 10, 120, 30, WS_CHILD | WS_VISIBLE, L"Cancel", WS_STICK_RIGHT | WS_STICK_BOTTOM);
		SetColor("btnCancel", APP_COLORS::GRAY);

		CreateBtn("btnOk", 150, 10, 120, 30, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"OK", WS_STICK_RIGHT | WS_STICK_BOTTOM);
		SetFocus(GetControlHWND("btnOk"));
	}
	else if (btnsType == MD_YESNO)
	{
		CreateBtn("btnNo", 10, 10, 120, 30, WS_CHILD | WS_VISIBLE, L"No", WS_STICK_RIGHT | WS_STICK_BOTTOM);
		SetColor("btnNo", APP_COLORS::GRAY);
		CreateBtn("btnYes", 150, 10, 120, 30, WS_CHILD | WS_VISIBLE | WS_TABSTOP, L"Yes", WS_STICK_RIGHT | WS_STICK_BOTTOM);
		SetFocus(GetControlHWND("btnYes"));
	}

	if (createSaveBtn)
	{
		CreateBtn("btnSave", 10, 10, 170, 30, WS_CHILD | WS_VISIBLE, L"Save This Message", WS_STICK_BOTTOM);
		SetColor("btnSave", APP_COLORS::GRAY);
	}

	ResizeDialog(bodyHeight);

	if (dlgType == MD_ERR)
	{
		MessageBeep(MB_ICONERROR);
	}
	else
	{
		MessageBeep(0);
	}
}

bool MessageDlg::SaveMessage()
{
	wchar_t szFile[_MAX_PATH] = L"Warning Message";
	const wchar_t szExt[] = L"txt\0";

	OPENFILENAME ofn = { sizeof ofn
	};

	ofn.lpstrFile = szFile;
	ofn.nMaxFile = sizeof(szFile) / sizeof(szFile[0]);
	ofn.lpstrFilter = ofn.lpstrDefExt = szExt;
	ofn.nFilterIndex = 1;
	ofn.Flags = OFN_SHOWHELP | OFN_OVERWRITEPROMPT;
	if (GetSaveFileName(&ofn) == TRUE)
	{
		HANDLE source = CreateFile(ofn.lpstrFile, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);

		if (source == INVALID_HANDLE_VALUE)
		{
			MessageBox(NULL, L"Source file not opened.", L"Error", MB_OK);
			CloseHandle(source);
			return false;
		}

		DWORD bytesWritten = 0;
		WriteFile(source, content, static_cast<DWORD> (wcslen(content)), &bytesWritten, NULL);
		CloseHandle(source);
	}

	return true;
}

bool MessageDlg::CheckPrompt()
{
	std::wstring val = GetWindowTextStr(GetControlHWND("promptEdit"));
	if (WstrToLow(val) != WstrToLow(std::wstring(promptStr)))
	{
		MessageDlg(dlgHwnd, L"Error",
			L"You entered incorrect string, try agan.",
			MD_OK, MD_ERR).OpenDlg();
		SetFocus(GetControlHWND("promptEdit"));
		return false;
	}

	EndDialog(dlgHwnd, IDOK);
	return true;
}

void MessageDlg::ExtractTextBoxStr()
{
	textBoxStr->clear();

	std::wstring val = GetWindowTextStr(GetControlHWND("textBoxMsgDlg"));
	std::for_each(val.begin(), val.end(), [&](wchar_t c)
		{
			textBoxStr->push_back(c);
		}
	);
	EndDialog(dlgHwnd, IDOK);
}

void MessageDlg::CreateDlgBody()
{
	int bodyMarginY = dlgType == MD_PROMPT || dlgType == MD_TEXTBOX ? -100 : -50;

	LPSIZE textSize = 0;
	HWND bodyIcon = CreateStatic("bodyIcon", 10, 10, iconWidth, bodyMarginY, WS_CHILD | WS_VISIBLE);
	HWND body = CreateStatic("body", 80, 10, bodyWidth, bodyMarginY, WS_CHILD | WS_VISIBLE, content);

	SetWindowSubclass(bodyIcon, MainProc, GetControlId("bodyIcon"), (DWORD_PTR)this);
	SetWindowSubclass(body, MainProc, GetControlId("body"), (DWORD_PTR)this);
	if (dlgType == MD_PROMPT)
	{
		HWND editItem = CreateEdit("promptEdit", 10, 60, -10, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, L"", WS_STICK_BOTTOM);
		SetWindowSubclass(editItem, MainProc, GetControlId("promptEdit"), (DWORD_PTR)this);
		SetFocus(editItem);
	}
	else if (dlgType == MD_TEXTBOX)
	{
		HWND editItem = CreateEdit("textBoxMsgDlg", 10, 60, -10, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, textBoxStr->c_str(), WS_STICK_BOTTOM);
		SetWindowSubclass(editItem, MainProc, GetControlId("textBoxMsgDlg"), (DWORD_PTR)this);
		SetFocus(editItem);
	}

	CalculateBodySize();

	scroll = new ScrollBar(GetControlHWND("body"), NULL);
	scroll->CreateScroll(21, -1);
	scroll->SetVScrollRange(bodyHeight);

	InvalidateRect(GetHWND(), NULL, TRUE);
	UpdateWindow(GetHWND());
}

void MessageDlg::CalculateBodySize()
{
	HWND hItem = GetControlHWND("body");

	int width = bodyWidth;

	HDC hdc = GetDC(hItem);
	Graphics gr(hdc);

	std::wstring formatedStr = BreakStringToLines(gr, content, *currentFont, width);

	RectF layoutRect(0, 0, static_cast<REAL> (width), -1);
	RectF boundRect;

	gr.MeasureString(formatedStr.c_str(), static_cast<INT> (formatedStr.length()), currentFont, layoutRect, &boundRect);

	bodyHeight = static_cast<int> (boundRect.Height < MIN_BODY_HEIGHT ? MIN_BODY_HEIGHT : boundRect.Height);

}

void MessageDlg::ResizeDialog(int dlgHeight)
{
	RECT rc;

	GetWindowRect(GetHWND(), &rc);

	int top = rc.top, left = rc.left;

	int screenWidth = GetSystemMetrics(SM_CXSCREEN);
	int screenHeight = GetSystemMetrics(SM_CYSCREEN);

	int maxHeight = screenHeight / 2;

	if (dlgHeight < MIN_DLG_HEIGHT)
	{
		dlgHeight = MIN_DLG_HEIGHT;
	}
	else if (dlgHeight > maxHeight)
	{
		dlgHeight = maxHeight;
	}

	top = screenHeight / 2 - dlgHeight / 2;

	int extraHeight = GetExtraWindowHeight(GetHWND());
	int extraWidth = GetExtraWindowWidth(GetHWND());

	if (dlgType == MD_PROMPT || dlgType == MD_TEXTBOX)
	{
		dlgHeight += 60;
	}

	SetWindowPos(dlgHwnd, 0, left, top, bodyWidth + iconWidth + extraWidth + 30, dlgHeight, NULL);
	OnResize();
}

void MessageDlg::DrawBody(HDC& hdcMem)
{
	int bodyOffsetY = 0;

	RECT rect, parentRect;
	HWND hItem = GetControlHWND("body");
	SolidBrush whiteBrush(GetColor(APP_COLORS::WHITE));
	SolidBrush blackBrush(GetColor(APP_COLORS::BLACK));
	GetWindowRect(hItem, &rect);
	GetWindowRect(GetParent(hItem), &parentRect);

	int sx = scroll->GetScrollBarPos(SB_VERT);

	int top = sx * -1;

	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;
	int parentHeight = parentRect.bottom - parentRect.top;

	Graphics gr(hdcMem);

	gr.FillRectangle(&whiteBrush, 0, 0, width, height);

	std::wstring formatedStr = BreakStringToLines(gr, content, *currentFont, width);

	RectF layoutRect(0, 0, static_cast<REAL> (width), -1);
	RectF boundRect;

	gr.MeasureString(formatedStr.c_str(), (INT)formatedStr.length(), currentFont, layoutRect, &boundRect);

	int contentHeight = static_cast<int> (boundRect.Height < MIN_BODY_HEIGHT ? MIN_BODY_HEIGHT : boundRect.Height);

	int totalItemHeight = (parentHeight - height) + contentHeight + bodyOffsetY;

	if (boundRect.Height < MIN_BODY_HEIGHT)
	{
		// center string
		bodyOffsetY = static_cast<int> (height / 2 - boundRect.Height / 2);
	}

	PointF origin(0, static_cast<REAL> (bodyOffsetY + top));
	gr.DrawString(formatedStr.c_str(), (INT)formatedStr.length(), currentFont, origin, &blackBrush);

	InvalidateRect(GetControlHWND("bodyIcon"), NULL, FALSE);
}

void MessageDlg::DrawBodyIcon(HDC& hdcMem)
{
	RECT rect;
	HWND hItem = GetControlHWND("bodyIcon");
	SolidBrush whiteBrush(GetColor(APP_COLORS::WHITE));
	GetWindowRect(hItem, &rect);

	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	Graphics gr(hdcMem);

	gr.FillRectangle(&whiteBrush, 0, 0, width, height);

	Bitmap* pBmp = NULL;

	if (dlgType == MD_ERR)
	{
		pBmp = LoadImageFromResource(NULL, MAKEINTRESOURCEA(IDB_ERR), "PNG");
	}
	else if (dlgType == MD_QUESTION)
	{
		pBmp = LoadImageFromResource(NULL, MAKEINTRESOURCEA(IDB_QUESTION), "PNG");
	}
	else if (dlgType == MD_WANING || dlgType == MD_PROMPT)
	{
		pBmp = LoadImageFromResource(NULL, MAKEINTRESOURCEA(IDB_WARN), "PNG");
	}
	else
	{
		pBmp = LoadImageFromResource(NULL, MAKEINTRESOURCEA(IDB_SUCCESS), "PNG");
	}

	UINT imgHeihgt = pBmp->GetHeight();
	gr.SetSmoothingMode(SmoothingModeHighQuality);
	gr.DrawImage(pBmp, 0, height / 2 - imgHeihgt / 2, 50, 50);

	delete pBmp;
}

void MessageDlg::SetPromptStr(const wchar_t* str)
{
	promptStr = str;
}
void MessageDlg::SetTextBoxStr(std::wstring& str)
{
	textBoxStr = &str;
}

template < typename T, INT_PTR(T::* P)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam) >
INT_PTR MessageDlg::DialogBoxThis(T* pThis, HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent)
{
	return DialogBoxParamA(hInstance, lpTemplateName, hWndParent, [](HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)->INT_PTR
		{
			if (uMsg == WM_INITDIALOG) SetWindowLongPtr(hWnd, DWLP_USER, lParam);
			T* pThis = reinterpret_cast<T*> (GetWindowLongPtr(hWnd, DWLP_USER));
			return pThis ? (pThis->*P)(hWnd, uMsg, wParam, lParam) : FALSE;
		}, reinterpret_cast<LPARAM> (pThis));
}

INT_PTR CALLBACK MessageDlg::DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	HDC hdc = (HDC)wParam;

	switch (uMsg)
	{
	case WM_CTLCOLORDLG:
	{
		return (INT_PTR)GetStockObject(WHITE_BRUSH);
	}

	case WM_INITDIALOG:
	{
		dlgHwnd = hWnd;
		OnDlgInit();
		SetWindowFont(dlgHwnd, dlgFont, true);
		return (INT_PTR)TRUE;
	}

	case WM_COMMAND:
	{
		UINT wId = LOWORD(wParam);
		if (wId == IDCANCEL)
		{
			EndDialog(hWnd, wId);
			return (INT_PTR)TRUE;
		}

		if (HIWORD(wParam) == BN_CLICKED)
		{
			if (wId == GetControlId("btnOk"))
			{
				if (dlgType == MD_PROMPT)
				{
					CheckPrompt();
					return (INT_PTR)TRUE;
				}
				else if (dlgType == MD_TEXTBOX)
				{
					ExtractTextBoxStr();
					return (INT_PTR)TRUE;
				}

				wId = IDOK;
			}
			else if (wId == IDOK)
			{
				if (dlgType == MD_PROMPT)
				{
					CheckPrompt();
					return (INT_PTR)TRUE;
				}
				else if (dlgType == MD_TEXTBOX)
				{
					ExtractTextBoxStr();
					return (INT_PTR)TRUE;
				}
			}
			else if (wId == GetControlId("btnCancel")) wId = IDCANCEL;
			else if (wId == GetControlId("btnYes"))
			{
				if (dlgType == MD_PROMPT)
				{
					CheckPrompt();
					return (INT_PTR)TRUE;
				}
				else if (dlgType == MD_TEXTBOX)
				{
					ExtractTextBoxStr();
					return (INT_PTR)TRUE;
				}

				wId = IDYES;
			}
			else if (wId == GetControlId("btnNo")) wId = IDNO;
			else if (wId == GetControlId("btnSave"))
			{
				SaveMessage();
				return (INT_PTR)TRUE;
			}

			EndDialog(hWnd, wId);

			return (INT_PTR)TRUE;
		}

		break;
	}
	}

	return (INT_PTR)FALSE;
}

LRESULT CALLBACK MessageDlg::MainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	MessageDlg* lpData = (MessageDlg*)dwRefData;
	RECT rect;

	switch (uMsg)
	{
	case WM_KEYDOWN:
	{
		if (wParam == VK_RETURN)
		{
			if (uIdSubclass == lpData->GetControlId("promptEdit"))
			{
				lpData->CheckPrompt();
				return TRUE;
			}
			else if (uIdSubclass == lpData->GetControlId("textBoxMsgDlg"))
			{
				lpData->ExtractTextBoxStr();
				return TRUE;
			}
		}

		break;
	}

	case WM_PAINT:
	{
		// Only for body
		if (uIdSubclass != lpData->GetControlId("bodyIcon") && uIdSubclass != lpData->GetControlId("body")) break;

		GetWindowRect(hWnd, &rect);

		PAINTSTRUCT ps;

		HDC hdc = BeginPaint(hWnd, &ps);

		HDC hdcMem = CreateCompatibleDC(hdc);

		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		HBITMAP bitmap = CreateCompatibleBitmap(hdc, width, height);
		HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, bitmap);

		if (uIdSubclass == lpData->GetControlId("bodyIcon"))
		{
			lpData->DrawBodyIcon(hdcMem);
		}
		else if (uIdSubclass == lpData->GetControlId("body"))
		{
			lpData->DrawBody(hdcMem);
		}

		BitBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);

		SelectObject(hdcMem, oldBitmap);
		DeleteObject(bitmap);
		DeleteDC(hdcMem);

		EndPaint(hWnd, &ps);

		break;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}