#include "GroupBoxCtrl.h"

GroupBoxCtrl::GroupBoxCtrl(HWND h, std::string name, int x, int y, int width, int height, int id, const wchar_t* text, DWORD styles, DWORD exStyles) :
	Control(CONTROL_TYPES::GROUPBOX, h, name, L"BUTTON", styles, x, y, width, height, id, text, exStyles)
{
	SetWindowSubclass(GetHWND(), CoreProces, GetId(), (DWORD_PTR)this);
}

GroupBoxCtrl::~GroupBoxCtrl()
{
	RemoveWindowSubclass(GetHWND(), CoreProces, GetId());
}

void GroupBoxCtrl::Draw(LPDRAWITEMSTRUCT pdis)
{
	int borderSize = 1;
	int textOffsetLeft = 20, textOffsetTop = 0;
	RECT rc = pdis->rcItem;
	Rect rect = { rc.left, rc.top, rc.right, rc.bottom
	};

	Pen borderColor(GetColor(APP_COLORS::GRAY_BORDER));
	SolidBrush whiteBrush(GetColor(APP_COLORS::WHITE));
	SolidBrush blackBrush(GetColor(APP_COLORS::BLACK));

	HDC hDc = pdis->hDC;
	HDC hdcMem = CreateCompatibleDC(hDc);

	int width = rect.Width - rect.X;
	int height = rect.Height - rect.Y;
	HBITMAP bitmap = CreateCompatibleBitmap(hDc, width, height);
	HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, bitmap);

	Graphics gr(hdcMem);

	wchar_t buff[MAX_PATH] = { 0 };

	GetWindowText(pdis->hwndItem, buff, _countof(buff));

	//Mesure string
	RectF layoutRect(0, 0, -1, (REAL)rc.bottom);
	RectF boundRect;

	gr.MeasureString(buff, (INT)wcslen(buff), currentFont, layoutRect, &boundRect);

	PointF origin((REAL)textOffsetLeft, (REAL)textOffsetTop);

	int borderOffset = (int)boundRect.Height / 2;

	gr.FillRectangle(&whiteBrush, 0, 0, width, height);
	gr.DrawRectangle(&borderColor, 0, borderOffset, width - borderSize, height - borderSize - borderOffset);

	gr.FillRectangle(&whiteBrush, origin.X - 5, origin.Y, boundRect.Width + 10, boundRect.Height);
	gr.DrawString(buff, (INT)wcslen(buff), currentFont, origin, &blackBrush);

	BitBlt(hDc, 0, rc.top, width, height, hdcMem, 0, 0, SRCCOPY);

	SelectObject(hdcMem, oldBitmap);
	DeleteObject(bitmap);
	DeleteDC(hdcMem);
	ReleaseDC(pdis->hwndItem, hDc);

}

LRESULT CALLBACK GroupBoxCtrl::CoreProces(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	GroupBoxCtrl* lpData = (GroupBoxCtrl*)dwRefData;

	switch (uMsg)
	{
	case WM_CTLCOLORBTN:
	{
		SetBkMode(HDC(wParam), TRANSPARENT);
		return LRESULT(HBRUSH(GetStockObject(NULL_BRUSH)));
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}