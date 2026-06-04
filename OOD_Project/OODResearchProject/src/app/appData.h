#pragma once
#include <iostream>
#include "../platform/platform_data.h"

using namespace sdl3platform;

namespace application {
	std::string ReadShaderFile(const char* _filename);

	struct AppState {
		sdl3platform::Platform platform;
		//scene
		//renderer
		//stats
		//render_instances
	};

	extern AppState gAppState;
}
