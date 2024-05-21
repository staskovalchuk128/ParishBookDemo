#include "CColors.h"

CColors::CColors() 
{

	colors[APP_COLORS::WHITE] = SColor({ 255, 255, 255 });
	colors[APP_COLORS::BLACK] = SColor({ 0, 0, 0 });
	colors[APP_COLORS::GRAY] = SColor({ 236, 240, 241 });
	colors[APP_COLORS::DARK_GRAY] = SColor({ 189, 195, 199 });
	colors[APP_COLORS::HOVERED_GRAY] = SColor({ 234, 234, 234 });
	colors[APP_COLORS::RED] = SColor({ 231, 76, 60 });
	colors[APP_COLORS::DARK_RED] = SColor({ 198, 67, 54 });
	colors[APP_COLORS::HOVERED_RED] = SColor({ 242, 79, 62 });
	colors[APP_COLORS::BLUE] = SColor({ 52, 152, 219 });
	colors[APP_COLORS::DARK_BLUE] = SColor({ 41, 128, 185 });
	colors[APP_COLORS::HOVERED_BLUE] = SColor({ 54, 160, 232 });
	colors[APP_COLORS::DEF_DARK_BLUE] = SColor({ 52, 73, 94 });
	colors[APP_COLORS::DEF_LIGHT_BLUE] = SColor({ 41, 128, 185 });
	colors[APP_COLORS::GRAY_BORDER] = SColor({ 204, 204, 204 });
	colors[APP_COLORS::GRAY_FONT] = SColor({ 100, 100, 100 });
	colors[APP_COLORS::GREEN] = SColor({ 34, 139, 34 });
}

CColors::~CColors() {}

Color CColors::GetColor(unsigned int colorId) 
{
	if (colors.find(colorId) == colors.end()) return Color(255, 255, 255);
	return Color(colors[colorId].r, colors[colorId].g, colors[colorId].b);
}