#include "CemeteryEditor.h"
#include "..//PBConnection.h"
#include "..//Components/PreloaderComponent.h"

CemeteryEditor::CemeteryEditor(HWND hWnd, HINSTANCE hInst)
	: PageCommon(hWnd, hInst)
{
}

CemeteryEditor::~CemeteryEditor()
{
	delete editorControls;
	delete noteAddingControls;
	delete noteEditingControls;
}

void CemeteryEditor::AddControls()
{
	HWND cemBody = CreateStatic("cemBody", 10, 10, -500, -10, WS_VISIBLE | WS_CHILD | SS_NOTIFY | WS_CLIPCHILDREN, L"");

	HWND editorHWND = CreateStatic("editorContent", 10, 10, 480, -80, WS_VISIBLE | WS_CHILD | SS_NOTIFY | WS_CLIPCHILDREN, L"", WS_STICK_RIGHT);
	editorControls = new Controls(editorHWND);

	CreateLine(490, 10, 1, -10, "sepLine1", APP_COLORS::GRAY_BORDER, WS_STICK_RIGHT);

	editorControls->CreateStatic("sGridCemGen", 10, 10, -1, 30, WS_VISIBLE | WS_CHILD | SS_CENTER | SS_NOTIFY | WS_CLIPCHILDREN, L"Grid Generation");
	editorControls->SetFontSize("sGridCemGen", 22);

	editorControls->CreateLine(10, 50, -10, 1, "sepLine2", APP_COLORS::GRAY_BORDER, WS_STICK_RIGHT);

	editorControls->CreateStatic("sCemRows", 10, 60, 100, 20, WS_VISIBLE | WS_CHILD , L"Rows");
	editorControls->CreateEdit("cemRows", 10, 90, 200, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, L"");

	editorControls->CreateStatic("sCemCols", 230, 60, 100, 20, WS_VISIBLE | WS_CHILD, L"Columns");
	editorControls->CreateEdit("semCols", 230, 90, 200, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, L"");

	editorControls->CreateBtn("generateGrid", 10, 140, 150, 30, WS_CHILD | WS_VISIBLE, L"Generate grid", WS_STICK_RIGHT);

	int itemsOffsetY = 30;
	int itemX = 10, itemY = 160;

	editorControls->CreateLine(itemX, itemY += itemsOffsetY, -10, 1, "sepLine1", APP_COLORS::GRAY_BORDER, WS_STICK_RIGHT);


	/*ADDING NOTE CONTROLS*/

	editorControls->CreateStatic("addNoteControls", itemX, itemY += 20, -1, -1, WS_VISIBLE | WS_CHILD | SS_CENTER | SS_NOTIFY | WS_CLIPCHILDREN, L"");

	itemY = 0;
	noteAddingControls = new Controls(editorControls->GetControlHWND("addNoteControls"));

	noteAddingControls->CreateStatic("sAddNote", itemX, itemY += 10, -1, 30, WS_VISIBLE | WS_CHILD | SS_CENTER | SS_NOTIFY | WS_CLIPCHILDREN, L"Adding Notes");
	noteAddingControls->SetFontSize("sAddNote", 22);

	itemY += itemsOffsetY;
	noteAddingControls->CreateStatic("sNote", itemX, itemY, 100, 20, WS_VISIBLE | WS_CHILD, L"Note");
	noteAddingControls->CreateEdit("note", itemX, itemY += 30, -10, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, L"");
	noteAddingControls->SetLabel("note", L"Note on the grid...");

	itemY += itemsOffsetY + 15;
	noteAddingControls->CreateStatic("sFontSize", itemX, itemY, 100, 20, WS_VISIBLE | WS_CHILD, L"Font size");
	noteAddingControls->CreateEdit("fontSize", itemX, itemY + 30, 220, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, L"22");

	noteAddingControls->CreateCheckBox("verticalOrient", 240, itemY + 30, -10, 30, WS_VISIBLE | WS_CHILD | BS_CHECKBOX, L"Vertical orientation");

	itemY += itemsOffsetY + 60;
	noteAddingControls->CreateBtn("createNote", itemX, itemY, 150, 30, WS_CHILD | WS_VISIBLE, L"Create note", WS_STICK_RIGHT);

	itemY += itemsOffsetY;
	noteAddingControls->CreateLine(itemX, itemY += itemsOffsetY, -10, 1, "sepLine1", APP_COLORS::GRAY_BORDER, WS_STICK_RIGHT);


	/*EDITING NOTE CONTROLS*/

	itemY = 160 + itemsOffsetY;
	editorControls->CreateStatic("editNoteControls", itemX, itemY += 10, -1, -1, WS_VISIBLE | WS_CHILD | SS_CENTER | SS_NOTIFY | WS_CLIPCHILDREN, L"");

	itemY = 0;
	noteEditingControls = new Controls(editorControls->GetControlHWND("editNoteControls"));

	noteEditingControls->CreateStatic("sAddNote", itemX, itemY += 20, -1, 30, WS_VISIBLE | WS_CHILD | SS_CENTER | SS_NOTIFY | WS_CLIPCHILDREN, L"Editing Note");
	noteEditingControls->SetFontSize("sAddNote", 22);

	itemY += itemsOffsetY;
	noteEditingControls->CreateStatic("sNote", itemX, itemY, 100, 20, WS_VISIBLE | WS_CHILD, L"Note");
	noteEditingControls->CreateEdit("noteEdit", itemX, itemY += 30, -10, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, L"");
	noteEditingControls->SetLabel("noteEdit", L"Note on the grid...");

	itemY += itemsOffsetY + 15;
	noteEditingControls->CreateStatic("sFontSize", itemX, itemY, 100, 20, WS_VISIBLE | WS_CHILD, L"Font size");
	noteEditingControls->CreateEdit("fontSizeEdit", itemX, itemY + 30, 220, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP, L"22");

	noteEditingControls->CreateCheckBox("verticalOrientEdit", 240, itemY + 30, -10, 30, WS_VISIBLE | WS_CHILD | BS_CHECKBOX, L"Vertical orientation");

	itemY += itemsOffsetY + 60;
	noteEditingControls->CreateBtn("endEditingNote", itemX, itemY, 150, 30, WS_CHILD | WS_VISIBLE, L"End editing");
	noteEditingControls->SetColor("endEditingNote", APP_COLORS::RED);
	noteEditingControls->CreateBtn("saveNoteChanges", itemX, itemY, 150, 30, WS_CHILD | WS_VISIBLE, L"Save changes", WS_STICK_RIGHT);

	itemY += itemsOffsetY;
	noteEditingControls->CreateLine(itemX, itemY += itemsOffsetY, -10, 1, "sepLine1", APP_COLORS::GRAY_BORDER, WS_STICK_RIGHT);

	//Hide editing controls by default
	ShowWindow(noteEditingControls->GetHWND(), SW_HIDE);


	InitDrawer(cemBody, CemeteryDrawerMode::EDITOR);

	SetWindowSubclass(editorHWND, PageMainProc, IDC_STATIC, (DWORD_PTR)this);
	SetWindowSubclass(noteAddingControls->GetHWND(), PageMainProc, IDC_STATIC, (DWORD_PTR)this);
	SetWindowSubclass(noteEditingControls->GetHWND(), PageMainProc, IDC_STATIC, (DWORD_PTR)this);
}

void CemeteryEditor::ShowEditNote()
{
	ShowWindow(noteAddingControls->GetHWND(), SW_HIDE);
	ShowWindow(noteEditingControls->GetHWND(), SW_SHOW);

}
void CemeteryEditor::ShowAddNote()
{
	ShowWindow(noteEditingControls->GetHWND(), SW_HIDE);
	ShowWindow(noteAddingControls->GetHWND(), SW_SHOW);
}


void CemeteryEditor::OnEditNote(CellNote* notePtr)
{
	currentNoteEdit = notePtr;

	SetWindowText(noteEditingControls->GetControlHWND("noteEdit"), currentNoteEdit->note.c_str());
	SetWindowText(noteEditingControls->GetControlHWND("fontSizeEdit"), std::to_wstring(currentNoteEdit->fontSize).c_str());
	noteEditingControls->SetBtnCheck("verticalOrientEdit", currentNoteEdit->vertical);
	
	ShowEditNote();
}

void CemeteryEditor::OnEndNoteEdit()
{
	ShowAddNote();
}

void CemeteryEditor::OnSaveNoteChanges()
{
	std::wstring note = GetWindowTextStr(noteEditingControls->GetControlHWND("noteEdit"));
	float fs = GetFloat(GetWindowTextStr(noteEditingControls->GetControlHWND("fontSizeEdit")));
	bool isVertical = noteEditingControls->IsBtnChecked("verticalOrientEdit");

	int isVerticalInt = isVertical ? 1 : 0;

	PBConnection::Request("saveCemeteryNote",
		{
			{ "noteId", std::to_wstring(currentNoteEdit->id) },
			{ "note", note },
			{ "fontSize", std::to_wstring(fs) },
			{ "isVertical", std::to_wstring(isVerticalInt) },
		},
		[&, note, fs, isVertical](PBResponse res)
		{

			currentNoteEdit->note = note;
			currentNoteEdit->fontSize = fs;
			currentNoteEdit->vertical = isVertical;


			drawer->Redraw();
		}
	);
}

void CemeteryEditor::OnGenerateGridClick()
{
	int rows = GetInt(GetWindowTextStr(editorControls->GetControlHWND("cemRows"))), cols = GetInt(GetWindowTextStr(editorControls->GetControlHWND("semCols")));
	if (rows <= 0 || cols <= 0)
	{
		MessageDlg(hWnd, L"Error",
			L"Ivalid Rows/Colums value. The value must be greater than zero.",
			MD_OK, MD_ERR).OpenDlg();
	}
	else
	{
		GenerateGrid(rows, cols);
	}
}

void CemeteryEditor::GenerateGrid(int rows, int cols)
{
	PreloaderComponent::Start(hWnd);
	PBConnection::Request("generateCemeteryGrid",
		{
			{ "rows", std::to_wstring(rows) },
			{ "cols", std::to_wstring(cols) }
		},
		[&](PBResponse res)
		{
			PreloaderComponent::Stop();


			MessageDlg(hWnd, L"Success",
				L"Grid successfuly generated.",
				MD_OK, MD_SUCCESS).OpenDlg();

			drawer->Redraw();

		}
	);


}

void CemeteryEditor::CreateNote()
{
	//createCemeteryNote
	std::wstring note = GetWindowTextStr(noteAddingControls->GetControlHWND("note"));
	float fs = GetFloat(GetWindowTextStr(noteAddingControls->GetControlHWND("fontSize")));
	int isVertical = noteAddingControls->IsBtnChecked("verticalOrient");

	if (Trim(note).length() == 0)
	{
		MessageDlg(hWnd, L"Error",
			L"Note cannot be an empty string.",
			MD_OK, MD_ERR).OpenDlg();
		return;
	}

	PBConnection::Request("createCemeteryNote",
		{
			{ "note", note },
			{"fontSize", std::to_wstring(fs) },
			{ "isVertical", std::to_wstring(isVertical) }
		},
		[&, note, isVertical, fs](PBResponse res)
		{
			std::map<std::string, std::wstring> mapRes = res.GetResultMap("main");
			if (mapRes.size() > 0)
			{
				drawer->AddNewNote(CellNote{ GetInt(mapRes["lastInsertId"]), note, RectF(50,50,0,0), RectF(50,50,0,0), fs, isVertical == 1});

				SetWindowText(noteAddingControls->GetControlHWND("note"), L"");
			}
		}
	);


}

bool CemeteryEditor::OnCommand(WPARAM wParam)
{

	return false;
}

LRESULT CALLBACK CemeteryEditor::PageMainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	CemeteryEditor* lpData = reinterpret_cast<CemeteryEditor*>(dwRefData);
	HDC hdc = (HDC)wParam;

	switch (uMsg)
	{
	case WM_COMMAND:
	{
		UINT wId = LOWORD(wParam);

		switch (HIWORD(wParam))
		{
		case BN_CLICKED:
		{
			if (wId == lpData->editorControls->GetControlId("generateGrid"))
			{
				lpData->OnGenerateGridClick();
				return true;
			}
			else if (wId == lpData->noteEditingControls->GetControlId("endEditingNote"))
			{
				lpData->OnEndNoteEdit();
				return true;
			}
			else if (wId == lpData->noteEditingControls->GetControlId("saveNoteChanges"))
			{
				lpData->OnSaveNoteChanges();
				return true;
			}
			else if (wId == lpData->noteAddingControls->GetControlId("createNote"))
			{
				lpData->CreateNote();
				return true;
			}

			break;
		}
		}

		break;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}
