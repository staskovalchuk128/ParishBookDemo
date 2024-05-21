#include "ComboboxCtrl.h"
#include <windowsx.h>

ComboboxCtrl::ComboboxCtrl(HWND h, std::string name, int x, int y, int width, int height, int id, const wchar_t* text, DWORD styles, DWORD exStyles) :
	Control(CONTROL_TYPES::COMBOBOX, h, name, L"COMBOBOX", styles, x, y, width, height, id, text, exStyles)
{
	SetWindowSubclass(GetHWND(), CoreProces, GetId(), (DWORD_PTR)this);
}

ComboboxCtrl::~ComboboxCtrl()
{
	RemoveWindowSubclass(GetHWND(), CoreProces, GetId());
}

void ComboboxCtrl::Draw(LPDRAWITEMSTRUCT pdis)
{
	LONG_PTR styles = GetWindowLongPtr(pdis->hwndItem, GWL_STYLE);

	RECT rc = pdis->rcItem;
	Rect rect = { rc.left, rc.top, rc.right, rc.bottom
	};

	Color TextClr, BgClr;
	Pen borderColor(Color(255, 41, 128, 185));
	SolidBrush btnColor(Color(255, 52, 152, 219));
	SolidBrush transparentBrush(Color(255, 240, 237, 255));
	SolidBrush grayBrush(Color(255, 246, 246, 246));
	SolidBrush whiteBrush(Color(255, 255, 255, 255));
	SolidBrush blackBrush(Color(255, 0, 0, 0));

	// change colors for an active item
	if (pdis->itemState & ODS_FOCUS || pdis->itemState & ODS_SELECTED)
	{
		btnColor.GetColor(&BgClr);
		whiteBrush.GetColor(&TextClr);
	}
	else
	{
		grayBrush.GetColor(&BgClr);
		blackBrush.GetColor(&TextClr);
	}

	SolidBrush currentBg(BgClr);
	SolidBrush textColor(TextClr);

	if (pdis->CtlType == ODT_COMBOBOX)
	{
		RECT CBRect;

		GetWindowRect(pdis->hwndItem, &CBRect);

		wchar_t buff[200] = { 0 };

		int strLen = ComboBox_GetLBTextLen(pdis->hwndItem, pdis->itemID);
		ComboBox_GetLBText(pdis->hwndItem, pdis->itemID, &buff);

		HDC hDc = pdis->hDC;
		HDC hdcMem = CreateCompatibleDC(hDc);

		int width = rect.Width - rect.X;
		int height = rect.Height - rect.Y;
		HBITMAP bitmap = CreateCompatibleBitmap(hDc, width, height);
		HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, bitmap);

		Graphics gr(hdcMem);

		gr.FillRectangle(&currentBg, 0, 0, width, height);

		//Mesure string
		RectF layoutRect(0, 0, -1, (REAL)rc.bottom);
		RectF boundRect;

		gr.MeasureString(buff, (INT)wcslen(buff), currentFont, layoutRect, &boundRect);

		//PointF origin(width / 2 - boundRect.Width / 2, height / 2 - boundRect.Height / 2);
		PointF origin((REAL)comboBoxOffset, (REAL)height / 2 - boundRect.Height / 2);

		gr.DrawString(buff, (INT)wcslen(buff), currentFont, origin, &textColor);

		BitBlt(hDc, 0, rc.top, width, height, hdcMem, 0, 0, SRCCOPY);

		SelectObject(hdcMem, oldBitmap);
		DeleteObject(bitmap);
		DeleteDC(hdcMem);
		ReleaseDC(pdis->hwndItem, hDc);
	}
}

LRESULT CALLBACK ComboboxCtrl::CoreProces(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	ComboboxCtrl* lpData = (ComboboxCtrl*)dwRefData;
	Pen borderColor(Color(255, 204, 204, 204), 2);
	SolidBrush bgColor(Color(255, 255, 255, 255));
	SolidBrush arColor(Color(255, 246, 246, 246));
	SolidBrush blackBrush(Color(255, 0, 0, 0));

	HDC hDc;
	wchar_t buff[MAX_PATH] = { 0 };

	switch (uMsg)
	{
	case WM_MOUSEWHEEL: // prevent scroll when mouse on control
	{
		SendMessage(GetParent(hWnd), WM_MOUSEWHEEL, wParam, lParam);
		return false;
	}
	case WM_PAINT:
	{
		RECT rc;

		GetWindowRect(hWnd, &rc);

		Rect rect = { rc.left, rc.top, rc.right, rc.bottom
		};

		int width = rc.right - rc.left;
		int height = rc.bottom - rc.top;

		PAINTSTRUCT ps;

		hDc = BeginPaint(hWnd, &ps);

		HDC hdcMem = CreateCompatibleDC(hDc);

		HBITMAP bitmap = CreateCompatibleBitmap(hDc, width, height);
		HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, bitmap);

		Graphics gr(hdcMem);

		gr.FillRectangle(&bgColor, 0, 0, width - 30, height);
		gr.FillRectangle(&arColor, width - 30, 0, 30, height);
		borderColor.SetWidth(1);
		gr.DrawLine(&borderColor, width - 30, 0, width - 30, height);
		borderColor.SetWidth(2);
		gr.DrawRectangle(&borderColor, 0, 0, width, height);

		gr.SetSmoothingMode(SmoothingModeHighQuality);
		Point p1(width - 25, 10), p2(width - 25 + 10, 20);

		Point p11(width - 25 + 10, 20), p22(width - 25 + 10 + 10, 10);

		gr.DrawLine(&borderColor, p1, p2);
		gr.DrawLine(&borderColor, p11, p22);

		ComboBox_GetText(hWnd, buff, _countof(buff));

		RectF layoutRect(0, 0, -1, (REAL)rc.bottom);
		RectF boundRect;
		gr.MeasureString(buff, (INT)wcslen(buff), lpData->currentFont, layoutRect, &boundRect);

		//PointF origin((width - 30) / 2 - boundRect.Width / 2, height / 2 - boundRect.Height / 2);
		PointF origin((REAL)lpData->comboBoxOffset, (REAL)height / 2 - (REAL)boundRect.Height / 2);

		Rect stringRC = { lpData->comboBoxOffset, 0, width - 30 - lpData->comboBoxOffset, height
		};

		gr.SetClip(stringRC);

		gr.DrawString(buff, (INT)wcslen(buff), lpData->currentFont, origin, &blackBrush);

		BitBlt(hDc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);

		SelectObject(hdcMem, oldBitmap);
		DeleteObject(bitmap);
		DeleteDC(hdcMem);

		EndPaint(hWnd, &ps);

		return TRUE;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}