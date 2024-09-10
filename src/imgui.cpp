#include "defines.h"
#include <imgui.h>
#include <imgui_impl_sdl2.h>
#include <imgui_impl_dx11.h>
#include <SDL.h>

#include "dx11.h"

void imgui_init(SDL_Window* window) {
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
	
	ImGui::StyleColorsDark();
	
	D3D_Render_Data dx_data = d3d_get_render_data();
	
	ImGui_ImplSDL2_InitForD3D(window);
	ImGui_ImplDX11_Init(dx_data.device, dx_data.device_context);
}

void imgui_deinit() {
	ImGui_ImplDX11_Shutdown();
	ImGui_ImplSDL2_Shutdown();
	ImGui::DestroyContext();
}

void imgui_begin_frame() {
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplSDL2_NewFrame();
	ImGui::NewFrame();
}

void imgui_end_frame() {
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void imgui_process_events(SDL_Event* event) {
	ImGui_ImplSDL2_ProcessEvent(event);
}