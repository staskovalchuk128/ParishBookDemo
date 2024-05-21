/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "Windows.h"
#include "../Common/PageCommon.h"

class Funds : public PageCommon 
{
public:
	Funds(HWND hWnd, HINSTANCE hInst);
	~Funds();

	void AddControls();

	void ShowFunds(std::function<void(void)> onFinished = nullptr);
	void SaveFund();
	void ShowSaveFund();
	void CancelEditFund();
	bool DeleteFund();
	bool OnCommand(WPARAM wParam);

private:
	std::map<int, int> fundsNum;
	int currentFundId;
	void ShowDonationsByFundId(int fundId);
	static LRESULT CALLBACK FundsProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
};
