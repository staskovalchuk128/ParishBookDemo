/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include <windows.h>
#include "MainWindow.h"

class App
{
public:
	App();
	~App();
	void Run();
	static void OpenWindow(AppWindowTypes type);
private:
	void Open(AppWindowTypes type);
	void RunWindow();
	bool shouldStopActiveWindow;

	bool shouldOpenAnotherWindow;
	AppWindowTypes tempWinType;
	MainWindow* mainWindow;

	static App* selfPtr;
};