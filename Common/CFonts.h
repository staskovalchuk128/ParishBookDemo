/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include "string"
#include "Windows.h"
#include "gdiplus.h"

using namespace Gdiplus;


#define CFONT_ARIAL					L"Arial"
#define CFONT_VERDANA				L"Verdana"
#define CFONT_LUCIDA_CALLIGRAPHY	L"Lucida Calligraphy"

class CFonts
{
public:
	CFonts();
	~CFonts();

	void CreateNewFont();
	void CreateDefaultFont();
	void SetNewFontFamily(const wchar_t* family);
	void SetNewFontSize(float fontSize);
	void SetNewFontUnit(Unit unit);
	void SetNewFontStyle(DWORD newFontSize);
	void UpdateFont(const wchar_t* newFamily, float newSize, DWORD newFontStyle);
protected:
	Unit fontUnit;
	DWORD fontStyle;
	float fontSize;
	const wchar_t* fontFamily;
	Font* currentFont;
};