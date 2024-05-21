/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "../Controls/TableCtrl.h"
#include "../Common/PageCommon.h"


class Recycle : public PageCommon 
{
public:
	Recycle(HWND hWnd, HINSTANCE hInst);
	~Recycle();

	void AddControls();
	bool OnCommand(WPARAM wParam);
	void UpdateTable(std::function<void(void)> callback = nullptr);
	void OnSearch();
	void DeleteItem();
	void RecoverItem();
private:
	TableCreater* table;

	std::map<std::wstring, std::wstring> rTableNames;
	static LRESULT CALLBACK Proc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};