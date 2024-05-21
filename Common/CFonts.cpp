#include "CFonts.h"
#include "..//Common/CFunctions.hpp"

CFonts::CFonts()
{
	fontFamily = CFONT_VERDANA;
	fontUnit = Unit::UnitPixel;
	currentFont = NULL;
	CreateDefaultFont();
	CreateNewFont();
}

CFonts::~CFonts()
{
	delete currentFont;
}

void CFonts::CreateNewFont()
{
	if (currentFont)
	{
		delete currentFont;
	}

	FontFamily _fontFamily(fontFamily);
	if (_fontFamily.GetLastStatus() == Status::FontFamilyNotFound)
	{
		int res = AddFontResourceW((GetCurrentPath() + L"/UsedFonts/LucidaCalligraphy.ttf").c_str());
		
		FontFamily _newFontFamily(fontFamily);
		currentFont = new Font(&_newFontFamily, fontSize, fontStyle, fontUnit);
	}
	else
	{
		currentFont = new Font(&_fontFamily, fontSize, fontStyle, fontUnit);
	}
}

void CFonts::CreateDefaultFont()
{
	fontStyle = FontStyleBold;
	fontSize = 13;
	fontFamily = L"Verdana";
}

void CFonts::SetNewFontFamily(const wchar_t* newFamily)
{
	fontFamily = newFamily;
}
void CFonts::SetNewFontUnit(Unit unit)
{
	fontUnit = unit;
}
void CFonts::SetNewFontSize(float newFontSize)
{
	fontSize = newFontSize;
}

void CFonts::SetNewFontStyle(DWORD newFontStyle)
{
	fontStyle = newFontStyle;
}

void CFonts::UpdateFont(const wchar_t* newFamily, float newSize, DWORD newFontStyle)
{
	fontFamily = newFamily;
	fontSize = newSize;
	fontStyle = newFontStyle;
	CreateNewFont();
}