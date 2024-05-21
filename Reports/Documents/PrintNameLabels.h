/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "../../Common/PageCommon.h"
#include "../../Components/UserSearcher.h"
#include "..//../Components/Printer/Printer.h"

using namespace std;


struct NameLabelItem {
	int id;
	std::wstring formalName, informalName, address, city;
	RECT itemRc;
	RECT deleteRc;
};



class PrintNameLabels : public PageCommon
{
	enum struct NameFormat
	{
		FORMAL, INFORMAL
	};
public:


	PrintNameLabels(HWND hWnd, HINSTANCE hInst);
	~PrintNameLabels();

	void AddAllFams();
	void AddFam(int famId);
	void AddControls();
	void DrawLabelsToControl(HDC& hdc);

	void DrawLabels(PrinterDrawer* printer);

	void UpdateLabels();
	bool OnCommand(WPARAM wParam);

private:
	void OnNameFormatChanged();
	void AddFamilyToList(std::map<std::string, std::wstring> result, bool inFront = false);
	std::wstring GetFamilyName(const NameLabelItem& item);

	static INT_PTR CALLBACK testDraw(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static LRESULT CALLBACK PageMainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

	std::shared_ptr<Printer> printer;
	NameFormat currentNameFormat;
	ScrollBar* itemsScrollbar;
	std::vector<NameLabelItem> labelItems;

	std::map<NameFormat, std::wstring> nameFormats;
};