#include "defines.h"
#include <SDL.h>
#include <SDL_syswm.h>
#include <imgui.h>

#include "dx11.h"
#include "imgui.h"

s32 main(void) {
	if (SDL_Init(SDL_INIT_VIDEO) < 0) {
		return -1;
	}

	SDL_Window* window = SDL_CreateWindow("test dx11", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
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

				default: {
				} break;
			}
		}
		
		// do rendering
		imgui_begin_frame();

		static D3D_Color color = { 0.0f, 0.0f, 0.75f, 1.00f };
		d3d_clear_color(color);
		ImGui::Begin("cool overlay");
		ImGui::ColorEdit3("clear color",(f32*) &color);
		ImGui::End();

		imgui_end_frame();
		d3d_swap_buffers(true);
	}
	
	imgui_deinit();
	d3d_destroy_device();
	SDL_DestroyWindow(window);
	SDL_Quit();
	return 0;
}

// actual entry point
#include <windef.h>
s32 WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, s32 nCmdShow) {
	return main();
}