#include "../features/gui.hpp"
#include "../engine/process.hpp"

#include "../include/win32.hpp"

void display_execute() {
	RECT rect = { };
	GetClientRect(m_window, &rect);

	m_display_size[0] = static_cast<float>(rect.right - rect.left);
	m_display_size[1] = static_cast<float>(rect.bottom - rect.top);
}

DWORD WINAPI overlay_execute(void* parameter) {
	auto& overlay = dx_overlay::instance();
	auto& process = dx_process::instance();
	auto& gui = dx_gui::instance();

	if (!overlay.create_overlay(L"Notepad"))
		return 0;

	MSG message = { };

	do
	{
		if (PeekMessageW(&message, m_overlay_window, 0, 0, PM_REMOVE)) {
			TranslateMessage(&message);
			DispatchMessageW(&message);
		}

		if (overlay.begin()) {
			display_execute();
			gui.present();
			overlay.end();
		}

		std::this_thread::sleep_for(std::chrono::milliseconds(1));

	} while (message.message != WM_QUIT);

	return 0;
}

int main(int argc, char* argv[]) {
	auto& process = dx_process::instance();

	while (!(m_window = FindWindowW(nullptr, L"Untitled - Notepad")))
		Sleep(1000);

	const auto process_array = dx_process::QueryProcessArray(L"notepad.exe");

	if (process_array.empty())
		return 0;

	if (!process.Create(process_array[0]))
		return 0;

	CreateThread(nullptr, 0, &overlay_execute, nullptr, 0, nullptr);

	_getch();
	return 0;
}