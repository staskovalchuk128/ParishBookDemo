/*
Copyright 2024 Stanislav Kovalchuk
*/

#pragma once
#include <windows.h>
#include "gdiplus.h"
#include <tchar.h>
#include "../Drawing/DrawCommon.h"
#include "../Controls/Controls.h"

using namespace Gdiplus;

struct Tabs 
{

    struct TabItem 
    {
        int id = 0;
        std::wstring name;
    };

    std::map<int, TabItem> tabs;
    void AddTab(int id, std::wstring name) 
    {
        int index = (int)tabs.size();
        tabs[index] = TabItem({ id, name });
    }
};


enum TAB_TYPES 
{
    DEFAULT, TABLE_BTNS
};


class TabControl : public Controls, public Common 
{
public:
    TabControl(HWND hwnd, Tabs tabs, int activeTabId, DWORD addsStyles = TCS_FIXEDWIDTH, int x = 230, int y = 50, int width = -10, int height = 30, bool createBody = true, DWORD exStyles = 0);
    ~TabControl();


    void AddTabs(Tabs tabs, int activeTabId);
    void SetFixedSize(int w, int h);

    void StartCommonControls(DWORD flags);


    int InsertItem(std::wstring txt, int item_index, int image_index, UINT mask, LPARAM lParam);
    int DeleteItem(int id);
    void DeleteAllTabs();
    void CreateBodyCtrl(int x, int y);


    int GetTabIndex(int id);
    int SelectTab(int id);

    void DrawTabControl(HDC& hdc);
    void DrawTabBody(LPDRAWITEMSTRUCT pdis);
    void DrawTableBtnsTabs(HDC &hdc);

    void SetBgColor(int colorId);
    void SetTabsType(int type);
    
    HWND GetTabsHWND();
    HWND GetBodyHWND();
private:
    bool createBody;
    int x, y, w, h;
    HWND hTabs;
    HWND hBody;
    int tabsType;
    int bgColor;

    static LRESULT CALLBACK tcProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
    static LRESULT CALLBACK bodyProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
    static LRESULT CALLBACK parentProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);

};

