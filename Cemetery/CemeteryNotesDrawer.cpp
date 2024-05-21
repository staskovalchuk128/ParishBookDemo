#include "CemeteryNotesDrawer.h"
#include "CemeteryDrawer.h"
#include "CemeteryCommon.h"
#include "..//PBConnection.h"
#include "..//Common/CFunctions.hpp"

CemeteryNotesDrawer::CemeteryNotesDrawer(CemeteryDrawer* drawerPtr, HWND hWnd) 
	: drawerPtr(drawerPtr), hWnd(hWnd)
{
	mouseDownAtNote = currentSelectedNote = NULL;
}

CemeteryNotesDrawer::~CemeteryNotesDrawer()
{
}

void CemeteryNotesDrawer::Draw(Graphics& gr)
{
	Pen noteBorderPen(Color(192, 57, 43));
	SolidBrush fontBrush(GetColor(APP_COLORS::BLACK));

	StringFormat strFormat;
	RectF boundRect;

	float currentZoom = drawerPtr->GetZoom();

	for (auto& it : notes)
	{
		std::wstring noteStr = it.note;

		SetNewFontSize(it.fontSize * currentZoom);
		CreateNewFont();
	
		if (it.vertical)
		{
			strFormat.SetFormatFlags(StringFormatFlagsDirectionVertical);
		}
		else
		{
			strFormat.SetFormatFlags(0);
		}

		gr.MeasureString(noteStr.c_str(), (INT)noteStr.size(), currentFont, PointF(it.currentCoord.X, it.currentCoord.Y), &strFormat, &boundRect);
		it.currentCoord.Width = boundRect.Width;
		it.currentCoord.Height = boundRect.Height;
		it.fixedCoord.Width = boundRect.Width;
		it.fixedCoord.Height = boundRect.Height;

		if (it.selected)
		{
			gr.DrawRectangle(&noteBorderPen, it.currentCoord);
		}

		PointF origin(it.currentCoord.X, it.currentCoord.Y);
		gr.DrawString(noteStr.c_str(), (INT)noteStr.size(), currentFont, origin, &strFormat, &fontBrush);

	}
}


void CemeteryNotesDrawer::UpdateNotesPosition()
{
	float areaOffsetX = (float)drawerPtr->GetAreaOffsetX();
	float areaOffsetY = (float)drawerPtr->GetAreaOffsetY();
	PointF currentScroll = drawerPtr->GetScroll();
	float currentZoom = drawerPtr->GetZoom();

	for (auto& it : notes)
	{
		it.currentCoord = it.fixedCoord;

		float offsetX = (it.fixedCoord.X - areaOffsetX) * (currentZoom - 1.0f), offsetY = (it.fixedCoord.Y - areaOffsetY) * (currentZoom - 1.0f);

		it.currentCoord.X -= currentScroll.X;
		it.currentCoord.Y -= currentScroll.Y;

		it.currentCoord.X += offsetX;
		it.currentCoord.Y += offsetY;
	}
}

void CemeteryNotesDrawer::MoveNote(POINT& pt)
{
	if (!mouseDownAtNote) return;

	CellNote* notePtr = mouseDownAtNote;

	notePtr->currentCoord.X = (float)pt.x - mouseDownAt.X;
	notePtr->currentCoord.Y = (float)pt.y - mouseDownAt.Y;


	notePtr->currentCoord.X = notePtr->currentCoord.X < 0 ? 0 : notePtr->currentCoord.X;
	notePtr->currentCoord.Y = notePtr->currentCoord.Y < 0 ? 0 : notePtr->currentCoord.Y;

	RECT rc;

	GetWindowRect(hWnd, &rc);

	int winWidth = rc.right - rc.left - (int)notePtr->currentCoord.Width;
	int winHeight = rc.bottom - rc.top - (int)notePtr->currentCoord.Height;

	notePtr->currentCoord.X = notePtr->currentCoord.X > winWidth ? winWidth : notePtr->currentCoord.X;
	notePtr->currentCoord.Y = notePtr->currentCoord.Y > winHeight ? winHeight : notePtr->currentCoord.Y;

	drawerPtr->Redraw();
}




void CemeteryNotesDrawer::LoadNotes(std::vector<std::map<std::string, std::wstring>>& notesData)
{
	for (int i = 0; i < notesData.size(); i++)
	{
		int id = GetInt(notesData[i]["id"]);
		std::wstring note = notesData[i]["note"];
		float positionX = GetFloat(notesData[i]["positionX"]);
		float positionY = GetFloat(notesData[i]["positionY"]);
		float fontSize = GetFloat(notesData[i]["fontSize"]);
		int vertical = GetInt(notesData[i]["vertical"]);

		notes.push_back(CellNote({ id,  note, RectF(positionX, positionY, 0.0f, 0.0f), RectF(positionX, positionY, 0.0f, 0.0f), fontSize, vertical == 1 }));
	}

	UpdateNotesPosition();

	/*
	PBConnection::Request("getCemeteryNotes",
		{
		},
		[&, onLoadedCallback](PBResponse res)
		{
			std::vector<std::map<std::string, std::wstring>> result = res.GetResult();

			
		}
	);
	*/
}


CellNote* CemeteryNotesDrawer::GetNoteFromPoint(POINT& pt)
{
	std::vector<CellNote>::iterator it = std::find_if(notes.begin(), notes.end(), [pt](const CellNote& note)
		{
			return pt.x >= note.currentCoord.X && pt.y >= note.currentCoord.Y && pt.x <= note.currentCoord.X + note.currentCoord.Width && pt.y <= note.currentCoord.Y + note.currentCoord.Height;
		}
	);
	if (it != notes.end())
	{
		return &(*it);
	}
	return NULL;
}

void CemeteryNotesDrawer::SelectNote(CellNote* note)
{
	for (auto& it : notes) // deselect all
	{
		it.selected = false;
	}
	note->selected = true;
	currentSelectedNote = note;
	drawerPtr->Redraw();
}

void CemeteryNotesDrawer::DeselectNote()
{
	for (auto& it : notes) // deselect all
	{
		it.selected = false;
	}
	currentSelectedNote = NULL;
	drawerPtr->Redraw();
}

void CemeteryNotesDrawer::OnLMouseDown(POINT& pt)
{
	mouseDownAt = PointF((float)pt.x, (float)pt.y);
	mouseDownAtNote = GetNoteFromPoint(pt);
	if (mouseDownAtNote)
	{
		mouseDownAt.X -= mouseDownAtNote->currentCoord.X;
		mouseDownAt.Y -= mouseDownAtNote->currentCoord.Y;
		mouseDownNoteFixedOffset.X = mouseDownAtNote->fixedCoord.X;
		mouseDownNoteFixedOffset.Y = mouseDownAtNote->fixedCoord.Y;
		SelectNote(mouseDownAtNote);
	}
}

bool CemeteryNotesDrawer::OnRMouseDown(POINT& pt)
{
	CellNote* note = GetNoteFromPoint(pt);
	if (!note) return false;

	SelectNote(note);

	POINT cursor;
	GetCursorPos(&cursor);
	HMENU hMenu = LoadMenu(NULL, MAKEINTRESOURCE(IDR_CEMETERYNOTEMENU));
	hMenu = GetSubMenu(hMenu, 0);
	TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, cursor.x, cursor.y, 0, hWnd, NULL);
	return true;
}

bool CemeteryNotesDrawer::OnLMouseUp(POINT& pt)
{
	bool preventParentEvent = false;
	if (mouseDownAtNote && mouseDownAtNote == GetNoteFromPoint(pt))
	{
		OnNoteLMouseClick(pt, mouseDownAtNote);
		preventParentEvent = true;
	}
	else {
		DeselectNote();
	}
	if (mouseDownAtNote)
	{
		float areaOffsetX = (float)drawerPtr->GetAreaOffsetX();
		float areaOffsetY = (float)drawerPtr->GetAreaOffsetY();
		PointF currentScroll = drawerPtr->GetScroll();
		float currentZoom = drawerPtr->GetZoom();

		RectF noteCoord = mouseDownAtNote->currentCoord;

		noteCoord.X += currentScroll.X;
		noteCoord.Y += currentScroll.Y;

		float offsetX = ((noteCoord.X - areaOffsetX) / currentZoom) * (currentZoom - 1.0f), offsetY = ((noteCoord.Y - areaOffsetY) / currentZoom) * (currentZoom - 1.0f);

		mouseDownAtNote->fixedCoord = noteCoord;
		mouseDownAtNote->fixedCoord.X -= offsetX;
		mouseDownAtNote->fixedCoord.Y -= offsetY;

		SaveNotePosition(mouseDownAtNote);
	}
	mouseDownAtNote = NULL;
	return preventParentEvent;
}

void CemeteryNotesDrawer::OnNoteLMouseClick(POINT& pt, CellNote* notePtr)
{
}

void CemeteryNotesDrawer::SaveNotePosition(CellNote* notePtr)
{
	PBConnection::Request("saveCemeteryNotePosition",
		{
			{"noteId", std::to_wstring(notePtr->id)},
			{"x", std::to_wstring(notePtr->fixedCoord.X)},
			{"y", std::to_wstring(notePtr->fixedCoord.Y)}
		},
		[&](PBResponse res)
		{
			std::vector<std::map<std::string, std::wstring>> result = res.GetResult();

			for (int i = 0; i < result.size(); i++)
			{
				int id = GetInt(result[i]["id"]);
				std::wstring note = result[i]["note"];
				float positionX = GetFloat(result[i]["positionX"]);
				float positionY = GetFloat(result[i]["positionY"]);
				float fontSize = GetFloat(result[i]["fontSize"]);
				int vertical = GetInt(result[i]["vertical"]);

				notes.push_back(CellNote({ id,  note, RectF(positionX, positionY, 0.0f, 0.0f), RectF(positionX, positionY, 0.0f, 0.0f), fontSize, vertical == 1 }));
			}

			UpdateNotesPosition();

		}
	);

}

bool CemeteryNotesDrawer::IsMovingNote()
{
	return mouseDownAtNote != NULL;
}

void CemeteryNotesDrawer::AddNewNote(CellNote note)
{
	notes.push_back(note);
	SelectNote(&*(notes.end() - 1));
	drawerPtr->Redraw();
}

void CemeteryNotesDrawer::OnEditNote()
{
	drawerPtr->GetParentPagePtr()->OnEditNote(currentSelectedNote);
}
void CemeteryNotesDrawer::OnDeleteNote()
{
	if (!currentSelectedNote) return;

	CellNote* notePtr = currentSelectedNote;

	PBConnection::Request("deleteCemeteryNote",
		{
			{"noteId", std::to_wstring(notePtr->id)}
		},
		[&, notePtr](PBResponse res)
		{
			DeselectNote();

			std::vector<CellNote>::iterator it = std::find_if(notes.begin(), notes.end(), [&](const CellNote& note)
				{
					return note.id == notePtr->id;
				}
			);
			if (it != notes.end())
			{
				notes.erase(it);
			}

			drawerPtr->Redraw();

		}
	);

}

void CemeteryNotesDrawer::OnZoom()
{
	UpdateNotesPosition();
}

void CemeteryNotesDrawer::OnScroll()
{
	UpdateNotesPosition();
}