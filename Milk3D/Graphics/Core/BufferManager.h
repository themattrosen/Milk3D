#pragma once

#include <vector>
#include <unordered_map>
#include <cstring>
#include "GraphicsDevice.h"
#include "Shader.h"
#include "GPUBuffer.h"

namespace Milk3D
{

	struct MatrixBuffer
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	class BufferManager
	{
	public:
		static BufferManager & GetInstance() { static BufferManager instance; return instance; }
		~BufferManager();

		static bool SetMatrixBuffer(DirectX::XMMATRIX const & world, DirectX::XMMATRIX const & view, DirectX::XMMATRIX const & projection);

		static ID3D11Buffer * CreateConstantBuffer(ShaderType type, UINT slot, size_t size);

		template <typename BufferClass>
		static ID3D11Buffer * CreateConstantBuffer(ShaderType type, UINT slot, UINT numBuffers = 1, size_t customSize = 1)
		{
			// Does it already exist?
			auto pair = buffers[type][slot];
			{
				// It already exists and fits our needs
				if (pair.buffer && pair.numBuffers == numBuffers)
					return pair.buffer;

				// We're uploading more/less than given, we need to recreate this buffer
				else
				{
					SAFE_RELEASE(pair.buffer);
				}
			}
			D3D11_BUFFER_DESC bufferDesc;
			if (customSize != 1)
			{
				bufferDesc.ByteWidth = static_cast<UINT>(numBuffers * customSize);
			}
			else
			{
				bufferDesc.ByteWidth = sizeof(BufferClass) * numBuffers;
			}

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
				std::cerr << "Failed to create constant buffer with slot: " << slot << std::endl;
				return nullptr;
			}

			// Store the buffer
			buffers[type][slot] = BufferType(buffer, numBuffers, static_cast<size_t>(bufferDesc.ByteWidth));
			return buffer;
		}

		static void * MapBuffer(ID3D11Buffer * buffer);

		template <typename BufferClass>
		static BufferClass * MapBuffer(ID3D11Buffer * buffer)
		{
			// Don't give me null stuff
			if (!buffer)
				return false;

			// Map our buffer
			D3D11_MAPPED_SUBRESOURCE mappedResource;
			if (FAILED(GraphicsDevice::GetDeviceContext()->Map(buffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource)))
			{
				return nullptr;
			}
			// Get a pointer to the data
			return static_cast<BufferClass*>(mappedResource.pData);
		}

		static bool SendBuffer(void const * data, ShaderType type, UINT slot);
		static bool SendBuffer(ID3D11Buffer * buffer, ShaderType type, UINT slot, UINT numBuffers = 1);

		template <typename BufferClass>
		static bool SendBuffer(BufferClass const * data, ShaderType type, UINT slot)
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

		template <typename BufferClass>
		static bool SendBuffer(std::vector<BufferClass> const & data, ShaderType type, UINT slot, size_t customSize = 1)
		{
			return SendBuffer(static_cast<void*>(data.data()), type, slot);
		}

		static void UnmapBuffer(ShaderType type, UINT slot);
		static void UnmapBuffer(ID3D11Buffer * buffer);

	private:
		BufferManager();
		friend GPUBuffer;

		struct BufferType
		{
			BufferType() = default;
			BufferType(ID3D11Buffer * buffer, UINT numBuffers, size_t size) : buffer(buffer), numBuffers(numBuffers), size(size) {}
			BufferType(BufferType const & other) : buffer(other.buffer), numBuffers(other.numBuffers), size(other.size) {}
			BufferType & operator=(BufferType const & other)
			{
				buffer = other.buffer;
				numBuffers = other.numBuffers;
				size = other.size;
				return *this;
			}
			ID3D11Buffer * buffer = nullptr;
			UINT numBuffers = 1;
			size_t size = 0;
		};
		static BufferType * FindBuffer(ShaderType type, UINT slot);

		static ID3D11Buffer * matrixBuffer;
		static std::unordered_map<ShaderType, std::unordered_map<UINT, BufferType>> buffers;
		static std::unordered_map<ShaderType, void(__stdcall ID3D11DeviceContext::*)(UINT, UINT, ID3D11Buffer * const *)> sendFunctions;
	};
}
