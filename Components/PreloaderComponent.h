/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include <Windows.h>

class GifDrawer;

class PreloaderComponent
{
public:
	static void Start(HWND hWnd);
	static void Stop();
private:
	static GifDrawer* drawer;
};

