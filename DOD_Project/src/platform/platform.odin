package platform
import sdl "vendor:sdl3"
import "core:fmt"

Init :: proc(_p : ^Platform) { 
    ok := sdl.Init({.VIDEO}); assert(ok)

    _p.width = 1920
    _p.height = 1080

    _p.window = sdl.CreateWindow(
        "DOD Research Project",
        _p.width,
        _p.height,
        nil
    )
    assert(_p.window != nil)

    _p.gpu = sdl.CreateGPUDevice({.SPIRV}, true, nil); assert(_p.gpu != nil)
    ok = sdl.ClaimWindowForGPUDevice(_p.gpu, _p.window); assert(ok)

    ok = sdl.SetGPUSwapchainParameters(_p.gpu, _p.window, .SDR, .IMMEDIATE); assert(ok)

    _p.running = true
    fmt.printfln("--- Platform & Window Initialized")
}

ExecuteSdlEvents :: proc(_p : ^Platform) {
    // Process events
    event : sdl.Event 
    for sdl.PollEvent(&event) {
        #partial switch event.type {
            case .QUIT:
                _p.running = false
        }
    }
}

Shutdown :: proc(_p : ^Platform) {
    if _p.window != nil {
        sdl.DestroyWindow(_p.window)
        _p.window = nil
    }
    sdl.Quit()
}