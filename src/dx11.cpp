#include "defines.h"
#include <d3d11.h>

#include "dx11.h"

static D3D_Render_Data d3d_render_data = {
	.device = nullptr,
	.device_context = nullptr,
	.swap_chain = nullptr,
	.render_target_view = nullptr,
};

b8 d3d_create_device(HWND window_hwnd) {
	DXGI_SWAP_CHAIN_DESC swap_chain_desc = {
		.BufferDesc = {
			.Width = 0,
			.Height = 0,
			.RefreshRate = {
				.Numerator = 60,
				.Denominator = 1,
			},
			.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
		},
		.SampleDesc = {
			.Count = 1,
			.Quality = 0,
		},
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = 2,
		.OutputWindow = window_hwnd,
		.Windowed = true,
		.SwapEffect = DXGI_SWAP_EFFECT_DISCARD,
		.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH,
	};

	u32 device_flags = 0;
	D3D_FEATURE_LEVEL feature_levels[] = { D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_0, };
	u32 feature_levels_len = sizeof(feature_levels) / sizeof(D3D_FEATURE_LEVEL);
	if (D3D11CreateDeviceAndSwapChain(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, device_flags, feature_levels, feature_levels_len, D3D11_SDK_VERSION, &swap_chain_desc, &d3d_render_data.swap_chain, &d3d_render_data.device, nullptr, &d3d_render_data.device_context) != S_OK) {
		return false;
	}
	
	d3d_create_render_target_view();

	return true;
}

void d3d_destroy_device() {
	d3d_destroy_render_target_view();

	if (d3d_render_data.swap_chain != nullptr) {
		d3d_render_data.swap_chain->Release();
		d3d_render_data.swap_chain = nullptr;
	}
	
	if (d3d_render_data.device_context != nullptr) {
		d3d_render_data.device_context->Release();
		d3d_render_data.device_context = nullptr;
	}
	
	if (d3d_render_data.device != nullptr) {
		d3d_render_data.device->Release();
		d3d_render_data.device = nullptr;
	}
}

void d3d_create_render_target_view() {
	ID3D11Texture2D* back_buffer;
	d3d_render_data.swap_chain->GetBuffer(0, IID_PPV_ARGS(&back_buffer));
	d3d_render_data.device->CreateRenderTargetView(back_buffer, nullptr, &d3d_render_data.render_target_view);
	back_buffer->Release();
}

void d3d_destroy_render_target_view() {
	if (d3d_render_data.render_target_view != nullptr) {
		d3d_render_data.render_target_view->Release();
		d3d_render_data.render_target_view = nullptr;
	}
}

D3D_Render_Data d3d_get_render_data() {
	return d3d_render_data;
}

void d3d_swap_buffers(b8 vsync) {
	u32 sync = 0;
	if (vsync) {
		sync = 1;
	}

	d3d_render_data.swap_chain->Present(sync, 0);
}

void d3d_resize_window() {
	d3d_destroy_render_target_view();
	d3d_render_data.swap_chain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	d3d_create_render_target_view();
}

void d3d_clear_color(D3D_Color color) {
	const f32 color_arr[4] = { color.r, color.g, color.b, color.a };
	d3d_render_data.device_context->OMSetRenderTargets(1, &d3d_render_data.render_target_view, nullptr);
	d3d_render_data.device_context->ClearRenderTargetView(d3d_render_data.render_target_view, color_arr);
}