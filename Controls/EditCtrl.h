/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Control.h"

#define CARETWIDTH		1
#define ES_DATE_MASK	1
#define ES_PHONE_MASK	2

typedef struct {
	int x;
	int y;
	int width;
	int height;
} SizeData;

class EditCtrl : public Control
{
public:
	const wchar_t* label;

	EditCtrl(HWND h, std::string name, int x, int y, int width, int height, int id, const wchar_t* text, bool parentIsDlg, DWORD styles = WS_CHILD | WS_VISIBLE, DWORD exStyles = 0);
	~EditCtrl();

	void DrawControl(HDC& hdc);
	void SelectAllOnFocus(bool res);
	void SelectAll();
	void Delete();
	void Redraw();

	void SetScroll(HDC hdc);


	int GetCharIndexUnderCursor(POINT pt);

	int GetCharRangeSize(HDC hdc, int from, int to);
	RECT GetCharRange(HDC hdc, int from, int to);

	bool HasFocus();
	bool DrawLabel(HDC& hdc);

	bool OnChar(char ch);
	void OnKeyDown(WPARAM wParam);
	void OnSetFocus();
	void OnKillFocus();
	void OnMouseMove(WPARAM wParam, LPARAM lParam);
	void OnLButtonDown(WPARAM wParam, LPARAM lParam);
	void SetLabel(const wchar_t* l);

	void OnTimer();

	SizeData GetSize();

	void SetEditValue(bool notifyParent = false);

	void SetFont(HDC hdc);
	void SetMask(int maskId);

	bool HandlePhoneMask(char ch);
	bool HandleDateMask(char ch);
	void OnDateMask();
	void OnPhoneMask();

	void PasteText();
private:
	HWND hWnd;
	HWND parent;
	bool isParentDlg;
	bool focus;
	wchar_t buf[MAX_PATH];
	int select, cursor;
	int curScrollX;
	int caretHeight;
	int borderSize;
	int paddingX;
	bool onlyNumbers;
	bool lockEditUpdate;
	bool dateMask, phoneMask;
	bool isPasswordType;

	bool selectAllOnFocus;

	static LRESULT CALLBACK ParentProces(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	static LRESULT CALLBACK CoreProces(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};


