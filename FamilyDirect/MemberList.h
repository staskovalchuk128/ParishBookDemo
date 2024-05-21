/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "../Controls/TableCtrl.h"
#include "../FamilyDirect/EditFamily.h"
#include "../Common/PageCommon.h"
#include "../Exporting/Exporting.h"

class MemberList : public PageCommon 
{
public:
	MemberList(HWND hWnd, HINSTANCE hInst);
	~MemberList();

	void AddControls();

	void UpdateMembersTable(std::function<void(void)> onTableUpdateOverCallback = nullptr);

	void CreateTable();

	bool OnDeleteMember();

	void OnSearchMember();

	bool HandleEditMember();

	void OnTableSort(int columnId, int type, DWORD_PTR context);


	void OnExport();


	bool OnCommand(WPARAM wParam);

	void OnSortByLetter(LetterSort item);
	void SwitchMemType(int type);
	void ChangeCurrentPage(int page);
private:
	std::wstring currentMemberViewType;
	std::wstring currentSearchBy;
	std::wstring currentPage;
	int currentSortByLetter;
	EditFamily* editFamily;
	std::vector<std::wstring> sortTabsIndexes;
	std::vector<std::wstring> showLimits;
	std::map<int, LetterSort> lettersIds;
	TableCreater* table;
	TabControl* memSortTabs;
	static LRESULT CALLBACK membersProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};