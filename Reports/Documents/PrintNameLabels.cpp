#include "PrintNameLabels.h"
#include "..//..//PBConnection.h"
#include "..//..//Components/PreloaderComponent.h"

PrintNameLabels::PrintNameLabels(HWND hw, HINSTANCE hI) : PageCommon(hw, hI)
{
	itemsScrollbar = NULL;
	printer = NULL;

	nameFormats[NameFormat::FORMAL] = L"Formal name (Mr. & Mrs.)";
	nameFormats[NameFormat::INFORMAL] = L"Informal name";

	currentNameFormat = NameFormat::FORMAL;
}

PrintNameLabels::~PrintNameLabels()
{
	PBConnection::AbortAllRequests();
	PreloaderComponent::Stop();
	delete itemsScrollbar;
}

bool PrintNameLabels::OnCommand(WPARAM wParam)
{
	UINT wId = LOWORD(wParam);

	switch (HIWORD(wParam))
	{
	case BN_CLICKED:
	{
		if (wId == GetControlId("deleteAll"))
		{
			int res = MessageDlg(hWnd, L"Confirmation",
				L"Are you sure you want to delete all the labels?",
				MD_YESNO, MD_WANING).OpenDlg();

			if (res == IDYES)
			{
				labelItems.clear();
				UpdateLabels();
			}

			return true;
		}
		else if (wId == GetControlId("addFam"))
		{
			USSelectedUser usData = USSelectedUser();
			UserSearcher* us = new UserSearcher(hWnd, &usData, UST_FAMILIESONLY);
			int answer = us->ShowDlg(hWnd);

			if (answer == IDOK)
			{
				AddFam(usData.id);
			}

			delete us;

			return true;
		}
		else if (wId == GetControlId("addAllFam"))
		{
			int res = MessageDlg(hWnd, L"Confirmation",
				L"Are you sure you want to add all the families?",
				MD_YESNO, MD_WANING).OpenDlg();

			if (res == IDYES)
			{
				labelItems.clear();
				AddAllFams();
			}

			return true;
		}
		else if (wId == GetControlId("print"))
		{
			printer = std::make_shared<Printer>(hWnd);
			printer->SetDrawPageNum(false);
			printer->SetDrawPrintDate(false);
			printer->SetUserBorder(false);
			printer->SetMinusPrinterBorder(true);
			printer->SetPaperSize(DMPAPER_LETTER);

			if (printer->InitPriner(std::bind(&PrintNameLabels::DrawLabels, this, std::placeholders::_1), false))
			{
				printer->Print();
			}

			return true;
		}

		break;
	}
	case CBN_SELCHANGE:
	{
		if (wId == GetControlId("nameFormat"))
		{
			OnNameFormatChanged();
			return true;
		}

		break;
	}
	}

	return false;
}

void PrintNameLabels::AddControls()
{
	AddAllFams();

	// AddFam(385822);

	HWND itemsBody = CreateStatic("itemsBody", 40, 130, -40, -40, WS_VISIBLE | WS_CHILD | SS_NOTIFY);

	itemsScrollbar = new ScrollBar(itemsBody, hInst);
	itemsScrollbar->CreateScroll(21, -1);
	itemsScrollbar->SetHScrollRange(-1);
	itemsScrollbar->SetVScrollRange(4000);

	SetWindowSubclass(itemsBody, PageMainProc, IDC_STATIC, (DWORD_PTR)this);

	CreateStatic("sm", 10, 10, 300, 20, WS_VISIBLE | WS_CHILD, L"Name format:");
	HWND monthH = CreateCombobox("nameFormat", 10, 40, 300, 20, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | CBS_OWNERDRAWFIXED | WS_TABSTOP | WS_VSCROLL);

	for (auto it: nameFormats)
	{
		SendMessage(monthH, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)it.second.c_str());
	}
	SendMessage(monthH, CB_SETCURSEL, (WPARAM)0, (LPARAM)0);



	CreateBtn("addAllFam", 330, 40, 150, 30, WS_CHILD | WS_VISIBLE, L"Add all families", 0);
	CreateBtn("addFam", 490, 40, 130, 30, WS_CHILD | WS_VISIBLE, L"Add family", 0);

	CreateBtn("deleteAll", 630, 40, 120, 30, WS_CHILD | WS_VISIBLE | BS_RIGHT, L"Delete all", 0);
	SetColor("deleteAll", APP_COLORS::RED);
	SetButtonIcon("deleteAll", IDB_DELETE);


	CreateBtn("print", 10, 40, 130, 30, WS_CHILD | WS_VISIBLE, L"Print", WS_STICK_RIGHT);

}

void PrintNameLabels::OnNameFormatChanged()
{
	int index = (int)SendMessage(GetControlHWND("nameFormat"), CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
	currentNameFormat = (NameFormat)index;
	UpdateLabels();
}

void PrintNameLabels::UpdateLabels()
{
	InvalidateRect(GetControlHWND("itemsBody"), NULL, true);
}

void PrintNameLabels::AddAllFams()
{
	PreloaderComponent::Start(hWnd);

	PBConnection::Request("getAllFamiliesForLables", {},
		[&](PBResponse res)
		{
			std::vector<std::map<std::string, std::wstring>> result = res.GetResult();
			for (int i = 0; i < result.size(); i++)
			{
				AddFamilyToList(result[i]);
			}
			UpdateLabels();
			PreloaderComponent::Stop();
		}
	);
}

void PrintNameLabels::AddFam(int famId)
{
	PBConnection::Request("getFamilyForLables", { {"familyId", std::to_wstring(famId)}},
		[&](PBResponse res)
		{
			std::vector<std::map<std::string, std::wstring>> result = res.GetResult();
			if (result.size() > 0)
			{
				AddFamilyToList(result[0], true);
			}
			UpdateLabels();
		}
	);
	
}

void PrintNameLabels::AddFamilyToList(std::map<std::string, std::wstring> result, bool inFront)
{
	std::wstring plus4Str = result["plus4"];
	plus4Str = plus4Str.size() > 0 ? L"-" + plus4Str : L"";

	NameLabelItem labelItem = NameLabelItem(
		{
		GetInt(result["id"]),
		result["mailingName"],
		result["informalName"],
		result["address"],
		result["city"] + L", " + result["state"] + L", " + result["zip"] + plus4Str,
		RECT(),
		RECT()
		}
	);

	if (inFront)
	{
		labelItems.insert(labelItems.begin(), labelItem);
	}
	else
	{
		labelItems.push_back(labelItem);
	}
}

std::wstring PrintNameLabels::GetFamilyName(const NameLabelItem& item)
{
	return currentNameFormat == NameFormat::INFORMAL ? item.informalName : item.formalName;
}

INT_PTR CALLBACK PrintNameLabels::testDraw(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	UNREFERENCED_PARAMETER(lParam);
	switch (uMsg)
	{
	case WM_CTLCOLORDLG:
		return (INT_PTR)GetStockObject(WHITE_BRUSH);
	case WM_INITDIALOG:
	{
		return (INT_PTR)TRUE;
	}

	case WM_COMMAND:
	{
		if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
		{
			EndDialog(hWnd, LOWORD(wParam));
			return (INT_PTR)TRUE;
		}

		break;
	}
	}

	return (INT_PTR)FALSE;
}

LRESULT CALLBACK PrintNameLabels::PageMainProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	PrintNameLabels* lpData = (PrintNameLabels*)dwRefData;
	HDC hdc = (HDC)wParam;

	switch (uMsg)
	{
	case WM_ERASEBKGND:
	{
		return false;
	}

	case WM_PAINT:
	{
		PAINTSTRUCT ps;

		hdc = BeginPaint(hWnd, &ps);

		lpData->DrawLabelsToControl(hdc);

		EndPaint(hWnd, &ps);

		break;
	}

	case WM_LBUTTONDOWN:
	{
		int x = GET_X_LPARAM(lParam), y = GET_Y_LPARAM(lParam);

		for (auto it : lpData->labelItems)
		{
			RECT rcDum;
			RECT mouseRc = { x, y, x + 1, y + 1
			};

			if (IntersectRect(&rcDum, &mouseRc, &it.deleteRc))
			{
				std::vector<NameLabelItem>::iterator itItem = std::find_if(lpData->labelItems.begin(), lpData->labelItems.end(), [&](NameLabelItem fItem)
					{
						return fItem.id == it.id;
					});
				if (itItem != lpData->labelItems.end())
				{
					lpData->labelItems.erase(itItem);
				}

				InvalidateRect(hWnd, NULL, TRUE);
			}
		}

		break;
	}

	case WM_MOUSEWHEEL:
	{
		short zDelta = (short)(0xFFFF & (wParam >> 16));
		if (zDelta > 0)
			PostMessage(lpData->itemsScrollbar->GetHScrollHWND(), WM_VSCROLL, SB_LINEUP, NULL);
		else
			PostMessage(lpData->itemsScrollbar->GetHScrollHWND(), WM_VSCROLL, SB_LINEDOWN, NULL);
		break;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);

}

void PrintNameLabels::DrawLabelsToControl(HDC& hdc)
{
	RECT rc;

	HWND thisHwnd = GetControlHWND("itemsBody");

	GetWindowRect(thisHwnd, &rc);

	HDC hdcMem = CreateCompatibleDC(hdc);

	int bodyX = 0;
	int bodyY = 0;
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	HBITMAP bitmap = CreateCompatibleBitmap(hdc, width, height);
	HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcMem, bitmap);

	Graphics gr(hdcMem);

	SolidBrush bgBrush(GetColor(APP_COLORS::WHITE));
	SolidBrush fontBrush(GetColor(APP_COLORS::BLACK));
	Pen borderPen(GetColor(APP_COLORS::GRAY_BORDER));

	gr.FillRectangle(&bgBrush, 0, 0, width, height);

	int totalItems = (int)labelItems.size();
	int itemsMarginX = 10;
	int itemsPerLine = 5;
	int itemWidth = (width - 21 - itemsMarginX * itemsPerLine) / itemsPerLine;	// 21 - scrollbar size
	int itemHeight = 150;

	Bitmap* pBmp = LoadImageFromResource(NULL, MAKEINTRESOURCEA(IDB_LABEL_PRINT_FRAME), "PNG");
	Bitmap* iconDelete = LoadImageFromResource(NULL, MAKEINTRESOURCEA(IDB_ERR), "PNG");

	int scrollPos = itemsScrollbar->GetScrollBarPos(SB_VERT);
	int itemX = itemsMarginX, itemY = itemsMarginX;

	itemY -= scrollPos;

	int totalHeight = 0;

	for (int i = 0, j = 0; i < totalItems; i++)
	{
		if (i > 0)
		{
			if (i % itemsPerLine == 0)
			{
				j++;
				itemX = itemsMarginX;
				itemY = (itemsMarginX * j) + (itemsMarginX + itemHeight * j);
				itemY -= scrollPos;
			}
			else
			{
				itemX += itemsMarginX + itemWidth;
			}
		}

		RECT rcDum;
		RECT rc1 = { bodyX, bodyY, width, height
		};

		RECT rc2 = { itemX, itemY, itemX + itemWidth, itemY + itemHeight
		};

		//check if visible

		if (!IntersectRect(&rcDum, &rc1, &rc2))
		{
			continue;
		}

		gr.FillRectangle(&bgBrush, itemX, itemY, itemWidth, itemHeight);
		gr.DrawRectangle(&borderPen, itemX, itemY, itemWidth - 1, itemHeight - 1);
		//    gr.DrawImage(pBmp, itemX, itemY, itemWidth, itemHeight);

		std::wstring famName = GetFamilyName(labelItems[i]);
		std::wstring famAdress = labelItems[i].address;
		std::wstring cityInfo = labelItems[i].city;

		std::wstring fullStr = famName + L"\n" + famAdress + L"\n" + cityInfo;

		//Mesure string
		RectF layoutRect((REAL)0, (REAL)0, (REAL)itemWidth * 5, (REAL)itemHeight);
		RectF boundRect;

		gr.MeasureString(fullStr.c_str(), (INT)fullStr.length(), currentFont, layoutRect, &boundRect);

		if (boundRect.Width > itemWidth)
		{
			size_t pos = famName.find(L" and ");
			if (pos != string::npos)
			{
				std::wstring toReplace = L" and ";
				famName.replace(pos, toReplace.size(), L" and\n");
			}
			else
			{
				pos = famName.find(L" &");
				if (pos != string::npos)
				{
					std::wstring toReplace = L" &";
					famName.replace(pos, toReplace.size(), L" &\n");
				}
			}

			fullStr = famName + L"\n" + famAdress + L"\n" + cityInfo;
			gr.MeasureString(fullStr.c_str(), (INT)fullStr.length(), currentFont, layoutRect, &boundRect);
		}

		std::wstring lastErr = GetLastErrorAsString();

		int textX = itemX + (itemWidth - (int)boundRect.Width) / 2;
		int textY = itemY + (itemHeight - (int)boundRect.Height) / 2;

		PointF origin((REAL)textX, (REAL)textY);

		gr.DrawString(fullStr.c_str(), (INT)fullStr.length(), currentFont, origin, &fontBrush);

		labelItems[i].deleteRc = { itemX, itemY, itemX + itemWidth, itemY + itemHeight
		};

		{

			int delX = 0, delY = 0, delWidth = static_cast<int> (itemHeight * 0.1f), delHeight = delWidth;

			delX = itemX + (itemWidth - delWidth) / 2;
			delY = itemY + itemHeight - delHeight - 20;
			gr.DrawImage(iconDelete, delX, delY, delWidth, delHeight);

			labelItems[i].deleteRc = { delX, delY, delX + delWidth, delY + delHeight
			};

		}
	}

	totalHeight = itemY + itemHeight + itemsMarginX + scrollPos;
	itemsScrollbar->SetVScrollRange(totalHeight);

	delete pBmp;

	BitBlt(hdc, 0, 0, width, height, hdcMem, 0, 0, SRCCOPY);

	SelectObject(hdcMem, oldBitmap);
	DeleteObject(bitmap);
	DeleteDC(hdcMem);
}

void PrintNameLabels::DrawLabels(PrinterDrawer* printerDrawer)
{

	float itemWidth = 66.5f;
	float itemHeight = 25.0f;

	float borderPaddingX = 4.0f;
	float borderPaddingY = 13.0f;
	float itemsPaddingX = 3.0f;

	float textXOffset = 10.0f;
	float textYOffset = 11.0f;

	int totalItems = (int)labelItems.size();
	int itemsPerLine = 3;
	int maxRowsOnPage = 10;

	float itemX = borderPaddingX, itemY = borderPaddingY;

	for (int i = 0, j = 0; i < totalItems; i++)
	{
		if (i > 0)
		{
			if (i % itemsPerLine == 0)
			{
				j++;
				itemX = borderPaddingX;
				itemY = borderPaddingY + itemHeight * j;
			}
			else
			{
				itemX += itemsPaddingX + itemWidth;
			}
		}

		if (j >= 10)
		{
			j = 0;
			itemY = borderPaddingY;
			printer->StartNewPage();
		}

		std::wstring famName = GetFamilyName(labelItems[i]);
		std::wstring famAdress = labelItems[i].address;
		std::wstring cityInfo = labelItems[i].city;

		std::wstring fullStr = famName + L"\n" + famAdress + L"\n" + cityInfo;
		
		RectF strSize = printer->MeasureText(fullStr);

		if (strSize.Width > itemWidth) {
			size_t pos = famName.find(L" and ");
			if (pos != string::npos) {
				std::wstring toReplace = L" and ";
				famName.replace(pos, toReplace.size(), L" and\n");
			}
			else {
				pos = famName.find(L" &");
				if (pos != string::npos) {
					std::wstring toReplace = L" &";
					famName.replace(pos, toReplace.size(), L" &\n");
				}
			}

			fullStr = famName + L"\n" + famAdress + L"\n" + cityInfo;
		}

		float textX = itemX + textXOffset;
		float textY = itemY + textYOffset;

		printerDrawer->DrawStr(fullStr, textX, textY);

	}
		

}