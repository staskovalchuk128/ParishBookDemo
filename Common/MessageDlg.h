/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "..//Drawing/DrawCommon.h"
#include "../Controls/Controls.h"
#include "../Controls/ScrollBarCtrl.h"

#define MD_OK         1
#define MD_OKCALNCEL  2
#define MD_YESNO      3

#define MD_SUCCESS    4
#define MD_ERR        5
#define MD_WANING     6
#define MD_QUESTION   7
#define MD_PROMPT     8
#define MD_TEXTBOX    9

#define MIN_BODY_HEIGHT 50
#define MIN_DLG_HEIGHT 200


class MessageDlg :public Controls
{
public:
	MessageDlg(HWND hWnd, const wchar_t* title, const wchar_t* content, int btnsType, int dlgType = MD_SUCCESS);

	void OnDlgInit();
	void CreateDlgBody();

	int OpenDlg(bool btnSave = false);

	void DrawBodyIcon(HDC& hdcMem);
	void DrawBody(HDC& hdcMem);


	void ResizeDialog(int bodyHeight);
	void CalculateBodySize();

	bool SaveMessage();
	void SetPromptStr(const wchar_t* str);
	void SetTextBoxStr(std::wstring& str);


private:
	bool CheckPrompt();
	void ExtractTextBoxStr();

	template <typename T, INT_PTR(T::* P)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)>
	INT_PTR DialogBoxThis(T* pThis, HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent);

	INT_PTR CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK MainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

	std::wstring* textBoxStr;
	const wchar_t* promptStr;
	const wchar_t* content;
	const wchar_t* title;
	int btnsType;
	int dlgType;

	HWND hParent;
	HFONT dlgFont;

	int bodyWidth, bodyHeight, iconWidth;
	int dialogWidth;

	bool createSaveBtn;
	HWND dlgHwnd;
	ScrollBar* scroll;

};