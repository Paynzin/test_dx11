#pragma once

#include "defines.h"
#include <d3d11.h>

struct D3D_Render_Data {
	ID3D11Device* device;
	ID3D11DeviceContext* device_context;
	IDXGISwapChain* swap_chain;
	ID3D11RenderTargetView* render_target_view;
};

struct D3D_Color {
	f32 r, g, b, a;
};

b8 d3d_create_device(HWND window_hwnd);
void d3d_destroy_device();
void d3d_create_render_target_view();
void d3d_destroy_render_target_view();
D3D_Render_Data d3d_get_render_data();
void d3d_swap_buffers(b8 vsync);
void d3d_resize_window();
void d3d_clear_color(D3D_Color color);