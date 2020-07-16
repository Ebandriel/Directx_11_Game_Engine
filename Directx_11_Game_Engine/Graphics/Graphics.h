#pragma once
#include "AdapterReader.h"
#include "Shaders.h"

class Graphics
{
public:
	bool Initialise(HWND hwnd, int width, int height);
	void RenderFrame();
protected:

private:
	bool InitialiseDirectX(HWND hwnd, int width, int height);
	bool InitialiseShaders();

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> inputLayout;

	VertexShader vertexShader;
	
};