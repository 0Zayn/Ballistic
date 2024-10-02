#include "OverlayHook.hpp"

static TextEditor Editor;

bool COverlayHook::CreateD3DDevice(HWND handle)
{
	DXGI_SWAP_CHAIN_DESC SwapChainDesc;
	RtlZeroMemory(&SwapChainDesc, sizeof(SwapChainDesc));

	SwapChainDesc.BufferCount = 2;
	SwapChainDesc.BufferDesc.Width = 0;
	SwapChainDesc.BufferDesc.Height = 0;
	SwapChainDesc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	SwapChainDesc.BufferDesc.RefreshRate.Numerator = 60;
	SwapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
	SwapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH;
	SwapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	SwapChainDesc.OutputWindow = handle;
	SwapChainDesc.SampleDesc.Count = 1;
	SwapChainDesc.SampleDesc.Quality = 0;
	SwapChainDesc.Windowed = TRUE;
	SwapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_DISCARD;

	const UINT CreateDeviceFlags = 0;
	D3D_FEATURE_LEVEL D3DFeatureLevel;
	const D3D_FEATURE_LEVEL FeatureLevelArr[2] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	HRESULT Result = D3D11CreateDeviceAndSwapChain(nullptr,
		D3D_DRIVER_TYPE_HARDWARE, nullptr, CreateDeviceFlags, FeatureLevelArr,
		2, D3D11_SDK_VERSION, &SwapChainDesc, &this->DxgiSwapChain, &this->D3D11Device,
		&D3DFeatureLevel, &this->D3D11DeviceContext);

	if (Result == DXGI_ERROR_UNSUPPORTED)
	{
		Result = D3D11CreateDeviceAndSwapChain(nullptr,
			D3D_DRIVER_TYPE_WARP, nullptr, CreateDeviceFlags, FeatureLevelArr,
			2, D3D11_SDK_VERSION, &SwapChainDesc, &this->DxgiSwapChain, &this->D3D11Device,
			&D3DFeatureLevel, &this->D3D11DeviceContext);
	}

	if (Result != S_OK)
		return false;

	this->CreateRenderTarget();
	return true;
}

void COverlayHook::CleanupD3DDevice()
{
	this->CleanupRenderTarget();

	if (this->DxgiSwapChain)
	{
		this->DxgiSwapChain->Release();
		this->DxgiSwapChain = nullptr;
	}

	if (this->D3D11DeviceContext)
	{
		this->D3D11DeviceContext->Release();
		this->D3D11DeviceContext = nullptr;
	}

	if (this->D3D11Device)
	{
		this->D3D11Device->Release();
		this->D3D11Device = nullptr;
	}
}

void COverlayHook::CreateRenderTarget()
{
	ID3D11Texture2D* D3D11BackBuffer;
	this->DxgiSwapChain->GetBuffer(0, IID_PPV_ARGS(&D3D11BackBuffer));
	if (D3D11BackBuffer != nullptr)
	{
		this->D3D11Device->CreateRenderTargetView(D3D11BackBuffer, nullptr, &this->D3D11RenderTargetView);
		D3D11BackBuffer->Release();
	}
}

void COverlayHook::CleanupRenderTarget()
{
	if (this->D3D11RenderTargetView)
	{
		this->D3D11RenderTargetView->Release();
		this->D3D11RenderTargetView = nullptr;
	}
}

__forceinline LRESULT __stdcall WindowProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	switch (msg)
	{
	case WM_SIZE:
		if (OverlayHook->D3D11Device != nullptr && wParam != SIZE_MINIMIZED)
		{
			OverlayHook->CleanupRenderTarget();
			OverlayHook->DxgiSwapChain->ResizeBuffers(2, (UINT)LOWORD(lParam), (UINT)HIWORD(lParam), DXGI_FORMAT_UNKNOWN, 0);
			OverlayHook->CreateRenderTarget();
		}
		return 0;
	case WM_SYSCOMMAND:
		if ((wParam & 0xfff0) == SC_KEYMENU)
			return 0;
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
	default:
		break;
	}
	return DefWindowProcA(hWnd, msg, wParam, lParam);
}

bool COverlayHook::Initialize()
{
	Editor.SetLanguageDefinition(TextEditor::LanguageDefinition::Lua());
	Editor.SetText(xorstr_("print(\"Hello, world!\")"));

	ImGui_ImplWin32_EnableDpiAwareness();

	WNDCLASSEX WindowClass;
	WindowClass.cbClsExtra = 0;
	WindowClass.cbSize = sizeof(WNDCLASSEX);
	WindowClass.cbWndExtra = 0;
	WindowClass.hbrBackground = reinterpret_cast<HBRUSH>(CreateSolidBrush(RGB(0, 0, 0)));
	WindowClass.hCursor = LoadCursorA(nullptr, IDC_ARROW);
	WindowClass.hIcon = LoadIconA(nullptr, IDI_APPLICATION);
	WindowClass.hIconSm = LoadIconA(nullptr, IDI_APPLICATION);
	WindowClass.hInstance = GetModuleHandleA(nullptr);
	WindowClass.lpfnWndProc = WindowProc;
	WindowClass.lpszClassName = Configuration::Name;
	WindowClass.lpszMenuName = nullptr;
	WindowClass.style = CS_VREDRAW | CS_HREDRAW;

	RegisterClassExA(&WindowClass);
	const auto Window = CreateWindowExA(WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_NOACTIVATE, WindowClass.lpszClassName, Configuration::Name,
		WS_POPUP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), nullptr, nullptr, WindowClass.hInstance, nullptr);

	SetLayeredWindowAttributes(Window, 0, 255, LWA_ALPHA);

	const MARGINS Margin = { -1 };
	DwmExtendFrameIntoClientArea(Window, &Margin);

	if (!this->CreateD3DDevice(Window))
	{
		this->CleanupD3DDevice();
		UnregisterClassA(WindowClass.lpszClassName, WindowClass.hInstance);
		return false;
	}

	ShowWindow(Window, SW_SHOW);
	UpdateWindow(Window);

	ImGui::CreateContext();
	ImGui::GetIO().IniFilename = nullptr;

	ImGui_ImplWin32_Init(Window);
	ImGui_ImplDX11_Init(this->D3D11Device, this->D3D11DeviceContext);

	const ImVec4 ClearColor = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);

	bool Init = true, Draw = true, Done = false;
	while (!Done)
	{
		MSG Message;
		while (PeekMessageA(&Message, nullptr, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&Message);
			DispatchMessageA(&Message);
			if (Message.message == WM_QUIT)
				Done = true;
		}

		if (Done)
			break;

		const auto TargetWindow = FindWindow(nullptr, xorstr_("Roblox"));
		const auto ForegroundWindow = GetForegroundWindow();

		if (TargetWindow != ForegroundWindow && Window != ForegroundWindow)
			MoveWindow(Window, 0, 0, 0, 0, true);
		else
		{
			RECT Rect;
			GetWindowRect(TargetWindow, &Rect);

			auto RSizeX = Rect.right - Rect.left;
			auto RSizeY = Rect.bottom - Rect.top;

			auto Status = false;
			MONITORINFO MonitorInfo = { sizeof(MONITORINFO) };
			if (GetMonitorInfoA(MonitorFromWindow(TargetWindow, MONITOR_DEFAULTTOPRIMARY), &MonitorInfo))
			{
				RECT WindowRect;
				if (GetWindowRect(TargetWindow, &WindowRect))
				{
					Status = WindowRect.left == MonitorInfo.rcMonitor.left
						&& WindowRect.right == MonitorInfo.rcMonitor.right
						&& WindowRect.top == MonitorInfo.rcMonitor.top
						&& WindowRect.bottom == MonitorInfo.rcMonitor.bottom;
				}
			}

			if (Status)
			{
				RSizeX += 16;
				RSizeY -= 24;
			}
			else
			{
				RSizeY -= 63;
				Rect.left += 8;
				Rect.top += 31;
			}
			MoveWindow(Window, Rect.left, Rect.top, RSizeX, RSizeY, 1);
		}

		if ((GetAsyncKeyState(VK_END) & 1))
			Draw = !Draw;

		ImGui_ImplDX11_NewFrame();
		ImGui_ImplWin32_NewFrame();
		ImGui::NewFrame();
		{
			if (GetForegroundWindow() == FindWindowA(nullptr, xorstr_("Roblox")) || GetForegroundWindow() == Window)
			{
				if (Draw)
				{
					ImGui::SetNextWindowSize(ImVec2(600, 355));
					ImGui::Begin(Configuration::Name, nullptr, ImGuiWindowFlags_NoResize);
					{
						if (ImGui::BeginTabBar(xorstr_("#Tabs"), ImGuiTabBarFlags_None))
						{
							if (ImGui::BeginTabItem(xorstr_("Execution")))
							{
								if (ImGui::Button(xorstr_("Execute")))
									Communication->ExecuteScript(Editor.GetText());

								ImGui::SameLine();

								if (ImGui::Button(xorstr_("Clear")))
									Editor.SetText("");

								Editor.Render(xorstr_("TextEditor"));
								ImGui::Separator();
								ImGui::EndTabItem();
							}


							if (ImGui::BeginTabItem(xorstr_("Player")))
							{
								ImGui::Text(xorstr_("WalkSpeed"));
								ImGui::SameLine();
								ImGui::InputInt(xorstr_("##WalkSpeed"), &Globals.WalkSpeed, 1, 2, 0);
								ImGui::SameLine();
								if (ImGui::Button(xorstr_("Set")))
								{
									Communication->ExecuteScript(std::format(R"(
											game:GetService("Players")["LocalPlayer"]["Character"]["Humanoid"]["WalkSpeed"] = {}
										)", Globals.WalkSpeed));
								}

								ImGui::Text(xorstr_("JumpPower"));
								ImGui::SameLine();
								ImGui::InputInt(xorstr_("##JumpPower"), &Globals.JumpPower, 1, 2, 0);
								ImGui::SameLine();
								if (ImGui::Button(xorstr_("Set")))
								{
									Communication->ExecuteScript(std::format(R"(
											game:GetService("Players")["LocalPlayer"]["Character"]["Humanoid"]["JumpPower"] = {}
										)", Globals.JumpPower));
								}

								ImGui::Separator();

								if (ImGui::Button(xorstr_("Reset Character")))
								{
									Communication->ExecuteScript(R"(
											game:GetService("Players")["LocalPlayer"]["Character"]["Humanoid"]["Health"] = 0
										)");
								}

								ImGui::EndTabItem();
							}

							if (ImGui::BeginTabItem(xorstr_("World")))
							{
								ImGui::Text(xorstr_("Gravity"));
								ImGui::SameLine();
								ImGui::InputFloat(xorstr_("##Gravity"), &Globals.Gravity, 1, 2, 0);
								ImGui::SameLine();
								if (ImGui::Button(xorstr_("Set")))
								{
									Communication->ExecuteScript(std::format(R"(
											workspace["Gravity"] = {}
										)", Globals.Gravity));
								}

								ImGui::EndTabItem();
							}


							if (ImGui::BeginTabItem(xorstr_("Settings")))
							{
								if (ImGui::Button(xorstr_("Panic")))
									exit(EXIT_SUCCESS);

								ImGui::EndTabItem();
							}

							ImGui::EndTabBar();
						}
					}
					ImGui::End();
				}
			}

			if (Draw)
				SetWindowLongA(Window, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_LAYERED | WS_EX_TOOLWINDOW);
			else
				SetWindowLongA(Window, GWL_EXSTYLE, WS_EX_TOPMOST | WS_EX_TRANSPARENT | WS_EX_LAYERED | WS_EX_TOOLWINDOW);

			ImGui::EndFrame();
			ImGui::Render();

			const float ClearColorWithAlpha[4] = { ClearColor.x * ClearColor.w, ClearColor.y * ClearColor.w, ClearColor.z * ClearColor.w, ClearColor.w };
			this->D3D11DeviceContext->OMSetRenderTargets(1, &this->D3D11RenderTargetView, nullptr);
			this->D3D11DeviceContext->ClearRenderTargetView(this->D3D11RenderTargetView, ClearColorWithAlpha);
			ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

			this->DxgiSwapChain->Present(1, 0);

			const float targetFrameTime = 1.0f / 60.0f; // 60 FPS
			static DWORD lastTime = timeGetTime();
			DWORD currentTime = timeGetTime();
			float deltaTime = (currentTime - lastTime) / 1000.0f;

			if (deltaTime < targetFrameTime)
			{
				std::this_thread::sleep_for(std::chrono::milliseconds(static_cast<int>((targetFrameTime - deltaTime) * 1000)));
			}

			lastTime = timeGetTime();
		}
	}

	Init = false;

	ImGui_ImplDX11_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	this->CleanupD3DDevice();
	DestroyWindow(Window);
	UnregisterClassA(WindowClass.lpszClassName, WindowClass.hInstance);
}