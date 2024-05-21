#include "StaticCtrl.h"

StaticCtrl::StaticCtrl(HWND h, std::string name, int x, int y, int width, int height, int id, const wchar_t* text, DWORD styles, DWORD exStyles, DWORD exStyle)
	: Control(CONTROL_TYPES::STATIC, h, name, L"STATIC", styles, x, y, width, height, id, text, exStyles, exStyle)
{
	
}

StaticCtrl::~StaticCtrl()
{
}