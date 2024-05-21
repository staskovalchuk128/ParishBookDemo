#include "ScrollBarCtrl.h"

ScrollBar::ScrollBar(HWND hWnd, HINSTANCE hInst) : hWnd(hWnd), hInst(hInst), Controls(GetParent(hWnd))
{
	this->hWndParent = GetParent(hWnd);
	useLocalCoord = true;

	scrollSize = 21;
	uClicked = 0;
	bNotify = false;

	dragging = false;
	dragPt = { 0, 0 };

	dragPos = 0;
	m_uHtPrev = -1;

	pause = trace = false;

	scrollMaxH = scrollMaxW = 0;

	hScrollShown = vScrollShown = false;
	hVScroll = hHScroll = NULL;
	wheelOffset = 30;

}

ScrollBar::~ScrollBar()
{
	RemoveWindowSubclass(hWnd, parentProc, GetWindowID(hWnd));
	RemoveWindowSubclass(hWndParent, parentProc, IDC_STATIC);
}

void ScrollBar::SetWheelOffset(int o)
{
	wheelOffset = o;
}

HWND ScrollBar::GetHScrollHWND()
{
	return hHScroll;
}

HWND ScrollBar::GetVScrollHWND()
{
	return hVScroll;
}

void ScrollBar::CreateScroll(int w, int h, int wheelOffset)
{
	int width = w, height = h, x = 0, y = 0;
	this->wheelOffset = wheelOffset;

	RECT parentRect;
	GetWindowRect(hWndParent, &parentRect);
	int parentWidth = parentRect.right - parentRect.left;
	int parentHeight = parentRect.bottom - parentRect.top;

	RECT localCoords = GetLocalCoordinates(hWnd);

	{

		// vertical scroll
		x = parentWidth - localCoords.right;
		y = localCoords.top;
		height = (parentHeight - localCoords.bottom) * -1;

		hVScroll = CreateCustom("VscrollBar", L"scrollbar", x, y, width, height, WS_CHILD | SBS_VERT, L"", WS_STICK_RIGHT);
	}

	{

		// horizontal scroll

		height = width;
		width = (parentWidth - localCoords.right) * -1;
		x = localCoords.left;
		// y = (parentHeight - localCoords.bottom - height) *-1;
		y = (parentHeight - localCoords.bottom);
		hHScroll = CreateCustom("HscrollBar", L"scrollbar", x, y, width, height, WS_CHILD | SBS_HORZ, L"", WS_STICK_BOTTOM);
	}

	BringWindowToTop(hVScroll);
	BringWindowToTop(hHScroll);

	InitScroll();
}

void ScrollBar::ResetSize()
{
	int width = 0, height = 0, x = 0, y = 0;

	RECT parentRect;
	GetClientRect(hWndParent, &parentRect);
	int parentWidth = parentRect.right - parentRect.left;
	int parentHeight = parentRect.bottom - parentRect.top;

	RECT localCoords = GetLocalCoordinates(hWnd);

	if (!GetControlPtr("VscrollBar")) return;

	{

		// vertical scroll
		x = parentWidth - localCoords.right;
		height = (parentHeight - localCoords.bottom) * -1;

		GetControlPtr("VscrollBar")->SetHeight(height);
		GetControlPtr("VscrollBar")->SetX(x);
	}

	{

		// horizontal scroll
		y = (parentHeight - localCoords.bottom);
		width = (parentWidth - localCoords.right) * -1;
		GetControlPtr("HscrollBar")->SetWidth(width);
		GetControlPtr("HscrollBar")->SetY(y);
	}

	OnResize();
}

void ScrollBar::InitScroll()
{
	SetWindowSubclass(hVScroll, scrollProc, GetControlId("VscrollBar"), (DWORD_PTR)this);
	SetWindowSubclass(hHScroll, scrollProc, GetControlId("HscrollBar"), (DWORD_PTR)this);
	SetWindowSubclass(hWnd, parentProc, GetWindowID(hWnd), (DWORD_PTR)this);
	SetWindowSubclass(hWndParent, parentProc, IDC_STATIC, (DWORD_PTR)this);

}

void ScrollBar::SetPos(HWND hItem, int pos)
{
	SCROLLINFO si = { 0 };

	si.cbSize = sizeof(si);
	si.fMask = SIF_POS;
	si.nPos = pos;
	si.nTrackPos = pos;
	SetScrollInfo(hItem, IsVertical(hItem) ? SB_VERT : SB_HORZ, &si, TRUE);
	InvalidateRect(hItem, NULL, TRUE);
	UpdateWindow(hItem);
}

void ScrollBar::SetScrollbarRange(HWND hItem, DWORD scrollType, int min, int max)
{
	SCROLLINFO si = { 0 };

	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE;
	si.nMin = min;
	si.nMax = max;

	{
		SCROLLINFO si = { 0 };

		si.cbSize = sizeof(si);
		si.fMask = SIF_PAGE;

		GetScrollInfo(hItem, scrollType, &si);

		if ((int)si.nPage > max)
		{
			ResetScroll(scrollType);
		}
	}

	SetScrollInfo(hItem, scrollType, &si, TRUE);
	InvalidateRect(hItem, NULL, TRUE);
	UpdateWindow(hItem);

}

void ScrollBar::SetVScrollRange(int scrollMaxHeight)
{
	scrollMaxH = scrollMaxHeight == -1 ? scrollMaxH : scrollMaxHeight;
	SetScrollbarRange(hVScroll, SB_VERT, 0, scrollMaxH);
}

void ScrollBar::SetHScrollRange(int scrollMaxWidth)
{
	scrollMaxW = scrollMaxWidth == -1 ? scrollMaxW : scrollMaxWidth;
	SetScrollbarRange(hHScroll, SB_HORZ, 0, scrollMaxW);
}

void ScrollBar::SetScrollSize(DWORD scrollType)
{
	HWND scrollHwnd = scrollType == SB_HORZ ? hHScroll : hVScroll;
	bool isVert = IsVertical(scrollHwnd);

	int addUnit = 0;

	RECT rect = GetLocalCoordinates(hWnd);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	SCROLLINFO oSi = { 0 };

	oSi.cbSize = sizeof(oSi);
	oSi.fMask = SIF_POS;
	GetScrollInfo(scrollHwnd, scrollType, &oSi);

	if (!isVert && vScrollShown == true) addUnit = scrollSize;
	if (isVert && hScrollShown == true) addUnit = scrollSize;

	SCROLLINFO si = { 0 };

	si.cbSize = sizeof(si);
	si.fMask = SIF_PAGE;
	si.nPage = IsVertical(scrollHwnd) ? height - addUnit : width - addUnit;

	SetScrollInfo(scrollHwnd, scrollType, &si, TRUE);

	si.fMask = SIF_POS | SIF_RANGE;

	GetScrollInfo(scrollHwnd, scrollType, &si);

	if ((int)si.nPage <= si.nMax)
	{
		if (scrollType == SB_HORZ) hScrollShown = true;
		else vScrollShown = true;
		ShowWindow(scrollHwnd, SW_SHOW);
	}
	else
	{
		if (scrollType == SB_HORZ) hScrollShown = false;
		else vScrollShown = false;
		ShowWindow(scrollHwnd, SW_HIDE);
	}

	int res = oSi.nPos - si.nPos;

	if (res > 0)
	{
		if (IsVertical(scrollHwnd))
		{
			SetChildYScrollOffset(hWnd, res);
		}
		else
		{
			SetChildXScrollOffset(hWnd, res);
		}
	}

	InvalidateRect(hVScroll, NULL, TRUE);
	InvalidateRect(hHScroll, NULL, TRUE);
	UpdateWindow(hVScroll);
	UpdateWindow(hHScroll);
}

static int asd = 0;

static int itemId = 0;
void ScrollBar::HandleScroll(int action, DWORD type, int thum)
{
	int nPos;
	int nOldPos;
	HWND scrollHwnd = type == SB_HORZ ? hHScroll : hVScroll;

	SCROLLINFO si = { 0 };

	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS;
	GetScrollInfo(scrollHwnd, type, &si);

	nOldPos = si.nPos;

	switch (action)
	{
	case SB_TOP:
		nPos = si.nMin;
		break;

	case SB_BOTTOM:
		nPos = si.nMax;
		break;

	case SB_LINEUP:
		//  nPos = si.nPos - 30;
		nPos = si.nPos - wheelOffset;
		break;

	case SB_LINEDOWN:
		//nPos = si.nPos + 30;
		nPos = si.nPos + wheelOffset;
		break;
	case SB_PAGEUP:
		nPos = si.nPos - 80;
		break;

	case SB_PAGEDOWN:
		nPos = si.nPos + 80;
		break;

	case SB_THUMBTRACK:
		nPos = thum;
		break;
	case SB_THUMBPOSITION:
	{
		nPos = thum;
		break;
	}

	default:
		nPos = si.nPos;
		break;
	}

	SetScrollPos(scrollHwnd, type, nPos, TRUE);

	nPos = GetScrollPos(scrollHwnd, type);

	if (nOldPos != nPos)
	{
		int newSPos = nOldPos - nPos;

		int vScroll = type == SB_VERT ? newSPos : 0, hScroll = type == SB_HORZ ? newSPos : 0;

		wchar_t buff[MAX_PATH] = { 0 };

		GetClassName(hWnd, buff, _countof(buff));

		if (std::wstring(buff) == L"SysListView32")
		{
			// list view scrolling

			if (IsVertical(scrollHwnd))
			{
				GetScrollInfo(scrollHwnd, type, &si);
				int scrollHeight = si.nMax - si.nPage;
				int diff = si.nTrackPos;

				double prc = floor((double)diff / (double)scrollHeight * 100);

				int scrollY = GetScrollPos(hWnd, SB_VERT);

				int countItems = ListView_GetItemCount(hWnd);

				int countPerPage = ListView_GetCountPerPage(hWnd);

				double scrollVal = ((double)countItems - (double)countPerPage) * ((double)prc / (double)100);

				scrollVal = (scrollVal - scrollY) * 40;

				ListView_Scroll(hWnd, 0, scrollVal);
			}
			else
			{
				newSPos *= -1;
				ListView_Scroll(hWnd, newSPos, 0);

				HWND hHeader = ListView_GetHeader(hWnd);

				InvalidateRect(hHeader, NULL, FALSE);
				UpdateWindow(hHeader);

				InvalidateRect(hWnd, NULL, FALSE);
				UpdateWindow(hWnd);
			}
		}
		else if (std::wstring(buff) == L"ListBox")
		{
			if (IsVertical(scrollHwnd))
			{
				RECT rc;
				GetWindowRect(hWnd, &rc);
				int controlHeight = rc.bottom - rc.top;

				GetScrollInfo(scrollHwnd, type, &si);
				int scrollHeight = si.nMax - si.nPage;
				int diff = si.nTrackPos;

				double prc = floor((double)diff / (double)scrollHeight * 100);

				int scrollY = GetScrollPos(hWnd, SB_VERT);

				int countItems = ListBox_GetCount(hWnd);

				int countPerPage = controlHeight / (si.nMax / countItems);

				double scrollVal = round(((double)countItems - (double)countPerPage) * ((double)prc / (double)100));

				ListBox_SetTopIndex(hWnd, scrollVal);

				InvalidateRect(hWnd, NULL, FALSE);
				UpdateWindow(hWnd);

				/*
				GetScrollInfo(scrollHwnd, type, &si);

				RECT rc;

				GetWindowRect(hWnd, &rc);

				int controlHeight = rc.bottom - rc.top;

				int scrollHeight = si.nMax - si.nPage;

				int countItems = ListBox_GetCount(hWnd);

				if (countItems > 0) { 	int diff = si.nTrackPos;

					int countPerPage = controlHeight / (si.nMax / countItems);

					double itemOffset = scrollHeight / (countItems - countPerPage);

					double nextItem = diff / itemOffset;

					ListBox_SetTopIndex(hWnd, nextItem);

					InvalidateRect(hWnd, NULL, FALSE);
					UpdateWindow(hWnd);
				}

				*/
			}
		}
		else
		{
			ScrollWindowEx(hWnd, hScroll, vScroll, NULL, NULL, NULL, NULL,
				SW_INVALIDATE | SW_ERASE | SW_SCROLLCHILDREN);

			InvalidateRect(hWnd, NULL, TRUE);
			UpdateWindow(hWnd);

			if (IsVertical(scrollHwnd))
			{
				SetChildYScrollOffset(hWnd, nOldPos - nPos);
			}
			else
			{
				SetChildXScrollOffset(hWnd, nOldPos - nPos);
			}
		}

		InvalidateRect(scrollHwnd, NULL, TRUE);
		UpdateWindow(scrollHwnd);

		BringWindowToTop(scrollHwnd);
	}
}

void ScrollBar::ResetScroll(DWORD type)
{
	HandleScroll(SB_TOP, type, 0);
}

int ScrollBar::GetScrollBarSize(DWORD type)
{
	SCROLLINFO si = { 0 };

	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE | SIF_PAGE;
	GetScrollInfo(type == SB_VERT ? hVScroll : hHScroll, type, &si);
	return si.nMax - si.nPage;
}

int ScrollBar::GetScrollBarPos(DWORD type)
{
	SCROLLINFO si = { 0 };

	si.cbSize = sizeof(si);
	si.fMask = SIF_TRACKPOS;
	GetScrollInfo(type == SB_VERT ? hVScroll : hHScroll, type, &si);
	return si.nTrackPos;
}

void ScrollBar::SetChildYScrollOffset(HWND h, int offset)
{
	/*
	Controls* childsCon = reinterpret_cast<Controls*>(GetWindowLongPtr(h, GWLP_USERDATA));
	if (childsCon != NULL) {
		for (auto it : childsCon->sControls)
		{
			childsCon->GetControlItem(it.first)->y += offset;
		}
	}

	*/
}

void ScrollBar::SetChildXScrollOffset(HWND h, int offset)
{
	/*
	Controls* childsCon = reinterpret_cast<Controls*>(GetWindowLongPtr(h, GWLP_USERDATA));
	if (childsCon != NULL) {
		for (auto it : childsCon->sControls)
		{
			childsCon->getControlItem(it.first)->x += offset;
		}
	}

	*/
}

bool ScrollBar::IsVertical(HWND hItem)
{
	LONG_PTR dwStyle = GetWindowLongPtr(hItem, GWL_STYLE);
	return dwStyle & SBS_VERT;
}

RECT ScrollBar::GetRect(HWND hItem, UINT uSBCode)
{
	bool isVert = IsVertical(hItem);

	RECT rc;
	GetWindowRect(hItem, &rc);
	int width = rc.right - rc.left;
	int height = rc.bottom - rc.top;

	if (vScrollShown && hScrollShown)
	{
		if (isVert) height -= width;
		else width -= height;
	}

	int itemW = isVert ? width : height, itemH = isVert ? height : width;

	SCROLLINFO si = { 0 };

	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS;
	GetScrollInfo(hItem, isVert ? SB_VERT : SB_HORZ, &si);

	if (si.nTrackPos == -1) si.nTrackPos = si.nPos;
	int nInterHei = itemH - 2 * itemW;
	if (nInterHei < 0) nInterHei = 0;
	int nSlideHei = si.nPage * nInterHei / (si.nMax - si.nMin + 1);
	if (nSlideHei < THUMB_MINSIZE) nSlideHei = THUMB_MINSIZE;
	if (nInterHei < THUMB_MINSIZE) nSlideHei = 0;
	if ((si.nMax - si.nMin - si.nPage + 1) == 0) nSlideHei = 0;
	int nEmptyHei = nInterHei - nSlideHei;
	int nArrowHei = itemW;
	if (nInterHei == 0) nArrowHei = itemH / 2;
	RECT rcRet = { 0, 0, itemW, nArrowHei
	};

	if (uSBCode == SB_LINEUP) goto end;
	rcRet.top = rcRet.bottom;
	if ((si.nMax - si.nMin - si.nPage + 1) == 0)
		rcRet.bottom += nEmptyHei / 2;
	else
		rcRet.bottom += nEmptyHei * si.nTrackPos / (si.nMax - si.nMin - si.nPage + 1);
	if (uSBCode == SB_PAGEUP) goto end;
	rcRet.top = rcRet.bottom;
	rcRet.bottom += nSlideHei;
	if (uSBCode == SB_THUMBTRACK) goto end;
	rcRet.top = rcRet.bottom;
	rcRet.bottom = itemH - nArrowHei;
	if (uSBCode == SB_PAGEDOWN) goto end;
	rcRet.top = rcRet.bottom;
	rcRet.bottom = itemH;
	if (uSBCode == SB_LINEDOWN) goto end;
end:
	if (!IsVertical(hItem))
	{
		int t = rcRet.left;
		rcRet.left = rcRet.top;
		rcRet.top = t;
		t = rcRet.right;
		rcRet.right = rcRet.bottom;
		rcRet.bottom = t;
	}

	return rcRet;
}

UINT ScrollBar::HitTest(HWND hItem, POINT pt)
{
	int nTestPos = pt.y;
	bool isVert = IsVertical(hItem);

	if (!isVert) nTestPos = pt.x;
	if (nTestPos < 0) return -1;

	SCROLLINFO si = { 0 };

	si.cbSize = sizeof(si);
	si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS;
	GetScrollInfo(hItem, isVert ? SB_VERT : SB_HORZ, &si);

	RECT rect = GetLocalCoordinates(hItem);
	int width = rect.right - rect.left;
	int height = rect.bottom - rect.top;

	if (vScrollShown && hScrollShown)
	{
		if (isVert) height -= width;
		else width -= height;
	}

	int itemW = isVert ? width : height, itemH = isVert ? height : width;

	int nInterHei = itemH - 2 * itemW;
	if (nInterHei < 0) nInterHei = 0;
	int nSlideHei = si.nPage * nInterHei / (si.nMax - si.nMin + 1);
	if (nSlideHei < THUMB_MINSIZE) nSlideHei = THUMB_MINSIZE;
	if (nInterHei < THUMB_MINSIZE) nSlideHei = 0;
	int nEmptyHei = nInterHei - nSlideHei;

	int nArrowHei = (nInterHei == 0) ? (itemH / 2) : itemW;
	int nBottom = 0;
	int nSegLen = nArrowHei;
	nBottom += nSegLen;
	UINT uHit = SB_LINEUP;
	if (nTestPos < nBottom) goto end;
	if (si.nTrackPos == -1) si.nTrackPos = si.nPos;
	uHit = SB_PAGEUP;
	if ((si.nMax - si.nMin - si.nPage + 1) == 0)
		nSegLen = nEmptyHei / 2;
	else
		nSegLen = nEmptyHei * si.nTrackPos / (si.nMax - si.nMin - si.nPage + 1);
	nBottom += nSegLen;
	if (nTestPos < nBottom) goto end;
	nSegLen = nSlideHei;
	nBottom += nSegLen;
	uHit = SB_THUMBTRACK;
	if (nTestPos < nBottom) goto end;
	nBottom = itemH - nArrowHei;
	uHit = SB_PAGEDOWN;
	if (nTestPos < nBottom) goto end;
	uHit = SB_LINEDOWN;
end:
	return uHit;
}

LRESULT CALLBACK ScrollBar::scrollProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	ScrollBar* lpData = (ScrollBar*)dwRefData;

	HDC hdc = (HDC)wParam;
	PAINTSTRUCT ps = {};

	HBRUSH hbrBackground = NULL;
	RECT rc;
	POINT pt;

	switch (uMsg)
	{
	case WM_TIMER:
	{
		SCROLLINFO si = { 0 };

		si.cbSize = sizeof(si);
		si.fMask = SIF_POS;
		GetScrollInfo(hWnd, lpData->IsVertical(hWnd) ? SB_VERT : SB_HORZ, &si);

		// TODO: Add your message handler code here and/or call default
		if (wParam == TIMERID_DELAY)
		{
			lpData->bNotify = true;
			wParam = TIMERID_NOTIFY;
			KillTimer(hWnd, TIMERID_DELAY);
			SetTimer(hWnd, TIMERID_NOTIFY, TIME_INTER, NULL);
		}

		if (wParam == TIMERID_NOTIFY && !lpData->pause)
		{
			assert(lpData->uClicked != -1 && lpData->uClicked != SB_THUMBTRACK);

			switch (lpData->uClicked)
			{
			case SB_LINEUP:
				if (si.nPos == si.nMin)
				{
					KillTimer(hWnd, TIMERID_NOTIFY);
					break;
				}

				SendMessage(GetParent(hWnd), lpData->IsVertical(hWnd) ? WM_VSCROLL : WM_HSCROLL, MAKELONG(SB_LINEUP, 0), (LPARAM)hWnd);
				break;
			case SB_LINEDOWN:
				if (si.nPos == si.nMax)
				{
					KillTimer(hWnd, TIMERID_NOTIFY);
					break;
				}

				SendMessage(GetParent(hWnd), lpData->IsVertical(hWnd) ? WM_VSCROLL : WM_HSCROLL, MAKELONG(SB_LINEDOWN, 0), (LPARAM)hWnd);
				break;
			case SB_PAGEUP:
			case SB_PAGEDOWN:
			{
				POINT pt;
				GetCursorPos(&pt);
				ScreenToClient(hWnd, &pt);
				RECT rc = lpData->GetRect(hWnd, SB_THUMBTRACK);
				if (PtInRect(&rc, pt))
				{
					KillTimer(hWnd, TIMERID_NOTIFY);
					break;
				}

				SendMessage(GetParent(hWnd), lpData->IsVertical(hWnd) ? WM_VSCROLL : WM_HSCROLL, MAKELONG(lpData->uClicked, 0), (LPARAM)hWnd);
			}

			break;
			default:
				assert(FALSE);
				break;
			}
		}

		break;
	}

	case WM_PAINT:
	{
		bool isVert = lpData->IsVertical(hWnd);
		int arrowSize = 20;
		int minThumbSize = 20;
		int scrollMargin = 4;

		DWORD scrollType = isVert ? SB_VERT : SB_HORZ;

		SCROLLINFO si = { 0 };

		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS;
		GetScrollInfo(hWnd, scrollType, &si);

		GetWindowRect(hWnd, &rc);
		int width = rc.right - rc.left;
		int height = rc.bottom - rc.top;

		const int dcWidth = width;
		const int dcHeight = height;

		if (lpData->vScrollShown && lpData->hScrollShown)
		{
			if (isVert) height -= width;
			else width -= height;
		}

		PAINTSTRUCT ps;
		hdc = BeginPaint(hWnd, &ps);

		HDC hdcBuffer = CreateCompatibleDC(hdc);
		HBITMAP hBitmapBuffer = CreateCompatibleBitmap(hdc, dcWidth, dcHeight);
		HBITMAP oldBitmap = (HBITMAP)SelectObject(hdcBuffer, hBitmapBuffer);

		SolidBrush rgColor(Color(255, 153, 153, 153));
		SolidBrush tBrush(Color(255, 236, 240, 241));
		// SolidBrush thumbBrush(Color(255, 104, 104, 104));
		SolidBrush thumbBrush(Color(255, 52, 73, 94));
		Graphics gr(hdcBuffer);

		gr.FillRectangle(&tBrush, 0, 0, width, height);

		if (lpData->vScrollShown && lpData->hScrollShown)
		{
			int eX = width, eY = height, eW, eH;
			if (isVert)
			{
				eW = eH = width;
				eX = 0;
			}
			else
			{
				eW = eH = height;
				eY = 0;
			}

			gr.FillRectangle(&tBrush, eX, eY, eW, eH);
		}

		RECT rcDest = lpData->GetRect(hWnd, SB_THUMBTRACK);

		int thX = isVert ? scrollMargin : rcDest.left, thY = isVert ? rcDest.top : scrollMargin;
		int thW = isVert ? width - scrollMargin * 2 : rcDest.right - rcDest.left, thH = isVert ? rcDest.bottom - rcDest.top : height - scrollMargin * 2;

		gr.FillRectangle(&thumbBrush, thX, thY, thW, thH);

		gr.SetSmoothingMode(SmoothingModeHighQuality);

		if (isVert)
		{
			Point vertices[] = {
{ 				10, 5
				},
				{ 5, 10 },
				{ 15, 10 }
			};

			gr.FillPolygon(&rgColor, vertices, sizeof(vertices) / sizeof(vertices[0]));

			Point vertices2[] = {
{ 				10, height - 5
				},
				{
					5, height - 10
				},
				{
					15, height - 10
				}
			};

			gr.FillPolygon(&rgColor, vertices2, sizeof(vertices) / sizeof(vertices[0]));
		}
		else
		{
			Point vertices[] = {
{ 				5, 10
				},
				{ 10, 5 },
				{ 10, 15 }
			};

			gr.FillPolygon(&rgColor, vertices, sizeof(vertices) / sizeof(vertices[0]));

			Point vertices2[] = {
{ 				width - 5, 10
				},
				{
					width - 10, 5
				},
				{
					width - 10, 15
				}
			};

			gr.FillPolygon(&rgColor, vertices2, sizeof(vertices) / sizeof(vertices[0]));
		}

		BitBlt(hdc, 0, 0, dcWidth, dcHeight, hdcBuffer, 0, 0, SRCCOPY);

		SelectObject(hdcBuffer, oldBitmap);
		DeleteObject(hBitmapBuffer);
		DeleteDC(hdcBuffer);

		EndPaint(hWnd, &ps);
		return TRUE;
	}

	case WM_LBUTTONUP:
	{
		ReleaseCapture();

		SCROLLINFO si = { 0 };

		si.cbSize = sizeof(si);
		si.fMask = SIF_POS | SIF_TRACKPOS;
		GetScrollInfo(hWnd, lpData->IsVertical(hWnd) ? SB_VERT : SB_HORZ, &si);

		if (lpData->dragging)
		{
			lpData->dragging = false;
			SendMessage(GetParent(hWnd), lpData->IsVertical(hWnd) ? WM_VSCROLL : WM_HSCROLL, MAKELONG(SB_THUMBPOSITION, si.nTrackPos), (LPARAM)hWnd);
		}
		else if (lpData->uClicked != -1)
		{
			if (lpData->bNotify)
			{
				KillTimer(hWnd, TIMERID_NOTIFY);
				lpData->bNotify = false;
			}
			else
			{
				KillTimer(hWnd, TIMERID_DELAY);
			}

			lpData->uClicked = -1;
		}

		return true;
	}

	case WM_LBUTTONDBLCLK:
	{
		return TRUE;
	}

	case WM_LBUTTONDOWN:
	{
		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);

		SetCapture(hWnd);
		UINT uHit = lpData->HitTest(hWnd, pt);
		if (uHit == SB_THUMBTRACK)
		{
			SCROLLINFO si = { 0 };

			si.cbSize = sizeof(si);
			si.fMask = SIF_POS | SIF_TRACKPOS;
			GetScrollInfo(hWnd, lpData->IsVertical(hWnd) ? SB_VERT : SB_HORZ, &si);

			lpData->dragging = true;
			lpData->dragPt = pt;
			lpData->dragPos = si.nPos;

			si.nTrackPos = si.nPos;

			SetScrollInfo(hWnd, lpData->IsVertical(hWnd) ? SB_VERT : SB_HORZ, &si, TRUE);
		}
		else
		{
			lpData->uClicked = uHit;
			SendMessage(GetParent(hWnd), lpData->IsVertical(hWnd) ? WM_VSCROLL : WM_HSCROLL, MAKELONG(lpData->uClicked, 0), (LPARAM)hWnd);
			SetTimer(hWnd, TIMERID_DELAY, TIME_DELAY, NULL);
			lpData->pause = false;
		}

		return TRUE;
	}

	case WM_MOUSEMOVE:
	{
		bool isVert = lpData->IsVertical(hWnd);

		RECT rect = GetLocalCoordinates(hWnd);
		int width = rect.right - rect.left;
		int height = rect.bottom - rect.top;

		int itemW = isVert ? width : height, itemH = isVert ? height : width;

		pt.x = GET_X_LPARAM(lParam);
		pt.y = GET_Y_LPARAM(lParam);

		SCROLLINFO si = { 0 };

		si.cbSize = sizeof(si);
		si.fMask = SIF_RANGE | SIF_PAGE | SIF_POS | SIF_TRACKPOS;
		GetScrollInfo(hWnd, isVert ? SB_VERT : SB_HORZ, &si);

		if (!lpData->trace && wParam != -1)
		{
			lpData->trace = true;
			TRACKMOUSEEVENT tme;
			tme.cbSize = sizeof(tme);
			tme.hwndTrack = hWnd;
			tme.dwFlags = TME_LEAVE;
			tme.dwHoverTime = 1;
			lpData->trace = _TrackMouseEvent(&tme);
		}

		if (lpData->dragging)
		{
			int nInterHei = itemH - 2 * itemW;
			int nSlideHei = si.nPage * nInterHei / (si.nMax - si.nMin + 1);
			if (nSlideHei < THUMB_MINSIZE) nSlideHei = THUMB_MINSIZE;
			if (nInterHei < THUMB_MINSIZE) nSlideHei = 0;
			int nEmptyHei = nInterHei - nSlideHei;
			int nDragLen = lpData->IsVertical(hWnd) ? pt.y - lpData->dragPt.y : pt.x - lpData->dragPt.x;
			int nSlide = (nEmptyHei == 0) ? 0 : (nDragLen * (int)(si.nMax - si.nMin - si.nPage + 1) / nEmptyHei);
			int nNewTrackPos = lpData->dragPos + nSlide;
			if (nNewTrackPos < si.nMin)
			{
				nNewTrackPos = si.nMin;
			}
			else if (nNewTrackPos > (int)(si.nMax - si.nMin - si.nPage + 1))
			{
				nNewTrackPos = si.nMax - si.nMin - si.nPage + 1;
			}

			if (nNewTrackPos != si.nTrackPos)
			{
				RECT rcThumb1 = lpData->GetRect(hWnd, SB_THUMBTRACK);
				si.nTrackPos = nNewTrackPos;
				RECT rcThumb2 = lpData->GetRect(hWnd, SB_THUMBTRACK);

				lpData->HandleScroll(SB_THUMBTRACK, lpData->IsVertical(hWnd) ? SB_VERT : SB_HORZ, si.nTrackPos);
				//int res = SetScrollPos(hWnd, lpData->IsVertical(hWnd) ? SB_VERT : SB_HORZ, si.nTrackPos, TRUE);
				//SendMessage(GetParent(hWnd), lpData->IsVertical(hWnd) ? WM_VSCROLL : WM_HSCROLL, MAKELONG(SB_THUMBTRACK, si.nTrackPos), (LPARAM)hWnd);
			}
		}
		else if (lpData->uClicked != -1)
		{
			RECT rc = lpData->GetRect(hWnd, lpData->uClicked);
			lpData->pause = !(PtInRect(&rc, pt));
			if (lpData->uClicked == SB_LINEUP || lpData->uClicked == SB_LINEDOWN)
			{
				// DrawArrow(m_uClicked, m_bPause ? 0 : 2);
			}
		}
		else
		{
			UINT uHit = lpData->HitTest(hWnd, pt);
			if (uHit != lpData->m_uHtPrev)
			{
				if (lpData->m_uHtPrev != -1)
				{
					if (lpData->m_uHtPrev == SB_THUMBTRACK) {}
					else if (lpData->m_uHtPrev == SB_LINEUP || lpData->m_uHtPrev == SB_LINEDOWN)
					{
						//DrawArrow(m_uHtPrev, 0);
					}
				}

				if (uHit != -1)
				{
					if (uHit == SB_THUMBTRACK) {}
					else if (uHit == SB_LINEUP || uHit == SB_LINEDOWN)
					{
						// DrawArrow(uHit, 1);
					}
				}

				lpData->m_uHtPrev = uHit;
			}
		}

		return TRUE;
	}

	case WM_ERASEBKGND:
	{
		return TRUE;
	}

	case WM_NCCALCSIZE:
	{
		LPNCCALCSIZE_PARAMS pNcCalcSizeParam = (LPNCCALCSIZE_PARAMS)lParam;
		DWORD dwStyle = ::GetWindowLong(hWnd, GWL_STYLE);
		DWORD dwExStyle = ::GetWindowLong(hWnd, GWL_EXSTYLE);
		BOOL bLeftScroll = dwExStyle & WS_EX_LEFTSCROLLBAR;
		int nWid = ::GetSystemMetrics(SM_CXVSCROLL);
		if (dwStyle & WS_VSCROLL)
		{
			if (bLeftScroll)
				pNcCalcSizeParam->rgrc[0].left -= nWid - 15;
			else
				pNcCalcSizeParam->rgrc[0].right += nWid - 15;
		}

		if (dwStyle & WS_HSCROLL) pNcCalcSizeParam->rgrc[0].bottom += nWid - 15;

		RECT rc, rcVert, rcHorz; ::GetWindowRect(hWnd, &rc); ::OffsetRect(&rc, -rc.left, -rc.top);

		nWid = 21;
		if (bLeftScroll)
		{
			int nLeft = pNcCalcSizeParam->rgrc[0].left;
			int nBottom = pNcCalcSizeParam->rgrc[0].bottom;
			rcVert.right = nLeft;
			rcVert.left = nLeft - nWid;
			rcVert.top = 0;
			rcVert.bottom = nBottom;
			rcHorz.left = nLeft;
			rcHorz.right = pNcCalcSizeParam->rgrc[0].right;
			rcHorz.top = nBottom;
			rcHorz.bottom = nBottom + nWid;
		}
		else
		{
			int nRight = pNcCalcSizeParam->rgrc[0].right;
			int nBottom = pNcCalcSizeParam->rgrc[0].bottom;
			rcVert.left = nRight;
			rcVert.right = nRight + nWid;
			rcVert.top = 0;
			rcVert.bottom = nBottom;
			rcHorz.left = 0;
			rcHorz.right = nRight;
			rcHorz.top = nBottom;
			rcHorz.bottom = nBottom + nWid;
		}

		return TRUE;
	}

	case WM_SIZE:
	{
		lpData->SetScrollSize(SB_VERT);
		lpData->SetScrollSize(SB_HORZ);
		break;
	}

	case WM_VSCROLL:
	{
		lpData->HandleScroll(LOWORD(wParam), SB_VERT, HIWORD(wParam));
		return true;
	}

	case WM_HSCROLL:
	{
		lpData->HandleScroll(LOWORD(wParam), SB_HORZ, HIWORD(wParam));
		return true;
	}
	
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK ScrollBar::parentProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData)
{
	ScrollBar* lpData = (ScrollBar*)dwRefData;

	switch (uMsg)
	{
	case WM_SIZE:
	{
		if (uIdSubclass == IDC_STATIC) break;
		lpData->ResetSize();
		break;
	}

	case WM_VSCROLL:
	{
		if (uIdSubclass != IDC_STATIC) break;


		switch (LOWORD(wParam))
		{
		case SB_THUMBTRACK:
		{
			SCROLLINFO si = { 0 };
			si.cbSize = sizeof(si);
			si.fMask = SIF_TRACKPOS;
			
			if (!GetScrollInfo(lpData->hVScroll, SB_VERT, &si))
			{
				lpData->HandleScroll(LOWORD(wParam), SB_VERT, HIWORD(wParam));
			}
			else
			{
				lpData->HandleScroll(LOWORD(wParam), SB_VERT, si.nTrackPos);
			}


			break;
		}
		}

		return true;
	}

	case WM_HSCROLL:
	{
		if (uIdSubclass != IDC_STATIC) break;

		switch (LOWORD(wParam))
		{
		case SB_THUMBTRACK:
		{
			SCROLLINFO si = { 0 };
			si.cbSize = sizeof(si);
			si.fMask = SIF_TRACKPOS;

			if (!GetScrollInfo(lpData->hHScroll, SB_HORZ, &si))
			{
				lpData->HandleScroll(LOWORD(wParam), SB_HORZ, HIWORD(wParam));
			}
			else
			{
				lpData->HandleScroll(LOWORD(wParam), SB_HORZ, si.nTrackPos);
			}


			return false;

		}
		}
		return false;
	}

	case WM_MOUSEWHEEL:
	{
		if (uIdSubclass != IDC_STATIC) break;
		short zDelta = (short)(0xFFFF & (wParam >> 16));
		if (zDelta > 0)
			PostMessage(lpData->hVScroll, WM_VSCROLL, SB_LINEUP, NULL);
		else
			PostMessage(lpData->hVScroll, WM_VSCROLL, SB_LINEDOWN, NULL);
		return 0;
	}
	}

	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}