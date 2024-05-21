/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include <functional>
#include <algorithm>
#include "..//Drawing/DrawCommon.h"

class CemeteryDrawer;

struct CellNote
{
	int id;
	std::wstring note;
	RectF fixedCoord;
	RectF currentCoord;
	float fontSize;
	bool vertical;
	bool selected = false;
};

class CemeteryNotesDrawer : private DrawCommon
{
public:
	CemeteryNotesDrawer(CemeteryDrawer* drawerPtr, HWND hWnd);
	~CemeteryNotesDrawer();
	void LoadNotes(std::vector<std::map<std::string, std::wstring>>& notes);

	void Draw(Graphics& gr);

	void MoveNote(POINT& pt);

	void OnLMouseDown(POINT& pt);
	bool OnLMouseUp(POINT& pt);
	bool OnRMouseDown(POINT& pt);

	CellNote* GetNoteFromPoint(POINT& pt);
	void OnNoteLMouseClick(POINT& pt, CellNote* notePtr);

	bool IsMovingNote();
	void DeselectNote();

	void AddNewNote(CellNote note);

	void OnEditNote();
	void OnDeleteNote();


	void OnZoom();
	void OnScroll();
private:
	HWND hWnd;
	CemeteryDrawer* drawerPtr;
	std::vector<CellNote> notes;
	CellNote* mouseDownAtNote;
	PointF mouseDownAt;
	PointF mouseDownNoteFixedOffset;
	CellNote* currentSelectedNote;
private:
	void SelectNote(CellNote* note);
	void SaveNotePosition(CellNote* notePtr);
	void UpdateNotesPosition();

};
