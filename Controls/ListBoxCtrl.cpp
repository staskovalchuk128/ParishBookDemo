#include "ListBoxCtrl.h"
#include <windowsx.h>
#include "ScrollBarCtrl.h"

ListBoxCtrl::ListBoxCtrl(HWND h, std::string name, int x, int y, int width, int height, int id, const wchar_t* text, DWORD styles, DWORD exStyles) :
	Control(CONTROL_TYPES::LISTBOX, h, name, L"LISTBOX", styles, x, y, width, height, id, text, exStyles)
{
	scroll = new ScrollBar(GetHWND(), NULL);

	scroll->CreateScroll(21, -1, 50);

	SetWindowSubclass(GetHWND(), CoreProces, GetId(), (DWORD_PTR)this);
	SetWindowSubclass(GetParent(GetHWND()), ParentProces, GetId(), (DWORD_PTR)this);
}

ListBoxCtrl::~ListBoxCtrl()
{
	delete scroll;
	RemoveWindowSubclass(GetParent(GetHWND()), ParentProces, GetId());
	RemoveWindowSubclass(GetHWND(), CoreProces, GetId());
}

void ListBoxCtrl::OnSelChange()
{
	HWND hItem = GetHWND();
	RECT ItemRect;
	RECT rc = GetLocalCoordinates(hItem);
	int controlHeight = rc.bottom - rc.top;
	int totalItems = ListBox_GetCount(hItem);

	if (totalItems <= 0) return;
	ListBox_GetItemRect(hItem, 0, &ItemRect);

	int itemHeight = ItemRect.bottom - ItemRect.top;

	int topIndex = ListBox_GetTopIndex(hItem);

	int itemsPerPage = controlHeight / itemHeight;

	int scrollVal = static_cast<int> ((double)(topIndex) / ((double)totalItems - (double)itemsPerPage) * (double)100);

	int scrollPos = scroll->GetScrollBarSize(SB_VERT) * scrollVal / 100;

	scroll->SetPos(scroll->GetVScrollHWND(), scrollPos);
}

void ListBoxCtrl::Draw(LPDRAWITEMSTRUCT pdis)
{
	int offset = 10;
	LONG_PTR styles = GetWindowLongPtr(pdis->hwndItem, GWL_STYLE);

	RECT rc = pdis->rcItem;
	Rect rect = { rc.left, rc.top, rc.right, rc.bottom
	};

	Color TextClr, BgClr;
	Pen borderColor(GetColor(APP_COLORS::GRAY_BORDER));
	SolidBrush activeBrush(Color(255, 52, 152, 219));
	SolidBrush transparentBrush(Color(255, 240, 237, 255));
	SolidBrush grayBrush(Color(255, 246, 246, 246));
	SolidBrush whiteBrush(Color(255, 255, 255, 255));
	SolidBrush blackBrush(Color(255, 0, 0, 0));

	// change colors for an active item
	if (pdis->itemState & ODS_FOCUS || pdis->itemState & ODS_SELECTED)
	{
		activeBrush.GetColor(&BgClr);
		whiteBrush.GetColor(&TextClr);
	}
	else
	{
		whiteBrush.GetColor(&BgClr);
		blackBrush.GetColor(&TextClr);
	}

	SolidBrush currentBg(BgClr);
	SolidBrush textColor(TextClr);
	RECT ItemRect;

	if (pdis->CtlType == ODT_LISTBOX)
	{
		ListBox_GetItemRect(pdis->hwndItem, pdis->itemID, &ItemRect);

		int itemsCount = ListBox_GetCount(pdis->hwndItem);

		wchar_t buff[200] = { 0 };

		int strLen = ListBox_GetTextLen(pdis->hwndItem, pdis->itemID);
		SendMessage(pdis->hwndItem, LB_GETTEXT, pdis->itemID, (LPARAM)buff);

		HDC hDc = pdis->hDC;
		HDC hdcMem = CreateCompatibleDC(hDc);

		int width = ItemRect.right - ItemRect.left;
		int height = ItemRect.bottom - ItemRect.top;

		HBITMAP bitmap = CreateCompatibleBitmap(hDc, width, height);
		HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, bitmap);

		Graphics gr(hdcMem);

		gr.FillRectangle(&currentBg, 0, 0, width, height);
		gr.DrawRectangle(&borderColor, 0, 0, width - 1, height - (pdis->itemID == itemsCount - 1 ? 1 : 0));

		//Mesure string
		RectF layoutRect(0, 0, -1, (REAL)rc.bottom);
		RectF boundRect;

		gr.MeasureString(buff, (INT)wcslen(buff), currentFont, layoutRect, &boundRect);

		//PointF origin(width / 2 - boundRect.Width / 2, height / 2 - boundRect.Height / 2);
		PointF origin((REAL)offset, (REAL)height / 2 - (REAL)boundRect.Height / 2);

		gr.DrawString(buff, (INT)wcslen(buff), currentFont, origin, &textColor);

		BitBlt(hDc, 0, ItemRect.top, width, height, hdcMem, 0, 0, SRCCOPY);

		SelectObject(hdcMem, oldBitmap);
		DeleteObject(bitmap);
		DeleteDC(hdcMem);
		//ReleaseDC(pdis->hwndItem, hDc);
	}
}

LRESULT CALLBACK ListBoxCtrl::CoreProces(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	ListBoxCtrl* lpData = (ListBoxCtrl*)dwRefData;

	switch (uMsg)
	{
	case WM_NCCALCSIZE:
	{
		// remove default scroll
		LONG_PTR styles = GetWindowLongPtr(hWnd, GWL_STYLE);
		if ((styles & WS_VSCROLL) == WS_VSCROLL)
		{
			SetWindowLongPtr(hWnd, GWL_STYLE, styles & ~WS_VSCROLL);
		}

		if ((styles & WS_HSCROLL) == WS_HSCROLL)
		{
			SetWindowLongPtr(hWnd, GWL_STYLE, styles & ~WS_HSCROLL);
		}

		return TRUE;
	}

	case WM_LBUTTONDOWN:
	{
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}

	case WM_NCPAINT:
	{
		return 0;
	}

	case WM_PAINT:
	{
		Pen borderColor(lpData->GetColor(APP_COLORS::GRAY_BORDER));
		SolidBrush whiteBrush(lpData->GetColor(APP_COLORS::WHITE));
		SolidBrush blackBrush(lpData->GetColor(APP_COLORS::BLACK));
		SolidBrush activeBrush(lpData->GetColor(APP_COLORS::BLUE));

		int totalHeight = 0;

		int itemsCount = ListBox_GetCount(hWnd);

		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hWnd, &ps);

		RECT rc;

		GetWindowRect(hWnd, &rc);

		HDC hdcMem = CreateCompatibleDC(hdc);

		int width = rc.right - rc.left;
		int height = rc.bottom - rc.top;

		HBITMAP bitmap = CreateCompatibleBitmap(hdc, width, height);
		HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, bitmap);

		Graphics gr(hdcMem);

		gr.FillRectangle(&whiteBrush, 0, 0, width, height);
		gr.DrawRectangle(&borderColor, 0, 0, width - 3, height - 3);

		RECT ItemRect;

		for (int i = 0; i < itemsCount; i++)
		{
			int curSel = ListBox_GetCurSel(hWnd);
			int itemId = i;

			bool selected = curSel == itemId;

			Color TextClr, BgClr;

			if (selected)
			{
				activeBrush.GetColor(&BgClr);
				whiteBrush.GetColor(&TextClr);
			}
			else
			{
				whiteBrush.GetColor(&BgClr);
				blackBrush.GetColor(&TextClr);
			}

			SolidBrush currentBg(BgClr);
			SolidBrush textColor(TextClr);

			ListBox_GetItemRect(hWnd, itemId, &ItemRect);

			int itemWidth = ItemRect.right - ItemRect.left;
			int itemHeight = ItemRect.bottom - ItemRect.top;

			wchar_t buff[200] = { 0 };

			int strLen = ListBox_GetTextLen(hWnd, itemId);
			SendMessage(hWnd, LB_GETTEXT, itemId, (LPARAM)buff);

			gr.DrawLine(&borderColor, 0, ItemRect.bottom, itemWidth, ItemRect.bottom);

			if (selected)
			{
				gr.FillRectangle(&currentBg, 0, ItemRect.top, itemWidth, itemHeight);
			}

			//Mesure string
			RectF layoutRect(0, 0, -1, (REAL)ItemRect.bottom);
			RectF boundRect;

			gr.MeasureString(buff, (INT)wcslen(buff), lpData->currentFont, layoutRect, &boundRect);

			//PointF origin(width / 2 - boundRect.Width / 2, height / 2 - boundRect.Height / 2);
			PointF origin(10, ItemRect.top + (itemHeight / 2 - boundRect.Height / 2));

			gr.DrawString(buff, (INT)wcslen(buff), lpData->currentFont, origin, &textColor);

			totalHeight += itemHeight;
		}

		lpData->scroll->SetVScrollRange(totalHeight);

		BitBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);

		SelectObject(hdcMem, oldBitmap);
		DeleteObject(bitmap);
		DeleteDC(hdcMem);

		EndPaint(hWnd, &ps);

		return TRUE;
	}

	case WM_MOUSEWHEEL:
	{
		SendMessage(GetParent(hWnd), WM_MOUSEWHEEL, wParam, lParam);
		return TRUE;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}

LRESULT CALLBACK ListBoxCtrl::ParentProces(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	ListBoxCtrl* lpData = (ListBoxCtrl*)dwRefData;

	switch (uMsg)
	{
	case WM_COMMAND:
	{
		UINT wId = LOWORD(wParam);
		if (HIWORD(wParam) == LBN_SELCHANGE)
		{
			HWND hItem = (HWND)lParam;
			lpData->OnSelChange();
			InvalidateRect(hItem, NULL, FALSE);
			return (INT_PTR)TRUE;
		}

		break;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}