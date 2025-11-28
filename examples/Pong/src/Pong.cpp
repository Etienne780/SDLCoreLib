#include <cmath>
#include <vector>
#include <array>
#include <CoreLib/Math/Vector4.h>
#include "Pong.h"

Pong::Pong()
    : Application("Pong", SDLCore::Version(1, 0)) {
}

void Pong::OnStart() {
    CreateWindow(&m_winID, "Pong", 800, 800);
}

void Pong::OnUpdate() {
}

void Pong::OnQuit() {
}