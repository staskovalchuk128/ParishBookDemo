#include "CemeteryDrawer.h"
#include <windowsx.h>
#include "..//Controls/Control.h"
#include "CemeteryCommon.h"
#include "..//PBConnection.h"
#include "..//Components/PreloaderComponent.h"

CemeteryDrawer* CemeteryDrawer::_this = NULL;

CemeteryDrawer::CemeteryDrawer(CemeteryCommon* parentPage, CemeteryDrawerMode mode, HWND hWnd)
	: parentPage(parentPage), currentMode(mode), hWnd(hWnd)
{
	mouseHook = NULL;
	_this = this;
	scrollMaxX = scrollMaxY = 0.0f;
	movable = false;
	rows = 0;
	columns = 0;
	zoomFactor = 1.0f;
	scrollX = scrollY = 0;
	minZoomFactor = 1.0f;
	maxZoomFactor = 5.0f;

	areaOffsetX = areaOffsetY = 40;

	mouseDownAtCell = NULL;


	notesDrawer = new CemeteryNotesDrawer(this, hWnd);


	SetWindowSubclass(hWnd, MainProc, IDC_STATIC, (DWORD_PTR)this);
}

CemeteryDrawer::~CemeteryDrawer()
{
	delete notesDrawer;
}

void CemeteryDrawer::Redraw()
{
	InvalidateRect(hWnd, NULL, TRUE);
}

void CemeteryDrawer::OnDraw(HDC& hdc)
{
	RECT rc;

	GetWindowRect(hWnd, &rc);

	HDC hdcMem = CreateCompatibleDC(hdc);

	int bodyX = 0;
	int bodyY = 0;
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	HBITMAP bitmap = CreateCompatibleBitmap(hdc, width, height);
	HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, bitmap);

	Graphics gr(hdcMem);

	SolidBrush bgBrush(Color(236, 240, 241));
	SolidBrush grayFontBrush(Color(68, 68, 68));
	SolidBrush rowNumBrush(Color(240, 240, 240));
	SolidBrush selectedRowNumBrush(Color(41, 128, 185));
	SolidBrush fontBrush(GetColor(APP_COLORS::BLACK));
	SolidBrush whiteBrush(GetColor(APP_COLORS::WHITE));
	Pen borderPen(GetColor(APP_COLORS::GRAY_BORDER));
	Pen cellBorderPen(Color(127, 140, 141));
	Pen noteBorderPen(Color(192, 57, 43));
	Pen searchedPen(Color(44, 62, 80), 4.0f);

	SolidBrush selectedCellBrush(Color(192, 57, 43));
	SolidBrush paidCellBrush(Color(46, 204, 113));
	SolidBrush reservedCellBrush(Color(243, 156, 18));
	SolidBrush freeCellBrush(Color(236, 240, 241));

	gr.FillRectangle(&bgBrush, 0, 0, width, height);

	RectF cellRc(0.0f, 0.0f, 0.0f, 0.0f);


	for (auto& it : grid)
	{
		cellRc = it.coord;

		if (it.selected)
		{
			gr.FillRectangle(&selectedCellBrush, cellRc);
		}
		else if (!it.active)
		{
			//	gr.FillRectangle(&bgBrush, cellRc);
			continue;
		}
		else if (it.graveInfo.status == L"Paid")
		{
			gr.FillRectangle(&paidCellBrush, cellRc);
		}
		else if (it.graveInfo.status == L"Reserved")
		{
			gr.FillRectangle(&reservedCellBrush, cellRc);
		}
		else
		{
			gr.FillRectangle(&freeCellBrush, cellRc);
		}

		gr.DrawRectangle(&cellBorderPen, cellRc);

		if (it.searched)
		{
			cellRc.X += 2.0f;
			cellRc.Y += 2.0f;
			cellRc.Width -= 4.0f;
			cellRc.Height -= 4.0f;
			gr.DrawRectangle(&searchedPen, cellRc);
		}

	}

	//Draw notes
	notesDrawer->Draw(gr);


	int prevRow = -1;
	for (auto& it : grid)
	{
		cellRc = it.coord;

		//set up font size
		if (it.row == 1 && it.col == 1)
		{
			std::wstring rowNumStr = L"50";
			RectF layoutRect(0, 0, -1, -1);
			RectF boundRect;

			SetNewFontSize(24);
			CreateNewFont();

			float currentFontSize = currentFont->GetSize();
			gr.MeasureString(rowNumStr.c_str(), (INT)rowNumStr.size(), currentFont, PointF(0.0f, 0.0f), &boundRect);

			while (boundRect.Width > cellRc.Width)
			{
				currentFontSize -= 1.0f;
				SetNewFontSize(currentFontSize);
				CreateNewFont();
				gr.MeasureString(rowNumStr.c_str(), (INT)rowNumStr.size(), currentFont, PointF(0.0f, 0.0f), &boundRect);
			}
		}

		// Draw Col Desc
		{
			if (it.row == 1)
			{
				bool highlighted = highlightedCols.find(it.col) != highlightedCols.end();

				SolidBrush* brushPtr = highlighted ? &selectedRowNumBrush : &rowNumBrush;
				SolidBrush* fontBrushPtr = highlighted ? &whiteBrush : &fontBrush;


				std::wstring rowNumStr = std::to_wstring(it.col);

				gr.FillRectangle(brushPtr, RectF(cellRc.X, 0.0f, cellRc.Width, (float)areaOffsetY));
				gr.DrawRectangle(&borderPen, RectF(cellRc.X, 0.0f, cellRc.Width, (float)areaOffsetY));

				RectF layoutRect(0, 0, (REAL)cellRc.Width, (REAL)cellRc.Height);
				RectF boundRect;

				gr.MeasureString(rowNumStr.c_str(), (INT)rowNumStr.size(), currentFont, layoutRect, &boundRect);
				int textX = ((int)cellRc.Width - (int)boundRect.Width) / 2;
				PointF origin((REAL)cellRc.X + textX, (REAL)((areaOffsetY / 2 - boundRect.Height / 2)));

				gr.DrawString(rowNumStr.c_str(), (INT)rowNumStr.size(), currentFont, origin, fontBrushPtr);
			}
		}


		// Draw row Desc
		{
			if (prevRow != it.row)
			{
				bool highlighted = highlightedRows.find(it.row) != highlightedRows.end();

				SolidBrush* brushPtr = highlighted ? &selectedRowNumBrush : &rowNumBrush;
				SolidBrush* fontBrushPtr = highlighted ? &whiteBrush : &fontBrush;

				std::wstring rowNumStr = std::to_wstring(it.row);
				gr.FillRectangle(brushPtr, RectF(0.0f, cellRc.Y, (float)areaOffsetX, cellRc.Height));
				gr.DrawRectangle(&borderPen, RectF(0.0f, cellRc.Y, (float)areaOffsetX, cellRc.Height));

				RectF layoutRect(0, 0, (REAL)cellRc.Width, (REAL)cellRc.Height);
				RectF boundRect;

				gr.MeasureString(rowNumStr.c_str(), (INT)rowNumStr.size(), currentFont, layoutRect, &boundRect);
				int textX = ((int)areaOffsetX - (int)boundRect.Width) / 2;
				PointF origin((REAL)textX, (REAL)((int)cellRc.Y + (((int)cellRc.Height - (int)boundRect.Height) / 2)));

				gr.DrawString(rowNumStr.c_str(), (INT)rowNumStr.size(), currentFont, origin, fontBrushPtr);
			}
		}

		prevRow = it.row;
	}

	//Draw filler
	{
		gr.FillRectangle(&rowNumBrush, Rect(0, 0, areaOffsetX, areaOffsetY));
	}




	gr.DrawRectangle(&borderPen, 0, 0, width - 1, height - 1);

	BitBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);

	SelectObject(hdcMem, oldBitmap);
	DeleteObject(bitmap);
	DeleteDC(hdcMem);
}

void CemeteryDrawer::OnScroll(POINT& pt, short zDelta)
{
	float zoomVal = 0.5f;
	float prevZoom = zoomFactor;

	if (zDelta > 0)
	{
		zoomFactor += zoomVal;
	}
	else
	{
		zoomFactor -= zoomVal;
	}

	zoomFactor = zoomFactor < 1.0f ? 1.0f : zoomFactor;
	zoomFactor = zoomFactor > maxZoomFactor ? maxZoomFactor : zoomFactor;

	RECT rc;
	GetWindowRect(hWnd, &rc);
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	scrollMaxX = ((width - areaOffsetY) * zoomFactor) - (width - areaOffsetY);
	scrollMaxY = ((height - areaOffsetY) * zoomFactor) - (height - areaOffsetY);

	float zoomFactorPrc = (zoomFactor - minZoomFactor) / (maxZoomFactor - minZoomFactor) * 100.0f;
	float widthWithPrc = ((float)width / 100.0f) * zoomFactorPrc + width;
	float heightWithPrc = ((float)height / 100.0f) * zoomFactorPrc + height;

	if (prevZoom != zoomFactor)
	{
		float scrollValX = pt.x * ((float)width / widthWithPrc);
		float scrollValY = pt.y * ((float)height / heightWithPrc);
		if (zDelta < 0)
		{
			scrollX -= pt.x - scrollValX;
			scrollY -= pt.y - scrollValY;
		}
		else
		{
			scrollX += scrollValX;
			scrollY += scrollValY;
		}
	}

	AdjustScroll();
	CalcCellsCoord();
	notesDrawer->OnZoom();

	Redraw();
}

void CemeteryDrawer::OnLMouseDown(POINT& pt)
{
	mouseDownAt = PointF((float)pt.x, (float)pt.y);
	mouseDownAt.X += scrollX;
	mouseDownAt.Y += scrollY;
	movable = true;
	mouseDownAtCell = GetCellFromPoint(pt);

	if (currentMode == CemeteryDrawerMode::EDITOR) // only for editor
	{
		notesDrawer->OnLMouseDown(pt);
		if (notesDrawer->IsMovingNote()) {
			mouseDownAtCell = NULL;
			DeselectCell();
		}
	}
}

void CemeteryDrawer::OnLMouseUp(POINT& pt)
{
	bool preventEvent = false;
	if (currentMode == CemeteryDrawerMode::EDITOR) // only for editor
	{
		UnhookWindowsHookEx(mouseHook);

		preventEvent = notesDrawer->OnLMouseUp(pt);
		if (preventEvent)
		{
			DeselectCell();
		}
	}


	if (!preventEvent && mouseDownAtCell && mouseDownAtCell == GetCellFromPoint(pt))
	{
		OnCellLMouseClick(pt, mouseDownAtCell);
		mouseDownAtCell = NULL;
	}

	movable = false;
}
void CemeteryDrawer::OnMouseMove(POINT& pt)
{
	if (currentMode == CemeteryDrawerMode::EDITOR && notesDrawer->IsMovingNote()) // only for editor
	{
		notesDrawer->MoveNote(pt);
	}
	else if (movable)
	{
		SHORT shiftKeyState = GetAsyncKeyState(VK_SHIFT);

		if (currentMode == CemeteryDrawerMode::EDITOR && (1 << 15) & shiftKeyState)  // only for editor
		{
			Cell* cell = GetCellFromPoint(pt);
			if (cell)
			{
				SelectCell(cell, false);
			}
		}
		else
		{

			scrollX = mouseDownAt.X - (float)pt.x;
			scrollY = mouseDownAt.Y - (float)pt.y;

			AdjustScroll();
			CalcCellsCoord();
			notesDrawer->OnScroll();
			Redraw();
		}

	}
}

void CemeteryDrawer::OnRMouseDown(POINT& pt)
{
	if (currentMode == CemeteryDrawerMode::EDITOR) // only for editor
	{
		bool preventEvent = notesDrawer->OnRMouseDown(pt);
		if (preventEvent) return;
	
		Cell* cell = GetCellFromPoint(pt);
		if (!cell) return;

		SelectCell(cell);

		POINT cursor;
		GetCursorPos(&cursor);
		HMENU hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_CEMETERYMENU));
		hMenu = GetSubMenu(hMenu, 0);
		TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, cursor.x, cursor.y, 0, hWnd, NULL);
	
	}
}

void CemeteryDrawer::AdjustScroll()
{
	scrollX = scrollX < 0.0f ? 0.0f : scrollX;
	scrollY = scrollY < 0.0f ? 0.0f : scrollY;

	scrollX = scrollX > scrollMaxX ? scrollMaxX : scrollX;
	scrollY = scrollY > scrollMaxY ? scrollMaxY : scrollY;
}

void CemeteryDrawer::OnCellLMouseClick(POINT& pt, Cell* cellPtr)
{
	SHORT ctrlKeyState = GetAsyncKeyState(VK_CONTROL);

	if ((1 << 15) & ctrlKeyState && currentMode == CemeteryDrawerMode::EDITOR) // only for editor
	{
		DeselectCell(cellPtr);
		return;
	}

	SHORT shiftKeyState = GetAsyncKeyState(VK_SHIFT);
	SelectCell(cellPtr, !((1 << 15) & shiftKeyState && currentMode == CemeteryDrawerMode::EDITOR));
	parentPage->OnGraveSelected(cellPtr->id);
}


void CemeteryDrawer::ReloadGrid()
{
	PreloaderComponent::Start(GetParent(hWnd));
	
	PBConnection::Request("getCemeteryGrid",
		{
		},
		[&](PBResponse res)
		{
			std::vector<std::map<std::string, std::wstring>> result = res.GetResultVec("graveData");
	
			for (int i = 0; i < result.size(); i++)
			{
				int id = GetInt(result[i]["id"]);
				int isActive = GetInt(result[i]["active"]);
				int row = GetInt(result[i]["row"]), col = GetInt(result[i]["column"]);
				int graveId = GetInt(result[i]["cemGraveId"]);
				std::wstring lastName = result[i]["lastName"];
				std::wstring firstName = result[i]["firstName"];
				std::wstring middleName = result[i]["middleName"];
				std::wstring status = result[i]["status"];

				GraveInfo gi{ graveId , lastName, firstName, middleName, status };

				grid.push_back(Cell({ id, row, col, RectF{}, false, isActive == 1, false, gi }));
			}


			if (result.size() == 0)
			{
				InvalidateRect(hWnd, NULL, TRUE);
				PreloaderComponent::Stop();
				return;
			}

			rows = (grid.end() - 1)->row;
			columns = (grid.end() - 1)->col;

			std::vector<std::map<std::string, std::wstring>> notesData = res.GetResultVec("notesData");

			notesDrawer->LoadNotes(notesData);

			CalcCellsCoord();

			PreloaderComponent::Stop();

			InvalidateRect(hWnd, NULL, TRUE);
		}
	);	
}

void CemeteryDrawer::SortGrid()
{
	std::sort(grid.begin(), grid.end(), [](const Cell& cellA, const Cell& cellB)
		{
			return cellA.row < cellB.row;
		}
	);
}

static float sFactor = 0.0f;

void CemeteryDrawer::CalcCellsCoord() 
{
	RECT rc;

	GetWindowRect(hWnd, &rc);

	int winWidth = rc.right - rc.left;
	int winHeight = rc.bottom - rc.top;

	winWidth -= areaOffsetX;
	winHeight -= areaOffsetY;

	float cellWith = (float)winWidth / columns;
	float cellHeight = (float)winHeight / rows;

	RectF cellRc;
	cellRc.Width = cellWith * zoomFactor;
	cellRc.Height = cellHeight * zoomFactor;

	sFactor = grid[0].coord.Width;

	int prevRow = -1;
	for (auto& it: grid)
	{
		cellRc.X = (it.col - 1) * cellRc.Width + (float)areaOffsetX;
		cellRc.Y = (it.row - 1) * cellRc.Height + (float)areaOffsetY;

		cellRc.X -= scrollX;
		cellRc.Y -= scrollY;


		it.coord = cellRc;
		prevRow = it.row;
	}

	sFactor = grid[0].coord.Width - sFactor;

}

CemeteryDrawer::Cell* CemeteryDrawer::GetCellFromPoint(POINT& pt)
{
	std::vector<Cell>::iterator it = std::find_if(grid.begin(), grid.end(), [pt](const Cell& cell)
		{
			return pt.x >= cell.coord.X && pt.y >= cell.coord.Y && pt.x <= cell.coord.X + cell.coord.Width && pt.y <= cell.coord.Y + cell.coord.Height;
		}
	);
	if (it != grid.end())
	{
		return &(*it);
	}
	return NULL;
}


CemeteryDrawer::Cell* CemeteryDrawer::GetCellById(int id)
{
	std::vector<Cell>::iterator it = std::find_if(grid.begin(), grid.end(), [id](const Cell& cell)
		{
			return cell.id == id;
		}
	);
	if (it != grid.end())
	{
		return &(*it);
	}
	return NULL;
}

bool CemeteryDrawer::IsCellSelected(Cell* cell)
{
	std::vector<Cell*>::iterator it = std::find_if(currentSelectedCells.begin(), currentSelectedCells.end(), [cell](const Cell* c)
		{
			return c == cell;
		}
	);
	if (it != currentSelectedCells.end())
	{
		return true;
	}
	return false;
}

void CemeteryDrawer::SelectCell(Cell* cell, bool clearPrev)
{
	if (IsCellSelected(cell)) return;

	if (clearPrev)
	{
		for (auto& it : grid) // deselect all
		{
			it.selected = false;
		}
		currentSelectedCells.clear();
		highlightedRows.clear();
		highlightedCols.clear();
	}
	currentSelectedCells.push_back(cell);
	cell->selected = true;
	highlightedRows.insert(cell->row);
	highlightedCols.insert(cell->col);
	Redraw();
}


void CemeteryDrawer::DeselectCell(Cell* cell)
{
	if (!cell)
	{
		for (auto &it: currentSelectedCells)
		{
			it->selected = false;
		}
		currentSelectedCells.clear();
		Redraw();
	}
	else
	{
		std::vector<Cell*>::iterator it = std::find_if(currentSelectedCells.begin(), currentSelectedCells.end(), [cell](const Cell* c)
			{
				return c == cell;
			}
		);
		if (it != currentSelectedCells.end())
		{
			currentSelectedCells.erase(it);
			cell->selected = false;
			Redraw();
		}
	}

}

void CemeteryDrawer::ChangeSelectedCellsStatus(int status)
{
	if (status != 0 && status != 1)
	{
		status = 1;
	}

	std::map<std::string, std::wstring> idsToSend = {};
	int index = 0;
	for (auto& it : currentSelectedCells)
	{
		idsToSend[std::to_string(index)] = std::to_wstring(it->id);
		index++;
	}

	PBConnection::Request("changeSelectedCemeteryCellsStatus",
		{
			{ "status", std::to_wstring(status) },
			{ "ids", CreateJsonFormMap(idsToSend) }
		},
		[&](PBResponse res)
		{

			Redraw();
		}
	);
}

int CemeteryDrawer::GetAreaOffsetX()
{
	return areaOffsetX;
}
int CemeteryDrawer::GetAreaOffsetY()
{
	return areaOffsetY;
}
PointF CemeteryDrawer::GetScroll()
{
	return PointF(scrollX, scrollY);
}
float CemeteryDrawer::GetZoom()
{
	return zoomFactor;
}

LRESULT CALLBACK CemeteryDrawer::MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam) {

	PKBDLLHOOKSTRUCT k = (PKBDLLHOOKSTRUCT)(lParam);
	POINT p;

	if (wParam == WM_LBUTTONUP)
	{
		GetCursorPos(&p);
		_this->OnLMouseUp(p);
	}

	return CallNextHookEx(NULL, nCode, wParam, lParam);
}


LRESULT CALLBACK CemeteryDrawer::MainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CemeteryDrawer* lpData = (CemeteryDrawer*)dwRefData;
	HDC hdc = (HDC)wParam;
	POINT pt;

	if (uMsg == WM_MOUSEWHEEL || uMsg == WM_LBUTTONDOWN || uMsg == WM_RBUTTONDOWN
		|| uMsg == WM_LBUTTONUP || uMsg == WM_MOUSEMOVE)
	{
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);
	}

	switch (uMsg)
	{
	case WM_COMMAND:
	{
		UINT wId = LOWORD(wParam);

		if (wId == ID_DELETE_TI)
		{
			lpData->ChangeSelectedCellsStatus(0);
			return false;
		}
		else if (wId == ID_RESTORE_CELL)
		{
			lpData->ChangeSelectedCellsStatus(1);
			return false;
		}
		else if (wId == ID_DELETE_NOTE)
		{
			lpData->notesDrawer->OnDeleteNote();
			return false;
		}
		else if (wId == ID_EDIT_NOTE)
		{
			lpData->notesDrawer->OnEditNote();
			return false;
		}

		break;
	}
	case WM_MOUSEWHEEL:
	{
		RECT rc;
		GetWindowRect(hWnd, &rc);

		pt.x -= rc.left;
		pt.y -= rc.top;

		lpData->OnScroll(pt, (short)(0xFFFF & (wParam >> 16)));
		return false;
	}
	case WM_LBUTTONDOWN:
	{
		lpData->OnLMouseDown(pt);
		//if(lpData->currentMode == CemeteryDrawerMode::EDITOR) lpData->mouseHook = SetWindowsHookEx(WH_MOUSE_LL, MouseHookProc, GetWindowInstance(hWnd), 0);
		return false;
	}
	case WM_RBUTTONDOWN:
	{
		lpData->OnRMouseDown(pt);
		return false;
	}
	case WM_LBUTTONUP:
	{
		lpData->OnLMouseUp(pt);
		return false;
	}
	case WM_MOUSEMOVE:
	{
		lpData->OnMouseMove(pt);
		return false;
	}
	case WM_ERASEBKGND:
	{
		return false;
	}
	case WM_PAINT:
	{
		PAINTSTRUCT ps;

		hdc = BeginPaint(hWnd, &ps);

		lpData->OnDraw(hdc);

		EndPaint(hWnd, &ps);

		break;
	}
	
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}

void CemeteryDrawer::AddNewNote(CellNote note)
{
	notesDrawer->AddNewNote(note);
}
CemeteryCommon* CemeteryDrawer::GetParentPagePtr()
{
	return parentPage;
}

void CemeteryDrawer::Search(std::wstring searchStr, std::wstring searchBy)
{
	this->searchBy = searchBy;
	this->searchStr = searchStr;

	size_t searchStrSize = searchStr.size();
	bool emptySearch = searchStrSize == 0;

	for (auto &it: grid)
	{
		if (emptySearch) // make all not searched
		{
			it.searched = false;
			continue;
		}

		bool contain = false;

		//search by last name
		contain = searchBy == L"lastName" ? WstrToLow(it.graveInfo.lastName).rfind(WstrToLow(searchStr), 0) != std::wstring::npos : false;

		//search by row/col
		if (searchBy == L"rowcol")
		{
			std::vector<std::wstring> rowcol = SplitStr(searchStr, L',');
			if (rowcol.size() >= 2)
			{
				int sRow = GetInt(Trim(rowcol[0])), sCol = GetInt(Trim(rowcol[1]));
				contain = it.row == sRow && it.col == sCol;
			}
		}

		if (contain)
		{
			it.searched = true;
		}
		else
		{
			it.searched = false;
		}

	}

	Redraw();
}