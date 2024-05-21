#include "ButtonCtrl.h"

ButtonCtrl::ButtonCtrl(HWND h, std::string name, int x, int y, int width, int height, int id, const wchar_t* text, DWORD styles, DWORD exStyles) :
	Control(CONTROL_TYPES::BUTTON, h, name, L"BUTTON", styles, x, y, width, height, id, text, exStyles)
{
	SetWindowSubclass(GetHWND(), CoreProces, GetId(), (DWORD_PTR)this);
}

ButtonCtrl::~ButtonCtrl()
{
	RemoveWindowSubclass(GetHWND(), CoreProces, GetId());
}

void ButtonCtrl::OnMouseHover()
{
	InvalidateRect(GetHWND(), NULL, FALSE);
}

void ButtonCtrl::OnMouseLeave()
{
	InvalidateRect(GetHWND(), NULL, FALSE);
}

void ButtonCtrl::Draw(LPDRAWITEMSTRUCT pdis)
{
	int iconSize = 20;
	RECT rc = pdis->rcItem;
	HWND hItem = pdis->hwndItem;
	LONG_PTR styles = GetWindowLongPtr(hItem, GWL_STYLE);
	const char* textAlign = "center";
	int textMargin = 10;

	if (styles & BS_RIGHT) textAlign = "right";
	else if (styles & BS_LEFT) textAlign = "left";

	Color btnColor, borderColor, fontColor;
	fontColor = GetColor(APP_COLORS::WHITE);

	int btnColorId = GetCtrlColor();
	bool isHovered = IsMouseHovered();

	if (btnColorId == APP_COLORS::RED)
	{
		if (isHovered)
		{
			btnColor = GetColor(APP_COLORS::HOVERED_RED);
		}
		else
		{
			btnColor = GetColor(btnColorId);
		}

		borderColor = GetColor(APP_COLORS::DARK_RED);
	}
	else if (btnColorId == APP_COLORS::GRAY)
	{
		if (isHovered)
		{
			btnColor = GetColor(APP_COLORS::HOVERED_GRAY);
		}
		else
		{
			btnColor = GetColor(btnColorId);
		}

		borderColor = GetColor(APP_COLORS::DARK_GRAY);
		fontColor = GetColor(APP_COLORS::BLACK);
	}
	else
	{
		if (isHovered)
		{
			btnColor = GetColor(APP_COLORS::HOVERED_BLUE);
		}
		else
		{
			btnColor = GetColor(APP_COLORS::BLUE);
		}

		borderColor = GetColor(APP_COLORS::DARK_BLUE);
	}

	Pen borderPen(borderColor, 2);
	SolidBrush btnBrush(btnColor);

	wchar_t buff[MAX_PATH];
	GetWindowText(pdis->hwndItem, buff, _countof(buff));

	HDC hDc = pdis->hDC;
	HDC hdcMem = CreateCompatibleDC(hDc);

	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	HBITMAP bitmap = CreateCompatibleBitmap(hDc, width, height);
	HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, bitmap);

	Graphics gr(hdcMem);

	if (GetCtrlStype() == BTN_STYLE_EMPTY_SQR)
	{
		DrawEmptySqr(gr, rc);
		fontColor = GetColor(APP_COLORS::BLACK);
	}
	else
	{
		gr.FillRectangle(&btnBrush, 0, 0, width, height);
		gr.DrawRectangle(&borderPen, 0, 0, width, height);
	}

	SolidBrush fontBrush(fontColor);

	//Mesure string
	RectF layoutRect(0, 0, -1, (REAL)rc.bottom);
	RectF boundRect;

	gr.MeasureString(buff, (INT)wcslen(buff), currentFont, layoutRect, &boundRect);

	int textX = width / 2 - (int)boundRect.Width / 2;
	if (textAlign == "right") textX = width - (int)boundRect.Width - textMargin;
	else if (textAlign == "left") textX = textMargin;

	PointF origin((REAL)textX, (REAL)(height / 2 - boundRect.Height / 2));

	gr.DrawString(buff, (INT)wcslen(buff), currentFont, origin, &fontBrush);
	REAL test = currentFont->GetSize();
	int iconId = GetCtrlIcon();

	if (iconId > 0)
	{
		if (wcslen(buff) == 0)
		{
			textMargin = width / 2 - iconSize / 2;
		}

		Bitmap* pBmp = LoadImageFromResource(NULL, MAKEINTRESOURCEA(iconId), "PNG");
		gr.DrawImage(pBmp, textMargin, height / 2 - iconSize / 2, iconSize, iconSize);
		delete pBmp;
	}

	BitBlt(hDc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);

	SelectObject(hdcMem, oldBitmap);
	DeleteObject(bitmap);
	DeleteDC(hdcMem);
	ReleaseDC(pdis->hwndItem, hDc);

}

void ButtonCtrl::DrawEmptySqr(Graphics& gr, RECT rc)
{
	SolidBrush whiteBrush = GetColor(APP_COLORS::WHITE);
	Pen borderPen = GetColor(APP_COLORS::BLUE);
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	gr.FillRectangle(&whiteBrush, 0, 0, width, height);
	if (IsActive()) gr.DrawRectangle(&borderPen, 0, 0, width - 1, height - 1);

}

LRESULT CALLBACK ButtonCtrl::CoreProces(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	ButtonCtrl* lpData = (ButtonCtrl*)dwRefData;

	switch (uMsg)
	{
	case WM_DRAWITEM:
	{
		LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT)lParam;
		lpData->Draw(pdis);
		break;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}