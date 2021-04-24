#include "gui.hpp"

auto MenuWidth = 150.f;
auto MenuHeight = 200.f;

auto ElementHeight = 17.f;

void dx_gui::present()
{
	auto& overlay = dx_overlay::instance();

	if (GetAsyncKeyState(VK_HOME) & 1)
	{
		m_render = !m_render;

		long style = GetWindowLong(m_overlay_window, GWL_EXSTYLE);

		if (m_render == true)
		{
			style &= ~WS_EX_LAYERED;
			SetWindowLong(m_overlay_window, GWL_EXSTYLE, style);
			SetForegroundWindow(m_overlay_window);
		}
		else
		{
			style |= WS_EX_LAYERED;
			SetWindowLong(m_overlay_window, GWL_EXSTYLE, style);
			SetForegroundWindow(m_overlay_window);
		}
	}

	if (!m_render)
		return;

	ImGui::Begin("");
	ImGui::End();
}
