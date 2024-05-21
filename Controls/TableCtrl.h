/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include <windows.h>
#include "Controls.h"
#include "../Drawing/DrawCommon.h"
#include "../Common/PageCommon.h"

class PBResponse;

static bool windowMaximized = false;


struct TableCData 
{
	std::wstring searchStr;
	std::string tableName, selectFields;
	std::string searchBy,addsRequestParams, addsJoints;
	std::wstring showLimit = L"25";

	TabControl* spBtns;

	int currentPage;
	std::vector<int> switchPageBtns;

	std::string currentSortBy;
	std::string currentSortType = "ASC";
	std::wstring sortByLetter;
	std::string sortByLetterColumn;

	std::vector<std::string> showLimits = { "25","50","100" };
	TableCData(HWND hWnd, HINSTANCE hInst)
	{
		currentPage = 0;
		if (!hWnd)
		{
			spBtns = NULL;
		}
		else 
		{
			spBtns = new TabControl(hWnd, Tabs(), 0, TCS_BOTTOM | TCS_FIXEDWIDTH, 10, 10, -10, 40, false, WS_STICK_BOTTOM);
			spBtns->SetTabsType(TAB_TYPES::TABLE_BTNS);
			SendMessage(spBtns->GetTabsHWND(), TCM_SETITEMSIZE, 0, MAKELPARAM(50, 40));
		}
	}
	~TableCData() 
	{
		if(spBtns) delete spBtns;
	}
	TabControl* GetBtns()
	{
		return spBtns;
	}

};
struct TableColumns
{
	std::wstring name;
	int width;
	std::string dbName;
	bool sortable;
	std::string colDataType = "text";
	std::string align = "left";
};

typedef struct
{
	HWND hlist;
	int  iSubItem;
	BOOL bSortAscending;
	std::string colDataType;
} TableSortCallbackStruct;

class TableCreater : public DrawCommon 
{
public:
	TableCreater(HWND hWnd, HINSTANCE hInst, Controls* controls);
	~TableCreater();


	void Create(const char* Id, int x, int y, int width = -1, int height = -1);


	TableColumns GetColumnInfo(int index);
	std::vector<TableColumns> GetColums();
	void SetRefClass(PageCommon* refClass);
	void CreateColumns(std::vector<TableColumns> items);
	void InsertColumns();
	void InsertColumn(int iCol, const wchar_t* text, int iWidth, std::string align = "left");

	void InsertItem(const wchar_t* text, int col);
	void InsertItem(std::wstring text, int col);
	void InsertItem(int text, int col);
	void InsertItem(float text, int col);

	void InsertRow(int i = 0);
	void ClearTable();
	void ResetScrollSize();

	LVITEMW GetSelectedRow();
	std::vector<LVITEMW> GetSelectedRows();

	int GetSelectedRowId();
	std::vector<int> GetSelectedRowIds();

	void DeleteRow(int id = -1);


	static int CALLBACK SortCompFunction(LPARAM lp1, LPARAM lp2, LPARAM sortData);
	bool OnColumnClick(LPNMLISTVIEW pLVInfo);
	void SetListViewSortIcon(HWND listView, int col, int sortOrder);
	bool IsSortableCol(int col);

	void CreateSwitchPagesBtns();
	void UpdateItem(int item, int subItem, std::wstring text);
	int UpdateTable(std::string action, std::map<std::string, std::wstring> params = {}, std::function<void(void)> onTableUpdateOverCallback = nullptr);
	void OnServerRespond(PBResponse response);
	void SetSearchStr(std::wstring searchStr);
	void SetSearchBy(std::string searchBy);
	void SetSearchShowLimit(std::wstring showLimit);
	void SetSortByLetter(std::wstring letter);
	void SetSortBy(std::string sortBy);
	void SetSortType(std::string sortT);
	void SetAddsRequest(std::string adds);
	void SetCurrentPage(int p);
	std::wstring GetTextFromCell(int row, int col);

	void ResizeColumns();

	std::string GetColTextAlign(int itemId);

	HWND GetHWND();
	const TableCData* GetTableCData();
	void SetTableCData(TableCData *d);

	void SetScrollOffset();
	void DrawTableHeader(HDC& hdc);
	void DrawTableBody(HDC& hdc);


	void SetScrollWheelOffert(int val);

	void SetTableSortCallback(std::function<void(int, int, DWORD_PTR)> _callback);
	
	void AllowMultiSelect();

	void SetNeedSwitchPageBtns(bool r);

	void ResortTable(TableSortCallbackStruct* data = NULL);
	void SetSortingParams(int col, bool asc);
private:
	Controls* con;
	long long nSortColumn = -1;
	bool bSortAscending = true;
	HWND hWnd = NULL;
	HWND hWndParent = NULL;
	HINSTANCE hInst = NULL;
	bool needSwitchPageBtns;

	int totalItemsFromRequest;
	int textMargin = 10;
	int bordersSize = 1;
	int totalHeaderWidth = 0;
	int headerHeight = 50;
	int rowHeight = 40;


	int x, y;
	int tableWidth, tableHeight;
	HWND hwndList;

	ScrollBar* tableScroll;
	std::string tableId;

	PageCommon* scClass;
	std::function<void(int, int, DWORD_PTR)> onTableSortCallback;


	TableCData* cData;


	std::vector<TableColumns> currentColumns;
	std::vector<int> GetSelectedRowIndexes();

	static LRESULT CALLBACK TableProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	static LRESULT CALLBACK ParentTableProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	void FillTableWithData(std::vector<std::map<std::string, std::wstring>> data);

	std::string prevRequestAction;
	std::map<std::string, std::wstring> prevRequestParams;
	std::vector <std::map<std::string, std::wstring> > tempTableData;


	std::function<void(void)> onTableUpdateOverCallback;
}; 