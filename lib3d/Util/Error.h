#pragma once

#include "Log.h"

#ifdef _DEBUG

#define CHECKHR(x) { HRESULT hr##__LINE__ = (x); if (FAILED(hr##__LINE__)) { Lib3D::LogError(L#x L" failed. hr = 0x%08x\n", hr##__LINE__); assert(false); throw std::exception(); } }

#else // _DEBUG

#define CHECKHR(x) { HRESULT hr##__LINE__ = (x); if (FAILED(hr##__LINE__)) { throw std::exception(); } }

#endif // _DEBUG