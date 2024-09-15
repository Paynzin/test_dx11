add_rules("mode.debug", "mode.release")
add_requires("libsdl", "imgui", { configs = { sdl2 = true, dx11 = true } })

target("test_dx11")
    set_kind("binary")
    set_languages("cxx20")
    add_files("src/*.cpp")
	add_rules("win.sdk.application")
	if is_mode("debug") then
		add_ldflags("/subsystem:console", { force = true })
	end
	add_links("d3d11", "d3dcompiler", "dxgi")
	add_packages("libsdl", "imgui")
target_end()