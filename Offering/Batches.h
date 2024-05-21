/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "../Controls/TableCtrl.h"
#include "../Common/PageCommon.h"
#include "AddBatch.h"
#include "EnterContribution.h"
#include "../Reports/ContReports.h"


class Batches : public PageCommon 
{
public:
	Batches(HWND hWnd, HINSTANCE hInst);
	~Batches();

	void AddControls();

	void CreateTable();

	bool HandleEditBatch();
	void ShowDeleteBatchDialog();
	void DeleteBatch();

	void OnSearchByDescBatch();
	void OnSearchByDateBatch(bool isFrom);

	void UpdateBatchesTable(std::function<void(void)> onTableUpdateOverCallback = nullptr);

	void OnTableSort(int columnId, int type, DWORD_PTR context);


	void OnEnterContr(int batchId);

	void OnAddBatch(int batchId = 0);

	void SwitchBathesType(int type);

	void PrintContr(int batchId);

	void OnExport();

	bool OnCommand(WPARAM wParam);

	void OnSearchByDate();

	std::wstring GetCurrentBatchViewStatus();

	void ChangeCurrentPage(int page);
private:
	std::wstring currentSearchBy;
	std::wstring currentPage;
	std::wstring currentBatchViewStatus;
	std::wstring currentDateFrom, currentDateTo;
	std::vector<std::wstring> showLimits;
	TableCreater* table;
	TabControl* batchesSortTabs;
	static LRESULT CALLBACK batchesProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};