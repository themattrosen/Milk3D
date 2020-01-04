#pragma once

// Windows
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>

// using namespace
#include <d3d11.h>
#include <dxgi1_6.h>
#include <d3d11shader.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <wrl.h>

using Microsoft::WRL::ComPtr;
