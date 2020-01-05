#pragma comment(lib, "dxguid.lib")

#include "Shader.h"
#include "GraphicsDevice.h"
#include <algorithm>
#include <fstream>
#include <iostream>

namespace Milk3D
{
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

	void Shader::Initialize
	(
		const char * vertexPath,
		const char * pixelPath,
		const char * geometryPath,
		const char * computePath
	)
	{
		Initialize
		({
			{ShaderType::Vertex, vertexPath, "VS", "vs_5_0"},
			{ShaderType::Pixel, pixelPath, "PS", "ps_5_0"},
			{ShaderType::Geometry, geometryPath, "GS", "gs_5_0"},
			{ShaderType::Compute, computePath, "CS", "cs_5_0"} 
		});
	}

	void Shader::Initialize(std::vector<ShaderInfo> const & shaders)
	{
		Compile(shaders, {});
	}

	void Shader::Initialize(const char * shaderPath, int shaderType)
	{
		std::vector<ShaderInfo> shaders;
		GetShaderInfo(shaderPath, shaderType, shaders);
		Compile(shaders, {});
	}

	void Shader::Initialize
	(
		std::vector<D3D11_INPUT_ELEMENT_DESC> const & semantics,
		const char * vertexPath,
		const char * pixelPath,
		const char * geometryPath,
		const char * computePath
	)
	{
		Initialize(semantics,
		{
			{ShaderType::Vertex, vertexPath, "VS", "vs_5_0"},
			{ShaderType::Pixel, pixelPath, "PS", "ps_5_0"},
			{ShaderType::Geometry, geometryPath, "GS", "gs_5_0"},
			{ShaderType::Compute, computePath, "CS", "cs_5_0"}
		});
	}

	void Shader::Initialize(std::vector<D3D11_INPUT_ELEMENT_DESC> const & semantics, std::vector<ShaderInfo> const & shaders)
	{
		Compile(shaders, semantics);
	}

	void Shader::Initialize(std::vector<D3D11_INPUT_ELEMENT_DESC> const & semantics, const char * shaderPath, int shaderType)
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
		if (!m_inputLayout) return;

		SetCurrentShader();

		auto deviceContext = GraphicsDevice::GetDeviceContext();
		deviceContext->IASetInputLayout(m_inputLayout.Get());
	}
	void Shader::SetCurrentShader()
	{
		auto deviceContext = GraphicsDevice::GetDeviceContext();
		deviceContext->VSSetShader(m_vertexShader.Get(), nullptr, 0);
		deviceContext->PSSetShader(m_pixelShader.Get(), nullptr, 0);
		deviceContext->GSSetShader(m_geometryShader.Get(), nullptr, 0);
		deviceContext->CSSetShader(m_computeShader.Get(), nullptr, 0);
	}

	//*** Getter functions ***//

	ID3D11InputLayout * const Shader::GetInputLayout() const
	{
		return m_inputLayout.Get();
	}
	ID3D11VertexShader * const Shader::GetVertexShader() const
	{
		return m_vertexShader.Get();
	}
	ID3D11PixelShader * const Shader::GetPixelShader() const
	{
		return m_pixelShader.Get();
	}
	ID3D11GeometryShader * const Shader::GetGeometryShader() const
	{
		return m_geometryShader.Get();
	}
	ID3D11ComputeShader * const Shader::GetComputeShader() const
	{
		return m_computeShader.Get();
	}

	void Shader::Reload()
	{
		Release();
		
		// To safely "reload" a shader we must figure out the new semantics ourselves, the old semantics may not be accurate and aren't worth storing
		Compile(std::vector<ShaderInfo>
		({
			{ ShaderType::Vertex, m_shaderPaths[ShaderType::Vertex], "VS", "vs_5_0" },
			{ ShaderType::Pixel, m_shaderPaths[ShaderType::Pixel], "PS", "ps_5_0" },
			{ ShaderType::Geometry, m_shaderPaths[ShaderType::Geometry], "GS", "gs_5_0" },
			{ ShaderType::Compute, m_shaderPaths[ShaderType::Compute], "CS", "cs_5_0" }
		}),{});
	}

	//------------------------------------------------------------------------------
	// Private:
	//------------------------------------------------------------------------------

	void Shader::Release()
	{
		m_inputLayout.Reset();
		m_vertexShader.Reset();
		m_pixelShader.Reset();
		m_geometryShader.Reset();
		m_computeShader.Reset();
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
			vertexShaderBuffer->GetBufferPointer(), vertexShaderBuffer->GetBufferSize(), &m_inputLayout)))
		{
			std::cout << "Failed to create Input Layout for Vertex Shader: " << file << std::endl;
			m_failed = true;
		}
	}

	void Shader::Compile(std::vector<ShaderInfo> const & shaders, std::vector<D3D11_INPUT_ELEMENT_DESC> const & semantics)
	{
		if (shaders.empty())
		{
			std::cout << "Not given any shaders." << std::endl;
			m_failed = true;
			return;
		}

		for (auto const & shader : shaders)
		{
			if (!shader.filePath) continue;

			m_shaderPaths[shader.type] = shader.filePath;

			ComPtr<ID3D10Blob> shaderBuffer;
			ComPtr<ID3D10Blob> errorMessage;

			if (FAILED(D3DCompileFromFile(ToWideString(std::string(shader.filePath)).c_str(), shader.shaderMacros.empty() ? nullptr : shader.shaderMacros.data(),
				D3D_COMPILE_STANDARD_FILE_INCLUDE, shader.entryPoint, shader.shaderVersion, shader.shaderFlags1, shader.shaderFlags2, &shaderBuffer, &errorMessage)))
			{
				HandleError(shader.filePath, errorMessage);
				return;
			}

			if (!CreateShader(shaderBuffer, shader.type, shader.filePath)) return;

			if (shader.type == ShaderType::Vertex && !m_inputLayout)
			{
				auto device = GraphicsDevice::GetDevice();

				if (semantics.empty())
				{
					CreateInputLayout(device, shader.filePath, shaderBuffer);
				}
				else
				{
					// Create vertex data input layout
					if (FAILED(device->CreateInputLayout(semantics.data(), static_cast<UINT>(semantics.size()),
						shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), &m_inputLayout)))
					{
						std::cout << "Failed to create Input Layout for Vertex Shader: " << shader.filePath << std::endl;
						m_failed = true;
					}
				}
			}
		}
	}

	bool Shader::CreateShader(ComPtr<ID3D10Blob> const & shaderBuffer, ShaderType shaderType, const char * filepath)
	{
		auto device = GraphicsDevice::GetDevice();
		
		m_failed = true;
		switch (shaderType)
		{
			case Vertex:
			{
				if (FAILED(device->CreateVertexShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &m_vertexShader)))
				{
					std::cout << "Failed to compile Vertex Shader with file path: " << filepath << std::endl;
					return false;
				}
				break;
			}

			case Pixel:
			{
				if (FAILED(device->CreatePixelShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &m_pixelShader)))
				{
					std::cout << "Failed to compile Pixel Shader with file path: " << filepath << std::endl;
					return false;
				}
				break;
			}

			case Geometry:
			{
				if (FAILED(device->CreateGeometryShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &m_geometryShader)))
				{
					std::cout << "Failed to compile Geometry Shader with file path: " << filepath << std::endl;
					return false;
				}
				break;
			}

			case Compute:
			{
				if (FAILED(device->CreateComputeShader(shaderBuffer->GetBufferPointer(), shaderBuffer->GetBufferSize(), NULL, &m_computeShader)))
				{
					std::cout << "Failed to compile Compute Shader with file path: " << filepath << std::endl;
					return false;
				}
				break;
			}
		}
		m_failed = false;
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
			m_failed = true;
		}
	}

}
