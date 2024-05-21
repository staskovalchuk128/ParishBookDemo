/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "shlobj_core.h"
#include "../Common/PageCommon.h"

class PBResponse;

class PrivacySettings : public PageCommon
{
public:
	PrivacySettings(HWND hw, HINSTANCE hI);
	~PrivacySettings();

	void AddControls();
	bool OnCommand(WPARAM wParam);

private:
	void SaveSettings();
};
