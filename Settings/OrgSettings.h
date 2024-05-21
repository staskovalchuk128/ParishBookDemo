/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "shlobj_core.h"
#include "../Common/PageCommon.h"

class PBResponse;

class OrgSettings : public PageCommon
{
public:
	OrgSettings(HWND hw, HINSTANCE hI);
	~OrgSettings();

	void AddControls();
	bool OnCommand(WPARAM wParam);

private:
	void OnOrgDataLoaded(PBResponse res);
	void SaveOrgSettings();
};
