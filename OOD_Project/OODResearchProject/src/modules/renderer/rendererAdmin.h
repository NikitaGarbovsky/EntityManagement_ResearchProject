#pragma once
#include <iostream>
#include "../../platform/platform_data.h"
#include "rendererData.h"
#include <span>
using namespace sdl3platform;

namespace renderer {
	void Init(
		Renderer* _renderer,
		sdl3platform::Platform* _platform,
		std::string _vert_code,
		std::string _frag_code);

	bool InitSpriteRendererResources(
		struct Renderer* _renderer,
		uint32_t _max_instances);

	bool InitSpritePipeline(Renderer* _renderer,
		std::span<const uint8_t> _vert_code,
		std::span<const uint8_t> _frag_code);
	
	void ShutdownSpriteRendererResources(
		struct Renderer* _renderer);

	void Shutdown(struct Renderer* _renderer);
}