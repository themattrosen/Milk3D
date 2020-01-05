#pragma once

#include "Graphics/Headers/DX11.h"
#include "Graphics/Core/Shader.h"

namespace Milk3D
{
	class GPUBuffer
	{
		public:
			GPUBuffer() = default;
			virtual ~GPUBuffer();

			GPUBuffer & operator=(GPUBuffer const & other);
			GPUBuffer(GPUBuffer const & other);

			template <class BufferType>
			void CreateConstantBuffer()
			{
				CreateConstantBuffer(sizeof(BufferType));
			}

			template <class BufferType>
			void CreateConstantBuffer(ShaderType type, UINT slot)
			{
				CreateConstantBuffer(sizeof(BufferType), type, slot);
			}

			template <class BufferType>
			void CreateConstantBuffer(void const * initialData, ShaderType type, UINT slot, D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD, bool unmap = true)
			{
				CreateConstantBuffer(sizeof(BufferType), initialData, type, slot, mapType, unmap);
			}
			void CreateConstantBuffer(D3D11_BUFFER_DESC bufferDesc);
			void CreateConstantBuffer(size_t bufferSize);
			void CreateConstantBuffer(size_t bufferSize, ShaderType type, UINT slot);
			void CreateConstantBuffer(size_t bufferSize, void const * initialData, ShaderType type, UINT slot, D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD, bool unmap = true);
			void Release();

			//*** General Functions ***//
			void * Map(D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD);

			template <class BufferType>
			BufferType * Map(D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD) { return static_cast<BufferType*>(Map(mapType)); }

			void Send(void const * data, ShaderType type, UINT slot, D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD, bool unmap = true);
			void Send(ShaderType type, UINT slot);
			void Send(void const * data, D3D11_MAP mapType = D3D11_MAP_WRITE_DISCARD, bool unmap = true);
			void Unmap();

			ID3D11Buffer * GetBuffer() { return m_buffer.Get(); };
			size_t GetBufferSize() const { return m_bufferSize; }
		private:
			Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
			size_t m_bufferSize = 0;
			ShaderType m_type = ShaderType::Vertex;
			UINT m_slot = 0;
	};
}