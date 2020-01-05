#include "GPUBuffer.h"
#include "GraphicsDevice.h"
#include "BufferManager.h"
#include <iostream>

namespace Milk3D
{
	GPUBuffer & GPUBuffer::operator=(GPUBuffer const & other)
	{
		m_buffer = other.m_buffer;
		m_bufferSize = other.m_bufferSize;
		m_slot = other.m_slot;
		m_type = other.m_type;
		return *this;
	}
	GPUBuffer::GPUBuffer(GPUBuffer const & other)
	{
		*this = other;
	}

	GPUBuffer::~GPUBuffer()
	{
		Release();
	}

	void GPUBuffer::CreateConstantBuffer(D3D11_BUFFER_DESC bufferDesc)
	{
		assert(bufferDesc.ByteWidth % 16 == 0 && "Size must be 16 byte aligned");
		m_bufferSize = bufferDesc.ByteWidth;

		// Create the buffer
		if (FAILED(GraphicsDevice::GetDevice()->CreateBuffer(&bufferDesc, NULL, &m_buffer)))
		{
			std::cout << "Failed to create constant buffer" << std::endl;
		}
	}

	void GPUBuffer::CreateConstantBuffer(size_t bufferSize)
	{
		assert(bufferSize % 16 == 0 && "Size must be 16 byte aligned");
		m_bufferSize = bufferSize;

		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.ByteWidth = static_cast<UINT>(bufferSize);
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		// Create the buffer
		if (FAILED(GraphicsDevice::GetDevice()->CreateBuffer(&bufferDesc, NULL, &m_buffer)))
		{
			std::cout << "Failed to create constant buffer" << std::endl;
		}
	}

	void GPUBuffer::CreateConstantBuffer(size_t bufferSize, ShaderType type, UINT slot)
	{
		CreateConstantBuffer(bufferSize);
		m_slot = slot;
		m_type = type;
		//BufferManager::AddBuffer(type, slot, *this);
	}
	void GPUBuffer::CreateConstantBuffer(size_t bufferSize, void const * initialData, ShaderType type, UINT slot, D3D11_MAP mapType, bool unmap)
	{
		CreateConstantBuffer(bufferSize, type, slot);
		Send(initialData, type, slot, mapType, unmap);
		//BufferManager::AddBuffer(type, slot, *this);
	}
	void GPUBuffer::Release()
	{
		m_buffer.Reset();
	}

	//*** General Functions ***//
	void * GPUBuffer::Map(D3D11_MAP mapType)
	{
		if (!m_buffer)
			return nullptr;

		// Map our buffer
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (FAILED(GraphicsDevice::GetDeviceContext()->Map(m_buffer.Get(), 0, mapType, 0, &mappedResource)))
		{
			return nullptr;
		}

		// Get a pointer to the data
		return mappedResource.pData;
	}
	void GPUBuffer::Send(void const * data, ShaderType type, UINT slot, D3D11_MAP mapType, bool unmap)
	{
		void * pData = Map(mapType);
		memcpy(pData, data, m_bufferSize);
		auto deviceContext = GraphicsDevice::GetDeviceContext();
		(deviceContext->*BufferManager::GetInstance().sendFunctions[type])(slot, 1, &m_buffer);
		if (unmap)
			Unmap();
	}

	void GPUBuffer::Send(ShaderType type, UINT slot)
	{
		auto deviceContext = GraphicsDevice::GetDeviceContext();
		(deviceContext->*BufferManager::GetInstance().sendFunctions[type])(slot, 1, &m_buffer);
	}

	void GPUBuffer::Send(void const * data, D3D11_MAP mapType, bool unmap)
	{
		void * pData = Map(mapType);
		memcpy(pData, data, m_bufferSize);
		auto deviceContext = GraphicsDevice::GetDeviceContext();
		(deviceContext->*BufferManager::GetInstance().sendFunctions[m_type])(m_slot, 1, &m_buffer);
		if (unmap)
			Unmap();
	}
	void GPUBuffer::Unmap()
	{
		GraphicsDevice::GetDeviceContext()->Unmap(m_buffer.Get(), 0);
	}
}