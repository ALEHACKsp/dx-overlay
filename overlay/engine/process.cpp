#include "../engine/process.hpp"

std::string ToLower(std::string data)
{
	std::transform(data.begin(), data.end(), data.begin(), ::tolower);
	return data;
}

std::wstring ToLower(std::wstring data)
{
	std::transform(data.begin(), data.end(), data.begin(), ::tolower);
	return data;
}

std::wstring ToWide(const std::string& data)
{
	return { data.begin(), data.end() };
}

bool dx_process::Create(const DWORD process_id)
{
	m_process_id = process_id;

	if (!m_process_id)
		return false;

	m_process = OpenProcess(static_cast<DWORD>(PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION),
		FALSE,
		m_process_id);

	if (!m_process)
		return false;

	return true;
}

void dx_process::Destroy()
{
	if (m_process)
		CloseHandle(m_process);
}

bool dx_process::ReadMemory(const std::uintptr_t location, void* data, const std::size_t size)
{
	return static_cast<bool>(ReadProcessMemory(m_process,
		reinterpret_cast<LPCVOID>(location),
		data,
		size,
		nullptr));
}

bool dx_process::WriteMemory(const std::uintptr_t location, const void* data, const std::size_t size)
{
	return static_cast<bool>(WriteProcessMemory(m_process,
		reinterpret_cast<LPVOID>(location),
		data,
		size,
		nullptr));
}

std::uintptr_t dx_process::VirtualAlloc(const std::uintptr_t location, const std::size_t size, const DWORD mode, const DWORD protection)
{
	return reinterpret_cast<std::uintptr_t>(VirtualAllocEx(m_process,
		reinterpret_cast<LPVOID>(location),
		size,
		mode,
		protection));
}

bool dx_process::VirtualProtect(const std::uintptr_t location, const std::size_t size, const DWORD protection, DWORD* previous)
{
	return static_cast<bool>(VirtualProtectEx(m_process,
		reinterpret_cast<LPVOID>(location),
		size,
		protection,
		previous));
}

std::wstring dx_process::GetBaseName()
{
	wchar_t base_name[512] = { };
	K32GetModuleBaseNameW(m_process, nullptr, base_name, 512);
	return base_name;
}

std::uintptr_t dx_process::GetImage(const std::wstring& name /*= L""*/)
{
	const auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, m_process_id);

	if (snapshot)
	{
		MODULEENTRY32W entry = { };
		entry.dwSize = sizeof(MODULEENTRY32W);

		if (Module32FirstW(snapshot, &entry))
		{
			do
			{
				if (!std::wcscmp(entry.szModule, name.c_str()))
				{
					CloseHandle(snapshot);
					return (std::uintptr_t)entry.hModule;
				}
			} while (Module32NextW(snapshot, &entry) != FALSE);
		}

		CloseHandle(snapshot);
	}

	return 0;
}

HANDLE dx_process::GetProcess()
{
	return m_process;
}

ProcessArray dx_process::QueryProcessArray(const std::wstring& name)
{
	std::vector< DWORD > process_array = { };

	auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

	if (snapshot)
	{
		PROCESSENTRY32 entry = { };
		entry.dwSize = sizeof(PROCESSENTRY32);

		if (Process32First(snapshot, &entry))
		{
			do
			{
				if (!std::wcscmp(entry.szExeFile, name.c_str()))
					process_array.emplace_back(entry.th32ProcessID);
			} while (Process32Next(snapshot, &entry));
		}

		CloseHandle(snapshot);
	}

	return process_array;
}
