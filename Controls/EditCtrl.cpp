#include "EditCtrl.h"
#include <windowsx.h>
#include "..//Core.h"

EditCtrl::EditCtrl(HWND h, std::string name, int x, int y, int width, int height, int id, const wchar_t* text, bool parentIsDlg, DWORD styles, DWORD exStyles) :
	Control(CONTROL_TYPES::EDIT, h, name, L"EDIT", styles, x, y, width, height, id, text, exStyles)
{
	this->isParentDlg = parentIsDlg;
	selectAllOnFocus = false;
	focus = false;

	GetWindowText(GetHWND(), buf, MAX_PATH);
	hWnd = GetHWND();

	parent = GetParent(NULL);
	//parent = Core::GetCorePtr()->GetCoreHWND();
	cursor = select = 0;
	curScrollX = 0;
	paddingX = 5;
	borderSize = 1;
	label = L"";
	lockEditUpdate = false;
	

	SetWindowSubclass(GetHWND(), CoreProces, GetId(), (DWORD_PTR)this);
	SetWindowSubclass(parent, ParentProces, GetId(), (DWORD_PTR)this);

	LONG_PTR _styles = GetWindowLongPtr(hWnd, GWL_STYLE);
	if ((_styles & WS_BORDER) == WS_BORDER)
	{
		SetWindowLongPtr(hWnd, GWL_STYLE, _styles & ~WS_BORDER);
	}

	onlyNumbers = (_styles & ES_NUMBER) == ES_NUMBER;
	isPasswordType = (_styles & ES_PASSWORD) == ES_PASSWORD;

	phoneMask = false;
	dateMask = false;

	caretHeight = 0;

	fontSize = 16;

}

EditCtrl::~EditCtrl()
{
	RemoveWindowSubclass(GetHWND(), CoreProces, GetId());
	RemoveWindowSubclass(parent, ParentProces, GetId());
}

void EditCtrl::SelectAllOnFocus(bool res)
{
	selectAllOnFocus = res;
}

void EditCtrl::SelectAll()
{
	int strLen = (int)wcslen(buf);
	if (strLen > 0)
	{
		select = 0;
		cursor = strLen;
	}
}

void EditCtrl::SetMask(int maskId)
{
	if (maskId == ES_DATE_MASK)
	{
		dateMask = true;
		OnDateMask();
		SetLabel(L"mm/dd/yyyy");
	}
	else if (maskId == ES_PHONE_MASK)
	{
		phoneMask = true;
		OnPhoneMask();
		SetLabel(L"xxx-xxx-const");
	}
}

bool EditCtrl::HandlePhoneMask(char ch)
{
	int keyNum = (int)ch;

	size_t len = wcslen(buf);

	if (keyNum >= 48 && keyNum <= 57)
	{
		//All the numbers
		if ((len == 3 || len == 7))
		{
			if (buf[cursor] == '-')
			{
				cursor = select = (int)len;
			}
			else
			{
				buf[len] = '-';
				select = cursor = (int)len + 1;
			}
		}
		else if (len >= 12)
		{
			// Full number

			if (select != cursor)
			{
				// Selected something
				Delete();
				memmove(buf + cursor + 1, buf + cursor, len);
				buf[cursor] = ch;
				//  cursor = select = cursor + 1;
				OnPhoneMask();
				Redraw();
				return false;
			}
			else
			{
				return false;
			}
		}
	}
	else if (keyNum == -1)
	{
		if (len != 3 && len != 7)
		{
			return true;
		}
	}
	else
	{
		return false;
	}

	wmemmove(buf + (cursor + (size_t)1), buf + cursor, wcslen(buf) - cursor);
	buf[cursor] = ch;
	select = cursor = cursor + 1;

	OnPhoneMask();

	Redraw();

	SetEditValue(true);

	return true;

}

void EditCtrl::OnPhoneMask()
{
	size_t len = wcslen(buf);

	for (size_t i = 0; i < len; i++)
	{
		if ((i == 3 || i == 7) && buf[i] != '-')
		{
			if (len != 12)
			{
				memmove(buf + i - 1, buf + i, len);
				cursor = select = cursor + 1;
			}

			buf[i] = '-';
		}
		else if ((i > 8 && buf[i] == '-'))
		{
			memmove(buf + i + 1, buf + i, len);
		}
	}
}

bool EditCtrl::HandleDateMask(char ch)
{
	int keyNum = (int)ch;

	size_t len = wcslen(buf);

	if (keyNum >= 48 && keyNum <= 57)
	{
		//All the numbers
		if ((len == 2 || len == 5))
		{
			if (buf[cursor] == '/')
			{
				cursor = select = (int)len;
			}
			else
			{
				if(cursor == 2 || cursor == 5) buf[len] = '/';
				select = cursor = (int)len + 1;
			}
		}
		else if (len >= 10)
		{
			// Full date

			if (select != cursor)
			{
				// Selected something
				Delete();

				wmemmove(buf + (cursor + (size_t)1), buf + cursor, wcslen(buf) - cursor);
	
				buf[cursor] = ch;
				cursor = select = cursor + 1;
					
				OnDateMask();
				Redraw();
				SetEditValue(true);
				return false;
			}
			else
			{
				return false;
			}
		}
		else if (cursor < len)
		{
			if (buf[cursor] == '/')
			{
				//cursor = select = (int)len;
			}
			else if (cursor < 5)
			{
				//buf[cursor] = ch;
				//Redraw();
			//	return false;
				// cursor = select = len;
			}
		}
	}
	else if (keyNum == 47)
	{
		if (len != 2 && len != 5) return false;
		/*
		if (len == 1 || len == 4)
		{
			memmove(buf + (len - 1) + 1, buf + (len - 1), len);
			buf[len - 1] = '0';
			cursor = select = cursor + 1;
		}
		else if (len != 2 && len != 5)
		{
			return false;
		}
		*/
	}
	else
	{
		return false;
	}

	wmemmove(buf + (cursor + (size_t)1), buf + cursor, wcslen(buf) - cursor);
	buf[cursor] = ch;
	select = cursor = cursor + 1;

	OnDateMask();

	Redraw();

	SetEditValue(true);

	return true;

}

void EditCtrl::OnDateMask()
{
	size_t len = wcslen(buf);

	for (size_t i = 0; i < len; i++)
	{
		if (i == 2 && buf[i] != '/' && buf[i - 1] == '/' ||
			i == 5 && buf[i] != '/' && buf[i - 1] == '/')
		{
		//	memmove(buf + (i - 2) + 1, buf + (i - 2), len);
			//buf[i - 2] = '0';
		}
		else if ((i == 2 || i == 5) && buf[i] != '/')
		{
			if (len != 10)
			{
				memmove(buf + i + 1, buf + i, len);
				cursor = select = cursor + 1;
			}

			buf[i] = '/';
		}
		else if ((i > 5 && buf[i] == '/'))
		{
			memmove(buf + i + 1, buf + i, len);
		}
	}

	//SetWindowText(hWnd, (LPCSTR)value.c_str());
	//SendMessage(hWnd, EM_SETSEL, value.length(), value.length());
}

SizeData EditCtrl::GetSize()
{
	RECT rc;

	GetClientRect(hWnd, &rc);

	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	rc.left = paddingX;

	width -= paddingX * 2;

	return SizeData({ rc.left, rc.top, width, height });
}

void EditCtrl::SetLabel(const wchar_t* l)
{
	label = l;
	Redraw();
}

void EditCtrl::SetEditValue(bool notifyParent) // notifyParent not used
{
	lockEditUpdate = true;
	SetWindowText(hWnd, buf);
	lockEditUpdate = false;
}

bool EditCtrl::HasFocus()
{
	return focus;
}

bool EditCtrl::DrawLabel(HDC& hdc)
{
	if (wcslen(label) == 0) return false;
	if (wcslen(buf) > 0) return false;

	SizeData sd = GetSize();

	int width = sd.width;
	int height = sd.height;

	RECT labelRc = { sd.x, sd.y, width, height
	};

	SetTextColor(hdc, RGB(112, 128, 144));

	DrawText(hdc, label, (INT)wcslen(label), &labelRc, DT_LEFT | DT_TOP | DT_CALCRECT | DT_NOPREFIX);

	labelRc.top = (height / 2) - ((labelRc.bottom - labelRc.top) / 2);
	labelRc.bottom = height;

	DrawText(hdc, label, -1, &labelRc, DT_LEFT | DT_TOP | DT_NOPREFIX);

	return true;
}

void EditCtrl::OnTimer() {}

void EditCtrl::Delete()
{
	size_t len = wcslen(buf);

	if (dateMask) 
	{
		if (buf[cursor] == L'/' && select == cursor + 1)
		{
		//	cursor = select = cursor;
		//	return;
		}
	}


	if (select > cursor)
	{
		wmemcpy(buf + cursor, buf + select, (len - select));
		wmemset(buf + (len - select + cursor), NULL, (MAX_PATH - len + select - cursor));
		select = cursor;
	}
	else if (select < cursor)
	{
		wmemcpy(buf + select, buf + cursor, (len - cursor));
		wmemset(buf + (len - cursor + select), NULL, (MAX_PATH - len + cursor - select));
		cursor = select;
		curScrollX = 0;
	}
	else
	{
		select = cursor;
	}

//	if (dateMask) OnDateMask();
	if (phoneMask) OnPhoneMask();

	SetEditValue();
}

RECT EditCtrl::GetCharRange(HDC hdc, int from, int to)
{
	std::wstring str(buf);

	SizeData sd = GetSize();

	RECT rc = { sd.x, sd.y, sd.width, sd.height
	};

	from = from < 0 ? 0 : from;
	to = to > str.length() ? (int)str.length() : to;

	if (to < from)
	{
		to = from;
		from = to;
	}

	int subCount = to - from;

	str = str.substr(from, subCount);

	DrawText(hdc, str.c_str(), (INT)str.length(), &rc, DT_LEFT | DT_TOP | DT_CALCRECT | DT_NOPREFIX);

	return rc;
}

int EditCtrl::GetCharRangeSize(HDC hdc, int from, int to)
{
	RECT rc = GetCharRange(hdc, from, to);
	return rc.right - rc.left;
}

void EditCtrl::Redraw()
{
	RECT r = GetLocalCoordinates(hWnd);

	int width = r.right - r.left;
	int height = r.bottom - r.top;

	HDC hdc;

	HideCaret(hWnd);

	hdc = GetDC(hWnd);

	HDC hdcMem = CreateCompatibleDC(hdc);

	HBITMAP bitmap = CreateCompatibleBitmap(hdc, width, height);
	HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, bitmap);

	SetFont(hdcMem);

	SetScroll(hdcMem);
	DrawControl(hdcMem);
	DrawLabel(hdcMem);

	BitBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);

	SelectObject(hdcMem, oldBitmap);
	DeleteObject(bitmap);
	DeleteDC(hdcMem);

	ReleaseDC(hWnd, hdc);

	ShowCaret(hWnd);
}

void EditCtrl::SetScroll(HDC hdc)
{
	SizeData sd = GetSize();

	int width = sd.width;
	int height = sd.height;

	RECT stringRc = { sd.x, sd.y, width, height
	}, rc;
	CopyRect(&rc, &stringRc);

	int cVal = cursor;

	if (cursor != select) cVal = select;	// Selecting

	if (cVal)
	{
		DrawText(hdc, buf, cVal, &rc, DT_LEFT | DT_TOP | DT_CALCRECT | DT_NOPREFIX);
		if (cVal == wcslen(buf))
		{
			DrawText(hdc, buf, (INT)wcslen(buf), &stringRc, DT_LEFT | DT_TOP | DT_CALCRECT | DT_NOPREFIX);

			if (rc.right > width)
			{
				curScrollX = rc.right - width;
			}
			else
			{
				curScrollX = 0;
			}
		}
		else if (rc.right - curScrollX > width)
		{
			curScrollX += GetCharRangeSize(hdc, cVal, cVal + 2);
		}
		else if (curScrollX > 0)
		{
			if (cVal == 1) curScrollX = 0;
			else if (rc.right - curScrollX <= 0)
			{
				curScrollX -= GetCharRangeSize(hdc, cVal - 2, cVal);
				curScrollX = curScrollX < 0 ? 0 : curScrollX;
			}
		}
		else
		{
			curScrollX = 0;
		}
	}
}

void EditCtrl::SetFont(HDC hdc)
{
	HFONT DisplayFont = CreateFont((int)fontSize, 0, 0, 0, FW_BOLD, false, false, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, L"Verdana");

	SelectObject(hdc, (HGDIOBJ)DisplayFont);
	SetBkMode(hdc, TRANSPARENT);

	DeleteObject(DisplayFont);
}

void EditCtrl::DrawControl(HDC& hdc)
{
	SetFont(hdc);

	Graphics gr(hdc);
	SolidBrush whiteBrush(GetColor(APP_COLORS::WHITE));
	SolidBrush blueBrush(GetColor(APP_COLORS::BLUE));

	RECT cr;

	GetClientRect(hWnd, &cr);

	int fW = cr.right - cr.left, fH = cr.bottom - cr.top;

	gr.FillRectangle(&whiteBrush, cr.left, cr.top, fW, fH);

	SizeData sd = GetSize();

	int width = sd.width;
	int height = sd.height;

	RECT stringRc = { sd.x, sd.y, width, height
	}, r;
	CopyRect(&r, &stringRc);


	std::wstring stringToDraw(buf);
	if (isPasswordType)
	{
		std::fill(stringToDraw.begin(), stringToDraw.begin() + stringToDraw.length(), L'•');
	}


	{

		// Draw selected rect

		if (select != cursor)
		{
			int selectedFrom = select > cursor ? cursor : select;
			int selectedTo = select > cursor ? select : cursor;

			std::wstring str(stringToDraw);
			str = str.substr(selectedFrom, selectedTo);

			Rect selectedRc = { stringRc.left, stringRc.top, width, height
			};

			int xPos = 0;
			for (size_t i = 1; i <= stringToDraw.length(); i++)
			{
				DrawText(hdc, stringToDraw.c_str(), (INT)i, &stringRc, DT_LEFT | DT_TOP | DT_CALCRECT | DT_NOPREFIX);
				xPos = stringRc.right;

				if (i == selectedFrom) selectedRc.X = xPos;
				else if (i == selectedTo)
				{
					selectedRc.Width = GetCharRangeSize(hdc, selectedFrom, selectedTo);
					break;
				}
			}

			selectedRc.X -= curScrollX;
			selectedRc.Y = 0;
			selectedRc.Height = height;

			gr.FillRectangle(&blueBrush, selectedRc);
		}
	}

	DrawText(hdc, stringToDraw.c_str(), (INT)stringToDraw.length(), &stringRc, DT_LEFT | DT_TOP | DT_CALCRECT | DT_NOPREFIX);

	stringRc.left -= curScrollX;

	stringRc.top = (height / 2) - ((stringRc.bottom - stringRc.top) / 2);
	stringRc.bottom = height;

	DrawText(hdc, stringToDraw.c_str(), -1, &stringRc, DT_LEFT | DT_TOP | DT_NOPREFIX);

	if (cursor)
	{
		DrawText(hdc, stringToDraw.c_str(), cursor, &r, DT_LEFT | DT_TOP | DT_CALCRECT | DT_NOPREFIX);

		if (curScrollX > 0)
		{
			if (cursor == stringToDraw.length()) r.right -= curScrollX;
			else
			{
				std::wstring str(stringToDraw);
				str = str.substr(0, cursor);

				DrawText(hdc, str.c_str(), (INT)str.length(), &r, DT_LEFT | DT_TOP | DT_CALCRECT | DT_NOPREFIX);
				r.right -= curScrollX;
			}
		}
	}
	else
	{
		r.right = stringRc.left;
	}

	if (HasFocus())
	{
		SetCaretPos(r.right, height / 2 - (caretHeight / 2));
	}
}

int EditCtrl::GetCharIndexUnderCursor(POINT pt)
{
	int val = -1;

	RECT r = { 0 };

	RECT editRc = GetWinRc(GetHWND());

	HDC hdc = GetDC(hWnd);

	SetFont(hdc);

	int leftPos = editRc.left - curScrollX;
	int totalStrWidth = 0;
	bool atEnd = true;

	for (size_t i = 1; i <= wcslen(buf); i++)
	{
		DrawText(hdc, buf, (INT)i, &r, DT_LEFT | DT_TOP | DT_CALCRECT | DT_NOPREFIX);

		r.right -= curScrollX - paddingX;

		int charW = GetCharRangeSize(hdc, (INT)i - 1, (INT)i);

		totalStrWidth += charW;

		if (pt.x >= leftPos && pt.x <= leftPos + totalStrWidth)
		{
			int cursorAt = 0;

			if (pt.x <= leftPos + totalStrWidth + (charW / 2))
			{
				cursorAt = (int)i - 1;
			}
			else
			{
				cursorAt = (int)i;
			}

			val = cursorAt;
			atEnd = false;
			break;
		}
		else if (pt.x <= leftPos) 
		{
			val = 0;
			atEnd = false;
			break;
		}

		//leftPos = r.right;
	}

	if (atEnd == true)
	{
		val = (int)wcslen(buf);
	}

	ReleaseDC(hWnd, hdc);

	return val;
}

bool EditCtrl::OnChar(char ch)
{
	int keyNum = (int)ch;

	if (dateMask)
	{
		HandleDateMask(ch);
		return true;
	}
	else if (phoneMask)
	{
		HandlePhoneMask(ch);
		return true;
	}

	if (onlyNumbers && (keyNum < 48 || keyNum > 57)) return false;
	Delete();

	if (wcslen(buf) + 1 < MAX_PATH)

	{
		wmemmove(buf + (cursor + (size_t)1), buf + cursor, wcslen(buf) - cursor);
		buf[cursor] = ch;
		cursor++;
		select = cursor;
	}

	Redraw();

	SetEditValue(true);

	return true;
}

void EditCtrl::PasteText()
{
	bool correct = true;
	HANDLE h;
	wchar_t* cb = NULL;

	Delete();

	OpenClipboard(NULL);
	h = GetClipboardData(CF_UNICODETEXT);

	cb = (wchar_t*)GlobalLock(h);
	cb = cb == NULL ? (wchar_t*)
		'\0' : cb;

	if (onlyNumbers && !IsNumber(cb != NULL ? cb : L"")) correct = false;

	std::wstring formatedStr = RemoveAllLineBreaks(cb != NULL ? cb : L"");
	cb = &formatedStr[0];

	if (correct)
	{
		size_t len = wcslen(cb), slen = wcslen(buf);

		if (cb)
		{
			wmemcpy(buf + (cursor + len), buf + cursor, (slen - cursor));
			wmemcpy(buf + cursor, cb, len);
			cursor = select = (int)wcslen(buf);
		}
	}

	GlobalUnlock(h);
	CloseClipboard();
	SetEditValue(true);
	Redraw();
}

void EditCtrl::OnKeyDown(WPARAM wParam)
{
	switch (wParam)
	{
	case 'V':
	{
		if (0x8000 & GetKeyState(VK_CONTROL))
		{
			PasteText();
		}

		break;
	}

	case 'A':
	{
		if (0x8000 & GetKeyState(VK_CONTROL))
		{
			select = 0;
			cursor = (int)wcslen(buf);
			Redraw();
		}

		break;
	}

	case 'C':
	{
		if (0x8000 & GetKeyState(VK_CONTROL))
		{
			std::wstring str(buf);
			if (select != cursor)
			{
				str = str.substr(select, (size_t)cursor - select);
			}

			const wchar_t* bufStr = str.c_str();

			LPWSTR dst;
			size_t len;
			HGLOBAL hMem;
			len = wcslen(bufStr) + 1;
			hMem = GlobalAlloc(GMEM_MOVEABLE, len * sizeof(wchar_t));
			if (hMem == NULL) break;
			dst = (LPWSTR)GlobalLock(hMem);
			dst = dst == NULL ? (LPWSTR)L"" : dst;
			memcpy(dst, bufStr, len * sizeof(wchar_t));
			GlobalUnlock(hMem);
			OpenClipboard(0);
			EmptyClipboard();
			SetClipboardData(CF_UNICODETEXT, hMem);
			CloseClipboard();
		}

		break;
	}

	case VK_RIGHT:
	{
		if (cursor - 1 >= MAX_PATH || cursor >= (int)wcslen(buf))
			break;

		cursor++;

		if (!(GetKeyState(VK_SHIFT) & 0x8000))
		{
			select = cursor;
		}

		Redraw();
		break;
	}

	case VK_TAB:
	{
		PostMessageW(GetParent(hWnd), WM_NEXTDLGCTL, GetKeyState(VK_SHIFT) & 0x8000, FALSE);
		break;
	}

	case VK_LEFT:
	{
		if (cursor <= 0)
			break;

		cursor--;

		if (!(GetKeyState(VK_SHIFT) & 0x8000))
		{
			select = cursor;
		}

		Redraw();
		break;
	}

	case VK_HOME:
	{
		cursor = 0;

		if (!(GetKeyState(VK_SHIFT) & 0x8000))
			select = cursor;

		Redraw();
		break;
	}

	case VK_END:
	{
		cursor = (int)wcslen(buf);

		if (!(GetKeyState(VK_SHIFT) & 0x8000))
			select = cursor;

		Redraw();
		break;
	}

	case VK_DELETE:
	{
		if (cursor >= (int)wcslen(buf))
		{
			Delete();
			Redraw();
			break;
		}

		if (select == cursor)
			select++;

		Delete();
		Redraw();
		break;
	}

	case VK_BACK:
	{
		if (cursor <= 0)
		{
			Delete();
			Redraw();
			break;
		}

		if (select == cursor)
			cursor--;

		Delete();
		Redraw();
		break;
	}
	}
}

void EditCtrl::OnSetFocus()
{
	focus = true;
	
	RECT r;
	GetClientRect(hWnd, &r);

	int strLen = (int)wcslen(buf);

	if (strLen > 0)
	{
		if (selectAllOnFocus)
		{
			cursor = 0;
			select = strLen;
		}
		else
		{

			POINT pt;
			GetCursorPos(&pt);

			int cursorAt = GetCharIndexUnderCursor(pt);

			//select = cursor = cursorAt;

			select = cursor = strLen;
		}
	}

	caretHeight = r.bottom - r.top;

	CreateCaret(hWnd, (HBITMAP)NULL, CARETWIDTH, caretHeight);

	ShowCaret(hWnd);
	Redraw();

	SendMessage(hWnd, WM_NCPAINT, NULL, NULL);

}

void EditCtrl::OnKillFocus()
{
	cursor = select = 0;
	
	HideCaret(hWnd);
	DestroyCaret();
	Redraw();

	SendMessage(hWnd, WM_NCPAINT, NULL, NULL);

	focus = false;
}

void EditCtrl::OnMouseMove(WPARAM wParam, LPARAM lParam)
{
	if (!HasFocus()) return;
	if (wParam == MK_LBUTTON)
	{
		POINT pt;
		GetCursorPos(&pt);

		int cursorAt = GetCharIndexUnderCursor(pt);

		select = cursorAt;

		Redraw();
	}
}

void EditCtrl::OnLButtonDown(WPARAM wParam, LPARAM lParam)
{
	if (HasFocus()) 
	{
		POINT pt;
		GetCursorPos(&pt);

		int cursorAt = GetCharIndexUnderCursor(pt);

		select = cursor = cursorAt;
		Redraw();
	}


	PostMessageW(GetParent(hWnd), WM_NEXTDLGCTL, (WPARAM)hWnd, TRUE);
}

LRESULT CALLBACK EditCtrl::CoreProces(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	EditCtrl* lpData = (EditCtrl*)dwRefData;
	Pen borderColor(Color(255, 204, 204, 204), 2);
	SolidBrush bgColor(Color(255, 255, 255, 255));
	SolidBrush textColor(lpData->GetColor(APP_COLORS::DARK_GRAY));
	HDC hdc = NULL;

	switch (uMsg)
	{
	case WM_TIMER:
	{
		lpData->OnTimer();
		break;
	}

	case WM_LBUTTONDOWN:
	{
		lpData->OnLButtonDown(wParam, lParam);
		break;
	}

	case WM_LBUTTONDBLCLK:
	{
		lpData->SelectAll();
		lpData->Redraw();
		break;
	}
		
	case WM_MOUSEMOVE:
	{
		lpData->OnMouseMove(wParam, lParam);
		break;
	}

	case WM_KILLFOCUS:
	{
		lpData->OnKillFocus();
		break;
	}

	case WM_SETFOCUS:
	{
		lpData->OnSetFocus();
		return FALSE;
	}

	/*
	case WM_GETDLGCODE: { 	return DLGC_WANTALLKEYS | DLGC_WANTARROWS;
	}

	*/
	case WM_KEYDOWN:
	{
		lpData->OnKeyDown(wParam);
		return TRUE;
	}

	case WM_CHAR:
	{
		if (wParam < VK_SPACE)
			return TRUE;

		lpData->OnChar((char)wParam);

		return TRUE;
	}

	case WM_SETTEXT:
	{
		if (lpData->lockEditUpdate) break;
		LPWSTR ch = (LPWSTR)lParam;
		const wchar_t* txtVal = *ch == NULL ? L"" : ch;
		memset(lpData->buf, 0, MAX_PATH);
		memcpy(lpData->buf, txtVal, wcslen(txtVal) * sizeof(wchar_t));
		lpData->Redraw();
		break;
	}

	case WM_ERASEBKGND:
	{
		return TRUE;
	}

	case WM_PASTE:
	{
		lpData->PasteText();
		break;
	}

	case WM_NCPAINT:
	{
		Pen red(lpData->GetColor(APP_COLORS::GRAY_BORDER), (REAL)lpData->borderSize);
		Pen focusedColor(lpData->GetColor(APP_COLORS::BLUE), (REAL)lpData->borderSize);
		Pen gray(Color(52, 73, 94), 1);
		SolidBrush whiteColor(lpData->GetColor(APP_COLORS::WHITE));
		RECT r = GetLocalCoordinates(hWnd);

		int width = r.right - r.left;
		int height = r.bottom - r.top;

		hdc = GetWindowDC(hWnd);

		HDC hdcMem = CreateCompatibleDC(hdc);

		HBITMAP bitmap = CreateCompatibleBitmap(hdc, width, height);
		HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, bitmap);

		Graphics gr(hdcMem);

		Rect rc = { 0, 0, width - lpData->borderSize, height - lpData->borderSize
		};

		gr.FillRectangle(&whiteColor, 0, 0, width, height);

		if (GetFocus() == hWnd)
		{
			gr.DrawRectangle(&focusedColor, rc);
		}
		else
		{
			gr.DrawRectangle(&red, rc);
		}

		BitBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);

		SelectObject(hdcMem, oldBitmap);
		DeleteObject(bitmap);
		DeleteDC(hdcMem);

		lpData->Redraw();

		ReleaseDC(hWnd, hdc);
		return TRUE;
	}

	case WM_PAINT:
	{
		HDC hdc;
		PAINTSTRUCT ps;

		RECT r = GetLocalCoordinates(hWnd);

		int width = r.right - r.left;
		int height = r.bottom - r.top;

		hdc = BeginPaint(hWnd, &ps);

		HDC hdcMem = CreateCompatibleDC(hdc);

		HBITMAP bitmap = CreateCompatibleBitmap(hdc, width, height);
		HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, bitmap);

		lpData->SetFont(hdcMem);

		lpData->DrawControl(hdcMem);
		lpData->DrawLabel(hdcMem);

		BitBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);

		SelectObject(hdcMem, oldBitmap);
		DeleteObject(bitmap);
		DeleteDC(hdcMem);

		EndPaint(hWnd, &ps);
		return TRUE;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}


LRESULT CALLBACK EditCtrl::ParentProces(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	EditCtrl* lpData = (EditCtrl*)dwRefData;

	switch (uMsg)
	{
	case WM_MOUSEMOVE:
	{
		lpData->OnMouseMove(wParam, lParam);
		break;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}