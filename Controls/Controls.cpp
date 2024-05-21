#include "Controls.h"

#include "ButtonCtrl.h"
#include "StaticCtrl.h"
#include "EditCtrl.h"
#include "ComboboxCtrl.h"
#include "GroupBoxCtrl.h"
#include "RadioBtnCtrl.h"
#include "CheckBoxCtrl.h"
#include "ListBoxCtrl.h"
#include "LineCtrl.h"

int Controls::controlsCount = 20;
int Controls::lastControlId = 2000;

std::vector<Controls*> Controls::allControls = {};

Controls::Controls(HWND hWnd)
{
	coreHwnd = hWnd;
	listBoxItemHeight = 35;
	comboBoxOffset = 10;
	redrawItems = true;
	useLocalCoord = false;

	totalParentWidth = totalParentHeight = 0;

	controlsProcId = Controls::controlsCount++;

	allControls.push_back(this);

	SetEventListener();

};

Controls::~Controls()
{
	allControls.erase(
		std::remove_if(allControls.begin(), allControls.end(), [&](Controls* ptr)
			{
				return ptr == this;
			}
		),
		allControls.end());

	RemoveEventListener();
	Clear();
}

void Controls::ReinitControls(HWND hWnd)
{
	RemoveEventListener();
	coreHwnd = hWnd;
	SetEventListener();

	HWND testPrent = hWnd;
	while (testPrent)
	{
		if (WC_DIALOG == MAKEINTATOM(GetClassLong(hWnd, GCW_ATOM)))
		{
			parentIsDlg = true;
			break;
		}
		testPrent = GetParent(testPrent);
	}
	

}

int Controls::GetListBoxItemHeight()
{
	return listBoxItemHeight;
}

int Controls::GetTotalParentWidth()
{
	totalParentWidth = 0;

	for (auto& it : controls)
	{
		if (it->IsStickRight()) continue;
		if (it->GetX() + it->GetWidth() > totalParentWidth)
		{
			totalParentWidth = it->GetX() + it->GetWidth();
		}
	}

	return totalParentWidth;
}

int Controls::GetTotalParentHeight()
{
	totalParentHeight = 0;

	for (auto& it : controls)
	{
		if (it->GetY() + it->GetHeight() > totalParentHeight)
		{
			totalParentHeight = it->GetY() + it->GetHeight();
		}
	}

	return totalParentHeight;
}

Control* Controls::GetControlPtr(std::string name)
{
	std::vector<Control*>::iterator it = std::find_if(controls.begin(), controls.end(), [name](Control* item)
		{
			return item->GetName() == name;
		});
	if (it == controls.end()) return NULL;
	return *it;
}

Control* Controls::GetControlPtr(int id)
{
	std::vector<Control*>::iterator it = std::find_if(controls.begin(), controls.end(), [id](Control* item)
		{
			return item->GetId() == id;
		});
	if (it == controls.end()) return NULL;
	return *it;
}

Control* Controls::GetControlPtr(HWND h)
{
	std::vector<Control*>::iterator it = std::find_if(controls.begin(), controls.end(), [h](Control* item)
		{
			return item->GetHWND() == h;
		});
	if (it == controls.end()) return NULL;
	return *it;
}

HWND Controls::GetHWND()
{
	return coreHwnd;
}

std::vector<Controls*> Controls::GetAllControls()
{
	return allControls;
}

std::vector<Control*> Controls::GetCurrentControls()
{
	return controls;
}

void Controls::SetColor(std::string id, int color)
{
	Control* item = GetControlPtr(id);
	if (item) item->SetColor(color);
}

void Controls::SetStyle(std::string id, int style)
{
	Control* item = GetControlPtr(id);
	if (item) item->SetStyle(style);
}

void Controls::SetButtonIcon(std::string btnId, int icon)
{
	Control* item = GetControlPtr(btnId);
	if (item) item->SetIcon(icon);
}

void Controls::SetListBoxItemHeight(int h)
{
	listBoxItemHeight = h;
}

void Controls::SetStatus(std::string id, bool status)
{
	Control* item = GetControlPtr(id);
	if (item) item->SetStatus(status);
}

void Controls::RemoveEventListener()
{
	if (!coreHwnd) return;
	RemoveWindowSubclass(coreHwnd, ControlProc, controlsProcId);
}

void Controls::SetEventListener()
{
	if (!coreHwnd) return;
	SetWindowSubclass(coreHwnd, ControlProc, controlsProcId, (DWORD_PTR)this);
}

int Controls::FindSubclassItem(std::string itemId)
{
	int index = -1;
	for (int i = 0; i < currentProcesess.size(); i++)
	{
		if (currentProcesess[i].sId == itemId)
		{
			index = i;
			break;
		}
	}

	return index;
}

bool Controls::SetSubclass(std::string itemId, SUBCLASSPROC proc, int procId, DWORD_PTR context)
{
	/*
	HWND h = GetControl(itemId);
	if (h == NULL) {
		MessageBox(NULL, "Subclass control not found", "Warning", MB_OK);
		return false;
	}

	SetWindowSubclass(h, proc, procId, context);
	currentProcesess.push_back(CurrentProcessItem({ itemId, h, proc, procId }));
	*/
	return true;
}

bool Controls::RemoveSubclass(std::string itemId)
{
	int index = FindSubclassItem(itemId);
	if (index < 0)
	{
		MessageBox(NULL, L"While removing: Subclass control not found", L"Warning", MB_OK);
		return false;
	}

	CurrentProcessItem item = currentProcesess[index];
	RemoveWindowSubclass(item.item, item.proc, item.id);
	currentProcesess.erase(currentProcesess.begin() + index);
	return true;
}

int Controls::GetNextControlId()
{
	return ++(Controls::lastControlId);
}

HWND Controls::CreateLine(int x, int y, int width, int height, const char* id, int color, DWORD exStyles)
{
	LineCtrl* ctrl = new LineCtrl(coreHwnd, id, x, y, width, height, GetNextControlId(), L"", WS_CHILD | WS_VISIBLE | SS_OWNERDRAW, exStyles);
	ctrl->SetColor(color);
	return InitControl(ctrl);
}

HWND Controls::CreateCustom(std::string name, const wchar_t* type, int x, int y, int width, int height, DWORD styles, const wchar_t* text, DWORD exStyles)
{
	Control* ctrl = new Control(CONTROL_TYPES::CUSTOM, coreHwnd, name, type, styles, x, y, width, height, GetNextControlId(), text, exStyles);
	return InitControl(ctrl);
}

HWND Controls::CreateListBox(std::string name, int x, int y, int width, int height, DWORD styles, const wchar_t* text, DWORD exStyles)
{
	ListBoxCtrl* ctrl = new ListBoxCtrl(coreHwnd, name, x, y, width, height, GetNextControlId(), text, styles, exStyles);
	return InitControl(ctrl);
}

HWND Controls::CreateCheckBox(std::string name, int x, int y, int width, int height, DWORD styles, const wchar_t* text, DWORD exStyles)
{
	CheckBoxCtrl* ctrl = new CheckBoxCtrl(coreHwnd, name, x, y, width, height, GetNextControlId(), text, styles, exStyles);
	return InitControl(ctrl);
}

HWND Controls::CreateRadioBtn(std::string name, int x, int y, int width, int height, DWORD styles, const wchar_t* text, DWORD exStyles)
{
	RadioBtnCtrl* ctrl = new RadioBtnCtrl(coreHwnd, name, x, y, width, height, GetNextControlId(), text, styles, exStyles);
	return InitControl(ctrl);
}

HWND Controls::CreateBtn(std::string name, int x, int y, int width, int height, DWORD styles, const wchar_t* text, DWORD exStyles)
{
	ButtonCtrl* ctrl = new ButtonCtrl(coreHwnd, name, x, y, width, height, GetNextControlId(), text, styles, exStyles);
	return InitControl(ctrl);
}

HWND Controls::CreateGroupBox(std::string name, int x, int y, int width, int height, DWORD styles, const wchar_t* text, DWORD exStyles)
{
	GroupBoxCtrl* ctrl = new GroupBoxCtrl(coreHwnd, name, x, y, width, height, GetNextControlId(), text, styles, exStyles);
	return InitControl(ctrl);
}

HWND Controls::CreateEdit(std::string name, int x, int y, int width, int height, DWORD styles, const wchar_t* text, DWORD exStyles)
{
	EditCtrl* ctrl = new EditCtrl(coreHwnd, name, x, y, width, height, GetNextControlId(), text, parentIsDlg, styles, exStyles);
	return InitControl(ctrl);
}

HWND Controls::CreateCombobox(std::string name, int x, int y, int width, int height, DWORD styles, const wchar_t* text, DWORD exStyles)
{
	ComboboxCtrl* ctrl = new ComboboxCtrl(coreHwnd, name, x, y, width, height, GetNextControlId(), text, styles, exStyles);
	return InitControl(ctrl);
}

HWND Controls::CreateStatic(std::string name, int x, int y, int width, int height, DWORD styles, const wchar_t* text, DWORD exStyles, DWORD exStyle)
{
	StaticCtrl* ctrl = new StaticCtrl(coreHwnd, name, x, y, width, height, GetNextControlId(), text, styles, exStyles, exStyle);
	return InitControl(ctrl);
}

HWND Controls::InitControl(Control* control)
{
	controls.push_back(control);
	return control->GetHWND();
}

void Controls::Clear()
{
	for (auto& it : controls)
	{
		delete it;
	}

	for (auto it : radioBtnGroups)
	{
		delete it.second;
	}

	controls.clear();
}

void Controls::RemoveControl(HWND item)
{
	Control* control = GetControlPtr(item);
	if (!control) return;

	std::vector<Control*>::iterator it = std::find(controls.begin(), controls.end(), control);
	if (it == controls.end()) return;

	delete control;

	controls.erase(it);

}

std::wstring Controls::GetComboBoxValue(std::string id)
{
	HWND hWnd = GetHWND();
	int itemId = GetControlId(id);

	wchar_t buff[1024] = { 0 };

	int dwSel = (int)SendDlgItemMessage(hWnd, itemId, CB_GETCURSEL, 0, 0);

	if (dwSel != LB_ERR)
	{
		SendDlgItemMessage(hWnd, itemId, CB_GETLBTEXT, dwSel, (LPARAM)buff);
	}

	return std::wstring(*buff != NULL ? buff : L"");
}

int Controls::GetComboBoxSelectedIndex(std::string id)
{
	HWND item = GetControlHWND(id);
	HWND hWnd = GetParent(item);
	int itemId = GetWindowID(item);

	return (int)SendMessageW(item, CB_GETCURSEL, 0, 0);
}

void Controls::RepositionControl(Control* item)
{
	MoveWindow(item->GetHWND(), item->GetX(true), item->GetY(true), item->GetWidth(true), item->GetHeight(true), TRUE);
}

void Controls::OnResize()
{
	for (auto it : controls)
	{
		if (!it->IsResizable()) continue;
		RepositionControl(it);
	}
}

void Controls::SetUnderLineText(HWND hItem)
{
	HFONT hOrigFont = (HFONT)SendMessage(hItem, WM_GETFONT, 0, 0);
	LOGFONT lf;
	GetObject(hOrigFont, sizeof(lf), &lf);
	lf.lfUnderline = TRUE;

	HFONT hFont = CreateFontIndirect(&lf);
	SendMessage(hItem, WM_SETFONT,
		reinterpret_cast<WPARAM> (hFont), 0);
}

template < typename KeyType, typename ValueType>
int Controls::GetNextControlId(const std::map<KeyType, ValueType>& x)
{
	using pairtype = std::pair<KeyType, ValueType>;
	auto id = *std::max_element(x.begin(), x.end(), [](const pairtype& p1, const pairtype& p2)
		{
			return p1.second < p2.second;
		});
	return ++id.second;
}

HWND Controls::GetControlHWND(std::string id)
{
	Control* ctrl = GetControlPtr(id);
	if (!ctrl) return NULL;
	return ctrl->GetHWND();
}

std::string Controls::GetControlName(int id)
{
	Control* ctrl = GetControlPtr(id);
	if (!ctrl) return "";
	return ctrl->GetName();
}

int Controls::GetControlId(std::string id)
{
	Control* ctrl = GetControlPtr(id);
	if (!ctrl) return -1;
	return ctrl->GetId();
}

void Controls::Hide(HWND item)
{
	ShowWindow(item, SW_HIDE);
}

void Controls::Show(HWND item)
{
	ShowWindow(item, SW_SHOW);
}

bool Controls::IsBtnChecked(std::string btnName)
{
	Control* ctrlPtr = GetControlPtr(btnName);
	if (!ctrlPtr) return false;
	if (ctrlPtr->GetTypeId() == CONTROL_TYPES::CHECKBOX)
	{
		return static_cast<CheckBoxCtrl*> (ctrlPtr)->IsChecked();
	}
	else if (ctrlPtr->GetTypeId() == CONTROL_TYPES::RADIO)
	{
		return static_cast<RadioBtnCtrl*> (ctrlPtr)->IsChecked();
	}

	return false;
}

int Controls::GetCheckedRadioBtnId(std::string groupName)
{
	if (radioBtnGroups.find(groupName) == radioBtnGroups.end()) return -1;
	return radioBtnGroups[groupName]->GetCheckedId();
}

void Controls::SetBtnCheck(std::string btnName, bool check)
{
	Control* ctrlPtr = GetControlPtr(btnName);
	if (!ctrlPtr) return;

	if (ctrlPtr->GetTypeId() == CONTROL_TYPES::CHECKBOX)
	{
		static_cast<CheckBoxCtrl*> (ctrlPtr)->SetCheck(check);
	}
	else if (ctrlPtr->GetTypeId() == CONTROL_TYPES::RADIO)
	{
		RadioBtnCtrl* ctrl = static_cast<RadioBtnCtrl*> (ctrlPtr);
		if (check == true && !ctrl->IsChecked())
		{
			ctrl->GetGroupPtr()->UncheckAll();
			ctrl->SetCheck(check);
		}
	}
}

void Controls::CreateRadioGroup(std::string name, std::vector<std::string > items, std::string checkedItem)
{
	RadioBtnGroup* rg = new RadioBtnGroup();
	for (auto it : items)
	{
		Control* ptr = GetControlPtr(it);
		if (!ptr) continue;
		RadioBtnCtrl* radioCtr = static_cast<RadioBtnCtrl*> (ptr);
		bool checked = checkedItem == it;
		radioCtr->SetCheck(checked);
		rg->items.push_back(radioCtr);
		radioCtr->SetGroupPtr(rg);
	}

	radioBtnGroups[name] = rg;
}

void Controls::SetLabel(std::string id, const wchar_t* label)
{
	Control* ctrl = GetControlPtr(id);
	if (!ctrl) return;
	static_cast<EditCtrl*> (ctrl)->SetLabel(label);
}

void Controls::SetLParam(std::string id, int v)
{
	Control* ctrl = GetControlPtr(id);
	if (!ctrl) return;
	ctrl->SetLParam(v);
}
void Controls::SetFontSize(std::string id, int fontSize)
{
	Control* item = GetControlPtr(id);

	if (item)
	{
		item->SetNewFontSize((float)fontSize);
		item->CreateNewFont();

		HFONT font = item->GetDefaultFont(fontSize);
		SendMessage(item->GetHWND(), WM_SETFONT,
			(WPARAM)font, 0);
	}
}

void Controls::SetEditMask(std::string id, int maskId)
{
	Control* item = GetControlPtr(id);
	if (item)
	{
		static_cast<EditCtrl*> (item)->SetMask(maskId);
	}
}

LRESULT CALLBACK Controls::ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	Controls* lpData = (Controls*)dwRefData;

	HDC hdc = (HDC)wParam;
	HBRUSH hbrBackground = NULL;

	switch (uMsg)
	{
	case WM_CTLCOLORBTN:
	{
		SetBkMode(HDC(wParam), TRANSPARENT);
		return LRESULT(HBRUSH(GetStockObject(NULL_BRUSH)));
	}

	/*

		   case WM_ERASEBKGND: { 	return TRUE;
		   }

		   case WM_CTLCOLORBTN: { 	SetBkMode(HDC(wParam), TRANSPARENT);
			   return LRESULT(HBRUSH(GetStockObject(NULL_BRUSH)));
		   }

		   case WM_CTLCOLOREDIT: { 	HBRUSH editBrush = CreateSolidBrush(RGB(244, 244, 244));
			   SetTextColor(hdc, RGB(113, 113, 113));

			   SetBkMode(HDC(wParam), TRANSPARENT);
			   return LRESULT(editBrush);
		   }

		   */


	case WM_CTLCOLORSTATIC:
	{
		Control* item = lpData->GetControlPtr((HWND)lParam);
		if (!item)
		{

			break;
		}

		SetTextColor(hdc, RGB(0, 0, 0));

		if (item)
		{
			if (item->GetCtrlColor() == APP_COLORS::RED)
			{
				SetTextColor(hdc, RGB(237, 41, 57));
			}
			else if (item->GetCtrlColor() == APP_COLORS::BLUE)
			{
				SetTextColor(hdc, RGB(52, 152, 219));
			}
			else if (item->GetCtrlColor() == APP_COLORS::GREEN)
			{
				SetTextColor(hdc, RGB(34, 139, 34));
			}
			else if (item->GetCtrlColor() == APP_COLORS::ORANGE)
			{
				SetTextColor(hdc, RGB(243, 156, 18));
			}
		}
		
		SetBkColor(hdc, RGB(255, 255, 255));

		hbrBackground = GetSysColorBrush(COLOR_WINDOW);
		return (INT_PTR)hbrBackground;
	}

	case WM_DRAWITEM:
	{
		LPDRAWITEMSTRUCT pdis = (LPDRAWITEMSTRUCT)lParam;

		if (lpData->redrawItems == false) break;

		Control* ctrlPtr = lpData->GetControlPtr(pdis->CtlID);
		if (!ctrlPtr) break;

		ctrlPtr->Draw(pdis);

		return TRUE;
	}

	case WM_MEASUREITEM:
	{
		MEASUREITEMSTRUCT* m = (MEASUREITEMSTRUCT*)lParam;

		if (m->CtlType == ODT_COMBOBOX)
		{
			m->itemHeight = 25;
			return TRUE;
		}
		else if (m->CtlType == ODT_LISTBOX)
		{
			m->itemHeight = lpData->listBoxItemHeight;
			return TRUE;
		}

		break;
	}

	/*
			   case WM_SIZE:
			   {
				lpData->OnResize();
				   break;
			   }

			   */
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}