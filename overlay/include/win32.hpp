#pragma once

#if !defined( WIN32_LEAN_AND_MEAN )
#define WIN32_LEAN_AND_MEAN
#endif // WIN32_LEAN_AND_MEAN

#if !defined( NOMINMAX )
#define NOMINMAX
#endif // NOMINMAX

#include <windows.h>
#include <winternl.h>

#if defined( DrawText )
#undef DrawText
#endif // DrawText

#pragma warning( push )
#pragma warning( disable : 4005 )
#include <ntstatus.h>
#pragma warning( pop )

#include <d3d9.h>
#pragma comment( lib, "d3d9.lib" )

#include <uxtheme.h>
#include <conio.h>

#include <dwmapi.h>
#pragma comment( lib, "dwmapi.lib" )

#include <chrono>
#include <map>
#include <unordered_map>