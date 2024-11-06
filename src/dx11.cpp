#include "base.h"
#include <d3d11.h>
#include <d3dcompiler.h>

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

	u32 device_flags = D3D11_CREATE_DEVICE_DEBUG;
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

void d3d_resize_window(s32 width, s32 height) {
	d3d_destroy_render_target_view();
	d3d_render_data.swap_chain->ResizeBuffers(0, 0, 0, DXGI_FORMAT_UNKNOWN, 0);
	d3d_create_render_target_view();
	
	D3D11_VIEWPORT viewport = { 0.0f, 0.0f, (f32) width, (f32) height, 0.0f, 1.0f };
	d3d_render_data.device_context->RSSetViewports(1, &viewport);
}

void d3d_clear_color(D3D_Color color) {
	const f32 color_arr[4] = { color.r, color.g, color.b, color.a };
	d3d_render_data.device_context->OMSetRenderTargets(1, &d3d_render_data.render_target_view, nullptr);
	d3d_render_data.device_context->ClearRenderTargetView(d3d_render_data.render_target_view, color_arr);
}

ID3D11Buffer* d3d_create_buffer(D3D11_BIND_FLAG buffer_type, void* buffer_internal_data, u32 buffer_size) {
	D3D11_BUFFER_DESC buffer_desc = {
		.ByteWidth = buffer_size,
		.Usage = D3D11_USAGE_DEFAULT, // TODO: unhardcode this and allow to select between options
		.BindFlags = (UINT) buffer_type,
		.CPUAccessFlags = 0, // TODO: see '.Usage'
		.MiscFlags = 0,
		.StructureByteStride = 0,
	};
	
	D3D11_SUBRESOURCE_DATA buffer_data = {
		.pSysMem = buffer_internal_data,
		.SysMemPitch = 0,
		.SysMemSlicePitch = 0,
	};
	
	ID3D11Buffer* buffer;
	d3d_render_data.device->CreateBuffer(&buffer_desc, &buffer_data, &buffer);
	
	return buffer;
}

ID3D11Buffer* d3d_create_vertex_buffer(void* buffer_vertex_data, u32 buffer_size) {
	return d3d_create_buffer(D3D11_BIND_VERTEX_BUFFER, buffer_vertex_data, buffer_size);
}

void d3d_bind_vertex_buffer(ID3D11Buffer* buffer) {
	const u32 a = 0; // idk d3d11 makes me do that, unless it crashes
	d3d_render_data.device_context->IASetVertexBuffers(0, 1, &buffer, &a, &a);
}

ID3D11Buffer* d3d_create_index_buffer(void* buffer_index_data, u32 buffer_size) {
	return d3d_create_buffer(D3D11_BIND_INDEX_BUFFER, buffer_index_data, buffer_size);
}

void d3d_bind_index_buffer(ID3D11Buffer* buffer) {
	d3d_render_data.device_context->IASetIndexBuffer(buffer, DXGI_FORMAT_R32_UINT, 0);
}

// this should destroy any kind of buffer
void d3d_destroy_buffer(ID3D11Buffer** buffer) {
	ID3D11Buffer* _buffer = *buffer;
	_buffer->Release();
	*buffer = nullptr;
}

ID3D11InputLayout* d3d_create_input_layout(D3D11_INPUT_ELEMENT_DESC* input_layout_info, u32 input_layout_info_count, ID3DBlob* shader_blob) {
	ID3D11InputLayout* input_layout = nullptr;
	d3d_render_data.device->CreateInputLayout(input_layout_info, input_layout_info_count, shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), &input_layout);
	
	return input_layout;
}

void d3d_bind_input_layout(ID3D11InputLayout* input_layout) {
	d3d_render_data.device_context->IASetInputLayout(input_layout);
}

ID3DBlob* d3d_compile_shader(String8 shader_source, D3D_Shader_Type shader_type) {
	ID3DBlob* shader_blob = nullptr;
	ID3DBlob* shader_errors = nullptr;
	u32 flags = D3DCOMPILE_OPTIMIZATION_LEVEL2;
	
	// 'uk' stands for 'unknown'
	String8 entry_point = create_string_from("uk_main");
	String8 feature_level = create_string_from("uk_5_0");
	switch (shader_type) {
		case D3D_Shader_Type::Vertex: {
			entry_point = "vs_main";
			feature_level = "vs_5_0";
		} break;
		
		case D3D_Shader_Type::Pixel: {
			entry_point = "ps_main";
			feature_level = "ps_5_0";
		} break;
		
		default: {
			goto clean_and_exit;
		} break;
	}

	if (D3DCompile(shader_source.data, shader_source.len, nullptr, nullptr, nullptr, entry_point.data, feature_level.data, flags, 0, &shader_blob, &shader_errors) != S_OK) {
		goto clean_and_exit;
	}
	
	clean_and_exit:
	destroy_string(&entry_point);
	destroy_string(&feature_level);
	return shader_blob;
}

ID3D11VertexShader* d3d_create_vertex_shader(String8 shader_source, ID3DBlob** out_shader_blob) {
	ID3DBlob* shader_blob = d3d_compile_shader(shader_source, D3D_Shader_Type::Vertex);
	ID3D11VertexShader* vertex_shader = nullptr;
	
	d3d_render_data.device->CreateVertexShader(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), nullptr, &vertex_shader);
	
	*out_shader_blob = shader_blob;
	return vertex_shader;
}

ID3D11PixelShader* d3d_create_pixel_shader(String8 shader_source) {
	ID3DBlob* shader_blob = d3d_compile_shader(shader_source, D3D_Shader_Type::Pixel);
	ID3D11PixelShader* pixel_shader = nullptr;
	
	d3d_render_data.device->CreatePixelShader(shader_blob->GetBufferPointer(), shader_blob->GetBufferSize(), nullptr, &pixel_shader);
	
	return pixel_shader;
}

void d3d_bind_vertex_shader(ID3D11VertexShader* shader) {
	d3d_render_data.device_context->VSSetShader(shader, nullptr, 0);
}

void d3d_bind_pixel_shader(ID3D11PixelShader* shader) {
	d3d_render_data.device_context->PSSetShader(shader, nullptr, 0);
}

void d3d_bind_primitives(D3D11_PRIMITIVE_TOPOLOGY primitive) {
	d3d_render_data.device_context->IASetPrimitiveTopology(primitive);
}

void d3d_draw(u32 vertex_count) {
	d3d_render_data.device_context->Draw(vertex_count, 0);
}