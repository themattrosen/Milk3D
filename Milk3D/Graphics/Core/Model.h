/*!------------------------------------------------------------------------------
//
 *****
 \file   Model.h
 \author Christopher Taylor
 \par    Project: DX11
 \par    C++ Header File
 *****
//------------------------------------------------------------------------------
*/

#pragma once

//------------------------------------------------------------------------------
// Includes/Defines:
//------------------------------------------------------------------------------

#include "Mesh.h"

//------------------------------------------------------------------------------
// Forward Declarations:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Namespaces:
//------------------------------------------------------------------------------
namespace DX11
{
	
	//------------------------------------------------------------------------------
	// Structures/Classes:
	//------------------------------------------------------------------------------

	class Model : public Mesh
	{
		public:
			Model();
			Model(const char * file, bool smoothNormals = false);
			Model(const char * file, int flags);
			~Model();

			void Load(const char * file, bool smoothNormals = false);
			void Load(const char * file, int flags);

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

