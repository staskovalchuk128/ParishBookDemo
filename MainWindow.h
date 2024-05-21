/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include <Windows.h>
#include <string>
#include <windowsx.h>
#include "resource.h"

#include "Common/Common.h"
#include "Drawing/WindowDrawer.h"

#include "Updater.h"

#define MAX_LOADSTRING     100
#define TITLEBARHEIGHT     40

enum struct AppWindowTypes
{
	CORE,
	AUTH
};

class AppWindowCommon;

class MainWindow : public Common
{
public:
	MainWindow();
	~MainWindow();

	void OpenWindow(AppWindowTypes winType);
private:
	AppWindowTypes currentWindowType;
	AppWindowCommon* currentWindow;
	std::wstring backupType;
	int borderWidth;
	WCHAR szTitle[MAX_LOADSTRING];
	WCHAR szWindowClass[MAX_LOADSTRING];
	Updater updater;

	void RegisterWindowClass();

	static LRESULT CALLBACK OnEvent(HWND Handle, UINT Message, WPARAM wParam, LPARAM lParam);

	LRESULT ProcessEvent(HWND hWnd, UINT Message, WPARAM wParam, LPARAM lParam);

};

