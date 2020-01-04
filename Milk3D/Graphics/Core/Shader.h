#pragma once

#include <vector>
#include <string>
#include <unordered_map>
#include "Graphics/Headers/DX11.h"
#include "Graphics/Headers/GraphicsCommon.h"

namespace Milk3D
{
	enum ShaderType
	{
		Vertex = 1,
		Pixel = 1 << 1,
		Geometry = 1 << 2,
		Compute = 1 << 3,
		Hull = 1 << 4,
		Domain = 1 << 5,

		NumShaderTypes = 6
	};

	struct ShaderInfo
	{
		ShaderInfo
		(
			ShaderType shaderType, 
			const char * filePath, 
			const char * entryPoint, 
			const char * shaderVersion,
			#ifdef _DEBUG
			UINT shaderFlags1 = D3D10_SHADER_ENABLE_STRICTNESS,
			UINT shaderFlags2 = 0,
			#else
			UINT shaderFlags1 = D3D10_SHADER_ENABLE_STRICTNESS,
			UINT shaderFlags2 = D3D10_SHADER_OPTIMIZATION_LEVEL3,
			#endif
			std::vector<D3D_SHADER_MACRO> const & shaderMacros = {}
		);

		ShaderInfo(ShaderInfo const & other);
		ShaderInfo & operator=(ShaderInfo const & other);

		ShaderType type;
		const char * filePath = nullptr;
		const char * entryPoint = nullptr;
		const char * shaderVersion = nullptr;

		#ifdef _DEBUG
		UINT shaderFlags1 = D3D10_SHADER_ENABLE_STRICTNESS;
		UINT shaderFlags2 = 0;
		#else
		UINT shaderFlags1 = D3D10_SHADER_ENABLE_STRICTNESS;
		UINT shaderFlags2 = D3D10_SHADER_OPTIMIZATION_LEVEL3;
		#endif

		std::vector<D3D_SHADER_MACRO> shaderMacros;
	};

	class Shader
	{
		public:
			Shader() = default;

			void Initialize(std::vector<ShaderInfo> const & shaders);
			void Initialize(const char * shaderPath, int shaderType);

			void Initialize
			(
				const char * vertexPath, 
				const char * pixelPath, 
				const char * geometryPath = nullptr, 
				const char * computePath = nullptr
			);

			void Initialize
			(
				std::vector<D3D11_INPUT_ELEMENT_DESC> const & semantics,
				const char * vertexPath,
				const char * pixelPath,
				const char * geometryPath = nullptr,
				const char * computePath = nullptr
			);

			void Initialize(std::vector<D3D11_INPUT_ELEMENT_DESC> const & semantics, std::vector<ShaderInfo> const & shaders);
			void Initialize(std::vector<D3D11_INPUT_ELEMENT_DESC> const & semantics, const char * shaderPath, int shaderType);

			void Release();

			~Shader();

			DELETE_COPY(Shader);

			//*** General functions ***//

			void Use();
			void SetCurrentShader();
			void Reload();

			//*** Getter functions ***//

			ID3D11InputLayout * const GetInputLayout() const;
			ID3D11VertexShader * const GetVertexShader() const;
			ID3D11PixelShader * const GetPixelShader() const;
			ID3D11GeometryShader * const GetGeometryShader() const;
			ID3D11ComputeShader * const GetComputeShader() const;

		private:
			void Compile(std::vector<ShaderInfo> const & shaders, std::vector<D3D11_INPUT_ELEMENT_DESC> const & semantics);
			void GetShaderInfo(const char * file, int shaderType, std::vector<ShaderInfo> & shaders);
			void HandleError(const char * file, Microsoft::WRL::ComPtr<ID3D10Blob> const & error);
			bool CreateShader(Microsoft::WRL::ComPtr<ID3D10Blob> const & shaderBuffer, ShaderType shaderType, const char * filepath);
			void CreateInputLayout(ID3D11Device * device, const char * file, Microsoft::WRL::ComPtr<ID3D10Blob> const & vertexShaderBuffer);

			Microsoft::WRL::ComPtr<ID3D11InputLayout> m_inputLayout;

			Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertexShader;
			Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixelShader;
			Microsoft::WRL::ComPtr<ID3D11GeometryShader> m_geometryShader;
			Microsoft::WRL::ComPtr<ID3D11ComputeShader> m_computeShader;

			std::unordered_map<ShaderType, const char *> m_shaderPaths = 
			{
				{ Vertex, nullptr },
				{ Pixel, nullptr },
				{ Geometry, nullptr },
				{ Compute, nullptr }
			};
	};
} 

