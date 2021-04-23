#include "gui.hpp"

auto MenuWidth = 150.f;
auto MenuHeight = 200.f;

auto ElementHeight = 17.f;

void dx_gui::present()
{
	auto& overlay = dx_overlay::instance();

	if (GetAsyncKeyState(VK_HOME) & 1)
		m_render = !m_render;

	if (!m_render)
		return;
}
