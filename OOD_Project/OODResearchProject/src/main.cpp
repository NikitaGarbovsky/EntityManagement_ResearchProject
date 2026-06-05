#include "app/app.h"
#include "app/appData.h"

using namespace application;

int main() {
    application::Init(&gAppState);
    application::Run(&gAppState);
    application::Shutdown(&gAppState);
}