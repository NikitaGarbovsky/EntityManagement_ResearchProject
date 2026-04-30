package platform
import sdl "vendor:sdl3"
import "core:fmt"

init :: proc() { 
    ok := sdl.Init({.VIDEO}); assert(ok)

    window = sdl.CreateWindow(
        "DOD Research Project",
        1920,
        1080,
        nil
    )
    assert(window != nil)

    running = true
    fmt.printfln("--- Platform & Window Initialized")
}

shutdown :: proc() {
    if window != nil {
        sdl.DestroyWindow(window)
    }
    sdl.Quit()
}