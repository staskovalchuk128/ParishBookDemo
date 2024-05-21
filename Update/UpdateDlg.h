/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "Commdlg.h"
#include "commctrl.h"
#include "../Updater.h"
#include "../Common/MessageDlg.h"
#include "../Common/DlgCommon.h"


class UpdateDlg : private DlgCommon, private Updater
{
public:
	UpdateDlg(HWND hWnd);
	~UpdateDlg();

	void ShowDlg();
	void OnDlgInit();
	void OnDlgClose();
	bool OnDlgCommand(WPARAM wParam);


	void StartUpdating();
};
