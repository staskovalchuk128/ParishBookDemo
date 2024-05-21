#include "CheckBoxCtrl.h"

CheckBoxCtrl::CheckBoxCtrl(HWND h, std::string name, int x, int y, int width, int height, int id, const wchar_t* text, DWORD styles, DWORD exStyles) :
	Control(CONTROL_TYPES::CHECKBOX, h, name, L"BUTTON", styles, x, y, width, height, id, text, exStyles)
{
	checked = false;
	SetWindowSubclass(GetHWND(), CoreProces, GetId(), (DWORD_PTR)this);
	SetWindowSubclass(GetParent(GetHWND()), ParentProces, GetId(), (DWORD_PTR)this);
}

CheckBoxCtrl::~CheckBoxCtrl()
{
	RemoveWindowSubclass(GetHWND(), CoreProces, GetId());
	RemoveWindowSubclass(GetParent(GetHWND()), ParentProces, GetId());
}

bool CheckBoxCtrl::IsChecked()
{
	return checked;
}

void CheckBoxCtrl::SetCheck(bool v)
{
	checked = v;
}

void CheckBoxCtrl::Draw(LPDRAWITEMSTRUCT pdis)
{
	int textOffsetLeft = 5;	// text offset from Arc
	int checkedArcDiff = 3;	// Arc size diff, when checked
	int checkBoxSize = 20;
	int borderSize = 1;

	RECT rc = pdis->rcItem;
	Rect checkBoxRect = { checkBoxSize, rc.top, checkBoxSize, checkBoxSize - borderSize
	};

	SolidBrush whiteBrush(GetColor(APP_COLORS::WHITE));
	SolidBrush fontColor(GetColor(APP_COLORS::BLACK));
	Pen whiteCheckedPen(GetColor(APP_COLORS::WHITE), 2);

	Color borderColor, btnColor;

	if (checked)
	{
		borderColor = GetColor(APP_COLORS::DARK_BLUE);
		btnColor = GetColor(APP_COLORS::BLUE);
	}
	else
	{
		borderColor = GetColor(APP_COLORS::DARK_GRAY);
		btnColor = GetColor(APP_COLORS::GRAY);
	}

	SolidBrush btnBrush(btnColor);
	Pen borderPen(borderColor);

	wchar_t buff[MAX_PATH] = { 0 };

	GetWindowText(pdis->hwndItem, buff, _countof(buff));

	HDC hDc = pdis->hDC;
	HDC hdcMem = CreateCompatibleDC(hDc);

	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	HBITMAP bitmap = CreateCompatibleBitmap(hDc, width, height);
	HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, bitmap);

	Graphics gr(hdcMem);

	gr.FillRectangle(&whiteBrush, rc.left, rc.top, width, height);

	//Mesure string
	RectF layoutRect(0, 0, -1, static_cast<REAL> (rc.bottom));
	RectF boundRect;

	gr.MeasureString(buff, (INT)wcslen(buff), currentFont, layoutRect, &boundRect);

	PointF origin(static_cast<REAL> ((height * 2) + textOffsetLeft), static_cast<REAL> (height / 2 - boundRect.Height / 2));

	gr.DrawString(buff, (INT)wcslen(buff), currentFont, origin, &fontColor);

	gr.FillRectangle(&btnBrush, checkBoxRect);
	gr.DrawRectangle(&borderPen, checkBoxRect);

	if (checked)
	{
		Point p1(checkBoxRect.X + 4, checkBoxRect.Y + 8), p2(checkBoxRect.X + 9, checkBoxRect.Y + 13);
		Point p3(checkBoxRect.X + 9, checkBoxRect.Y + 13), p4(checkBoxRect.X + 16, checkBoxRect.Y + 3);
		gr.SetSmoothingMode(SmoothingModeHighQuality);
		gr.DrawLine(&whiteCheckedPen, p1, p2);
		gr.DrawLine(&whiteCheckedPen, p3, p4);
	}

	BitBlt(hDc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);

	SelectObject(hdcMem, oldBitmap);
	DeleteObject(bitmap);
	DeleteDC(hdcMem);
	ReleaseDC(pdis->hwndItem, hDc);

}

LRESULT CALLBACK CheckBoxCtrl::CoreProces(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CheckBoxCtrl* lpData = (CheckBoxCtrl*)dwRefData;

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

LRESULT CALLBACK CheckBoxCtrl::ParentProces(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CheckBoxCtrl* lpData = (CheckBoxCtrl*)dwRefData;

	switch (uMsg)
	{
	case WM_COMMAND:
	{
		UINT wId = LOWORD(wParam);

		if (HIWORD(wParam) == BN_CLICKED && wId == lpData->GetId())
		{
			lpData->SetCheck(!lpData->IsChecked());
		}

		break;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}