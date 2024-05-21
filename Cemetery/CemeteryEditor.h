/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "../Common/PageCommon.h"
#include "CemeteryDrawer.h"
#include "CemeteryCommon.h"

class CemeteryEditor : public CemeteryCommon, public PageCommon
{
public:
	CemeteryEditor(HWND hWnd, HINSTANCE hInst);
	~CemeteryEditor();

	void AddControls();
	bool OnCommand(WPARAM wParam);

	void OnEditNote(CellNote* notePtr);
private:
	CellNote* currentNoteEdit;
	Controls* editorControls;
	Controls* noteEditingControls;
	Controls* noteAddingControls;
private:
	void CreateNote();
	void GenerateGrid(int rows, int cols);

	void OnGenerateGridClick();

	void ShowEditNote();
	void ShowAddNote();

	void OnEndNoteEdit();
	void OnSaveNoteChanges();

	static LRESULT CALLBACK PageMainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};

