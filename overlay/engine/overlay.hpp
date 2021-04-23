#pragma once

#include "../include/singleton.hpp"
#include <string>
#include "../include/win32.hpp"
#include "../imgui/imgui.hpp"
#include "../imgui/imgui_impl_dx9.hpp"
#include "../imgui/imgui_impl_win32.hpp"

#undef DrawText
inline HWND m_window = nullptr;
inline int m_display_size[2] = { 0,0 };

struct window_rect_data_t
	: public RECT
{
public:
	int GetWidth() const {
		return (right - left);
	}

	int GetHeight() const {
		return (bottom - top);
	}
};

enum TextFlags
	: unsigned
{
	TextLeft = 0x00,
	TextRight = 0x01,
	TextCenterV = 0x02,
	TextCenterH = 0x04,
	TextCenter = (TextCenterV | TextCenterH),
};

extern HWND m_overlay_window;
extern HWND m_target_window;

class dx_overlay
	: public Singleton< dx_overlay >
{
public:
	bool create_overlay(const std::wstring& class_name);

public:
	virtual bool begin();
	virtual void end();

public:
	virtual window_rect_data_t& get_rect_data();
	IDirect3D9* m_direct = nullptr;
	IDirect3DDevice9* m_direct_device = nullptr;

private:
	virtual bool direct_create();
	virtual ImDrawList* get_draw_list();

private:
	static LRESULT WINAPI message_manager(HWND window, UINT message, WPARAM wparam, LPARAM lparam);

	window_rect_data_t m_target_window_size = { };
};