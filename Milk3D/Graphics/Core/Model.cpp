/*!------------------------------------------------------------------------------
//
 *****
 \file   Model.cpp
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
#include "Model.h"
#include "Assimp.h"

//------------------------------------------------------------------------------
// Namespaces:
//------------------------------------------------------------------------------
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

	Model::Model() : Mesh(0, 0)
	{

	}

	Model::Model(const char * file, bool smoothNormals)
	{
		Load(file, smoothNormals);
	}

	Model::Model(const char * file, int flags)
	{
		Load(file, flags);
	}

	Model::~Model()
	{
		Release();
	}

	void Model::Load(const char * file, int flags)
	{
		Assimp::Importer importer;

		const aiScene * scene = importer.ReadFile(file, flags);

		if (!scene || !scene->HasMeshes())
		{
			std::cout << "Failed to load Model from file: " << file << std::endl;
			return;
		}

		for (unsigned m = 0; m < scene->mNumMeshes; m++)
		{
			auto mesh = scene->mMeshes[m];
			if (!mesh)
			{
				return;
			}

			std::vector<VertexType> vertices;

			// Load our vertices
			vertices.reserve(mesh->mNumVertices);

			DirectX::XMFLOAT3 minElements{ 0,0,0 };
			DirectX::XMFLOAT3 maxElements{ 0,0,0 };

			if (mesh->mNumVertices != 0)
			{
				minElements.x = mesh->mVertices[0].x;
				minElements.y = mesh->mVertices[0].y;
				minElements.z = mesh->mVertices[0].z;
				
				maxElements = minElements;
			}

			for (unsigned int i = 0; i < mesh->mNumVertices; i++)
			{
				VertexType vertex;
				vertex.position.x = mesh->mVertices[i].x;

				if (vertex.position.x < minElements.x)
					minElements.x = vertex.position.x;
				if (vertex.position.x > maxElements.x)
					maxElements.x = vertex.position.x;

				vertex.position.y = mesh->mVertices[i].y;

				if (vertex.position.y < minElements.y)
					minElements.y = vertex.position.y;
				if (vertex.position.y > maxElements.y)
					maxElements.y = vertex.position.y;

				vertex.position.z = mesh->mVertices[i].z;

				if (vertex.position.z < minElements.z)
					minElements.z = vertex.position.z;
				if (vertex.position.z > maxElements.z)
					maxElements.z = vertex.position.z;

				vertex.position.w = 1.0f;

				// Load our normals, if they exist
				if (mesh->mNormals)
				{
					vertex.normal.x = mesh->mNormals[i].x;
					vertex.normal.y = mesh->mNormals[i].y;
					vertex.normal.z = mesh->mNormals[i].z;
				}

				// Load our texture coordinates, if they exist
				if (mesh->HasTextureCoords(m))
				{
					// Flip x-axis for left handed coordinate system
					vertex.texCoord.x = 1.0f - mesh->mTextureCoords[m][i].x;
					vertex.texCoord.y = 1.0f - mesh->mTextureCoords[m][i].y;
				}

				// Load tanget and binormal vectors
				if (mesh->HasTangentsAndBitangents())
				{
					vertex.tangent.x = mesh->mTangents[i].x;
					vertex.tangent.y = mesh->mTangents[i].y;
					vertex.tangent.z = mesh->mTangents[i].z;

					vertex.binormal.x = mesh->mBitangents[i].x;
					vertex.binormal.y = mesh->mBitangents[i].y;
					vertex.binormal.z = mesh->mBitangents[i].z;
				}


				vertices.push_back(vertex);
			}

			std::vector<UINT> indices;

			// Load our indices
			indices.reserve(mesh->mNumFaces * 3);
			for (unsigned int i = 0; i < mesh->mNumFaces; i++)
			{
				aiFace face = mesh->mFaces[i];
				for (unsigned j = 0; j < face.mNumIndices; j++)
				{
					indices.push_back(face.mIndices[j]);
				}
			}

			AddVertexBuffer(vertices.data(), vertices.size(), sizeof(VertexType));
			AddIndexBuffer(indices.data(), indices.size(), sizeof(UINT), m);

			vertexCount = vertices.size();
			indexCount = indices.size();

			// Calculate AABB
			boundingBox.Construct(minElements, maxElements);

			// TODO: Load textures from mesh
		}
	}
	void Model::Load(const char * file, bool smoothNormals)
	{
		Load(file,
			aiProcess_Triangulate |
			(smoothNormals ? aiProcess_GenSmoothNormals : aiProcess_GenNormals) |
			aiProcess_GenUVCoords |
			aiProcess_CalcTangentSpace |
			aiProcess_JoinIdenticalVertices |
			aiProcess_OptimizeMeshes
		);
	}

	//------------------------------------------------------------------------------
	// Private:
	//------------------------------------------------------------------------------

	void Model::CalculateAABB()
	{
		// TODO:
	}
	

} // Namespace: DX11

//------------------------------------------------------------------------------
// Other:
//------------------------------------------------------------------------------
