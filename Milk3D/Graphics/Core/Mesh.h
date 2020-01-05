#pragma once

#include <vector>
#include "Graphics/Headers/DX11.h"

namespace Milk3D
{
	class Mesh
	{
		public:
			Mesh() = default;
			void Initialize(void * initialVertexData, size_t vertexCount, UINT vertexStride, void * initialIndexData, size_t indexCount, UINT indexStride, UINT indexOffset = 0);
			virtual ~Mesh();

			//*** General Functions ***//
			void AddVertexBuffer(const void * initialData, size_t vertexCount, size_t stride);
			void AddVertexBuffer(ID3D11Buffer * vertexBuffer, size_t stride, UINT offset = 0);
			void AddVertexBuffer(D3D11_BUFFER_DESC const & bufferDesc, D3D11_SUBRESOURCE_DATA * initialData, size_t stride, UINT offset = 0);
			void AddVertexBuffer(D3D11_BUFFER_DESC const & bufferDesc, const void * initialData, size_t stride, UINT offset = 0);
			void AddVertexBuffer(D3D11_USAGE bufferUsage, D3D11_SUBRESOURCE_DATA * initialData, size_t vertexCount, size_t stride, UINT offset = 0);
			void AddVertexBuffer(D3D11_USAGE bufferUsage, const void * initialData, size_t vertexCount, size_t stride, UINT offset = 0);

			void AddIndexBuffer(const void * initialData, size_t indexCount, size_t stride, UINT offset);
			void AddIndexBuffer(ID3D11Buffer * indexBuffer, UINT offset = 0, DXGI_FORMAT format = DXGI_FORMAT_R32_UINT);
			void AddIndexBuffer(D3D11_BUFFER_DESC const & bufferDesc, D3D11_SUBRESOURCE_DATA * initialData, UINT offset = 0, DXGI_FORMAT format = DXGI_FORMAT_R32_UINT);
			void AddIndexBuffer(D3D11_BUFFER_DESC const & bufferDesc, const void * initialData, UINT offset = 0, DXGI_FORMAT format = DXGI_FORMAT_R32_UINT);
			void AddIndexBuffer(D3D11_USAGE bufferUsage, D3D11_SUBRESOURCE_DATA * initialData, size_t indexCount, size_t stride, UINT offset = 0, DXGI_FORMAT format = DXGI_FORMAT_R32_UINT);
			void AddIndexBuffer(D3D11_USAGE bufferUsage, const void * initialData, size_t indexCount, size_t stride, UINT offset = 0, DXGI_FORMAT format = DXGI_FORMAT_R32_UINT);

			void SetVertices(void * data, unsigned vertexBuffer);

			virtual void Use(UINT vertexStartSlot = 0, UINT indexBuffer = 0);
			virtual void Release();

			//*** Getter Functions ***//

			size_t VertexCount() const { return m_vertexCount; }
			size_t IndexCount() const { return m_indexCount; }
			bool Failed() const { return m_failed; }

		protected:

			void Initialize(size_t vertexCount, size_t indexCount);

			//*** Members ***//
			size_t m_vertexCount = 0;
			size_t m_indexCount = 0;

			std::vector<UINT> m_strides;
			std::vector<UINT> m_offsets;

			std::vector<ID3D11Buffer*> m_vertexBuffers;

			struct IndexBuffer
			{
				IndexBuffer(Microsoft::WRL::ComPtr<ID3D11Buffer> buff, DXGI_FORMAT format, UINT offset) : buffer(buff), format(format), offset(offset) {}
				Microsoft::WRL::ComPtr<ID3D11Buffer> buffer;
				DXGI_FORMAT format = DXGI_FORMAT_R32_UINT;
				UINT offset = 0;
			};
			std::vector<IndexBuffer> m_indexBuffers;
			bool m_failed = false;
	};

	#include <type_traits>

	template<typename T>
	struct HasConstIterator
	{
		private:
			typedef char Yes;
			typedef struct { char array[2]; } No;

			template<typename C> static Yes Test(typename C::const_iterator*) { return Yes; }
			template<typename C> static No Test(...) { return No; }
		public:
			static const bool value = sizeof(Test<T>(0)) == sizeof(Yes);
			typedef T type;
	};

	template <typename VertexType, typename IndexType = UINT>
	class GeneralMesh : public Mesh
	{
		public:
			GeneralMesh(void * initialVertexData, UINT vertexCount, UINT vertexStride, void * initialIndexData, UINT indexCount, UINT indexStride, UINT indexOffset = 0) :
				Mesh(initialVertexData, vertexCount, vertexStride, initialIndexData, indexCount, indexStride, indexOffset)
			{

			}

			GeneralMesh(std::vector<VertexType> const & vertices, std::vector<IndexType> const & indices, UINT vertexOffset = 0, UINT indexOffset = 0)
			{
				AddVertexBuffer(vertices, vertexOffset);
				AddIndexBuffer(indices, indexOffset);
			}
			void AddVertexBuffer(D3D11_SUBRESOURCE_DATA * initialData, UINT vertexCount, UINT stride, UINT offset = 0)
			{
				Mesh::AddVertexBuffer(initialData->pSysMem, vertexCount, sizeof(VertexType));
			}
			void AddVertexBuffer(const void * initialData, UINT vertexCount, UINT offset = 0)
			{
				Mesh::AddVertexBuffer(initialData, vertexCount, sizeof(VertexType));
			}

			template <typename Container>
			void AddVertexBuffer(Container const & container, UINT offset = 0)
			{
				static_assert(std::is_same<typename Container::value_type, VertexType>::value, "Vertex Type of container and General Mesh must match.");
				static_assert(HasConstIterator<Container>::value, "Must be a container type.");
				
				AddVertexBuffer(container.data(), static_cast<UINT>(container.size()), offset);
			}

			void AddIndexBuffer(D3D11_SUBRESOURCE_DATA * initialData, UINT indexCount, UINT offset = 0, DXGI_FORMAT format = DXGI_FORMAT_R32_UINT)
			{
				Mesh::AddIndexBuffer(initialData->pSysMem, indexCount, sizeof(IndexType), offset);
			}
			void AddIndexBuffer(const void * initialData, UINT indexCount, UINT offset = 0, DXGI_FORMAT format = DXGI_FORMAT_R32_UINT)
			{
				Mesh::AddIndexBuffer(initialData, indexCount, sizeof(IndexType), offset);
			}
			template <typename Container>
			void AddIndexBuffer(Container const & container, UINT offset = 0, DXGI_FORMAT format = DXGI_FORMAT_R32_UINT)
			{
				static_assert(std::is_same<typename Container::value_type, IndexType>::value, "Index Type of container and General Mesh must match.");
				static_assert(HasConstIterator<Container>::value, "Must be a container type");

				AddIndexBuffer(container.data(), static_cast<UINT>(container.size()), offset, format);
			}
	};
}


