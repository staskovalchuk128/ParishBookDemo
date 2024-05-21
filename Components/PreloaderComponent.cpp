#include "PreloaderComponent.h"
#include "GifDrawer/GifDrawer.h"

GifDrawer* PreloaderComponent::drawer = NULL;

void PreloaderComponent::Start(HWND hWnd)
{
	if (drawer) return;
	drawer = new GifDrawer(hWnd);
}

void PreloaderComponent::Stop()
{
	if (!drawer) return;
	delete drawer;
	drawer = NULL;
}