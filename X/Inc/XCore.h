//====================================================================================================
// Filename:	XCore.h
// Created by:	Peter Chan
//====================================================================================================

#ifndef INCLUDED_XENGINE_CORE_H
#define INCLUDED_XENGINE_CORE_H

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX

#include <Windows.h>

#include <cmath>
#include <cstdlib>
#include <ctime>
#include <map>
#include <memory>
#include <random>
#include <sstream>
#include <string>
#include <unordered_map>
#include <vector>

//----------------------------------------------------------------------------------------------------

#if defined(_DEBUG)
	#define XLOG(format, ...)\
		do {\
			char buffer[4096];\
			int ret = _snprintf_s(buffer, _countof(buffer), _TRUNCATE, "%s(%d) "##format, __FILE__, __LINE__, __VA_ARGS__);\
			OutputDebugStringA(buffer);\
			if (ret == -1) OutputDebugStringA("** message truncated **\n");\
			OutputDebugStringA("\n");\
		} while (false)

	#define XASSERT(condition, format, ...)\
		do {\
			if (!(condition))\
			{\
				XLOG(format, __VA_ARGS__);\
				DebugBreak();\
			}\
		} while (false)

	#define XVERIFY(condition, format, ...)\
		do {\
			if (!(condition))\
			{\
				XLOG(format, __VA_ARGS__);\
				DebugBreak();\
			}\
		} while (false)
#else
	#define XLOG(format, ...)
	#define XASSERT(condition, format, ...)
	#define XVERIFY(condition, format, ...) condition;
#endif

//----------------------------------------------------------------------------------------------------

namespace X {

typedef std::size_t TextureId;

//----------------------------------------------------------------------------------------------------

template <typename T>
inline void SafeDelete(T*& ptr)
{
	delete ptr;
	ptr = nullptr;
}

//----------------------------------------------------------------------------------------------------

template <typename T>
inline void SafeDeleteArray(T*& ptr)
{
	delete[] ptr;
	ptr = nullptr;
}

//----------------------------------------------------------------------------------------------------

template <typename T>
inline void SafeRelease(T*& ptr)
{
	if (ptr != nullptr)
	{
		ptr->Release();
		ptr = nullptr;
	}
}

} // namespace X

#endif // #ifndef INCLUDED_XENGINE_CORE_H