/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "../Controls/Controls.h"

class DlgCommon : protected Controls
{
public:
	DlgCommon(HWND hWnd);
	~DlgCommon();


	virtual void OnDlgInit();
	virtual void OnDlgClose();
	virtual bool OnBeforeDlgClose();
	virtual bool OnDlgCommand(WPARAM wParam);
	virtual void OnDlgResized();

	int OpenDlg(HWND hWnd, const char* title);

	template <typename T, INT_PTR(T::* P)(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)>
	INT_PTR DialogBoxThis(T* pThis, HINSTANCE hInstance, LPCSTR lpTemplateName, HWND hWndParent);

private:
	const char* dlgTitle;
protected:
	HWND hWnd;
	HINSTANCE hInst;
	int fixedDlgWidth, fixedDlgHeight;
	int minDlgWidth, minDlgHeight;
	int dlgPaddingX;
	INT_PTR CALLBACK DlgProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	void DoneLoadingDlg(bool removeLoadingPlaceholder = true);
};