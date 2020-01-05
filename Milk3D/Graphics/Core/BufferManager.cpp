#include "BufferManager.h"
#include <iostream>

namespace Milk3D
{
	ID3D11Buffer * BufferManager::matrixBuffer = nullptr;
	std::unordered_map<ShaderType, std::unordered_map<UINT, BufferManager::BufferType>> BufferManager::buffers;
	std::unordered_map<ShaderType, void(__stdcall ID3D11DeviceContext::*)(UINT, UINT, ID3D11Buffer *const *)> BufferManager::sendFunctions;

	BufferManager::BufferManager()
	{
		sendFunctions[Pixel] = &ID3D11DeviceContext::PSSetConstantBuffers;
		sendFunctions[Vertex] = &ID3D11DeviceContext::VSSetConstantBuffers;
		sendFunctions[Geometry] = &ID3D11DeviceContext::GSSetConstantBuffers;
		sendFunctions[Hull] = &ID3D11DeviceContext::HSSetConstantBuffers;
		sendFunctions[Domain] = &ID3D11DeviceContext::DSSetConstantBuffers;
		sendFunctions[Compute] = &ID3D11DeviceContext::CSSetConstantBuffers;
		matrixBuffer = CreateConstantBuffer<MatrixBuffer>(Vertex, 0);
	}

	BufferManager::~BufferManager()
	{
		for (auto & it : buffers)
		{
			for (auto & buffer : it.second)
			{
				SAFE_RELEASE(buffer.second.buffer);
			}
		}
		matrixBuffer = nullptr;
	}

	bool BufferManager::SetMatrixBuffer(DirectX::XMMATRIX const & world, DirectX::XMMATRIX const& view, DirectX::XMMATRIX const & project)
	{
		// Lock buffer and get data pointer
		MatrixBuffer * data = MapBuffer<MatrixBuffer>(matrixBuffer);
		if (!data)
		{
			return false;
		}

		// Copy the matrices into the constant buffer
		data->world = XMMatrixTranspose(world);
		data->view = XMMatrixTranspose(view);
		data->projection = XMMatrixTranspose(project);

		// Unlock data
		UnmapBuffer(matrixBuffer);

		SendBuffer(matrixBuffer, Vertex, 0);

		return true;
	}

	bool BufferManager::SendBuffer(ID3D11Buffer * buffer, ShaderType type, UINT slot, UINT numBuffers)
	{
		if (!buffer)
			return false;
		auto deviceContext = GraphicsDevice::GetDeviceContext();
		(deviceContext->*sendFunctions[type])(slot, numBuffers, &buffer);
		return true;
	}

	void BufferManager::UnmapBuffer(ShaderType type, UINT slot)
	{
		auto buffer = FindBuffer(type, slot);
		if (buffer->buffer)
		{
			GraphicsDevice::GetDeviceContext()->Unmap(buffer->buffer, 0);
		}
	}

	void BufferManager::UnmapBuffer(ID3D11Buffer * buffer)
	{
		GraphicsDevice::GetDeviceContext()->Unmap(buffer, 0);
	}

	ID3D11Buffer * BufferManager::CreateConstantBuffer(ShaderType type, UINT slot, size_t size)
	{
		// Must be 16 byte aligned
		if (size % 16 != 0)
		{
			std::cout << "Constant buffers must be 16 byte aligned, failed for slot: " << slot << " with size: " << size << std::endl;
			return nullptr;
		}

		D3D11_BUFFER_DESC bufferDesc;
		bufferDesc.ByteWidth = static_cast<UINT>(size);

		// Setup the buffer description
		bufferDesc.Usage = D3D11_USAGE_DYNAMIC;
		bufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
		bufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
		bufferDesc.MiscFlags = 0;
		bufferDesc.StructureByteStride = 0;

		ID3D11Buffer * buffer = nullptr;

		// Create the buffer
		if (FAILED(GraphicsDevice::GetDevice()->CreateBuffer(&bufferDesc, NULL, &buffer)))
		{
			std::cout << "Failed to create constant buffer with slot: " << slot << std::endl;
			return nullptr;
		}

		// Store the buffer
		buffers[type][slot] = BufferType(buffer, 1, size);
		return buffer;
	}
	void * BufferManager::MapBuffer(ID3D11Buffer * buffer)
	{
		// Don't give me null stuff
		if (!buffer)
			return nullptr;

		// Map our buffer
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (FAILED(GraphicsDevice::GetDeviceContext()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			return nullptr;
		}
		// Get a pointer to the data
		return mappedResource.pData;
	}
	bool BufferManager::SendBuffer(void const * data, ShaderType type, UINT slot)
	{
		// We didn't find your buffer, why are you like this? Have an error
		auto buffer = FindBuffer(type, slot);
		if (!buffer)
			return false;

		auto deviceContext = GraphicsDevice::GetDeviceContext();

		// Map our resource
		D3D11_MAPPED_SUBRESOURCE mappedResource;
		if (FAILED(deviceContext->Map(buffer->buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
		{
			return false;
		}

		// Memcopy our data over
		if (mappedResource.pData)
		{
			memcpy(mappedResource.pData, data, buffer->size);
		}
		else
		{
			return false;
		}

		// Send buffer
		deviceContext->Unmap(buffer->buffer, 0);

		SendBuffer(buffer->buffer, type, slot, buffer->numBuffers);
		return true;
	}

	BufferManager::BufferType * BufferManager::FindBuffer(ShaderType type, UINT slot)
	{
		auto first = buffers.find(type);
		if (first == buffers.end())
			return nullptr;
		auto second = first->second.find(slot);
		if (second == first->second.end())
			return nullptr;
		return &second->second;
	}

}