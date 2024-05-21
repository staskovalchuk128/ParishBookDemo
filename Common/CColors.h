/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "map"
#include "Windows.h"
#include "gdiplus.h"

using namespace Gdiplus;

enum APP_COLORS 
{
	WHITE, BLACK, GRAY, DARK_GRAY, HOVERED_GRAY, RED, DARK_RED, HOVERED_RED, BLUE, DARK_BLUE, HOVERED_BLUE, DEF_DARK_BLUE, DEF_LIGHT_BLUE, GRAY_BORDER, GRAY_FONT, GREEN, ORANGE
};

typedef struct 
{
	int r, g, b;
} SColor;

class CColors 
{
public:
	CColors();
	~CColors();

	Color GetColor(unsigned int colorId);
private:
	std::map<unsigned int, SColor> colors;
};