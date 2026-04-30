package application

import sdl "vendor:sdl3"
import "core:fmt"
import "../platform"

init :: proc() {
    platform.init()
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