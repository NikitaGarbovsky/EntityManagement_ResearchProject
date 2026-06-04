#include "app/app.h"
#include "app/appData.h"

using namespace application;

int main() {
    application::init(&gAppState);
    application::run(&gAppState);
    application::shutdown(&gAppState);
}