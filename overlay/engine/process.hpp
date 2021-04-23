#pragma once

#include <windows.h>
#include <tlhelp32.h>
#include <psapi.h>

#include <iostream>
#include <thread>

#include <map>
#include <unordered_map>

#include <cctype>
#include <algorithm>

#include "../include/singleton.hpp"

std::string ToLower(std::string data);
std::wstring ToLower(std::wstring data);
std::wstring ToWide(const std::string& data);

using ProcessArray = std::vector< DWORD >;

using ImportMap = std::unordered_map< std::wstring, std::unordered_map< std::string, std::uintptr_t > >;

class dx_process
	: public Singleton< dx_process >
{
public:
	virtual bool Create(const DWORD process_id);
	virtual void Destroy();

public:
	virtual bool ReadMemory(const std::uintptr_t location, void* data, const std::size_t size);
	virtual bool WriteMemory(const std::uintptr_t location, const void* data, const std::size_t size);

	virtual std::uintptr_t VirtualAlloc(const std::uintptr_t location, const std::size_t size, const DWORD mode, const DWORD protection);
	virtual bool VirtualProtect(const std::uintptr_t location, const std::size_t size, const DWORD protection, DWORD* previous);

public:
	template< typename T, typename LocationT >
	bool Read(LocationT location, T* data)
	{
		return ReadMemory((std::uintptr_t)(location), (LPVOID)(data), sizeof(T));
	}

	template< typename T, typename LocationT >
	T Read(LocationT location)
	{
		T result = { };
		Read((std::uintptr_t)(location), &result);
		return result;
	}

	template<typename T, typename LocationT >
	bool Write(LocationT location, T data)
	{
		return WriteMemory((std::uintptr_t)(location), (const void*)data, sizeof(T));
	}

public:
	virtual std::wstring GetBaseName();
	virtual std::uintptr_t GetImage(const std::wstring& name = L"");

	virtual HANDLE GetProcess();

public:
	static ProcessArray QueryProcessArray(const std::wstring& name);

private:
	DWORD m_process_id = 0;
	HANDLE m_process = nullptr;
};
