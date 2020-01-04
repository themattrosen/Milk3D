#pragma once

#include "Mesh.h"

namespace Milk3D
{

	class Model : public Mesh
	{
		public:
			Model() = default;
			void Initialize(const char * file, bool smoothNormals = false);
			void Initialize(const char * file, int flags);
			~Model();

		private:

			struct VertexType
			{
				DirectX::XMFLOAT4 position = DirectX::XMFLOAT4(0, 0, 0, 1);
				DirectX::XMFLOAT2 texCoord = DirectX::XMFLOAT2(0, 1);
				DirectX::XMFLOAT3 normal = DirectX::XMFLOAT3(0, 0, 1);
				DirectX::XMFLOAT3 tangent = DirectX::XMFLOAT3(0, 0, 1);
				DirectX::XMFLOAT3 binormal = DirectX::XMFLOAT3(0, 0, 1);
			};

			void CalculateAABB();
	};
	
	//------------------------------------------------------------------------------
	// Public Functions:
	//------------------------------------------------------------------------------


} // Namespace: DX11

//------------------------------------------------------------------------------
// Other:
//------------------------------------------------------------------------------

