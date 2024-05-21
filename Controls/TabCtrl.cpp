#include "TabCtrl.h"

TabControl::TabControl(HWND hwnd, Tabs tabs, int activeTabId, DWORD addsStyles, int x, int y, int width, int height, bool createBody, DWORD exStyles) : Common(hwnd, NULL), Controls(hwnd)
{
	bgColor = -1;
	tabsType = TAB_TYPES::DEFAULT;

	this->hWnd = hwnd;
	this->hInst = hInst;

	this->x = x;
	this->y = y;
	this->w = width;
	this->h = height;
	this->createBody = createBody;

	//TCS_OWNERDRAWFIXED
	//hTabs = CreateCustom("tab", WC_TABCONTROL, x, y, width, height, WS_CHILD | WS_VISIBLE | addsStyles, L"", 0, stickRight, stickBottom);
	hTabs = CreateCustom("tab", WC_TABCONTROL, x, y, width, height, WS_CHILD | WS_VISIBLE | addsStyles, L"", exStyles);

	SetWindowSubclass(hTabs, tcProc, GetControlId("tab"), (DWORD_PTR)this);
	SetWindowSubclass(hWnd, parentProc, IDC_STATIC, (DWORD_PTR)this);

	AddTabs(tabs, activeTabId);

	if (createBody == true)
	{
		int bodyX = x, bodyY = y + 29;
		CreateBodyCtrl(bodyX, bodyY);
	}
}

TabControl::~TabControl()
{
	RemoveWindowSubclass(hWnd, parentProc, IDC_STATIC);
	RemoveWindowSubclass(hTabs, tcProc, GetControlId("tab"));
}

void TabControl::AddTabs(Tabs tabs, int activeTabId)
{
	int index = 0, activeItem = 0;
	for (auto& it : tabs.tabs)
	{
		InsertItem(it.second.name, it.first, 0, NULL, it.second.id);
		if (activeTabId == it.second.id) activeItem = index;
		index++;
	}

	TabCtrl_SetCurSel(hTabs, activeItem);
}

HWND TabControl::GetTabsHWND()
{
	return hTabs;
}

HWND TabControl::GetBodyHWND()
{
	return hBody;
}

void TabControl::SetBgColor(int colorId)
{
	bgColor = colorId;
}

void TabControl::SetTabsType(int type)
{
	tabsType = type;
}

void TabControl::CreateBodyCtrl(int x, int y)
{
	hBody = CreateStatic("tabBody", x, y, -10, -10, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN | SS_OWNERDRAW);
}

void TabControl::StartCommonControls(DWORD flags)
{
	INITCOMMONCONTROLSEX iccx;
	iccx.dwSize = sizeof(INITCOMMONCONTROLSEX);
	iccx.dwICC = flags;
	InitCommonControlsEx(&iccx);

}

void TabControl::SetFixedSize(int w, int h)
{
	SendMessage(hTabs, TCM_SETITEMSIZE, 0, MAKELPARAM(w, h));
}

int TabControl::InsertItem(std::wstring txt, int item_index, int image_index, UINT mask, LPARAM lParam)
{
	std::vector<wchar_t> tmp(txt.begin(), txt.end());
	tmp.push_back(('\0'));

	TCITEM tabPage = { 0 };

	tabPage.mask = TCIF_TEXT | TCIF_IMAGE | TCIF_PARAM;
	tabPage.pszText = &tmp[0];
	tabPage.cchTextMax = static_cast<int> (txt.length());
	tabPage.iImage = 1;
	tabPage.lParam = lParam;

	if (TabCtrl_InsertItem(hTabs, item_index, &tabPage) == -1)
	{
		DestroyWindow(hTabs);
		return NULL;
	}

	return 0;
}

int TabControl::GetTabIndex(int id)
{
	int index = -1;
	int countItems = TabCtrl_GetItemCount(hTabs);

	for (int i = 0; i < countItems; i++)
	{
		TCITEM tabPage = { 0 };

		tabPage.mask = TCIF_PARAM;

		TabCtrl_GetItem(hTabs, i, &tabPage);
		if (tabPage.lParam == id)
		{
			index = i;
			break;
		}
	}

	return index;
}

int TabControl::SelectTab(int id)
{
	int index = GetTabIndex(id);
	TabCtrl_SetCurSel(hTabs, index);
	return 0;
}

void TabControl::DeleteAllTabs()
{
	if (hTabs != NULL)
	{
		TabCtrl_DeleteAllItems(hTabs);
	}
}

int TabControl::DeleteItem(int id)
{
	int index = GetTabIndex(id);

	if (TabCtrl_DeleteItem(hTabs, index) == -1)
	{
		DestroyWindow(hTabs);
		return NULL;
	}

	return 0;
}

void TabControl::DrawTableBtnsTabs(HDC& hdc)
{
	int borderSize = 1;

	RECT rc;
	RECT tabRc;

	SolidBrush gbBrush(GetColor(APP_COLORS::WHITE));
	SolidBrush blueBrush(GetColor(APP_COLORS::BLUE));
	SolidBrush whiteBrush(GetColor(APP_COLORS::WHITE));
	SolidBrush blackBrush(GetColor(APP_COLORS::BLACK));
	SolidBrush grayBrush(GetColor(APP_COLORS::GRAY_FONT));
	SolidBrush unactiveColor(Color(255, 246, 246, 246));
	Pen borderColor(GetColor(APP_COLORS::GRAY_BORDER));
	Pen blueBorderColor(GetColor(APP_COLORS::DARK_BLUE));

	GetWindowRect(hTabs, &rc);

	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	Rect frameRect = { 0, 0, width - borderSize, height - borderSize
	};

	int nTab = TabCtrl_GetItemCount(hTabs);
	int nSel = TabCtrl_GetCurSel(hTabs);

	HDC hdcBuffer = CreateCompatibleDC(hdc);
	HBITMAP hBitmapBuffer = CreateCompatibleBitmap(hdc, width, height);
	HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcBuffer, hBitmapBuffer);

	Graphics gr(hdcBuffer);

	gr.FillRectangle(&gbBrush, 0, 0, width, height);
	gr.DrawRectangle(&borderColor, frameRect);

	while (nTab--)
	{
		bool selected = nTab == nSel ? true : false;

		wchar_t buff[100] = { 0 };

		TCITEM tie = { 0 };

		tie.mask = TCIF_TEXT;
		tie.cchTextMax = 100;
		tie.pszText = buff;
		TabCtrl_GetItem(hTabs, nTab, &tie);	// get tab text
		TabCtrl_GetItemRect(hTabs, nTab, &tabRc);	// get tab size

		int tabWidth = tabRc.right - tabRc.left;
		int tabHeight = tabRc.bottom - tabRc.top;

		tabRc.left -= 2;
		tabRc.top += 1;

		if (selected == true)
		{
			gr.FillRectangle(&blueBrush, tabRc.left, 0, tabWidth, tabHeight);
			gr.DrawRectangle(&blueBorderColor, tabRc.left, 0, tabWidth, tabHeight - borderSize);
		}
		else
		{
			gr.FillRectangle(&unactiveColor, tabRc.left, 0, tabWidth, tabHeight);
			gr.DrawRectangle(&borderColor, tabRc.left, 0, tabWidth, tabHeight - borderSize);
		}

		RectF layoutRect(0, 0, static_cast<REAL> (rc.right), static_cast<REAL> (rc.bottom));
		RectF boundRect;

		gr.MeasureString(buff, (INT)wcslen(buff), currentFont, layoutRect, &boundRect);

		PointF origin(tabRc.left + 1 + tabWidth / 2 - boundRect.Width / 2, tabHeight / 2 - boundRect.Height / 2);

		if (selected == true)
		{
			gr.DrawString(buff, (INT)wcslen(buff), currentFont, origin, &whiteBrush);
		}
		else
		{
			gr.DrawString(buff, (INT)wcslen(buff), currentFont, origin, &grayBrush);
		}
	}

	BitBlt(hdc, 0, 0, width, height, hdcBuffer, 0, 0, SRCCOPY);

	SelectObject(hdcBuffer, oldBitmap);
	DeleteObject(hBitmapBuffer);
	DeleteDC(hdcBuffer);
}

void TabControl::DrawTabControl(HDC& hdc)
{
	int borderSize = 1;
	RECT rc;
	RECT tabRc;

	SolidBrush blueBrush(GetColor(APP_COLORS::BLUE));
	SolidBrush whiteBrush(GetColor(APP_COLORS::WHITE));
	SolidBrush blackBrush(GetColor(APP_COLORS::BLACK));
	SolidBrush grayBrush(GetColor(APP_COLORS::GRAY_FONT));
	Pen borderColor(GetColor(APP_COLORS::GRAY_BORDER));
	SolidBrush unactiveColor(Color(255, 246, 246, 246));

	Color backgroundColor;

	if (bgColor != -1)
	{
		backgroundColor = GetColor(bgColor);
	}
	else
	{
		backgroundColor = Color(255, 239, 236, 254);
	}

	SolidBrush gbBrush(backgroundColor);

	GetWindowRect(hTabs, &rc);

	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	int nTab = TabCtrl_GetItemCount(hTabs);
	int nSel = TabCtrl_GetCurSel(hTabs);

	HDC hdcBuffer = CreateCompatibleDC(hdc);
	HBITMAP hBitmapBuffer = CreateCompatibleBitmap(hdc, width, height);
	HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcBuffer, hBitmapBuffer);

	Graphics gr(hdcBuffer);

	gr.FillRectangle(&gbBrush, 0, 0, width, height);
	Point linePoint1(0, height - borderSize), linePoint2(width, height - borderSize);
	gr.DrawLine(&borderColor, linePoint1, linePoint2);

	while (nTab--)
	{
		bool selected = nTab == nSel ? true : false;

		wchar_t buff[100] = { 0 };

		TCITEM tie = { 0 };

		tie.mask = TCIF_TEXT;
		tie.cchTextMax = 100;
		tie.pszText = buff;
		TabCtrl_GetItem(hTabs, nTab, &tie);	// get tab text
		TabCtrl_GetItemRect(hTabs, nTab, &tabRc);	// get tab size

		tabRc.left -= 2;
		tabRc.top -= 2;

		int tabWidth = tabRc.right - tabRc.left;
		int tabHeight = tabRc.bottom - tabRc.top;

		if (selected == true)
		{
			gr.FillRectangle(&whiteBrush, tabRc.left, tabRc.top, tabWidth, tabHeight);
			gr.DrawRectangle(&borderColor, tabRc.left, tabRc.top, tabWidth, tabHeight);
			gr.FillRectangle(&blueBrush, tabRc.left, tabRc.top, tabWidth, 2);
		}
		else
		{
			gr.FillRectangle(&unactiveColor, tabRc.left, tabRc.top, tabWidth, tabHeight);
			gr.DrawRectangle(&borderColor, tabRc.left, tabRc.top, tabWidth, tabHeight - 3);
		}

		RectF layoutRect(0, 0, static_cast<REAL> (rc.right), static_cast<REAL> (rc.bottom));
		RectF boundRect;

		gr.MeasureString(buff, (INT)wcslen(buff), currentFont, layoutRect, &boundRect);

		PointF origin(tabRc.left + tabWidth / 2 - boundRect.Width / 2, tabRc.top + tabHeight / 2 - boundRect.Height / 2);

		if (selected == true)
		{
			gr.DrawString(buff, (INT)wcslen(buff), currentFont, origin, &blackBrush);
		}
		else
		{
			gr.DrawString(buff, (INT)wcslen(buff), currentFont, origin, &grayBrush);
		}
	}

	BitBlt(hdc, 0, 0, width, height, hdcBuffer, 0, 0, SRCCOPY);

	SelectObject(hdcBuffer, oldBitmap);
	DeleteObject(hBitmapBuffer);
	DeleteDC(hdcBuffer);

}

void TabControl::DrawTabBody(LPDRAWITEMSTRUCT pdis)
{
	int borderSize = 1;
	RECT rc = pdis->rcItem;
	Rect rect = { rc.left, rc.top, rc.right, rc.bottom
	};

	Pen borderColor(GetColor(APP_COLORS::GRAY_BORDER));
	SolidBrush whiteBrush(GetColor(APP_COLORS::WHITE));

	HDC hDc = pdis->hDC;
	HDC hdcMem = CreateCompatibleDC(hDc);

	int width = rect.Width - rect.X;
	int height = rect.Height - rect.Y;
	HBITMAP bitmap = CreateCompatibleBitmap(hDc, width, height);
	HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, bitmap);

	Graphics gr(hdcMem);

	gr.FillRectangle(&whiteBrush, 0, 0, width, height);
	gr.DrawRectangle(&borderColor, 0, 0, width - borderSize, height - borderSize);

	BitBlt(hDc, 0, rc.top, width, height, hdcMem, 0, 0, SRCCOPY);

	SelectObject(hdcMem, oldBitmap);
	DeleteObject(bitmap);
	DeleteDC(hdcMem);
	ReleaseDC(pdis->hwndItem, hDc);
}

LRESULT CALLBACK TabControl::tcProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	TabControl* lpData = (TabControl*)dwRefData;
	HDC hdc = (HDC)wParam;

	switch (uMsg)
	{
	case WM_ERASEBKGND:
	{
		return TRUE;
	}

	case WM_HSCROLL:
	{
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}

	case WM_PAINT:
	{
		if (uIdSubclass != lpData->GetControlId("tab")) break;	//  Only for tab control

		PAINTSTRUCT ps;

		hdc = BeginPaint(hWnd, &ps);
		if (lpData->tabsType == TAB_TYPES::TABLE_BTNS)
		{
			lpData->DrawTableBtnsTabs(hdc);
		}
		else
		{
			lpData->DrawTabControl(hdc);
		}

		EndPaint(hWnd, &ps);

		return TRUE;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}

LRESULT CALLBACK TabControl::bodyProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	TabControl* lpData = (TabControl*)dwRefData;
	HDC hdc = (HDC)wParam;

	switch (uMsg)
	{
	case WM_ERASEBKGND:
	{
		return TRUE;
	}

	case WM_HSCROLL:
	{
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}

	case WM_PAINT:
	{
		if (uIdSubclass != lpData->GetControlId("tab")) break;	//  Only for tab control

		PAINTSTRUCT ps;

		hdc = BeginPaint(hWnd, &ps);
		if (lpData->tabsType == TAB_TYPES::TABLE_BTNS)
		{
			lpData->DrawTableBtnsTabs(hdc);
		}
		else
		{
			lpData->DrawTabControl(hdc);
		}

		EndPaint(hWnd, &ps);

		return TRUE;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK TabControl::parentProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	TabControl* lpData = (TabControl*)dwRefData;
	HDC hdc = (HDC)wParam;

	switch (uMsg)
	{
	case WM_DRAWITEM:
	{
		LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT)lParam;
		if (pdis->CtlID == lpData->GetControlId("tabBody"))
		{
			lpData->DrawTabBody(pdis);
			return true;
		}

		break;
	}

	case WM_SIZE:
	{
		lpData->OnResize();
		break;
	}

	case WM_NOTIFY:
	{
		switch (((LPNMHDR)lParam)->code)
		{
		case NM_CUSTOMDRAW:
		{
			int a = 0;
			break;
		}

		case TCN_SELCHANGING:
		{
			return FALSE;
		}

		case TCN_SELCHANGE:
		{
			if (((LPNMHDR)lParam)->idFrom == lpData->GetControlId("tab"))
			{
				InvalidateRect(lpData->GetControlHWND("tab"), NULL, FALSE);
			}

			break;
		}
		}

		break;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}