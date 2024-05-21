#include "AddBatch.h"
#include "..//Common/MessageDlg.h"
#include "..//PBConnection.h"

AddBatch::AddBatch(HWND hWnd, HINSTANCE hInst, int batchId) : DlgCommon(hWnd)
{
	iFundId = 0;
	currentBatchId = batchId;
	dialogHwnd = NULL;

	const char* title = currentBatchId > 0 ? "Editing Batch" : "Adding Batch";
	OpenDlg(hWnd, title);
}

AddBatch::~AddBatch() {}

int AddBatch::GetBatchId()
{
	return currentBatchId;
}

bool AddBatch::OnDlgCommand(WPARAM wParam)
{
	UINT wId = LOWORD(wParam);

	switch (HIWORD(wParam))
	{
	case BN_CLICKED:
	{
		//Check if `Name as` batch btn clicked
		std::vector<std::string> strArr = SplitStr(GetControlPtr(wId)->GetName(), '_');

		if (strArr.size() > 1 && strArr[0] == "batchName")
		{
			AutoFillBatchDesc(GetWindowTextStr(GetDlgItem(hWnd, wId)));
			return true;
		}
		else if (wId == GetControlId("btnSave"))
		{
			try
			{
				SaveBatch();
			}

			catch (std::wstring err)
			{
				MessageDlg(hWnd, L"Error",
					err.c_str(),
					MD_OK, MD_ERR).OpenDlg();
			}

			catch (std::exception& e)
			{
				MessageDlg(hWnd, L"Error",
					StrToWStr(e.what()).c_str(),
					MD_OK, MD_ERR).OpenDlg();
			}

			return true;
		}
		else if (wId == GetControlId("btnClose"))
		{
			EndDialog(hWnd, wId);
			return true;
		}

		break;
	}
	}

	return false;
}

void AddBatch::SaveBatch()
{
	int fundindex = (int)SendMessage(GetControlHWND("funds"), (UINT)CB_GETCURSEL, (WPARAM)0, (LPARAM)0);
	int fundId = fundsIndexes[fundindex];

	std::wstring bDesc = GetWindowTextStr(GetControlHWND("description")),
		contDate = GetWindowTextStr(GetControlHWND("date")),
		amount = GetWindowTextStr(GetControlHWND("amount")),
		comment = GetWindowTextStr(GetControlHWND("comment"));

	contDate = ChangeDateFormat(contDate);

	amount = GetFloat(amount) == 0 ? L"0.0" : amount;


	if (bDesc.length() == 0)
	{
		throw std::wstring(L"Enter description");
	}
	else
	{


		PBConnection::Request("saveBatch",
			{
				{"batchId", std::to_wstring(currentBatchId) },
				{"description", bDesc },
				{"date", contDate },
				{"amount", amount },
				{"comment", comment },
				{"fundId", std::to_wstring(fundId) },
			},
			[&](PBResponse res)
			{
				currentBatchId = GetInt(res.GetResultMap("main")["id"]);
				EndDialog(hWnd, 0);
			}
		);
	}
		/*
		DB db;



		int batchId = 0;

		std::string sqlQuery;

		if (currentBatchId > 0)
		{
			sqlQuery = "UPDATE batches SET `fundId` = ?, `description` = ?,`date` = ?,`amount` = ?,`comment` = ? WHERE `id` = ?";

			db.SetData(sqlQuery, { std::to_wstring(fundId), bDesc, contDate, amount, comment, std::to_wstring(currentBatchId) });

			batchId = 0;

			MessageDlg(hWnd, L"Success",
				L"Batch updated successfuly",
				MD_OK, MD_SUCCESS).OpenDlg();
		}
		else
		{

			sqlQuery = "INSERT INTO batches (`fundId`, `description`, `date`, `amount`, `comment`) VALUES(?,?,?,?,?)";

			DBResults ir = db.SetData(sqlQuery, { std::to_wstring(fundId), bDesc, contDate, amount, comment });

			currentBatchId = batchId = ir.lastInsertId;
		}
	}
	*/

}

bool AddBatch::AutoFillBatchDesc(std::wstring name)
{
	HWND descH = GetControlHWND("description");
	std::wstring desc = name;

	std::wstring contDate = GetWindowTextStr(GetControlHWND("date"));

	if (contDate.length() > 0)
	{
		std::wstring nDateFormat = ChangeDateFormat(contDate);
		std::replace(contDate.begin(), contDate.end(), '/', '-');	// replace all 'x' to 'y'
		// nDateFormat = nDateFormat.substr(0, 5) + nDateFormat.substr(8, 2) + "-" + nDateFormat.substr(5, 2);
		nDateFormat = nDateFormat.substr(5, 2) + nDateFormat.substr(8, 2) + L"-" + nDateFormat.substr(0, 5);
		desc += L" " + contDate;
	}

	SetWindowTextW(descH, desc.c_str());

	return true;
}

void AddBatch::OnInitMainDialog()
{

	int controlWidth = 280;
	int offsetBC = 25, offsetBG = 50;
	int lcX = 20, lcY = 40, rcX = 320, rcY = 40;

	/*LEFT COL*/
	CreateStatic("sFund", lcX, lcY, controlWidth, 20, WS_VISIBLE | WS_CHILD, L"Fund:");
	HWND fundH = CreateCombobox("funds", lcX, lcY += offsetBC, controlWidth, 20, CBS_DROPDOWNLIST | CBS_HASSTRINGS | WS_CHILD | WS_OVERLAPPED | WS_VISIBLE | CBS_OWNERDRAWFIXED | WS_TABSTOP | WS_VSCROLL);

	CreateStatic("sAmount", lcX, lcY += offsetBG, controlWidth, 20, WS_VISIBLE | WS_CHILD, L"Amount:");
	CreateEdit("amount", lcX, lcY += offsetBC, controlWidth, 30, WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_MULTILINE | WS_TABSTOP, sAmount.c_str());

	/*END LEFT COL*/

	/*RIGHT COL*/
	CreateStatic("sDate", rcX, rcY, controlWidth, 20, WS_VISIBLE | WS_CHILD, L"Date:");
	HWND dateHwnd = CreateCustom("date", DATETIMEPICK_CLASS, rcX, rcY += offsetBC, controlWidth, 30, WS_VISIBLE | WS_CHILD | WS_TABSTOP);

	SYSTEMTIME st = GetSysDate(sDate);
	DateTime_SetFormat(dateHwnd, L"MM/dd/yyyy");
	DateTime_SetSystemtime(dateHwnd, GDT_VALID, &st);

	CreateStatic("sDesc", rcX, rcY += offsetBG, controlWidth, 20, WS_VISIBLE | WS_CHILD, L"Description:");
	CreateEdit("description", rcX, rcY += offsetBC, controlWidth, 30, WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_MULTILINE | WS_TABSTOP, sDesc.c_str());

	CreateStatic("sComment", rcX, rcY += offsetBG, controlWidth, 20, WS_VISIBLE | WS_CHILD, L"Comment:");
	CreateEdit("comment", rcX, rcY += offsetBC, controlWidth, 30, WS_VISIBLE | WS_CHILD | ES_AUTOHSCROLL | ES_MULTILINE | WS_TABSTOP, sComment.c_str());

	/*END RIGHT COL*/

	int nameAsGBExtraHeight = 0;
	//Get batch names
	{
		int brow = 0;
		int maxWidth = 400;
		int itemX = 30, itemY = 310;
		int itemsOffsetX = 20;
		int itemWidth = 200, itemHeight = 30;

		for (int i = 0; i < batchNames.size(); i++)
		{
			std::wstring bname = batchNames[i]["name"];
			std::string controlName = "batchName_" + WstrToStr(bname);




			itemY = 310 + (itemsOffsetX + itemHeight) * brow;

			CreateBtn(controlName, itemX, itemY, itemWidth, itemHeight, WS_CHILD | WS_VISIBLE, bname.c_str());
			SetColor(controlName, APP_COLORS::GRAY);

			itemX += itemWidth + itemsOffsetX;

			if (itemX > maxWidth)
			{
				itemX = 30;
				brow++;
			}
		}

		nameAsGBExtraHeight = (itemHeight + 30) * (brow + 1);

	}


	//CreateBtn("naOffertory", 30, 310, 130, 30, WS_CHILD | WS_VISIBLE, L"OFFERTORY");
	//CreateBtn("naOfficeCollection", 180, 310, 180, 30, WS_CHILD | WS_VISIBLE, L"OFFICE COLLECTION");
	//SetColor("naOffertory", APP_COLORS::GRAY);
	//SetColor("naOfficeCollection", APP_COLORS::GRAY);

	CreateGroupBox("sbd", 10, 10, rcX + controlWidth, rcY += 40, WS_VISIBLE | WS_CHILD | BS_GROUPBOX, L"Batch Details");
	CreateGroupBox("sna", 10, rcY + 20, rcX + controlWidth, nameAsGBExtraHeight, WS_VISIBLE | WS_CHILD | BS_GROUPBOX, L"Name as");

	int sFundId = 0;

	for (int i = 0; i < funds.size(); i++)
	{
		int fI = static_cast<int> (SendMessage(fundH, (UINT)CB_ADDSTRING, (WPARAM)0, (LPARAM)funds[i]["name"].c_str()));
		if (GetInt(funds[i]["id"]) == iFundId) sFundId = fI;
		fundsIndexes[fI] = GetInt(funds[i]["id"]);
	}

	SendMessage(fundH, CB_SETCURSEL, (WPARAM)sFundId, (LPARAM)0);

	CreateBtn("btnClose", 10, rcY + 40 + nameAsGBExtraHeight, 120, 30, WS_CHILD | WS_VISIBLE, L"Close", WS_STICK_RIGHT);
	SetColor("btnClose", APP_COLORS::GRAY);

	CreateBtn("btnSave", 150, rcY + 40 + nameAsGBExtraHeight, 120, 30, WS_CHILD | WS_VISIBLE, L"Save", WS_STICK_RIGHT);

	SetFocus(GetControlHWND("amount"));

}

void AddBatch::OnDlgInit()
{
	PBConnection::Request("getInfoForAddEditBatch", { { "batchId", std::to_wstring(currentBatchId) }},
		[&](PBResponse res)
		{
			if (currentBatchId > 0)
			{
				std::map<std::string, std::wstring> batchInfo = res.GetResultMap("batchInfo");

				sDesc = batchInfo["description"], sDate = batchInfo["date"],
					sAmount = batchInfo["amount"], sComment = batchInfo["comment"],
					iFundId = GetInt(batchInfo["fundId"]);
			}

			batchNames = res.GetResultVec("bNames");
			funds = res.GetResultVec("funds");

			OnInitMainDialog();
			DoneLoadingDlg();
		}
	);
}

INT_PTR CALLBACK AddBatch::MainDialogProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_INITDIALOG:
	{
		OnInitMainDialog();
		return (INT_PTR)TRUE;
	}

	case WM_CTLCOLORDLG:
	{
		return (INT_PTR)GetStockObject(WHITE_BRUSH);
	}

	case WM_COMMAND:
	{
		UINT wId = LOWORD(wParam);
		if (wId == IDCANCEL)
		{
			EndDialog(hWnd, wId);
			return (INT_PTR)TRUE;
		}

		break;
	}
	}

	return (INT_PTR)FALSE;
}