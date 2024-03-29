#include "overlay.hpp"

HWND m_overlay_window = nullptr;
HWND m_target_window = nullptr;

#define IMGUI_DEFINE_MATH_OPERATORS

bool dx_overlay::create_overlay(const std::wstring& class_name) {
	WNDCLASSEX window_class_ex = { };

	window_class_ex.cbSize = sizeof(WNDCLASSEX);

	window_class_ex.style = CS_HREDRAW | CS_VREDRAW;
	window_class_ex.lpfnWndProc = message_manager;
	window_class_ex.cbClsExtra = 0;
	window_class_ex.cbWndExtra = 0;
	window_class_ex.hInstance = nullptr;
	window_class_ex.hIcon = nullptr;
	window_class_ex.hCursor = LoadCursorW(nullptr, IDC_ARROW);
	window_class_ex.hbrBackground = HBRUSH(RGB(0, 0, 0));
	window_class_ex.lpszMenuName = L"";
	window_class_ex.lpszClassName = L"Hammer & Chisel Inc.";
	window_class_ex.hIconSm = nullptr;

	if (!RegisterClassExW(&window_class_ex))
		return false;

	m_target_window = (class_name.empty() ? GetDesktopWindow() : FindWindowW(class_name.c_str(), nullptr));

	if (!GetWindowRect(m_target_window, &m_target_window_size))
		return false;

	const auto ex_styles = WS_EX_LAYERED | WS_EX_TRANSPARENT | WS_EX_TOPMOST | WS_EX_NOACTIVATE;

	m_overlay_window = CreateWindowExW(ex_styles, L"Hammer & Chisel Inc.", L"", WS_POPUP | WS_VISIBLE,
		m_target_window_size.left, m_target_window_size.top, m_target_window_size.GetWidth(), m_target_window_size.GetHeight(), nullptr, nullptr, nullptr, nullptr);

	if (!m_overlay_window)
		return false;

	MARGINS margins = { m_target_window_size.left, m_target_window_size.top, m_target_window_size.GetWidth(), m_target_window_size.GetHeight() };
	DwmExtendFrameIntoClientArea(m_overlay_window, &margins);

	if (!SetLayeredWindowAttributes(m_overlay_window, RGB(0, 0, 0), 255, LWA_ALPHA))
		return false;

	if (!ShowWindow(m_overlay_window, SW_SHOW))
		return false;

	if (!direct_create())
		return false;

	return true;
}

bool dx_overlay::begin() {
	m_direct_device->Clear(0, nullptr, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 0, 0), 1.f, 0);
	m_direct_device->BeginScene();

	if (m_window != GetForegroundWindow())
	{
		m_direct_device->EndScene();
		m_direct_device->Present(nullptr, nullptr, nullptr, nullptr);

		return false;
	}

	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	return true;
}

void dx_overlay::end() {
	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());

	m_direct_device->EndScene();
	m_direct_device->Present(nullptr, nullptr, nullptr, nullptr);

	GetWindowRect(m_target_window, &m_target_window_size);
	SetWindowPos(m_overlay_window, HWND_TOP, m_target_window_size.left, m_target_window_size.top, m_target_window_size.GetWidth(), m_target_window_size.GetHeight(), SWP_ASYNCWINDOWPOS);
}

ImDrawList* dx_overlay::get_draw_list() {
	return (ImGui::GetOverlayDrawList());
}

window_rect_data_t& dx_overlay::get_rect_data() {
	return m_target_window_size;
}

HHOOK hMouseHook;
LRESULT CALLBACK mouse_manager(int nCode, WPARAM wParam, LPARAM lParam) {
	ImGuiIO& io = ImGui::GetIO();
	MOUSEHOOKSTRUCT* pMouseStruct = (MOUSEHOOKSTRUCT*)lParam;
	if (pMouseStruct != NULL) {
		switch (wParam)
		{
		case WM_LBUTTONDOWN:
			io.MouseDown[0] = true;
			break;
		case WM_LBUTTONUP:
			io.MouseDown[0] = false;
			io.MouseReleased[0] = true;
			break;
		case WM_RBUTTONDOWN:
			io.MouseDown[1] = true;
			break;
		case WM_RBUTTONUP:
			io.MouseDown[1] = false;
			io.MouseReleased[1] = true;
			break;
		}
	}
	return CallNextHookEx(hMouseHook, nCode, wParam, lParam);
}

bool dx_overlay::direct_create() {
	static auto initialized = false;

	if (!initialized) {
		m_direct = Direct3DCreate9(D3D_SDK_VERSION);

		if (!m_direct)
			return false;

		D3DPRESENT_PARAMETERS parameters = { };

		parameters.Windowed = true;
		parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
		parameters.BackBufferFormat = D3DFMT_A8R8G8B8;
		parameters.BackBufferWidth = m_target_window_size.GetWidth();
		parameters.BackBufferHeight = m_target_window_size.GetHeight();
		parameters.hDeviceWindow = m_overlay_window;
		parameters.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

		auto result = m_direct->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_overlay_window, D3DCREATE_HARDWARE_VERTEXPROCESSING, &parameters, &m_direct_device);

		if (FAILED(result))
			return false;

		ImGui::CreateContext();

		if (!ImGui_ImplWin32_Init(m_overlay_window))
			return false;

		if (!ImGui_ImplDX9_Init(m_direct_device))
			return false;

		HHOOK MouseHook;
		MouseHook = SetWindowsHookEx(WH_MOUSE, mouse_manager, 0, GetCurrentThreadId());

		ImFontConfig font_config = { };

		font_config.OversampleH = 1;
		font_config.OversampleV = 1;
		font_config.PixelSnapH = true;

		auto& io = ImGui::GetIO();

		initialized = true;
	}

	return true;
}

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT WINAPI dx_overlay::message_manager(HWND window, UINT message, WPARAM wparam, LPARAM lparam) {
	if (ImGui_ImplWin32_WndProcHandler(window, message, wparam, lparam))
		return true;

	if (message == WM_DESTROY)
		ExitProcess(EXIT_SUCCESS);

	return DefWindowProcW(window, message, wparam, lparam);
}