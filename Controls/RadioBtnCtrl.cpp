#include "RadioBtnCtrl.h"
#include "Controls.h"

RadioBtnCtrl::RadioBtnCtrl(HWND h, std::string name, int x, int y, int width, int height, int id, const wchar_t* text, DWORD styles, DWORD exStyles) :
	Control(CONTROL_TYPES::RADIO, h, name, L"BUTTON", styles |= BS_AUTORADIOBUTTON, x, y, width, height, id, text, exStyles)
{
	groupPtr = NULL;
	checked = false;
	SetWindowSubclass(GetHWND(), CoreProces, GetId(), (DWORD_PTR)this);
	SetWindowSubclass(GetParent(GetHWND()), ParentProces, GetId(), (DWORD_PTR)this);
}

RadioBtnCtrl::~RadioBtnCtrl()
{
	RemoveWindowSubclass(GetHWND(), CoreProces, GetId());
	RemoveWindowSubclass(GetParent(GetHWND()), ParentProces, GetId());
}

bool RadioBtnCtrl::IsChecked()
{
	return checked;
}

void RadioBtnCtrl::SetCheck(bool v)
{
	checked = v;
}

void RadioBtnCtrl::SetGroupPtr(RadioBtnGroup* groupPtr)
{
	this->groupPtr = groupPtr;
}

RadioBtnGroup* RadioBtnCtrl::GetGroupPtr()
{
	return groupPtr;
}

void RadioBtnCtrl::Draw(LPDRAWITEMSTRUCT pdis)
{
	int textOffsetLeft = 5;	// text offset from Arc
	int checkedArcDiff = 3;	// Arc size diff, when checked

	RECT rc = pdis->rcItem;
	Rect rect = { rc.left, rc.top, rc.right, rc.bottom
	};

	SolidBrush CheckedColor(Color(255, 41, 128, 185));
	Pen CheckedPenColor(Color(255, 41, 128, 185));
	Pen borderColor(Color(255, 204, 204, 204));
	SolidBrush btnColor(Color(255, 255, 255, 255));
	SolidBrush grayFontBrush(Color(255, 0, 0, 0));
	SolidBrush transparentBrush(Color(255, 240, 237, 255));

	wchar_t buff[MAX_PATH] = { 0 };

	GetWindowText(pdis->hwndItem, buff, _countof(buff));

	HDC hDc = pdis->hDC;
	HDC hdcMem = CreateCompatibleDC(hDc);

	int width = rect.Width;
	int height = rect.Height;

	HBITMAP bitmap = CreateCompatibleBitmap(hDc, width, height);
	HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, bitmap);

	Graphics gr(hdcMem);

	gr.FillRectangle(&btnColor, rect.X, rect.Y, width, height);

	//Mesure string
	RectF layoutRect(0, 0, -1, (REAL)rc.bottom);
	RectF boundRect;

	gr.MeasureString(buff, (INT)wcslen(buff), currentFont, layoutRect, &boundRect);

	PointF origin((REAL)((height * 2) + textOffsetLeft), (REAL)height / 2 - (REAL)boundRect.Height / 2);

	gr.DrawString(buff, (INT)wcslen(buff), currentFont, origin, &grayFontBrush);

	REAL x = (REAL)height;
	REAL y = (REAL)0;
	REAL w = (REAL)height - 1;
	REAL h = (REAL)height - 1;
	REAL startAngle = 0.0f;
	REAL sweepAngle = 360.0f;

	gr.SetSmoothingMode(SmoothingModeHighQuality);

	if (checked == true)
	{
		gr.DrawArc(&CheckedPenColor, x, y, w, h, startAngle, sweepAngle);

		RectF rc = { x + checkedArcDiff, y + checkedArcDiff, w - checkedArcDiff * 2, h - checkedArcDiff * 2
		};

		gr.FillPie(&CheckedColor, rc, startAngle, sweepAngle);
	}
	else
	{
		gr.DrawArc(&borderColor, x, y, w, h, startAngle, sweepAngle);
	}

	BitBlt(hDc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);

	SelectObject(hdcMem, oldBitmap);
	DeleteObject(bitmap);
	DeleteDC(hdcMem);
	ReleaseDC(pdis->hwndItem, hDc);
}

LRESULT CALLBACK RadioBtnCtrl::CoreProces(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	RadioBtnCtrl* lpData = (RadioBtnCtrl*)dwRefData;

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

LRESULT CALLBACK RadioBtnCtrl::ParentProces(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	RadioBtnCtrl* lpData = (RadioBtnCtrl*)dwRefData;

	switch (uMsg)
	{
	case WM_COMMAND:
	{
		UINT wId = LOWORD(wParam);

		if (HIWORD(wParam) == BN_CLICKED && wId == lpData->GetId())
		{
			lpData->groupPtr->UncheckAll();
			lpData->SetCheck(true);
		}

		break;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}