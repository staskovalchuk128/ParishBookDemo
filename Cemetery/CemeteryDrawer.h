/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "..//Drawing/DrawCommon.h"
#include "CemeteryNotesDrawer.h"
#include <set>

enum struct CemeteryDrawerMode
{
	SIMPLE, EDITOR
};

class CemeteryCommon;

class CemeteryDrawer : private DrawCommon
{	
public:
	struct GraveInfo
	{
		int id;
		std::wstring lastName, firstName, middleName, status;
	};

	struct Cell
	{
		int id;
		int row, col;
		RectF coord;
		bool selected;
		bool active;
		bool searched;
		GraveInfo graveInfo;
	};

	CemeteryDrawer(CemeteryCommon* parentPage, CemeteryDrawerMode mode, HWND hWnd);
	~CemeteryDrawer();

	void Redraw();
	void OnDraw(HDC& hdc);

	void OnScroll(POINT& pt, short zDelta);
	void OnLMouseDown(POINT& pt);
	void OnRMouseDown(POINT& pt);
	void OnLMouseUp(POINT& pt);
	void OnMouseMove(POINT& pt);
	void OnCellLMouseClick(POINT& pt, Cell* cellPtr);

	void ReloadGrid();

	void AddNewNote(CellNote note);

	CemeteryCommon* GetParentPagePtr();

	int GetAreaOffsetX();
	int GetAreaOffsetY();
	PointF GetScroll();
	float GetZoom();

	void Search(std::wstring searchStr, std::wstring searchBy);
	CemeteryDrawer::Cell* GetCellById(int id);
private:
	CemeteryDrawerMode currentMode;
	static CemeteryDrawer* _this;
	CemeteryNotesDrawer* notesDrawer;
	CemeteryCommon* parentPage;
	HHOOK mouseHook;
	int areaOffsetX, areaOffsetY;
	bool movable;
	float zoomFactor, minZoomFactor, maxZoomFactor;
	float scrollX, scrollY;
	float scrollMaxX, scrollMaxY;
	PointF mouseDownAt;
	int rows, columns;
	HWND hWnd;
	std::vector<Cell*> currentSelectedCells;
	Cell* mouseDownAtCell;
	std::vector<Cell> grid;
	std::set<int> highlightedRows;
	std::set<int> highlightedCols;
	std::wstring searchStr, searchBy;
private:

	static LRESULT CALLBACK MouseHookProc(int nCode, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK MainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
	void SortGrid();
	void CalcCellsCoord();
	void AdjustScroll();
	void SelectCell(Cell* cell, bool clearPrev = true);
	void DeselectCell(Cell* cell = NULL);
	void ChangeSelectedCellsStatus(int status);

	CemeteryDrawer::Cell* GetCellFromPoint(POINT& pt);

	bool IsCellSelected(Cell* cell);

};