#include "Graphics.h"

bool Graphics::Initialise(HWND hwnd, int width, int height)
{
	if (!InitialiseDirectX(hwnd, width, height))return false;
	return true;
}

bool Graphics::InitialiseDirectX(HWND hwnd, int width, int height)
{
	std::vector<AdapterData> adapters = AdapterReader::GetAdapters();
	return true;
}

