#pragma once

#define SAFE_RELEASE(P) if (P) { (P)->Release(); P = nullptr; }

#define UNREFERENCED_PARAMETER(P) (P)

template <typename... Args>
void UNREFERENCED(Args &&...)
{

}

#define DELETE_COPY(CLASS) \
	void operator=(CLASS const &) = delete; \
	void operator=(CLASS &&) = delete; \
	CLASS(CLASS const &) = delete; \
	CLASS(CLASS &&) = delete;

class NonCopyable
{
public:
	NonCopyable() = default;
	DELETE_COPY(NonCopyable)
};

#include <string>
#include <windef.h>
inline std::string HrToString(HRESULT hr)
{
	char str[64] = {};
	sprintf_s(str, "HRESULT of 0x%08X", static_cast<UINT>(hr));
	return std::string(str);
}

#include <stringapiset.h>
inline std::wstring ToWideString(std::string const & str, int * result = nullptr)
{
	int size_needed = MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), NULL, 0);
	std::wstring outStr(size_needed, 0);
	int r = MultiByteToWideChar(CP_UTF8, 0, &str[0], static_cast<int>(str.size()), &outStr[0], size_needed);
	if (result) *result = r;
	return outStr;
}

#include <algorithm>

inline std::string & ToLower(std::string & str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}

class HrException : public std::runtime_error
{
public:
	HrException(HRESULT hr) : std::runtime_error(HrToString(hr)), hr(hr) {}
	HrException(HRESULT hr, std::string const & errorMessage) : std::runtime_error(errorMessage), hr(hr) {}
private:
	const HRESULT hr;
};

#include <intsafe.h>
inline void ThrowIfFailed(HRESULT hr)
{
	if (FAILED(hr))
	{
		throw HrException(hr);
	}
}
inline void ThrowIfFailed(HRESULT hr, std::string const & errorMessage)
{
	if (FAILED(hr))
	{
		throw HrException(hr, errorMessage);
	}
}