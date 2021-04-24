#include "gui.hpp"

void dx_gui::present() {
	auto& overlay = dx_overlay::instance();

	if (GetAsyncKeyState(VK_HOME) & 1) {
		m_render = !m_render;

		long style = GetWindowLong(m_overlay_window, GWL_EXSTYLE);

		if (m_render == true) {
			style &= ~WS_EX_LAYERED;
			SetWindowLong(m_overlay_window, GWL_EXSTYLE, style);
			SetForegroundWindow(m_overlay_window);
		}
		else {
			style |= WS_EX_LAYERED;
			SetWindowLong(m_overlay_window, GWL_EXSTYLE, style);
			SetForegroundWindow(m_overlay_window);
		}
	}

	if (!m_render)
		return;

	if (ImGui::Begin("")) {

		ImGui::End();
	}
}
