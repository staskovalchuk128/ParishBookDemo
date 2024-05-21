/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "windowsx.h"
#include "assert.h"
#include "../Controls/Controls.h"


#define TIMERID_NOTIFY	100
#define TIMERID_DELAY	200
#define TIME_DELAY		500
#define TIME_INTER		100
#define THUMB_MINSIZE   30

class ScrollBar : public Controls 
{
public:
	ScrollBar(HWND hWnd, HINSTANCE hInst);
	~ScrollBar();

	void CreateScroll(int w, int h, int wheelOffset = 30);
	void SetScrollSize(DWORD scrollType);
	void InitScroll();

	void SetScrollbarRange(HWND hItem, DWORD scrollType, int min, int max);

	void SetVScrollRange(int scrollMaxHeight = -1);
	void SetHScrollRange(int scrollMaxWidth = -1);

	void HandleScroll(int action, DWORD type, int thum = 0);

	bool IsVertical(HWND hWnd);
	UINT HitTest(HWND hItem, POINT pt);
	RECT GetRect(HWND hItem, UINT uSBCode);


	void SetPos(HWND hItem, int pos);
	int GetScrollBarSize(DWORD type);
	int GetScrollBarPos(DWORD type);

	void ResetScroll(DWORD type);

	void ResetSize();

	void SetWheelOffset(int o);
	HWND GetHScrollHWND();
	HWND GetVScrollHWND();
private:
	int wheelOffset;
	bool hScrollShown, vScrollShown;
	HWND hVScroll, hHScroll;

	int scrollSize;
	UINT uClicked;
	bool bNotify;

	bool dragging;
	POINT dragPt;
	int dragPos;
	UINT m_uHtPrev;
	
	bool pause;
	bool trace;

	int scrollMaxH, scrollMaxW;
	HWND hWndParent;
	HWND hWnd;
	HINSTANCE hInst;
	void SetChildXScrollOffset(HWND h, int offset);
	void SetChildYScrollOffset(HWND h, int offset);
	static LRESULT CALLBACK scrollProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	static LRESULT CALLBACK parentProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};


