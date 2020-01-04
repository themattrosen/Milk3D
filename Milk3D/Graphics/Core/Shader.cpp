/*!------------------------------------------------------------------------------
//
 *****
 \file   Shader.cpp
 \author Christopher Taylor
 \par    Project: DX11
 \par    C++ Source File
 *****
//------------------------------------------------------------------------------
*/

//------------------------------------------------------------------------------
// Includes/Defines:
//------------------------------------------------------------------------------

#pragma comment(lib, "dxguid.lib")

#include "Precompiled.h"
#include "Shader.h"
#include "DX11Device.h"
#include <string>
#include "DX12Device.h"
#include <algorithm>

//------------------------------------------------------------------------------
// Namespaces:
//------------------------------------------------------------------------------
namespace DX11
{

	//------------------------------------------------------------------------------
	// Static/Constant Variables:
	//------------------------------------------------------------------------------

	//------------------------------------------------------------------------------
	// Structures/Classes:
	//------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------
	// Helper Functions:
	//------------------------------------------------------------------------------
	
	//------------------------------------------------------------------------------
	// Public:
	//------------------------------------------------------------------------------

	ShaderInfo::ShaderInfo
	(
		ShaderType shaderType,
		const char * filePath,
		const char * entryPoint,
		const char * shaderVersion,
		UINT shaderFlags1,
		UINT shaderFlags2,
		std::vector<D3D_SHADER_MACRO> const & shaderMacros
	) : type(shaderType), filePath(filePath), entryPoint(entryPoint), shaderVersion(shaderVersion),
		shaderFlags1(shaderFlags1), shaderFlags2(shaderFlags2), shaderMacros(shaderMacros) 
	{
	}

	ShaderInfo::ShaderInfo(ShaderInfo const & other) { *this = other; }
	ShaderInfo & ShaderInfo::operator=(ShaderInfo const & other)
	{
		type = other.type;
		filePath = other.filePath;
		entryPoint = other.entryPoint;
		shaderVersion = other.shaderVersion;
		shaderFlags1 = other.shaderFlags1;
		shaderFlags2 = other.shaderFlags2;
		shaderMacros = other.shaderMacros;
		return *this;
	}


	//*** Shader Class ***//

	Shader::Shader
	(
		const char * vertexPath,
		const char * pixelPath,
		const char * geometryPath,
		const char * computePath
	) : Shader
		({
			{ShaderType::Vertex, vertexPath, "VS", "vs_5_0"},
			{ShaderType::Pixel, pixelPath, "PS", "ps_5_0"},
			{ShaderType::Geometry, geometryPath, "GS", "gs_5_0"},
			{ShaderType::Compute, computePath, "CS", "cs_5_0"}
		})
	{

	}

	Shader::Shader(std::vector<ShaderInfo> const & shaders)
	{
		Compile(shaders, {});
	}

	Shader::Shader(const char * shaderPath, int shaderType)
	{
		std::vector<ShaderInfo> shaders;
		GetShaderInfo(shaderPath, shaderType, shaders);
		Compile(shaders, {});
	}

	Shader::Shader
	(
		std::vector<D3D11_INPUT_ELEMENT_DESC> const & semantics,
		const char * vertexPath,
		const char * pixelPath,
		const char * geometryPath,
		const char * computePath
	) : Shader(semantics, 
		{
			{ShaderType::Vertex, vertexPath, "VS", "vs_5_0"},
			{ShaderType::Pixel, pixelPath, "PS", "ps_5_0"},
			{ShaderType::Geometry, geometryPath, "GS", "gs_5_0"},
			{ShaderType::Compute, computePath, "CS", "cs_5_0"}
		})
	{
	}

	Shader::Shader(std::vector<D3D11_INPUT_ELEMENT_DESC> const & semantics, std::vector<ShaderInfo> const & shaders)
	{
		Compile(shaders, semantics);
	}

	Shader::Shader(std::vector<D3D11_INPUT_ELEMENT_DESC> const & semantics, const char * shaderPath, int shaderType)
	{
		std::vector<ShaderInfo> shaders;
		GetShaderInfo(shaderPath, shaderType, shaders);
		Compile(shaders, semantics);
	}

	Shader::~Shader()
	{
		Release();
	}

	//*** General functions ***//

	void Shader::Use()
	{
		if (!inputLayout) return;

		SetCurrentShader();

		// TODO: Make this thread safe
		auto deviceContext = DX11Device::GetDeviceContext();
		deviceContext->IASetInputLayout(inputLayout.Get());
	}
	void Shader::SetCurrentShader()
	{
		// TODO: Make this thread safe
		auto deviceContext = DX11Device::GetDeviceContext();
		deviceContext->VSSetShader(vertexShader.Get(), nullptr, 0);
		deviceContext->PSSetShader(pixelShader.Get(), nullptr, 0);
		deviceContext->GSSetShader(geometryShader.Get(), nullptr, 0);
		deviceContext->CSSetShader(computeShader.Get(), nullptr, 0);
	}

	//*** Getter functions ***//

	ID3D11InputLayout * const Shader::GetInputLayout() const
	{
		return inputLayout.Get();
	}
	ID3D11VertexShader * const Shader::GetVertexShader() const
	{
		return vertexShader.Get();
	}
	ID3D11PixelShader * const Shader::GetPixelShader() const
	{
		return pixelShader.Get();
	}
	ID3D11GeometryShader * const Shader::GetGeometryShader() const
	{
		return geometryShader.Get();
	}
	ID3D11ComputeShader * const Shader::GetComputeShader() const
	{
		return computeShader.Get();
	}

	void Shader::Reload()
	{
		Release();
		
		// To safely "reload" a shader we must figure out the new semantics ourselves, the old semantics may not be accurate and aren't worth storing
		Compile(std::vector<ShaderInfo>
		({
			{ ShaderType::Vertex, shaderPaths[ShaderType::Vertex], "VS", "vs_5_0" },
			{ ShaderType::Pixel,  shaderPaths[ShaderType::Pixel], "PS", "ps_5_0" },
			{ ShaderType::Geometry,  shaderPaths[ShaderType::Geometry], "GS", "gs_5_0" },
			{ ShaderType::Compute,  shaderPaths[ShaderType::Compute], "CS", "cs_5_0" }
		}),{});
	}

	//------------------------------------------------------------------------------
	// Private:
	//------------------------------------------------------------------------------

	void Shader::Release()
	{
		inputLayout.Reset();
		vertexShader.Reset();
		pixelShader.Reset();
		geometryShader.Reset();
		computeShader.Reset();
	}
	
	void Shader::CreateInputLayout(ID3D11Device * device, const char * file, Microsoft::WRL::ComPtr<ID3D10Blob> const & vertexShaderBuffer)
	{
		// Reflect shader info
		ComPtr<ID3D11ShaderReflection> vertexShaderReflection;
		if (FAILED(D3DReflect(vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), IID_ID3D11ShaderReflection, reinterpret_cast<void**>(vertexShaderReflection.GetAddressOf()))))
		{
			return;
		}

		// Get shader description
		D3D11_SHADER_DESC shaderDesc = { 0 };
		if (FAILED(vertexShaderReflection->GetDesc(&shaderDesc)))
		{
			return;
		}

		// Read input layout description from shader info
		std::vector<D3D11_INPUT_ELEMENT_DESC> semantics(shaderDesc.InputParameters);
		for (UINT i = 0; i < shaderDesc.InputParameters; i++)
		{
			D3D11_SIGNATURE_PARAMETER_DESC paramDesc;
			if (FAILED(vertexShaderReflection->GetInputParameterDesc(i, &paramDesc)))
			{
				continue;
			}

			// TODO: Handle per instance data
			D3D11_INPUT_ELEMENT_DESC & elementDesc = semantics[i];
			elementDesc.SemanticName = paramDesc.SemanticName;
			elementDesc.SemanticIndex = paramDesc.SemanticIndex;
			elementDesc.InputSlot = 0;
			elementDesc.AlignedByteOffset = i == 0 ? 0 : D3D11_APPEND_ALIGNED_ELEMENT;
			elementDesc.InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
			elementDesc.InstanceDataStepRate = 0;

			if (paramDesc.Mask == 1)
			{
				switch (paramDesc.ComponentType)
				{
					case (D3D_REGISTER_COMPONENT_UINT32): elementDesc.Format = DXGI_FORMAT_R32_UINT; break;
					case (D3D_REGISTER_COMPONENT_SINT32): elementDesc.Format = DXGI_FORMAT_R32_SINT; break;
					default: case (D3D_REGISTER_COMPONENT_FLOAT32): elementDesc.Format = DXGI_FORMAT_R32_FLOAT; break;
				}
			}
			else if (paramDesc.Mask <= 3)
			{
				switch (paramDesc.ComponentType)
				{
				case (D3D_REGISTER_COMPONENT_UINT32): elementDesc.Format = DXGI_FORMAT_R32G32_UINT; break;
				case (D3D_REGISTER_COMPONENT_SINT32): elementDesc.Format = DXGI_FORMAT_R32G32_SINT; break;
				default: case (D3D_REGISTER_COMPONENT_FLOAT32): elementDesc.Format = DXGI_FORMAT_R32G32_FLOAT; break;
				}
			}
			else if (paramDesc.Mask <= 7)
			{
				switch (paramDesc.ComponentType)
				{
				case (D3D_REGISTER_COMPONENT_UINT32): elementDesc.Format = DXGI_FORMAT_R32G32B32_UINT; break;
				case (D3D_REGISTER_COMPONENT_SINT32): elementDesc.Format = DXGI_FORMAT_R32G32B32_SINT; break;
				default: case (D3D_REGISTER_COMPONENT_FLOAT32): elementDesc.Format = DXGI_FORMAT_R32G32B32_FLOAT; break;
				}
			}
			else if (paramDesc.Mask <= 15)
			{
				switch (paramDesc.ComponentType)
				{
				case (D3D_REGISTER_COMPONENT_UINT32): elementDesc.Format = DXGI_FORMAT_R32G32B32A32_UINT; break;
				case (D3D_REGISTER_COMPONENT_SINT32): elementDesc.Format = DXGI_FORMAT_R32G32B32A32_SINT; break;
				default: case (D3D_REGISTER_COMPONENT_FLOAT32): elementDesc.Format = DXGI_FORMAT_R32G32B32A32_FLOAT; break;
				}
			}
		}

		// Create vertex data input layout
		if (FAILED(device->CreateInputLayout(semantics.data(), shaderDesc.InputParameters,
			vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &inputLayout)))
		{
			std::cout << "Failed to create Input Layout for Vertex Shader: " << file << std::endl;
		}
	}

	void Shader::Compile(std::vector<ShaderInfo> const & shaders, std::vector<D3D11_INPUT_ELEMENT_DESC> const & semantics)
	{
		if (shaders.empty())
		{
			std::cout << "Not given any shaders." << std::endl;
			return;
		}

		for (auto const & shader : shaders)
		{
			if (!shader.filePath) continue;

			shaderPaths[shader.type] = shader.filePath;

			ComPtr<ID3D10Blob> shaderBuffer;
			ComPtr<ID3D10Blob> errorMessage;

			if (FAILED(D3DCompileFromFile(ToWideString(std::string(shader.filePath)).c_str(), shader.shaderMacros.empty() ? nullptr : shader.shaderMacros.data(),
				D3D_COMPILE_STANDARD_FILE_INCLUDE, shader.entryPoint, shader.shaderVersion, shader.shaderFlags1, shader.shaderFlags2, &shaderBuffer, &errorMessage)))
			{
				HandleError(shader.filePath, errorMessage);
				return;
			}

			if (!CreateShader(shaderBuffer, shader.type, shader.filePath)) return;

			if (shader.type == ShaderType::Vertex && !inputLayout)
			{
				auto device = DX11Device::GetDevice();

				if (semantics.empty())
				{
					CreateInputLayout(device, shader.filePath, shaderBuffer);
				}
				else
				{
					// Create vertex data input layout
					if (FAILED(device->CreateInputLayout(semantics.data(), static_cast<UINT>(semantics.size()),
						shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), &inputLayout)))
					{
						std::cout << "Failed to create Input Layout for Vertex Shader: " << shader.filePath << std::endl;
					}
				}
			}
		}
	}

	bool Shader::CreateShader(ComPtr<ID3D10Blob> const & shaderBuffer, ShaderType shaderType, const char * filepath)
	{
		auto device = DX11Device::GetDevice();
		
		switch (shaderType)
		{
			case Vertex:
			{
				if (FAILED(device->CreateVertexShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &vertexShader)))
				{
					std::cout << "Failed to compile Vertex Shader with file path: " << filepath << std::endl;
					return false;
				}
				break;
			}

			case Pixel:
			{
				if (FAILED(device->CreatePixelShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &pixelShader)))
				{
					std::cout << "Failed to compile Pixel Shader with file path: " << filepath << std::endl;
					return false;
				}
				break;
			}

			case Geometry:
			{
				if (FAILED(device->CreateGeometryShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &geometryShader)))
				{
					std::cout << "Failed to compile Geometry Shader with file path: " << filepath << std::endl;
					return false;
				}
				break;
			}

			case Compute:
			{
				if (FAILED(device->CreateComputeShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &computeShader)))
				{
					std::cout << "Failed to compile Compute Shader with file path: " << filepath << std::endl;
					return false;
				}
				break;
			}
		}
		return true;
	}

	void Shader::GetShaderInfo(const char * file, int shaderType, std::vector<ShaderInfo> & shaders)
	{
		if (shaderType & ShaderType::Vertex) shaders.emplace_back(ShaderType::Vertex, file, "VS", "vs_5_0");
		if (shaderType & ShaderType::Pixel) shaders.emplace_back(ShaderType::Pixel, file, "PS", "ps_5_0");
		if (shaderType & ShaderType::Geometry) shaders.emplace_back(ShaderType::Geometry, file, "GS", "gs_5_0");
		if (shaderType & ShaderType::Compute) shaders.emplace_back(ShaderType::Compute, file, "CS", "cs_5_0");
	}

	void Shader::HandleError(const char * file, ComPtr<ID3D10Blob> const & error)
	{
		if (error->GetBufferSize() > 0)
		{
			std::cout << "Shader: " << file << " encountered error: " << static_cast<const char*>(error->GetBufferPointer()) << std::endl;
		}
	}

} // Namespace: DX11

//------------------------------------------------------------------------------
// Other:
//------------------------------------------------------------------------------
