#include <cmath>
#include <vector>
#include <array>
#include <CoreLib/Math/Vector4.h>
#include "Tetris.h"

Tetris::Tetris()
    : Application("Tetris", SDLCore::Version(1, 0)) {
}

void Tetris::OnStart() {
    CreateWindow(&m_winID, "Tetris", 800, 800);
}

void Tetris::OnUpdate() {
   
}

void Tetris::OnQuit() {
}