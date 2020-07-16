#include "Graphics.h"

bool Graphics::Initialise(HWND hwnd, int width, int height)
{
	if (!InitialiseDirectX(hwnd, width, height))return false;
	if (!InitialiseShaders())return false;
	return true;
}

void Graphics::RenderFrame()
{
	float bgcolour[] = { 0.0f,0.0f,1.0f,1.0f };
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgcolour);
	this->swapchain->Present(1, NULL);
}

bool Graphics::InitialiseDirectX(HWND hwnd, int width, int height)
{
	std::vector<AdapterData> adapters = AdapterReader::GetAdapters();
	if (adapters.size() < 1)
	{
		ErrorLogger::Log("No DXGI Adapters found.");
		return false;
	}
	// setup swapchain
	DXGI_SWAP_CHAIN_DESC scd;
	ZeroMemory(&scd, sizeof(DXGI_SWAP_CHAIN_DESC));
	scd.BufferDesc.Width = width;
	scd.BufferDesc.Height = height;
	scd.BufferDesc.RefreshRate.Numerator = 60;
	scd.BufferDesc.RefreshRate.Denominator = 1;
	scd.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	scd.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	scd.BufferDesc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	scd.SampleDesc.Count = 1;
	scd.SampleDesc.Quality = 0;
	
	scd.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	scd.BufferCount = 1;
	scd.OutputWindow = hwnd;
	scd.Windowed = TRUE;
	scd.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;
	scd.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;

	HRESULT hr;
	//initialise device
	hr = D3D11CreateDeviceAndSwapChain(
								adapters[0].pAdapter, //Adapter
								D3D_DRIVER_TYPE_UNKNOWN,
								NULL, //Software Driver
								NULL, //Runtime Layer Flags
								NULL, //Feature Levels
								0, //no of feature levels
								D3D11_SDK_VERSION,
								&scd, //swapchain
								this->swapchain.GetAddressOf(), //swapchain address
								this->device.GetAddressOf(), //device address
								NULL, //supported feature level
								this->deviceContext.GetAddressOf()); //device context address
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create device and swapchain");
		return false;
	}

	Microsoft::WRL::ComPtr<ID3D11Texture2D> backBuffer;
	hr = this->swapchain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(backBuffer.GetAddressOf()));
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "GetBuffer Failed.");
		return false;
	}

	hr = this->device->CreateRenderTargetView(backBuffer.Get(),NULL,this->renderTargetView.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Create Render Target View Failed.");
		return false;
	}

	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), NULL);
	
	return true;
}

bool Graphics::InitialiseShaders()
{
	std::wstring shaderfolder = L"";
#pragma region DetermineShaderPath
	if (IsDebuggerPresent() == TRUE)
	{
#ifdef _DEBUG // debug mode
	#ifdef _WIN64 //x64
		shaderfolder = L"..\\x64\\Debug\\";
	#else //x86
		shaderfolder = L"..\\Debug\\";
	#endif
	#else // release mode
	#ifdef _WIN64 // x64
		shaderfolder = L"..\\x64\\Release\\";
	#else //x86
		shaderfolder = L"..\\Release\\";
	#endif
#endif


	}
	if (!vertexShader.Initialise(this->device, shaderfolder + L"vertexshader.cso"))return false;

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION",0,DXGI_FORMAT::DXGI_FORMAT_R32G32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,0 }
	};

	UINT numElements = ARRAYSIZE(layout);

	HRESULT hr = this->device->CreateInputLayout(layout,numElements, vertexShader.GetBuffer()->GetBufferPointer(),vertexShader.GetBuffer()->GetBufferSize(),this->inputLayout.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "failed to create input layout.");
		return false;
	}
	return true;
}

