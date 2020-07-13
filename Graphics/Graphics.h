#pragma once
#include "AdapterReader.h"

class Graphics
{
public:
	bool Initialise(HWND hwnd, int width, int height);
protected:

private:
	bool InitialiseDirectX(HWND hwnd, int width, int height);

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;
};