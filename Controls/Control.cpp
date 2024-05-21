#include "Control.h"


Control::Control(int typeId, HWND h, std::string name, 
	const wchar_t* type, DWORD styles, int x, int y, 
	int width, int height, int id, const wchar_t* text, DWORD exStyles, DWORD exStyle)
{
	this->exStyle = exStyle;
	this->typeId = typeId;
	this->hWnd = h;
	this->type = type;
	this->name = name;
	this->lParam = lParam;
	this->exStyles = exStyles;
	this->styles = styles;
	this->x = x;
	this->y = y;
	this->width = width;
	this->height = height;
	this->id = id;
	this->text = text;
	this->stickRight = (exStyles | WS_STICK_RIGHT) == exStyles;
	this->stickBottom = (exStyles | WS_STICK_BOTTOM) == exStyles;
	icon = -1;
	resizeable = true;
	hItem = NULL;
	isLine = false;
	color = style = -1;
	active = false;
	mouseHovered = false;

	Create();

	SetWindowSubclass(GetHWND(), CoreProces, GetId(), (DWORD_PTR)this);
}

Control::~Control()
{
	RemoveWindowSubclass(GetHWND(), CoreProces, GetId());
	BOOL res = DestroyWindow(hItem);
	if (!res)
	{
		SendMessage(hItem, WM_CLOSE, 0, 0);
	}
};

HFONT Control::GetDefaultFont(int fontSize)
{
	HFONT font = CreateFont(fontSize, 0, 0, 0, FW_BOLD, false, false, false, DEFAULT_CHARSET,
		OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY,
		DEFAULT_PITCH | FF_DONTCARE, L"Verdana");

	return font;
}

void Control::SetStyles(DWORD styles)
{
	this->styles |= styles;	//not finised
}
void Control::SetLParam(int v)
{
	this->lParam = v;
}
void Control::SetStyle(int style)
{
	this->style = style;
}

void Control::SetColor(int color)
{
	this->color = color;
}

void Control::SetIcon(int icon)
{
	this->icon = icon;
}

void Control::SetStatus(bool status)
{
	active = status;
	RedrawWindow(hItem, NULL, NULL, RDW_INVALIDATE);
}

void Control::SetX(int x)
{
	this->x = x;
}

void Control::SetY(int y)
{
	this->y = y;
}

void Control::SetPosition(int x, int y)
{
	this->x = x;
	this->y = y;
}

void Control::SetWidth(int w)
{
	this->width = w;
}

void Control::SetHeight(int h)
{
	this->height = h;
}

void Control::SetSize(int w, int h)
{
	this->width = w;
	this->height = h;
}

bool Control::IsMouseHovered()
{
	return mouseHovered;
}

bool Control::IsLayered()
{
	return layered;
}

bool Control::IsResizable()
{
	return resizeable;
}

bool Control::IsActive()
{
	return active;
}

std::string Control::GetName()
{
	return name;
}

int Control::GetTypeId()
{
	return typeId;
}

int Control::GetCtrlIcon()
{
	return icon;
}

int Control::GetCtrlColor()
{
	return color;
}

int Control::GetCtrlStype()
{
	return style;
}

int Control::GetId()
{
	return id;
}

int Control::GetConstX()
{
	return x;
}

int Control::GetX(bool useClientRc)
{
	SPData parentSize = GetWinSize(hWnd, useClientRc);
	int nx = stickRight == true ? parentSize.width - (x + GetWidth()) : x;

	if ((exStyles | WS_PRC_POS) == exStyles)
	{
		nx = static_cast<int>((float)parentSize.width / 100.0f * (float)x);
	}

	return nx;
}

int Control::GetConstY()
{
	return y;
}

int Control::GetY(bool useClientRc)
{
	SPData parentSize = GetWinSize(hWnd, useClientRc);
	int ny = stickBottom == true ? parentSize.height - (y + height) : y;

	return ny;
}

int Control::GetWidth(bool useClientRc)
{
	SPData parentSize = GetWinSize(hWnd, useClientRc);

	int w = width == -1 ? parentSize.width - x : width;	// -1 means 100%
	w = width < -1 ? parentSize.width - x + w : w;

	if ((exStyles | WS_PRC_SIZE) == exStyles)
	{
		w = static_cast<int>((float)parentSize.width / 100.0f * (float)width);
	}
	//w = width < -1 ? parentSize.width - x + w : w;
	
	return w;
}

int Control::GetHeight(bool useClientRc)
{
	SPData parentSize = GetWinSize(hWnd, useClientRc);
	int h = height == -1 ? parentSize.height - x : height;	// -1 means 100%
	h = height < -1 ? parentSize.height - y + h : h;
	return h;
}

const wchar_t* Control::GetText()
{
	return text;
}

const wchar_t* Control::GetType()
{
	return type;
}

DWORD Control::GetStyles()
{
	return styles;
}

DWORD Control::GetExStyles()
{
	return exStyles;
}

int Control::GetLParam()
{
	return lParam;
}

bool Control::IsStickBottom()
{
	return stickBottom;
}

bool Control::IsStickRight()
{
	return stickRight;
}

HWND Control::GetHWND()
{
	return hItem;
}

void Control::Create()
{
	HFONT DisplayFont = GetDefaultFont(16);

	if (WstrToLow(type) == L"button")
	{
		if ((styles & BS_TYPEMASK) == BS_GROUPBOX)
		{
			styles &= ~BS_GROUPBOX;
		}

		styles |= BS_OWNERDRAW;
	}

	styles |= WS_CLIPSIBLINGS;

	if (WstrToLow(type) == L"edit")
	{
		exStyle |= WS_EX_CLIENTEDGE;
	}

	HMENU menuHandle = (HMENU)(UINT_PTR)id;
	if (exStyle & WS_EX_LAYERED)
	{
		layered = true;
		menuHandle = NULL;
	}

	// This way position can be larger than 32767
	ScrollWindow(hWnd, -GetX(), -GetY(), NULL, NULL);
	this->hItem = CreateWindowEx(exStyle, type, text, styles, 0, 0, GetWidth(), GetHeight(), hWnd, menuHandle, NULL, (LPVOID)(UINT_PTR)lParam);
	ScrollWindow(hWnd, GetX(), GetY(), NULL, NULL);

	if (!hItem)
	{
		throw (L"Couldn't create control '" + StrToWStr(name) + L"' : " + GetLastErrorAsString());
	}

	SendMessage(this->hItem, WM_SETFONT,
		(WPARAM)DisplayFont, 0);

	//DeleteObject(DisplayFont);

}

LRESULT CALLBACK Control::CoreProces(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	Control* lpData = (Control*)dwRefData;

	switch (uMsg)
	{
	case WM_MOUSEMOVE:
	{
		TRACKMOUSEEVENT mouseEvt;

		mouseEvt.cbSize = sizeof(TRACKMOUSEEVENT);
		mouseEvt.dwFlags = TME_LEAVE | TME_HOVER;
		mouseEvt.dwHoverTime = 1;
		mouseEvt.hwndTrack = hWnd;

		TrackMouseEvent(&mouseEvt);
		break;
	}

	case WM_MOUSEHOVER:
	{
		lpData->mouseHovered = true;
		lpData->OnMouseHover();
		return TRUE;
	}

	case WM_MOUSELEAVE:
	{
		lpData->mouseHovered = false;
		lpData->OnMouseLeave();
		return TRUE;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}