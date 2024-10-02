#pragma once
#include <Windows.h>
#include <iostream>
#include <thread>
#include <dwmapi.h>
#include <d3d11.h>

#define IMGUI_DEFINE_MATH_OPERATORS
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/TextEditor.h"

#pragma comment(lib, "d3d11.lib")

#include "xorstr/xorstr.hpp"

#include "../Communication/Communication.hpp"
#include "../Configuration.hpp"
#include "../Globals.hpp"

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

typedef struct Screen
{
	int X = 0, Y = 0;
	int Width = 0, Height = 0;

	bool operator==(const Screen& Screen)
	{
		return X == Screen.X && Y == Screen.Y && Width == Screen.Width && Height == Screen.Height;
	}

	bool operator!=(const Screen& Screen)
	{
		return !(*this == Screen);
	}
} CScreen;

class COverlayHook
{
public:
	ID3D11Device* D3D11Device = nullptr;
	ID3D11DeviceContext* D3D11DeviceContext = nullptr;
	IDXGISwapChain* DxgiSwapChain = nullptr;
	ID3D11RenderTargetView* D3D11RenderTargetView = nullptr;
public:
	bool CreateD3DDevice(HWND handle);
	void CleanupD3DDevice();
	void CreateRenderTarget();
	void CleanupRenderTarget();
public:
	bool Initialize();
};

inline auto OverlayHook = std::make_unique<COverlayHook>();