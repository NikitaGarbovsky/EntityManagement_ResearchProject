package application

import sdl "vendor:sdl3"
import "core:fmt"
import "../platform"
import assimp "../../Dependencies/odin-assimp"

init :: proc() {
    platform.init()

    // Version number of assimp binding
    fmt.printfln("Assimp Version: {}.{}", assimp.get_version_major(), assimp.get_version_minor())

    // TESTING ASSIMP IMPORT
    flags :=
        u32(assimp.PostProcessSteps.Triangulate) |
        u32(assimp.PostProcessSteps.FlipUVs) |
        u32(assimp.PostProcessSteps.JoinIdenticalVertices)

    model := assimp.import_file("Resources/Dragon 2.5_fbx.fbx", flags)
    if model == nil {
        fmt.println("Assimp failed:", assimp.get_error_string())
        return
    }

    defer assimp.release_import(model)

    fmt.println("Assimp loaded model")
    fmt.println("Meshes:", model.mNumMeshes)
}

run :: proc() {
    event : sdl.Event

    for platform.running {
        for sdl.PollEvent(&event) {
            #partial switch event.type {
                case .QUIT: 
                platform.running = false
            }
        }
    }
}

shutdown :: proc() {
    platform.shutdown()
    fmt.println("Shutdown Successfully")
}