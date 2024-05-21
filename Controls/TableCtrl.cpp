#include "TableCtrl.h"
#include <vector>
#include <map>
#include "..//Connection/PBResponse.h"
#include "..//PBConnection.h"
#include "..//Components/PreloaderComponent.h"

TableCreater::TableCreater(HWND hWnd, HINSTANCE hInst, Controls* con) : con(con)
{
	needSwitchPageBtns = true;
	onTableUpdateOverCallback = nullptr;
	onTableSortCallback = nullptr;
	cData = NULL;
	nSortColumn = -1;
	hwndList = NULL;

	textMargin = 10;
	bordersSize = 1;
	totalHeaderWidth = 0;
	headerHeight = 50;
	rowHeight = 40;

	x = y = 0;
	tableWidth = tableHeight = 0;

	tableScroll = NULL;

	scClass = NULL;

	this->hWnd = hWnd;
	hWndParent = GetParent(hWnd);
	this->hInst = hInst;
}

TableCreater::~TableCreater()
{
	HWND hHeader = ListView_GetHeader(hwndList);

	RemoveWindowSubclass(hWndParent, ParentTableProcess, IDC_STATIC);
	RemoveWindowSubclass(hWnd, ParentTableProcess, IDC_STATIC);
	RemoveWindowSubclass(hwndList, TableProcess, con->GetControlId(tableId));
	RemoveWindowSubclass(hHeader, TableProcess, 1);

	con->RemoveControl(hwndList);

	delete cData;
	delete tableScroll;
}

void TableCreater::Create(const char* id, int x, int y, int width, int height)
{
	this->x = x;
	this->y = y;
	this->tableId = std::string(id);

	hWndParent = GetParent(hWnd);

	SetWindowSubclass(hWndParent, ParentTableProcess, IDC_STATIC, (DWORD_PTR)this);
	SetWindowSubclass(hWnd, ParentTableProcess, IDC_STATIC, (DWORD_PTR)this);
	HWND test = GetParent(hWnd);

	hwndList = con->CreateCustom(id, WC_LISTVIEW, x, y, width, height, WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SINGLESEL | LVS_OWNERDRAWFIXED);

	tableScroll = new ScrollBar(hwndList, hInst);
	tableScroll->CreateScroll(21, -1, rowHeight * 3);

	HWND hHeader = ListView_GetHeader(hwndList);

	SetWindowSubclass(hwndList, TableProcess, con->GetControlId(tableId), (DWORD_PTR)this);
	SetWindowSubclass(hHeader, TableProcess, 1, (DWORD_PTR)this);
}

void TableCreater::AllowMultiSelect()
{
	// remove default scroll
	LONG_PTR styles = GetWindowLongPtr(hwndList, GWL_STYLE);
	SetWindowLongPtr(hwndList, GWL_STYLE, styles & ~LVS_SINGLESEL);
}

void TableCreater::SetRefClass(PageCommon* refClass)
{
	scClass = refClass;
}

HWND TableCreater::GetHWND()
{
	return hwndList;
}

const TableCData* TableCreater::GetTableCData()
{
	return cData;
}

void TableCreater::SetTableCData(TableCData* d)
{
	if (cData) delete cData;
	cData = d;
}

void TableCreater::SetScrollWheelOffert(int val)
{
	tableScroll->SetWheelOffset(rowHeight * val);
}

void TableCreater::ClearTable()
{
	if (hwndList)
	{
		SendMessage(hwndList, LVM_DELETEALLITEMS, NULL, 0);
	}
}

void TableCreater::ResetScrollSize()
{
	tableScroll->ResetSize();
}

void TableCreater::DeleteRow(int id)
{
	if (id == -1)
	{
		LRESULT iSelect = SendMessage(hwndList, LVM_GETNEXTITEM,
			-1, LVNI_SELECTED | LVNI_FOCUSED);

		SendMessage(hwndList, LVM_DELETEITEM, iSelect, 0);
	}
	else
	{
		SendMessage(hwndList, LVM_DELETEITEM, id, 0);
	}

}

void TableCreater::UpdateItem(int item, int subItem, std::wstring text)
{
	ListView_SetItemText(hwndList, item, subItem, (LPWSTR)text.c_str());
}

std::vector<TableColumns> TableCreater::GetColums()
{
	return currentColumns;
}

TableColumns TableCreater::GetColumnInfo(int index)
{
	if (index > currentColumns.size() - 1)
	{
		throw std::string("Colunm not found");
	}

	return currentColumns[index];
}

void TableCreater::CreateColumns(std::vector<TableColumns> items)
{
	currentColumns = items;
}

void TableCreater::InsertColumns()
{
	HWND hHeader = ListView_GetHeader(hwndList);

	size_t currentColCount = Header_GetItemCount(hHeader);

	// delete prev colums
	while (currentColCount > 0)
	{
		Header_DeleteItem(hHeader, 0);
		currentColCount = Header_GetItemCount(hHeader);
	}

	for (int i = 0; i < currentColumns.size(); i++)
	{
		InsertColumn(i, currentColumns[i].name.c_str(), currentColumns[i].width, currentColumns[i].align);
	}

	size_t headerCount = Header_GetItemCount(hHeader);

	for (size_t i = 0; i < headerCount; i++)
	{
		HDITEM hi = { 0 };

		hi.mask = HDI_FORMAT;
		Header_GetItem(hHeader, i, &hi);
		hi.fmt = HDF_OWNERDRAW;
		Header_SetItem(hHeader, i, &hi);
	}
}

void TableCreater::ResizeColumns()
{
	float dWidth = 0.0;
	float colWidth = 0;
	RECT rc;

	con->OnResize();

	int scrollSize = GetSystemMetrics(SM_CXVSCROLL);

	GetWindowRect(hwndList, &rc);
	int width = rc.right - rc.left - scrollSize;

	for (size_t i = 0; i < currentColumns.size(); i++)
	{
		dWidth = (float)currentColumns[i].width / 100.f;
		colWidth = dWidth * width;
		ListView_SetColumnWidth(hwndList, i, colWidth);
	}
}

void TableCreater::InsertColumn(int iCol, const wchar_t* text, int iWidth, std::string align)
{
	LVCOLUMN lvc;
	float colWidth = 0;
	float dWidth;

	int scrollSize = GetSystemMetrics(SM_CXVSCROLL);

	RECT rc;
	GetWindowRect(hwndList, &rc);
	int width = rc.right - rc.left - scrollSize;
	dWidth = (float)iWidth / 100.f;

	colWidth = dWidth * width;
	DWORD tAlign = LVCFMT_LEFT;
	if (align == "center") tAlign = LVCFMT_CENTER;
	if (align == "right") tAlign = LVCFMT_RIGHT;

	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM | LVCF_IMAGE;
	lvc.fmt = LVCFMT_CENTER;
	lvc.cx = static_cast<int> (colWidth);
	lvc.pszText = (LPWSTR)text;
	lvc.iSubItem = iCol;
	lvc.iImage = 1;
	ListView_InsertColumn(hwndList, iCol, &lvc);
}

void TableCreater::InsertRow(int i)
{
	LVITEMW lvi = { 0 };

	lvi.mask = LVIF_PARAM;
	lvi.iSubItem = 0;
	lvi.lParam = i;
	ListView_InsertItem(hwndList, &lvi);
}

void TableCreater::InsertItem(const wchar_t* text, int col)
{
	ListView_SetItemText(hwndList, 0, col, (LPWSTR)text);
}

void TableCreater::InsertItem(std::wstring text, int col)
{
	ListView_SetItemText(hwndList, 0, col, (LPWSTR)text.c_str());
}

void TableCreater::InsertItem(int text, int col)
{
	char cId[128];
	sprintf_s(cId, "%i", text);
	ListView_SetItemText(hwndList, 0, col, (LPWSTR)cId);
}

void TableCreater::InsertItem(float text, int col)
{
	char cId[128];
	sprintf_s(cId, "%.2f", text);
	ListView_SetItemText(hwndList, 0, col, (LPWSTR)cId);
}

LVITEMW TableCreater::GetSelectedRow()
{
	LVITEMW item = { 0 };

	item.mask = LVIF_PARAM;
	item.iItem = ListView_GetNextItem(hwndList, -1, LVNI_SELECTED);

	ListView_GetItem(hwndList, &item);

	return item;

}

std::vector<LVITEMW> TableCreater::GetSelectedRows()
{
	std::vector<LVITEMW> items;

	int iPos = -1;
	do
	{
		LVITEMW item = { 0 };
		item.mask = LVIF_PARAM;
		iPos = ListView_GetNextItem(hwndList, iPos, LVNI_SELECTED);
		if (iPos == -1) break;
		item.iItem = iPos;
		ListView_GetItem(hwndList, &item);
		items.push_back(item);

	} while (iPos != -1);

	return items;
}

std::vector<int> TableCreater::GetSelectedRowIndexes()
{
	std::vector<int> ids;
	std::vector<LVITEMW> items = GetSelectedRows();
	for (auto& it : items)
	{
		ids.push_back((int)it.iItem);
	}
	return ids;
}

int TableCreater::GetSelectedRowId()
{
	int id = -1;
	LVITEMW item = GetSelectedRow();
	if (item.iItem < 0) return id;
	id = static_cast<int> (item.lParam);
	return id;
}

std::vector<int> TableCreater::GetSelectedRowIds()
{
	std::vector<int> ids;
	std::vector<LVITEMW> items = GetSelectedRows();
	for (auto &it: items)
	{
		ids.push_back((int)it.lParam);
	}
	return ids;
}

bool TableCreater::IsSortableCol(int col)
{
	try
	{
		TableColumns colInfo = GetColumnInfo(col);
		return colInfo.sortable;
	}

	catch (std::string s) {}

	catch (std::exception e) {}

	return false;
}


int CALLBACK TableCreater::SortCompFunction(LPARAM lp1, LPARAM lp2, LPARAM sortParam)
{
	TableSortCallbackStruct* data = (TableSortCallbackStruct*)sortParam;
	
	WCHAR buf1[1024] = { 0 };
	WCHAR buf2[1024] = { 0 };
	INT iNum1 = 0, iNum2 = 0;
	INT iIndex = 0;

	LVFINDINFO ItemInfo;

	ItemInfo.flags = LVFI_PARAM;

	ItemInfo.lParam = lp1;
	iIndex = ListView_FindItem(data->hlist, -1, &ItemInfo);
	ListView_GetItemText(data->hlist, iIndex, data->iSubItem, buf1, sizeof(buf1));

	ItemInfo.lParam = lp2;
	iIndex = ListView_FindItem(data->hlist, -1, &ItemInfo);
	ListView_GetItemText(data->hlist, iIndex, data->iSubItem, buf2, sizeof(buf2));

	int res = 0;

	if (data->colDataType == "int")
	{
		res = GetInt(buf1) > GetInt(buf2);
	}
	else if (data->colDataType == "float")
	{
		res = GetFloat(buf1) > GetFloat(buf2);
	}
	else
	{
		res = wcscmp(buf1, buf2);
	}
	return data->bSortAscending ? res <= 0 : res >= 0;
}

bool TableCreater::OnColumnClick(LPNMLISTVIEW pLVInfo)
{
	if (currentColumns.size() == 0) return false;

	// get new sort parameters
	if (pLVInfo->iSubItem == nSortColumn)
		bSortAscending = !bSortAscending;
	else
	{
		nSortColumn = pLVInfo->iSubItem;
		bSortAscending = TRUE;
	}

	if (GetKeyState(VK_SHIFT) & 0x8000)
	{
		ListView_SetItemState(hwndList, -1, LVIS_SELECTED, LVIS_SELECTED);
		return true;
	}

	if (!IsSortableCol(pLVInfo->iSubItem)) return false;

	if (scClass != NULL)
	{
		try
		{
			scClass->OnTableSort(pLVInfo->iSubItem, bSortAscending + 1, (DWORD_PTR)this);
		}

		catch (std::exception e) {}
	}
	else if (onTableSortCallback != nullptr)
	{

		TableSortCallbackStruct data;
		data.hlist = pLVInfo->hdr.hwndFrom;
		data.iSubItem = (int)pLVInfo->iSubItem;
		data.bSortAscending = bSortAscending;
		data.colDataType = currentColumns[data.iSubItem].colDataType;


		ResortTable();
	}

	SetListViewSortIcon(pLVInfo->hdr.hwndFrom, static_cast<int> (pLVInfo->iSubItem), bSortAscending + 1);

	return true;
}

void TableCreater::SetSortingParams(int col, bool asc)
{
	nSortColumn = col;
	bSortAscending = asc;
}

void TableCreater::ResortTable(TableSortCallbackStruct* data)
{
	bool needDelete = false;
	if (!data)
	{
		if (nSortColumn == -1) return;
		data = new TableSortCallbackStruct();
		data->hlist = hwndList;
		data->iSubItem = (int)nSortColumn;
		data->bSortAscending = bSortAscending;
		data->colDataType = currentColumns[nSortColumn].colDataType;
		needDelete = true;
	}
	ListView_SortItems(data->hlist, &TableCreater::SortCompFunction, data);
	if (needDelete) delete data;
	//ListView_SortItems(pLVInfo->hdr.hwndFrom, &TableCreater::SortCompFunction, data);
}

void TableCreater::SetListViewSortIcon(HWND listView, int col, int sortOrder)
{
	HWND headerWnd;
	const int bufLen = 256;
	wchar_t headerText[bufLen] = { 0 };

	HD_ITEM item;

	int numColumns, curCol;
	headerWnd = ListView_GetHeader(listView);
	numColumns = Header_GetItemCount(headerWnd);

	for (curCol = 0; curCol < numColumns; curCol++)
	{
		item.mask = HDI_FORMAT | HDI_TEXT;
		item.pszText = headerText;
		item.cchTextMax = bufLen - 1;
		SendMessage(headerWnd, HDM_GETITEM, curCol, (LPARAM)&item);

		if ((sortOrder != 0) && (curCol == col))
			switch (sortOrder)
			{
			case 1:
				item.fmt &= !HDF_SORTUP;
				item.fmt |= HDF_SORTDOWN;
				break;
			case 2:
				item.fmt &= !HDF_SORTDOWN;
				item.fmt |= HDF_SORTUP;
				break;
			}
		else
		{
			item.fmt &= !HDF_SORTUP & !HDF_SORTDOWN;
		}

		item.fmt |= HDF_STRING | HDF_OWNERDRAW;
		item.mask = HDI_FORMAT | HDI_TEXT;
		SendMessage(headerWnd, HDM_SETITEM, curCol, (LPARAM)&item);
	}
}

void TableCreater::CreateSwitchPagesBtns()
{
	cData->spBtns->DeleteAllTabs();

	int countAll = totalItemsFromRequest;
	int showLimit = GetInt(cData->showLimit.length() == 0 ? L"25" : cData->showLimit);

	cData->currentPage = cData->currentPage <= 0 ? 1 : cData->currentPage;

	int pageCount = 0;
	if (0 == countAll) {}
	else
	{
		pageCount = static_cast<int> (ceil(static_cast<float> (countAll) / static_cast<float> (showLimit)));

		if (cData->currentPage > pageCount)
		{
			cData->currentPage = 1;
		}
	}

	Tabs tabs;

	int startPageCount, endPageCount;
	int maxButtons = 1000;
	int centerPos = static_cast<int> (ceil(static_cast<float> (maxButtons) / 2.0));
	int centerOffset = static_cast<int> (round(static_cast<float> (maxButtons) / 2.0));

	if (pageCount > 1)
	{
		if (cData->currentPage > centerPos) startPageCount = cData->currentPage - 2;
		else startPageCount = 1;
		endPageCount = startPageCount + (maxButtons - 1);
		if (endPageCount > pageCount)
		{
			endPageCount = pageCount;
			startPageCount = pageCount - (maxButtons - 1);
		}

		if (startPageCount < 1) startPageCount = 1;

		for (int i = startPageCount, j = 0; i <= endPageCount; i++, j++)
		{
			cData->spBtns->InsertItem(std::to_wstring(i), j, 0, NULL, i);
			tabs.AddTab(i, std::to_wstring(i));
		}

		cData->spBtns->SelectTab(cData->currentPage);

		ShowWindow(cData->spBtns->GetTabsHWND(), SW_SHOW);
	}
	else
	{
		ShowWindow(cData->spBtns->GetTabsHWND(), SW_HIDE);
	}
}

void TableCreater::SetSearchStr(std::wstring searchStr)
{
	cData->searchStr = searchStr;
}

void TableCreater::SetSearchBy(std::string searchBy)
{
	cData->searchBy = searchBy;
}

void TableCreater::SetSearchShowLimit(std::wstring showLimit)
{
	cData->showLimit = showLimit;
}

void TableCreater::SetSortByLetter(std::wstring letter)
{
	cData->sortByLetter = letter;
}

void TableCreater::SetSortBy(std::string sortBy)
{
	cData->currentSortBy = sortBy;
}

void TableCreater::SetSortType(std::string sortT)
{
	cData->currentSortType = sortT;
}

void TableCreater::SetAddsRequest(std::string adds)
{
	cData->addsRequestParams = adds;
}

void TableCreater::SetCurrentPage(int p)
{
	cData->currentPage = p;
}

std::wstring TableCreater::GetTextFromCell(int row, int col)
{
	wchar_t buff[1024] = { 0 };
	ListView_GetItemText(hwndList, row, col, buff, sizeof(buff));
	return std::wstring(buff);
}


int TableCreater::UpdateTable(std::string action, std::map<std::string, std::wstring> params, std::function<void(void)> callback)
{
	ClearTable();


	onTableUpdateOverCallback = callback;

	prevRequestAction = action;
	prevRequestParams = params;

	PreloaderComponent::Start(hWnd);


	PBConnection::Request(action,
		{
			params
		},
		std::bind(&TableCreater::OnServerRespond, this, std::placeholders::_1)
	);

	return 1;
}

void TableCreater::SetNeedSwitchPageBtns(bool r)
{
	needSwitchPageBtns = r;
}



void TableCreater::OnServerRespond(PBResponse response)
{

	if (!response.Succeed())
	{
		PreloaderComponent::Stop();
		if (onTableUpdateOverCallback != nullptr) onTableUpdateOverCallback();
		MessageDlg(hWnd, L"Database Error", response.GetError().c_str(), MD_OK).OpenDlg();
		return;
	}

	if (needSwitchPageBtns)
	{
		tempTableData = response.GetResult();

		//Count all items
		prevRequestParams["countAllItems"] = L"true";
		PBConnection::Request(prevRequestAction,
			{
				prevRequestParams
			},
			[&](PBResponse countResponse)
			{
				PreloaderComponent::Stop();

				if (countResponse.GetResult().size() == 1)
				{
					std::map<std::string, std::wstring> countRes = countResponse.GetResult()[0];
					totalItemsFromRequest = GetInt(countRes["countAllItems"]);
				}
				else totalItemsFromRequest = 0;

				FillTableWithData(tempTableData);

				CreateSwitchPagesBtns();


				if (onTableUpdateOverCallback != nullptr) onTableUpdateOverCallback();
			}
		);
	}
	else
	{
		PreloaderComponent::Stop();
		
		FillTableWithData(response.GetResult());

		if (onTableUpdateOverCallback != nullptr) onTableUpdateOverCallback();

	}

}

void TableCreater::FillTableWithData(std::vector<std::map<std::string, std::wstring>> data)
{
	ClearTable();

	std::vector<TableColumns> columns = GetColums();
	SendMessage(hwndList, WM_SETREDRAW, FALSE, 0);

	for (std::vector<std::map<std::string, std::wstring>>::reverse_iterator it = data.rbegin(); it != data.rend(); ++it)
	{
		InsertRow(GetInt((*it)["id"]));
		for (int i = 0; i < columns.size(); i++)
		{
			InsertItem((*it)[columns[i].dbName], i);
		}
	}

	SendMessage(hwndList, WM_SETREDRAW, TRUE, 0);
}

std::string TableCreater::GetColTextAlign(int itemId)
{
	std::string textAlign = "left";
	if (itemId >= 0 && itemId < currentColumns.size()) return currentColumns[itemId].align;
	return textAlign;
}

void TableCreater::DrawTableHeader(HDC& hdc)
{
	HWND hHeader = ListView_GetHeader(hwndList);

	RECT tableRC;
	int sortIX = 20, sortIY = 5;	// sort icon sizes
	RECT tabRc = { 0, 0, 0, 0 };

	GetWindowRect(hHeader, &tableRC);

	int width = tableRC.right - tableRC.left;
	int height = tableRC.bottom - tableRC.top;

	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP bitmap = CreateCompatibleBitmap(hdc, width, height);
	HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, bitmap);

	int itemsCount = Header_GetItemCount(hHeader);

	Pen whitePen(GetColor(APP_COLORS::WHITE));
	Pen headerBorderColor(Color(255, 37, 51, 65));
	SolidBrush headerColor(GetColor(APP_COLORS::DEF_DARK_BLUE));
	SolidBrush whiteColor(GetColor(APP_COLORS::WHITE));

	Graphics gr(hdcMem);

	gr.FillRectangle(&headerColor, bordersSize, bordersSize, width - bordersSize, height - bordersSize);

	for (int i = 0; i < itemsCount; i++)
	{
		gr.ResetClip();
		std::string textAlign = GetColTextAlign(i);

		wchar_t buff[100] = { 0 };

		HDITEM hd = { 0 };

		hd.mask = HDI_FORMAT | HDI_TEXT | HDI_HEIGHT;
		hd.cchTextMax = 100;
		hd.pszText = buff;
		Header_GetItem(hHeader, i, &hd);

		Header_GetItemRect(hHeader, i, &tabRc);

		int tabWidth = tabRc.right - tabRc.left;
		int tabHeight = tabRc.bottom - tabRc.top;

		gr.DrawRectangle(&headerBorderColor, tabRc.left, 0, tabWidth, height);

		RectF layoutRect(0, 0, -1, (REAL)tabRc.bottom);
		RectF boundRect;

		gr.MeasureString(buff, (INT)wcslen(buff), currentFont, layoutRect, &boundRect);

		PointF origin;

		origin.Y = tabRc.top + tabHeight / 2 - boundRect.Height / 2;

		if (textAlign == "center")
		{
			origin.X = (REAL)(tabRc.left + tabWidth / 2 - boundRect.Width / 2);
		}
		else if (textAlign == "right")
		{
			origin.X = (REAL)(tabRc.left + width - boundRect.Width - textMargin);
		}
		else
		{
			origin.X = (REAL)(tabRc.left + textMargin);
		}

		if (boundRect.Width > tabWidth)
		{
			Rect stringRC = { tabRc.left, tabRc.top, tabWidth, height
			};

			gr.SetClip(stringRC);
		}

		gr.DrawString(buff, (INT)wcslen(buff), currentFont, origin, &whiteColor);

		if (hd.fmt & HDF_SORTUP)
		{
			gr.SetSmoothingMode(SmoothingModeHighQuality);
			Point p1(tabRc.left + tabWidth - sortIX, sortIY), p2(tabRc.left + tabWidth - sortIX + sortIY, sortIY * 2);
			Point p11(tabRc.left + tabWidth - sortIX + sortIY, sortIY * 2), p22(tabRc.left + tabWidth - sortIX + sortIY * 2, sortIY);
			gr.DrawLine(&whitePen, p1, p2);
			gr.DrawLine(&whitePen, p11, p22);
		}
		else if (hd.fmt & HDF_SORTDOWN)
		{
			gr.SetSmoothingMode(SmoothingModeHighQuality);
			Point p1(tabRc.left + tabWidth - sortIX, sortIY * 2), p2(tabRc.left + tabWidth - sortIX + sortIY, sortIY);
			Point p11(tabRc.left + tabWidth - sortIX + sortIY, sortIY), p22(tabRc.left + tabWidth - sortIX + sortIY * 2, sortIY * 2);
			gr.DrawLine(&whitePen, p1, p2);
			gr.DrawLine(&whitePen, p11, p22);
		}
	}

	gr.DrawRectangle(&headerBorderColor, tabRc.right, 0, width - tabRc.right, height);

	BitBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);

	SelectObject(hdcMem, oldBitmap);
	DeleteObject(bitmap);
	DeleteDC(hdcMem);

}

void TableCreater::DrawTableBody(HDC& hdc)
{
	RECT tableRC;
	GetWindowRect(hwndList, &tableRC);

	int width = tableRC.right - tableRC.left;
	int height = tableRC.bottom - tableRC.top;

	HDC hdcMem = CreateCompatibleDC(hdc);
	HBITMAP bitmap = CreateCompatibleBitmap(hdc, width, height);
	HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, bitmap);

	int itemsCount = ListView_GetItemCount(hwndList);
	HWND hHeader = ListView_GetHeader(hwndList);

	Color clr;
	SolidBrush oddColor(Color(255, 246, 246, 246));
	SolidBrush evenColor(Color(255, 231, 231, 231));
	SolidBrush selectedItemBrush(Color(255, 209, 220, 232));
	Pen borderColor(GetColor(APP_COLORS::GRAY_BORDER));
	SolidBrush blackBrush(GetColor(APP_COLORS::BLACK));

	int columnsCount = Header_GetItemCount(hHeader);

	int scrollPosX = GetScrollPos(hwndList, SB_HORZ);
	std::vector<int> selectedRows = GetSelectedRowIndexes();

	int itemsCountPerPage = ListView_GetCountPerPage(hwndList);

	RECT cItemRC = { 0 };

	int startFrom = 0;
	int iPos = ListView_GetNextItem(hwndList, -1, LVNI_ALL);
	while (iPos != -1)
	{
		ListView_GetItemRect(hwndList, iPos, &cItemRC, LVIR_BOUNDS);
		if (cItemRC.top - headerHeight >= 0)
		{
			startFrom = iPos;
			break;
		}

		iPos = ListView_GetNextItem(hwndList, iPos, LVNI_ALL);
	}

	Graphics gr(hdcMem);

	RECT colRc = { 0, 0, 0, 0 };

	int endAt = startFrom + itemsCountPerPage;

	for (int itemId = startFrom, offsetTop = headerHeight; itemId <= endAt; itemId++, offsetTop += rowHeight)
	{
		// Reseting clip 
		gr.ResetClip();

		// check if there is item in the table
		bool itemExists = itemId < itemsCount;

		// Pick the color for odd and even rows
		if (itemId % 2 == 0) evenColor.GetColor(&clr);
		else oddColor.GetColor(&clr);

		// pick the color for the selected rows
		if (std::find(selectedRows.begin(), selectedRows.end(), itemId) != selectedRows.end())
		{
			selectedItemBrush.GetColor(&clr);
		}

		// create brish with the selected color
		SolidBrush currentBg(clr);

		// Fill the row rectangle
		gr.FillRectangle(&currentBg, 0, bordersSize, width, rowHeight);

		// go thrue all columns and draw subitems
		for (int i = 0; i < columnsCount; i++)
		{
			// Reseting clip again
			gr.ResetClip();
			Header_GetItemRect(hHeader, i, &colRc);

			colRc.left -= scrollPosX;
			colRc.right -= scrollPosX;

			gr.DrawRectangle(&borderColor, colRc.left, 0, colRc.right - colRc.left, rowHeight);

			//if row is without text then just continue, no need to draw any text
			if (itemExists == false) continue;

			int colWidth = ListView_GetColumnWidth(hwndList, i);

			RECT subItemRect;
			std::string textAlign = GetColTextAlign(i);

			wchar_t buff[2048] = { 0 };

			ListView_GetItemText(hwndList, itemId, i, buff, sizeof(buff));

			ListView_GetSubItemRect(hwndList, itemId, i, LVIR_BOUNDS, &subItemRect);

			RectF layoutRect(0, 0, -1, (REAL)colRc.bottom);
			RectF boundRect;

			gr.MeasureString(buff, (INT)wcslen(buff), currentFont, layoutRect, &boundRect);

			PointF origin;
			origin.Y = rowHeight / 2 - boundRect.Height / 2;

			if (textAlign == "center")
			{
				origin.X = (REAL)(subItemRect.left + colWidth / 2 - boundRect.Width / 2);
			}
			else if (textAlign == "right")
			{
				origin.X = (REAL)(subItemRect.right - boundRect.Width - textMargin);
			}
			else
			{
				origin.X = (REAL)(subItemRect.left + textMargin);
			}

			if (boundRect.Width > colWidth)
			{
				Rect stringRC = { colRc.left, colRc.top, colWidth, rowHeight
				};

				gr.SetClip(stringRC);
			}

			gr.DrawString(buff, (INT)wcslen(buff), currentFont, origin, &blackBrush);
		}

		gr.DrawRectangle(&borderColor, colRc.right, 0, width - colRc.right - 1, rowHeight);

		BitBlt(hdc, 0, offsetTop, width, rowHeight, hdcMem, 0, 0, SRCCOPY);
	}

	// Draw bottom line
	gr.DrawLine(&borderColor, 0, 1, width, 1);
	BitBlt(hdc, 0, height - 1, width, height, hdcMem, 0, 0, SRCCOPY);

	SelectObject(hdcMem, oldBitmap);
	DeleteObject(bitmap);
	DeleteDC(hdcMem);

}

void TableCreater::SetScrollOffset()
{
	RECT colRc;
	HWND hHeader = ListView_GetHeader(hwndList);
	int itemsCount = ListView_GetItemCount(hwndList);

	int columnCount = Header_GetItemCount(hHeader);

	int totalWidth = 0;

	for (int i = 0; i < columnCount; i++)
	{
		Header_GetItemRect(hHeader, i, &colRc);
		totalWidth += colRc.right - colRc.left;
	}

	tableScroll->SetVScrollRange(headerHeight + rowHeight * itemsCount);
	tableScroll->SetHScrollRange(totalWidth);

}

void TableCreater::SetTableSortCallback(std::function<void(int, int, DWORD_PTR)> _callback)
{
	onTableSortCallback = _callback;
}

LRESULT CALLBACK TableCreater::TableProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	HDC hdc = (HDC)wParam;
	TableCreater* lpData = (TableCreater*)dwRefData;
	int id = lpData->con->GetControlId(lpData->tableId);
	HWND hListView = lpData->con->GetControlHWND(lpData->tableId);

	switch (uMsg)
	{
	case WM_NCCALCSIZE:
	{
		// remove default scroll
		if (uIdSubclass != id) break;
		LONG_PTR styles = GetWindowLongPtr(hWnd, GWL_STYLE);
		if ((styles & WS_VSCROLL) == WS_VSCROLL)
		{
			SetWindowLongPtr(hWnd, GWL_STYLE, styles & ~WS_VSCROLL);
		}

		if ((styles & WS_HSCROLL) == WS_HSCROLL)
		{
			SetWindowLongPtr(hWnd, GWL_STYLE, styles & ~WS_HSCROLL);
		}

		return TRUE;
	}

	case LVM_SCROLL:
	{
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}

	case LVM_ENSUREVISIBLE:
	{
		InvalidateRect(hWnd, NULL, FALSE);
		break;
	}

	case HDM_LAYOUT:
	{ 
		// Moving scrollbar when changing focused item
		{
			hListView = lpData->con->GetControlHWND(lpData->tableId);
			int topIndex = ListView_GetTopIndex(hListView);

			int totalItems = ListView_GetItemCount(hListView);
			int itemsPerPage = ListView_GetCountPerPage(hListView);

			int scrollVal = static_cast<int> ((double)(topIndex) / ((double)totalItems - (double)itemsPerPage) * (double)100);

			int scrollPos = lpData->tableScroll->GetScrollBarSize(SB_VERT) * scrollVal / 100;

			lpData->tableScroll->SetPos(lpData->tableScroll->GetVScrollHWND(), scrollPos);
		}

		LPHDLAYOUT pHL = reinterpret_cast<LPHDLAYOUT> (lParam);
		RECT* pRect = pHL->prc;
		WINDOWPOS* pWPos = pHL->pwpos;
		LRESULT r = DefSubclassProc(hWnd, uMsg, wParam, lParam);
		pWPos->cy = lpData->headerHeight;	// Header height
		pRect->top = lpData->headerHeight;	// Header offset top
		InvalidateRect(hListView, NULL, FALSE);
		lpData->SetScrollOffset();

		return r;
	}

	case WM_SIZE:
	{
		if (uIdSubclass != id) break;
		lpData->SetScrollOffset();
		break;
	}

	case WM_MEASUREITEM:
	{
		MEASUREITEMSTRUCT* m = (MEASUREITEMSTRUCT*)lParam;
		m->itemHeight = lpData->rowHeight;
		return TRUE;
	}

	case WM_MOUSEWHEEL:
	{
		short zDelta = (short)(0xFFFF & (wParam >> 16));
		if (zDelta > 0)
			PostMessage(lpData->tableScroll->GetVScrollHWND(), WM_VSCROLL, SB_LINEUP, NULL);
		else
			PostMessage(lpData->tableScroll->GetVScrollHWND(), WM_VSCROLL, SB_LINEDOWN, NULL);
		break;
	}

	case WM_PAINT:
	{
		if (uIdSubclass == 1)
		{
			PAINTSTRUCT ps;
			hdc = BeginPaint(hWnd, &ps);
			lpData->DrawTableHeader(hdc);
			EndPaint(hWnd, &ps);
			return TRUE;
		}

		if (uIdSubclass != id) break;

		PAINTSTRUCT ps;
		hdc = BeginPaint(hWnd, &ps);
		lpData->DrawTableBody(hdc);
		EndPaint(hWnd, &ps);

		return TRUE;
	}

	case WM_ERASEBKGND:
	{
		return TRUE;
	}

	case WM_NOTIFY:
	{
		switch (((LPNMHDR)lParam)->code)
		{
		case NM_RELEASEDCAPTURE:
		{
			InvalidateRect(hListView, NULL, TRUE);
			break;
		}

		case HDN_DIVIDERDBLCLICKA:
		case HDN_DIVIDERDBLCLICKW:
		{
			return TRUE;
		}
		}

		break;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}

LRESULT CALLBACK TableCreater::ParentTableProcess(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	HDC hdc = (HDC)wParam;
	TableCreater* lpData = (TableCreater*)dwRefData;

	switch (uMsg)
	{
	case WM_SIZE:
	{
		lpData->tableScroll->OnResize();

		HWND hListView = lpData->con->GetControlHWND(lpData->tableId);
		if (wParam == SIZE_MAXIMIZED)
		{
			SendMessage(hListView, WM_SETREDRAW, FALSE, 0);
			lpData->ResizeColumns();
			SendMessage(hListView, WM_SETREDRAW, TRUE, 0);
			windowMaximized = true;
			break;
		}
		else if (windowMaximized == true && wParam == SIZE_RESTORED)
		{
			SendMessage(hListView, WM_SETREDRAW, FALSE, 0);
			lpData->ResizeColumns();
			SendMessage(hListView, WM_SETREDRAW, TRUE, 0);
			windowMaximized = false;
			break;
		}

		break;
	}

	case WM_MEASUREITEM:
	{
		MEASUREITEMSTRUCT* m = (MEASUREITEMSTRUCT*)lParam;
		m->itemHeight = lpData->rowHeight;
		return TRUE;
	}

	case WM_NOTIFY:
	{
		switch (((LPNMHDR)lParam)->code)
		{
		case LVN_COLUMNCLICK:
		{
			lpData->OnColumnClick((LPNMLISTVIEW)lParam);
			return true;
		}

		case LVN_KEYDOWN:
		{
			LPNMLVKEYDOWN pnkd = (LPNMLVKEYDOWN)lParam;
			if (pnkd->wVKey == VK_RIGHT || pnkd->wVKey == VK_LEFT)
			{
				int scrollPosX = GetScrollPos(pnkd->hdr.hwndFrom, SB_HORZ);
				lpData->tableScroll->SetPos(lpData->tableScroll->GetHScrollHWND(), scrollPosX);
			}

			break;
		}

		case LVN_ITEMCHANGED:
		{
			HWND hListView = lpData->con->GetControlHWND(lpData->tableId);

			LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW> (lParam);
			if (pNMLV->uChanged & LVIF_STATE)
			{
				if ((pNMLV->uNewState ^ pNMLV->uOldState) & LVIS_SELECTED)
				{
					InvalidateRect(hListView, NULL, FALSE);
				}
			}

			return TRUE;
		}
		}

		break;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}



