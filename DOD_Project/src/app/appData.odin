package application

import "../modules/renderer"
import sdl3platform "../platform"
import "../modules/ecs"

// Maintains state of the whole application
AppState :: struct {
	platform : sdl3platform.Platform,
	world : ecs.Entity_World,
	renderer : renderer.Renderer, 
	stats : sdl3platform.Frame_Stats,
}
appState : AppState

