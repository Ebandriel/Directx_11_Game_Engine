#pragma once
#include "AdapterReader.h"
#include "Shaders.h"
#include "Vertex.h"

class Graphics
{
public:
	bool Initialise(HWND hwnd, int width, int height);
	void RenderFrame();
protected:

private:
	bool InitialiseDirectX(HWND hwnd, int width, int height);
	bool InitialiseShaders();
	bool InitialiseScene();

	Microsoft::WRL::ComPtr<ID3D11Device> device;
	Microsoft::WRL::ComPtr<ID3D11DeviceContext> deviceContext;
	Microsoft::WRL::ComPtr<IDXGISwapChain> swapchain;
	Microsoft::WRL::ComPtr<ID3D11RenderTargetView> renderTargetView;



	VertexShader vertexShader;
	PixelShader pixelShader;

	Microsoft::WRL::ComPtr<ID3D11Buffer> vertexBuffer;
	Microsoft::WRL::ComPtr<ID3D11RasterizerState> rasterizerState;
};