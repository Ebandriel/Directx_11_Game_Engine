#pragma once
#include "../ErrorLogger.h"
#pragma comment(lib, "D3DCompiler.lib")
#include <wrl/client.h>
#include <d3d11.h>
#include <d3dcompiler.h>

class VertexShader
{
public:
	bool Initialise(Microsoft::WRL::ComPtr<ID3D11Device>& device, std::wstring shaderpath);
	ID3D11VertexShader* GetShader();
	ID3D10Blob* GetBuffer();
private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> shader = nullptr;
	Microsoft::WRL::ComPtr<ID3D10Blob> shader_buffer = nullptr;
};