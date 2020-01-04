/*!------------------------------------------------------------------------------
//
 *****
 \file   Mesh.cpp
 \author Christopher Taylor
 \par    Project: DX11
 \par    C++ Source File
 *****
//------------------------------------------------------------------------------
*/

//------------------------------------------------------------------------------
// Includes/Defines:
//------------------------------------------------------------------------------

#include "Precompiled.h"
#include "Mesh.h"
#include "DX11Device.h"
#include "Common.h"

//------------------------------------------------------------------------------
// Namespaces:
//------------------------------------------------------------------------------

using namespace Microsoft::WRL;

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

	Mesh::Mesh(void * initialVertexData, size_t vertexCount, UINT vertexStride, void * initialIndexData, size_t indexCount, UINT indexStride, UINT indexOffset)
	{
		AddVertexBuffer(initialVertexData, vertexCount, vertexStride);
		AddIndexBuffer(initialIndexData, indexCount, indexStride, indexOffset);
	}
	Mesh::~Mesh()
	{
		Release();
	}

	void Mesh::SetVertices(void * data, unsigned vertexBuffer)
	{
		auto deviceContext = DX11Device::GetDeviceContext();
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		HRESULT hr = deviceContext->Map(vertexBuffers[vertexBuffer], 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
		if (FAILED(hr))
		{
			std::cout << "Failed to set vertices for vertex buffer: " << vertexBuffer << std::endl;
			return;
		}

		std::memcpy(mappedResource.pData, data, vertexCount * strides[vertexBuffer]);

		deviceContext->Unmap(vertexBuffers[vertexBuffer], 0);
	}
	

	//*** Vertex Buffers ***//

	void Mesh::AddVertexBuffer(const void * initialData, size_t vertexCount, size_t stride)
	{
		AddVertexBuffer(D3D11_USAGE_DEFAULT, initialData, vertexCount, stride);
	}

	void Mesh::AddVertexBuffer(ID3D11Buffer * vertexBuffer, size_t stride, UINT offset)
	{
		vertexBuffers.push_back(vertexBuffer);
		strides.push_back(static_cast<UINT>(stride));
		offsets.push_back(offset);
	}
	void Mesh::AddVertexBuffer(D3D11_BUFFER_DESC const & bufferDesc, D3D11_SUBRESOURCE_DATA * initialData, size_t stride, UINT offset)
	{
		auto device = DX11Device::GetDevice();

		ID3D11Buffer * buffer = nullptr;
		if (FAILED(device->CreateBuffer(&bufferDesc, initialData, &buffer)))
		{
			std::cerr << "Failed to add Vertex Buffer for Mesh." << std::endl;
			return;
		}
		AddVertexBuffer(buffer, stride, offset);
	}

	void Mesh::AddVertexBuffer(D3D11_BUFFER_DESC const & bufferDesc, const void * initialData, size_t stride, UINT offset)
	{
		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = initialData;
		AddVertexBuffer(bufferDesc, &data, stride, offset);
	}

	void Mesh::AddVertexBuffer(D3D11_USAGE bufferUsage, D3D11_SUBRESOURCE_DATA * initialData, size_t vertexCount_, size_t stride, UINT offset)
	{
		vertexCount = vertexCount_;
		UINT cpuAccess = bufferUsage != D3D11_USAGE_DYNAMIC ? 0 : D3D11_CPU_ACCESS_WRITE;
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage = bufferUsage;
		bufferDesc.ByteWidth = static_cast<UINT>(vertexCount_ * stride);
		bufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
		bufferDesc.CPUAccessFlags = cpuAccess;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		AddVertexBuffer(bufferDesc, initialData, stride, offset);
	}
	void Mesh::AddVertexBuffer(D3D11_USAGE bufferUsage, const void * initialData, size_t vertexCount, size_t stride, UINT offset)
	{
		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = initialData;
		AddVertexBuffer(bufferUsage, &data, vertexCount, stride, offset);
	}

	//*** Index Buffers ***//

	void Mesh::AddIndexBuffer(const void * initialData, size_t indexCount_, size_t stride, UINT offset)
	{
		AddIndexBuffer(D3D11_USAGE_DEFAULT, initialData, indexCount_, stride, offset);
	}

	void Mesh::AddIndexBuffer(ID3D11Buffer * indexBuffer, UINT offset, DXGI_FORMAT format)
	{
		indexBuffers.push_back(IndexBuffer(indexBuffer, format, offset));
	}
	void Mesh::AddIndexBuffer(D3D11_BUFFER_DESC const & bufferDesc, D3D11_SUBRESOURCE_DATA * initialData, UINT offset, DXGI_FORMAT format)
	{
		auto device = DX11Device::GetDevice();

		ID3D11Buffer * buffer;
		if (FAILED(device->CreateBuffer(&bufferDesc, initialData, &buffer)))
		{
			std::cerr << "Failed to add Index Buffer for Mesh." << std::endl;
			return;
		}
		AddIndexBuffer(buffer, offset, format);
	}

	void Mesh::AddIndexBuffer(D3D11_BUFFER_DESC const & bufferDesc, const void * initialData, UINT offset, DXGI_FORMAT format)
	{
		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = initialData;
		AddIndexBuffer(bufferDesc, &data, offset, format);
	}

	void Mesh::AddIndexBuffer(D3D11_USAGE bufferUsage, D3D11_SUBRESOURCE_DATA * initialData, size_t indexCount_, size_t stride, UINT offset, DXGI_FORMAT format)
	{
		indexCount = indexCount_;
		UINT cpuAccess = bufferUsage != D3D11_USAGE_DYNAMIC ? 0 : D3D11_CPU_ACCESS_WRITE;
		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.Usage = bufferUsage;
		bufferDesc.ByteWidth = static_cast<UINT>(indexCount_ * stride);
		bufferDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
		bufferDesc.CPUAccessFlags = cpuAccess;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		AddIndexBuffer(bufferDesc, initialData, offset, format);
	}
	void Mesh::AddIndexBuffer(D3D11_USAGE bufferUsage, const void * initialData, size_t indexCount_, size_t stride, UINT offset, DXGI_FORMAT format)
	{
		D3D11_SUBRESOURCE_DATA data = { 0 };
		data.pSysMem = initialData;
		AddIndexBuffer(bufferUsage, &data, indexCount_, stride, offset, format);
	}

	void Mesh::Use(UINT vertexBufferSlot, UINT indexSlot)
	{
		auto deviceContext = DX11Device::GetDeviceContext();
		deviceContext->IASetVertexBuffers(vertexBufferSlot, static_cast<UINT>(vertexBuffers.size()), vertexBuffers.data(), strides.data(), offsets.data());
		auto const & indexBuffer = indexBuffers[indexSlot];
		deviceContext->IASetIndexBuffer(indexBuffer.buffer.Get(), indexBuffer.format, indexBuffer.offset);
	}

	void Mesh::Release()
	{
		for (auto & vertexBuffer : vertexBuffers)
		{
			SAFE_RELEASE(vertexBuffer);
		}
		vertexBuffers.clear();

		for (auto & indexBuffer : indexBuffers)
		{
			indexBuffer.buffer.Reset();
		}

		indexBuffers.clear();
	}
		
	//------------------------------------------------------------------------------
	// Private:
	//------------------------------------------------------------------------------

} // Namespace: DX11

//------------------------------------------------------------------------------
// Other:
//------------------------------------------------------------------------------
