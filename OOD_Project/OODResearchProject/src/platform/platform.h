#pragma once
#include "platform_data.h"

namespace sdl3platform {
    void init(Platform* _platform);
    void shutdown(Platform* _platform);
    void ExecuteSdlEvents(Platform* _platform);
}


