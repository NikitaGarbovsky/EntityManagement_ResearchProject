package main 

import application "app"

main :: proc() {
    application.Init(&application.appState)
    application.Run(&application.appState)
    application.Shutdown(&application.appState)
}