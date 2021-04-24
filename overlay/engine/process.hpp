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

std::string to_lower(std::string data);
std::wstring to_lower(std::wstring data);
std::wstring to_wide(const std::string& data);

using ProcessArray = std::vector< DWORD >;

using ImportMap = std::unordered_map< std::wstring, std::unordered_map< std::string, std::uintptr_t > >;

class dx_process
	: public Singleton< dx_process >
{
public:
	virtual bool create_process(const DWORD process_id);
	virtual void destroy_process();

public:
	virtual bool read_memory(const std::uintptr_t location, void* data, const std::size_t size);
	virtual bool write_memory(const std::uintptr_t location, const void* data, const std::size_t size);

	virtual std::uintptr_t virtual_alloc(const std::uintptr_t location, const std::size_t size, const DWORD mode, const DWORD protection);
	virtual bool virtual_protect(const std::uintptr_t location, const std::size_t size, const DWORD protection, DWORD* previous);

public:
	template< typename T, typename LocationT >
	bool Read(LocationT location, T* data)
	{
		return read_memory((std::uintptr_t)(location), (LPVOID)(data), sizeof(T));
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
		return write_memory((std::uintptr_t)(location), (const void*)data, sizeof(T));
	}

public:
	virtual std::wstring get_base_name();
	virtual std::uintptr_t get_image(const std::wstring& name = L"");

	virtual HANDLE get_process();

public:
	static ProcessArray query_process_array(const std::wstring& name);

private:
	DWORD m_process_id = 0;
	HANDLE m_process = nullptr;
};
