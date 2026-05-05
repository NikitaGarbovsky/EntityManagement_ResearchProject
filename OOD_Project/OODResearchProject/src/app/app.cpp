#include "app.h"
#include <iostream>

#include "SDL3/SDL_events.h"
#include "../platform/platform_data.h"
#include "../platform/platform.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace application {
    
void init() {
    platform::init();

    // TESTING ASSIMP IMPORTER
    Assimp::Importer importer;

    const aiScene* scene = importer.ReadFile("../Resources/Dragon 2.5_fbx.fbx", aiProcess_Triangulate | aiProcess_FlipUVs | aiProcess_JoinIdenticalVertices);

    if (!scene) { 
        std::cout << "Assimp failed: " << importer.GetErrorString() << "\n"; 
        return; 
    }

    std::cout << "Assimp loaded model\n";
    std::cout << "Meshes: " << scene->mNumMeshes << "\n";
    std::cout << "Materials: " << scene->mNumMaterials << "\n";

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



