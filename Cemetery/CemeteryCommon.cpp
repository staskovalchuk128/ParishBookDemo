#include "CemeteryCommon.h"

CemeteryCommon::CemeteryCommon()
{
}

CemeteryCommon::~CemeteryCommon()
{
	delete drawer;
}

void CemeteryCommon::InitDrawer(HWND hWnd, CemeteryDrawerMode mode)
{
	drawer = new CemeteryDrawer(this, mode, hWnd);
	drawer->ReloadGrid();
}

void CemeteryCommon::OnGraveSelected(int graveId)
{
}
void CemeteryCommon::OnNoteClick(CellNote* notePtr)
{
}
void CemeteryCommon::OnEditNote(CellNote* notePtr)
{
}