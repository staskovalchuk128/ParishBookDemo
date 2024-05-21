/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "../Controls/TableCtrl.h"
#include "../Common/PageCommon.h"
#include "EditFamily.h"
#include "AddNewFamily.h"
#include "../Exporting/Exporting.h"
#include "../Reports/ContReports.h"

class FamilyList : public PageCommon 
{
public:
	FamilyList(HWND hWnd, HINSTANCE hInst);
	~FamilyList();

	void AddControls();

	void OnSearchFamily();

	void CreateTable();
	void ShowFamilyEdit();
	void ShowAddFamily();

	void OnDeleteFamily();

	bool HandleEditFamily();

	void OnSetEvnNum();

	void OnTableSort(int columnId, int type, DWORD_PTR context);

	void UpdateFamiliesTable(std::function<void(void)> onTableUpdateOverCallback = nullptr);

	void OnExport();

	bool OnCommand(WPARAM wParam);

	void OnSortByLetter(LetterSort item);

	void SwitchFamType(int type);

	void ChangeCurrentPage(int page);
private:
	std::wstring currentFamilyViewType;
	std::wstring currentSearchBy;
	std::wstring currentPage;
	int currentSortByLetter;
	std::map<int, LetterSort> lettersIds;
	TableCreater* table;
	int currentFamilyEditId;

	std::vector<std::wstring> sortTabsIndexes;
	std::vector<std::wstring> showLimits;

	EditFamily* editFamily;
	AddNewFamily* addNewFamily;

	TabControl* famSortTabs;
	static LRESULT CALLBACK familiesProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

};