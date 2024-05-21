/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "commctrl.h"
#include "gdiplus.h"
#include "..//Common/CFunctions.hpp"
#include "..//Common/Common.h"
#include "..//Drawing/DrawCommon.h"

using namespace Gdiplus;

#define WS_PRC_SIZE     0x80000000L
#define WS_PRC_POS      0x40000000L
#define WS_STICK_RIGHT  0x20000000L
#define WS_STICK_BOTTOM 0x10000000L

enum CONTROL_TYPES {
	BUTTON, LISTBOX, CHECKBOX, RADIO, GROUPBOX, EDIT, COMBOBOX, STATIC, LINE, CUSTOM
};

class Control : public DrawCommon, private Common
{
public:
	Control(int ctrlTypeId, HWND h, std::string name, const wchar_t* type, DWORD styles, int x, int y, int width, int height, int id, const wchar_t* text, DWORD exStyles, DWORD exStyle = 0);
	virtual ~Control();

	virtual void Draw(LPDRAWITEMSTRUCT pdis) {};
	virtual void OnMouseHover(){}
	virtual void OnMouseLeave(){}

	HFONT GetDefaultFont(int fontSize);

	void Create();

	void SetLParam(int v);
	void SetStyles(DWORD styles);
	void SetStyle(int style);
	void SetColor(int color);
	void SetIcon(int icon);
	void SetStatus(bool status);
	void SetX(int x);
	void SetY(int y);
	void SetPosition(int x, int y);
	void SetWidth(int w);
	void SetHeight(int h);
	void SetSize(int w, int h);

	bool IsLayered();
	bool IsMouseHovered();
	bool IsResizable();
	bool IsActive();
	std::string GetName();
	int GetTypeId();
	int GetCtrlIcon();
	int GetCtrlColor();
	int GetCtrlStype();
	int GetId();
	int GetConstX();
	int GetX(bool useClientRc = false);
	int GetConstY();
	int GetY(bool useClientRc = false);
	int GetWidth(bool useClientRc = false);
	int GetHeight(bool useClientRc = false);
	const wchar_t* GetText();
	const wchar_t* GetType();
	DWORD GetStyles();
	DWORD GetExStyles();
	int GetLParam();
	bool IsStickBottom();
	bool IsStickRight();
	HWND GetHWND();
private:
	int typeId;
	HWND hWnd, hItem;
	int x, y, width, height, id;
	const wchar_t* text;
	std::string name;
	const wchar_t* type;
	DWORD styles, exStyles;
	int lParam;
	bool stickRight, stickBottom;
	int icon;
	bool resizeable;
	bool isLine;
	int style;
	int color;
	bool active;
	bool layered;
	DWORD exStyle;
	static LRESULT CALLBACK CoreProces(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
protected:
	bool mouseHovered;
};