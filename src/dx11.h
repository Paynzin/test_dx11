#pragma once

#include "base.h"
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

enum D3D_Shader_Type {
	Vertex,
	Pixel,
};

struct D3D_Vertex {
	f32 x, y, z;
};

b8 d3d_create_device(HWND window_hwnd);
void d3d_destroy_device();
void d3d_create_render_target_view();
void d3d_destroy_render_target_view();
D3D_Render_Data d3d_get_render_data();
void d3d_swap_buffers(b8 vsync);
void d3d_resize_window(s32 width, s32 height);
void d3d_clear_color(D3D_Color color);
ID3D11Buffer* d3d_create_buffer(D3D11_BIND_FLAG buffer_type, void* buffer_internal_data, u32 buffer_size);
ID3D11Buffer* d3d_create_vertex_buffer(void* buffer_vertex_data, u32 buffer_size);
void d3d_bind_vertex_buffer(ID3D11Buffer* buffer);
ID3D11Buffer* d3d_create_index_buffer(void* buffer_index_data, u32 buffer_size);
void d3d_bind_index_buffer(ID3D11Buffer* buffer);
void d3d_destroy_buffer(ID3D11Buffer** buffer);
ID3D11InputLayout* d3d_create_input_layout(D3D11_INPUT_ELEMENT_DESC* input_layout_info, u32 input_layout_info_count, ID3DBlob* shader_blob);
void d3d_bind_input_layout(ID3D11InputLayout* input_layout);
ID3DBlob* d3d_compile_shader(String8 shader_source, D3D_Shader_Type shader_type);
ID3D11VertexShader* d3d_create_vertex_shader(String8 shader_source, ID3DBlob** out_shader_blob);
ID3D11PixelShader* d3d_create_pixel_shader(String8 shader_source);
void d3d_bind_vertex_shader(ID3D11VertexShader* shader);
void d3d_bind_pixel_shader(ID3D11PixelShader* shader);
void d3d_bind_primitives(D3D11_PRIMITIVE_TOPOLOGY primitive);
void d3d_draw(u32 vertex_count);