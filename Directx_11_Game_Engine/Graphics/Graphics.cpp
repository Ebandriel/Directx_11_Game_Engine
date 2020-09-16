#include "Graphics.h"

bool Graphics::Initialise(HWND hwnd, int width, int height)
{
	if (!InitialiseDirectX(hwnd, width, height))return false;
	if (!InitialiseShaders())return false;
	if (!InitialiseScene())return false;
	return true;
}

void Graphics::RenderFrame()
{
	float bgcolour[] = { 0.0f,0.0f,0.0f,1.0f };
	this->deviceContext->ClearRenderTargetView(this->renderTargetView.Get(), bgcolour);
	this->deviceContext->ClearDepthStencilView(this->depthStencilView.Get(), D3D11_CLEAR_DEPTH | D3D11_CLEAR_STENCIL, 1.0f, 0);
	this->deviceContext->IASetInputLayout(this->vertexShader.GetInputLayout());
	this->deviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY::D3D10_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	this->deviceContext->RSSetState(this->rasterizerState.Get());
	this->deviceContext->OMSetDepthStencilState(this->depthStencilState.Get(), 0);
	this->deviceContext->VSSetShader(vertexShader.GetShader(), NULL, 0);
	this->deviceContext->PSSetShader(pixelShader.GetShader(), NULL, 0);
	UINT stride = sizeof(Vertex);
	UINT offset = 0;


	// triangle2
	this->deviceContext->IASetVertexBuffers(0, 1, vertexBuffer2.GetAddressOf(), &stride, &offset);
	this->deviceContext->Draw(4, 0);

	// triangle1
	this->deviceContext->IASetVertexBuffers(0, 1, vertexBuffer.GetAddressOf(), &stride, &offset);
	this->deviceContext->Draw(4, 0);


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
	scd.BufferDesc.Width = static_cast<int>(width);
	scd.BufferDesc.Height = static_cast<int>(height);
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

	//depth stencil buffer
	D3D11_TEXTURE2D_DESC depthStencilDesc;
	depthStencilDesc.Width = width;
	depthStencilDesc.Height = height;
	depthStencilDesc.MipLevels = 1;
	depthStencilDesc.ArraySize = 1;
	depthStencilDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilDesc.SampleDesc.Count = 1;
	depthStencilDesc.SampleDesc.Quality = 0;
	depthStencilDesc.Usage = D3D11_USAGE_DEFAULT;
	depthStencilDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthStencilDesc.CPUAccessFlags = 0;
	depthStencilDesc.MiscFlags = 0;

	hr = this->device->CreateTexture2D(&depthStencilDesc, NULL, this->depthStencilBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Create Depth Stencil Buffer Failed.");
		return false;
	}

	hr = this->device->CreateDepthStencilView(this->depthStencilBuffer.Get(), NULL, this->depthStencilView.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Create Depth Stencil View Failed.");
		return false;
	}
	this->deviceContext->OMSetRenderTargets(1, this->renderTargetView.GetAddressOf(), this->depthStencilView.Get());

	//create dpeth sencil state
	D3D11_DEPTH_STENCIL_DESC depthstencildesc;
	ZeroMemory(&depthstencildesc, sizeof(D3D11_DEPTH_STENCIL_DESC));
	depthstencildesc.DepthEnable = true;
	depthstencildesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK::D3D11_DEPTH_WRITE_MASK_ALL;
	depthstencildesc.DepthFunc = D3D11_COMPARISON_FUNC::D3D11_COMPARISON_LESS_EQUAL;
	hr = this->device->CreateDepthStencilState(&depthstencildesc, this->depthStencilState.GetAddressOf());
	if (hr != S_OK)
	{
		ErrorLogger::Log(hr, "Create Depth Stencil State Failed.");
		return false;
	}

	//create viewport
	D3D11_VIEWPORT viewport;
	ZeroMemory(&viewport, sizeof(D3D11_VIEWPORT));
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = static_cast<float>(width);
	viewport.Height = static_cast<float>(height);
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	//set viewport
	this->deviceContext->RSSetViewports(1, &viewport);

	//create rasterizer state 
	D3D11_RASTERIZER_DESC rasterizerDesc;
	ZeroMemory(&rasterizerDesc, sizeof(D3D11_RASTERIZER_DESC));
	rasterizerDesc.FillMode = D3D11_FILL_MODE::D3D11_FILL_SOLID;
	rasterizerDesc.CullMode = D3D11_CULL_MODE::D3D11_CULL_BACK;
	hr = this->device->CreateRasterizerState(&rasterizerDesc, this->rasterizerState.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create rasterizer state.");
		return false;
	}


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

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION",0,DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,0 },
		{"COLOUR" , 0, DXGI_FORMAT::DXGI_FORMAT_R32G32B32_FLOAT,0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_CLASSIFICATION::D3D11_INPUT_PER_VERTEX_DATA,0}
	};

	UINT numElements = ARRAYSIZE(layout);

	if (!vertexShader.Initialise(this->device, shaderfolder + L"vertexshader.cso",layout,numElements))return false;
	if (!pixelShader.Initalise(this->device, shaderfolder + L"pixelshader.cso"))return false;




	return true;
}

bool Graphics::InitialiseScene()
{
	//Triangle 1 (red)
	Vertex v[] =
	{
		Vertex(-0.5f,-0.5f, 1.0f, 1.0f,0.0f,0.0f), // bl
		Vertex(-0.0f,0.5f, 1.0f, 1.0f,0.0f,0.0f),//tm
		Vertex(0.5f, -0.5f,1.0f, 1.0f ,0.0f,0.0f),//br
		
	};

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(v);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = v;

	HRESULT hr = this->device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, this->vertexBuffer.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create Vertex buffer.");
		return false;
	}

	//Triangle 2 (green)
	Vertex v2[] =
	{
		Vertex(-0.25f,-0.25f,0.0f,0.0f,1.0f,0.0f), // bl
		Vertex(-0.0f,0.25f,0.0f,0.0f,1.0f,0.0f),//tm
		Vertex(0.25f,-0.25f,0.0f,0.0f,1.0f,0.0f),//br

	};

	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));

	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * ARRAYSIZE(v2);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;
	vertexBufferDesc.MiscFlags = 0;

	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = v2;

	hr = this->device->CreateBuffer(&vertexBufferDesc, &vertexBufferData, this->vertexBuffer2.GetAddressOf());
	if (FAILED(hr))
	{
		ErrorLogger::Log(hr, "Failed to create Vertex buffer.");
		return false;
	}
	return false;
}

