#include "LineCtrl.h"

LineCtrl::LineCtrl(HWND h, std::string name, int x, int y, int width, int height, int id, const wchar_t* text, DWORD styles, DWORD exStyles) :
	Control(CONTROL_TYPES::LINE, h, name, L"STATIC", styles, x, y, width, height, id, text, exStyles) {}

LineCtrl::~LineCtrl() {}

void LineCtrl::Draw(LPDRAWITEMSTRUCT pdis)
{
	RECT rc = pdis->rcItem;
	HWND hItem = pdis->hwndItem;

	Color lineColor;
	int ctrlColor = GetCtrlColor();

	if (ctrlColor == APP_COLORS::BLUE)
	{
		lineColor = GetColor(APP_COLORS::BLUE);
	}
	else if (ctrlColor == APP_COLORS::RED)
	{
		lineColor = GetColor(APP_COLORS::RED);
	}
	else
	{
		lineColor = GetColor(APP_COLORS::GRAY_BORDER);
	}

	SolidBrush brush(lineColor);

	HDC hDc = pdis->hDC;
	HDC hdcMem = CreateCompatibleDC(hDc);

	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	HBITMAP bitmap = CreateCompatibleBitmap(hDc, width, height);
	HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, bitmap);

	Graphics gr(hdcMem);

	gr.FillRectangle(&brush, 0, 0, width, height);

	BitBlt(hDc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);

	SelectObject(hdcMem, oldBitmap);
	DeleteObject(bitmap);
	DeleteDC(hdcMem);
	ReleaseDC(hItem, hDc);
}