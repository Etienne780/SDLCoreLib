#include <cmath>
#include <vector>
#include <array>
#include <CoreLib/Math/Vector4.h>
#include "App.h"

App::App()
    : Application("Pong", SDLCore::Version(1, 0)) {
}

void App::OnStart() {
    CreateWindow(&m_winID, "Pong", 800, 800);
}

void App::OnUpdate() {
}

void App::OnQuit() {
}