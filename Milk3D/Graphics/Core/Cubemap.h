#pragma once

//------------------------------------------------------------------------------
// Includes/Defines:
//------------------------------------------------------------------------------

#include "Texture.h"
#include <array>

//------------------------------------------------------------------------------
// Forward Declarations:
//------------------------------------------------------------------------------

//------------------------------------------------------------------------------
// Namespaces:
//------------------------------------------------------------------------------
namespace Milk3D
{
	
	//------------------------------------------------------------------------------
	// Structures/Classes:
	//------------------------------------------------------------------------------

	class Cubemap : public Texture
	{
		public:
			Cubemap() = default;
			void Initialize(const char * filename, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, UINT mipLevel = 1);

			/*
			6 separate files in the following order: right (+X), left (-X), top (+Y), bottom (-Y), front (+Z), back (-Z).
			This is important, otherwise the faces will show up differently than you expect.
			*/
			void Initialize(std::array<const char *, 6> const & filenames, DXGI_FORMAT format = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB, UINT mipLevel = 1);
	};
}

