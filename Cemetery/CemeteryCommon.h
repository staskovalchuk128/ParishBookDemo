/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "CemeteryDrawer.h"

class CemeteryCommon
{
public:
	CemeteryCommon();
	virtual ~CemeteryCommon();

	virtual void OnGraveSelected(int graveId);
	virtual void OnNoteClick(CellNote* notePtr);
	virtual void OnEditNote(CellNote* notePtr);
protected:
	int currentGraveId;
	CemeteryDrawer* drawer;
protected:
	void InitDrawer(HWND hWnd, CemeteryDrawerMode mode);
};

