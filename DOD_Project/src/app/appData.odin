package application

import renderermodule "../modules/renderer"
import sdl3platform "../platform"
import "../modules/ecs"

// Maintains state of the whole application
AppState :: struct {
	platform : sdl3platform.Platform,
	world : ecs.Entity_World,
	renderer : renderermodule.Renderer, 
	stats : sdl3platform.Frame_Stats,
	render_instances : [dynamic]renderermodule.Sprite_Instance,
}
appState : AppState

