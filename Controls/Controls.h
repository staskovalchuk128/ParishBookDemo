/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "commctrl.h"
#include "windowsx.h"
#include "gdiplus.h"
#include "Control.h"
#include "..//Drawing/DrawCommon.h"

using namespace Gdiplus;

struct CurrentProcessItem {
	std::string sId;
	HWND item;
	SUBCLASSPROC proc;
	int id;
};

class Controls : public DrawCommon 
{
public:
	static int controlsCount;
	bool redrawItems;
	bool useLocalCoord;

	bool SetSubclass(std::string itemId, SUBCLASSPROC proc, int procId, DWORD_PTR context);
	int FindSubclassItem(std::string itemId);
	bool RemoveSubclass(std::string itemId);



	Controls(HWND hWnd);
	virtual ~Controls();


	void ReinitControls(HWND hWnd);

	HWND CreateCustom(std::string name, const wchar_t* type, int x, int y, int width, int height, DWORD styles = WS_CHILD | WS_VISIBLE, const wchar_t* text = L"", DWORD exStyles = 0);
	HWND CreateListBox(std::string name, int x, int y, int width, int height, DWORD styles = WS_CHILD | WS_VISIBLE, const wchar_t* text = L"", DWORD exStyles = 0);
	HWND CreateCheckBox(std::string name, int x, int y, int width, int height, DWORD styles = WS_CHILD | WS_VISIBLE, const wchar_t* text = L"", DWORD exStyles = 0);
	HWND CreateRadioBtn(std::string name, int x, int y, int width, int height, DWORD styles = WS_CHILD | WS_VISIBLE, const wchar_t* text = L"", DWORD exStyles = 0);
	HWND CreateBtn(std::string name, int x, int y, int width, int height, DWORD styles = WS_CHILD | WS_VISIBLE, const wchar_t* text = L"", DWORD exStyles = 0);
	HWND CreateGroupBox(std::string name, int x, int y, int width, int height, DWORD styles = WS_VISIBLE | WS_CHILD | BS_GROUPBOX, const wchar_t* text = L"", DWORD exStyles = 0);
	HWND CreateEdit(std::string name, int x, int y, int width, int height, DWORD styles = WS_VISIBLE | WS_CHILD | WS_TABSTOP, const wchar_t* text = L"", DWORD exStyles = 0);
	HWND CreateCombobox(std::string name, int x, int y, int width, int height, DWORD styles = CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | CBS_OWNERDRAWFIXED, const wchar_t* text = L"", DWORD exStyles = 0);
	HWND CreateStatic(std::string name, int x, int y, int width, int height, DWORD styles = WS_CHILD | WS_VISIBLE, const wchar_t* text = L"", DWORD exStyles = 0, DWORD exStyle = 0);
	HWND CreateLine(int x, int y, int width, int height, const char* id, int color, DWORD exStyles = 0);
	void OnResize();


	void Clear();
	void RemoveControl(HWND item);

	std::wstring GetComboBoxValue(std::string id);
	int GetComboBoxSelectedIndex(std::string id);


	template<typename KeyType, typename ValueType>
	int GetNextControlId(const std::map<KeyType, ValueType>& x);

	HWND GetControlHWND(std::string id);
	std::string GetControlName(int id);
	int GetControlId(std::string id);

	void RepositionControl(Control* item);

	void Hide(HWND item);
	void Show(HWND item);

	void RemoveEventListener();
	void SetEventListener();

	void SetUnderLineText(HWND hItem);


	void SetButtonIcon(std::string btnId, int icon);
	bool IsBtnChecked(std::string btnName);
	int GetCheckedRadioBtnId(std::string groupName);
	void SetBtnCheck(std::string btnName, bool check);


	void CreateRadioGroup(std::string name, std::vector<std::string> items, std::string checkedItem);

	void SetLabel(std::string id, const wchar_t* label);


	void SetLParam(std::string id, int v);
	void SetFontSize(std::string id, int fontSize);
	void SetEditMask(std::string id, int maskId);

	void SetColor(std::string id, int color);
	void SetStyle(std::string id, int style);
	void SetStatus(std::string id, bool status);
	void SetListBoxItemHeight(int h);

	int GetListBoxItemHeight();
	int GetTotalParentWidth();
	int GetTotalParentHeight();
	Control* GetControlPtr(std::string name);
	Control* GetControlPtr(int id);
	Control* GetControlPtr(HWND h);
	HWND GetHWND();
	static std::vector<Controls*> GetAllControls();
	std::vector<Control*> GetCurrentControls();
private:
	HWND coreHwnd;
	bool parentIsDlg;
	std::vector<Control*> controls;
	int totalParentHeight, totalParentWidth;
	int listBoxItemHeight;
	int comboBoxOffset;
	static int lastControlId;
	std::vector<int> groupBoxes;
	std::vector<CurrentProcessItem> currentProcesess;
	static std::vector<Controls*> allControls;


	int controlsProcId;
	HWND InitControl(Control* control);
	int GetNextControlId();

	std::map<std::string, RadioBtnGroup*> radioBtnGroups;
protected:
	static LRESULT CALLBACK ControlProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};