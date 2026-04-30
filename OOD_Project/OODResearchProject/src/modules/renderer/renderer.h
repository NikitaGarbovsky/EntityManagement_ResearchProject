#pragma once

namespace renderer {
    
    void init();
    void shutdown();
    
    void begin_frame();
    void render_frame();
    void end_frame();
}