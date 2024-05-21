/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "commctrl.h"
#include "../Controls/TableCtrl.h"
#include "../Common/DlgCommon.h"


struct ContEditItemData
{
	int id = 0;
	std::wstring name;
	std::wstring memo;
	int envelope = 0;
	int fund = 0;
	float amount = 0;
};


struct ContributionData 
{
	int id = 0, familyId = 0, memberId = 0, fundId = 0, lookUpId = 0, contType = 0, enteredEnvNum = 0;
	float totalAmount = 0, currentAmount = 0, enteredAmount = 0;
	std::wstring description, memo, fundName, enteredUserName, enteredMemo, statusOfMemberOrFamily;
	std::vector<ContEditItemData> editItemsData;
};


class EditContributionItem : public DlgCommon {
public:

	EditContributionItem(HWND hWnd, std::vector<int> itemIds, std::map<int, std::wstring> fundsList, ContributionData* Data);
	~EditContributionItem();

	void OnDlgInit();
	bool OnDlgCommand(WPARAM wParam);

	int ShowDlg();

	bool OnSaveContItem();

private:
	bool multiplyEdit;
	ContributionData* cData;

	std::map<int, int>lookUpTypesIndexes, conTypesIndexes, tableUserIndexes, searchUserIndexes;
	std::map<std::wstring, int> fundsIds;
	std::map<int, std::wstring> fundsList;
	std::vector<int> itemIds;

	void AddControls();
	void LoadData();
	static LRESULT CALLBACK ecProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};