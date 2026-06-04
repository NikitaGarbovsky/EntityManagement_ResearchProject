#include "app.h"
#include <iostream>
#include <fstream>
#include "appData.h"

#include "SDL3/SDL_events.h"
#include "../platform/platform_data.h"
#include "../platform/platform.h"



namespace application {
    
    void init(AppState* _appState) {
        sdl3platform::init(&_appState->platform);

        // Load precompiled shader files
        ReadShaderFile("../Resources/Shaders/sprite_batch.frag.spv");
        ReadShaderFile("../Resources/Shaders/sprite_batch.vert.spv");

        // renderer init
        // scene.init
        // sdl3platform::initframestats

        // set renderer camera & zoom here

	    

        std::cout << "--- App Successfully Initialized\n";
    }
    
    void run(AppState* _appState){
        std::cout << "--- Running Simlutation";

        while (_appState->platform.running) {
            sdl3platform::ExecuteSdlEvents(&_appState->platform);
        }
        
    }
    
    void shutdown(AppState* _appState) {
        sdl3platform::shutdown(&_appState->platform);
        std::cout << "Shutdown Successfully\n";
    }
}
