#include "app.h"
#include <iostream>

#include "SDL3/SDL_events.h"
#include "../platform/platform_data.h"
#include "../platform/platform.h"

namespace application {
    
void init() {
    platform::init();
    std::cout << "Application Successfully Initialized\n";
}
    
void run() {
    SDL_Event event;
    
    while (platform::running) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_EVENT_QUIT:
                platform::running = false;
            }
        }
    }
}
    
void shutdown() {
    platform::shutdown();
    std::cout << "Shutdown Successfully\n";
}


}



