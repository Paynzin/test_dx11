#include "base.h"
#include <SDL.h>
#include <SDL_syswm.h>
#include <imgui.h>

#include "dx11.h"
#include "imgui.h"

s32 SDL_main(s32 argc, c8** argv) {
	set_string_allocator({
		.alloc = SDL_malloc,
		.realloc = SDL_realloc,
		.free = SDL_free
	});

	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		return -1;
	}

	SDL_Window* window = SDL_CreateWindow("test dx11", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
	if (window == nullptr) {
		return -1;
	}

	// get native window handle for windows
	SDL_SysWMinfo wm_info;
	SDL_VERSION(&wm_info.version);
	SDL_GetWindowWMInfo(window, &wm_info);
	HWND window_hwnd = (HWND) wm_info.info.win.window;
	
	if (!d3d_create_device(window_hwnd)) {
		return -1;
	}
	
	d3d_resize_window(800, 600);
	
	imgui_init(window);

	b8 quit = false;
	while (!quit) {
		SDL_Event event;
		while (SDL_PollEvent(&event)) {
			imgui_process_events(&event);

			switch (event.type) {
				case SDL_QUIT: {
					quit = true;
				} break;
				
				case SDL_WINDOWEVENT: {
					SDL_WindowEvent window_event = event.window;
					if (window_event.event == SDL_WINDOWEVENT_RESIZED) {
						s32 width = window_event.data1;
						s32 height = window_event.data2;
						d3d_resize_window(width, height);
					}
				} break;

				default: {
				} break;
			}
		}

		// do rendering
		imgui_begin_frame();

		static D3D_Color color = { 0.0f, 0.0f, 0.75f, 1.00f };
		d3d_clear_color(color);
		ImGui::Begin("cool overlay");
		ImGui::ColorEdit3("clear color", (f32*) &color);
		ImGui::End();
		
		static b8 is_initialized = false;
		static ID3D11InputLayout* triangle_input_layout = nullptr;
		static ID3D11Buffer* triangle_vertex_buffer = nullptr;
		static ID3D11VertexShader* triangle_vertex_shader = nullptr;
		static ID3D11PixelShader* triangle_pixel_shader = nullptr;
		if (!is_initialized) {
			String8 triangle_vertex_shader_string = read_entire_file_as_string(create_string_from("shaders/default.hlsl"));
			String8 triangle_pixel_shader_string = read_entire_file_as_string(create_string_from("shaders/default.hlsl"));
			D3D11_INPUT_ELEMENT_DESC triangle_vertex_data_layout = { "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 };
			D3D_Vertex triangle_vertex_data[] = {
				{ 0.0f, 0.5f, 0.0f }, // top
				{ -0.5f, -0.5f, 0.0f }, // left
				{ 0.5f, -0.5f, 0.0f } // right
			};
			
			triangle_vertex_buffer = d3d_create_vertex_buffer((void*) triangle_vertex_data, sizeof(triangle_vertex_data));
			ID3DBlob* vertex_shader_blob = nullptr;
			triangle_vertex_shader = d3d_create_vertex_shader(triangle_vertex_shader_string, &vertex_shader_blob);
			triangle_pixel_shader = d3d_create_pixel_shader(triangle_pixel_shader_string);
			triangle_input_layout = d3d_create_input_layout(&triangle_vertex_data_layout, 1, vertex_shader_blob);
			is_initialized = true;
		}
		
		d3d_bind_input_layout(triangle_input_layout);
		d3d_bind_vertex_buffer(triangle_vertex_buffer);
		d3d_bind_primitives(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
		d3d_bind_vertex_shader(triangle_vertex_shader);
		d3d_bind_pixel_shader(triangle_pixel_shader);
		
		d3d_draw(3);
		
		imgui_end_frame();
		d3d_swap_buffers(true);
	}
	
	imgui_deinit();
	d3d_destroy_device();
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}