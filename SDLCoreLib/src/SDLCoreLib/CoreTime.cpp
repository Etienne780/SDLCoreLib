#include <CoreLib/Log.h>
#include <SDL3/SDL.h>
#include "CoreTime.h"

namespace SDLCore {

    static uint64_t s_lastTimeNS = 0;
    static uint64_t s_currentTimeNS = 0;
    static double s_deltaTimeSec = 0.0;
    static double s_frameRate = 0.0;

    uint64_t Time::GetTime() {
        return SDL_GetTicks();
    }

    uint64_t Time::GetTimeNS() {
        return SDL_GetTicksNS();
    }

    double Time::GetTimeSec() {
        return static_cast<double>(GetTime()) / 1000.0;
    }

    uint32_t Time::GetDeltaTime() {
        return static_cast<uint32_t>(s_deltaTimeSec * 1000.0f);
    }

    double Time::GetDeltaTimeSec() {
        return s_deltaTimeSec / 1000;
    }

    double Time::GetFrameRate() {
        return s_frameRate;
    }

    void Time::Update() {
        s_currentTimeNS = GetTimeNS();
        s_deltaTimeSec = static_cast<double>(s_currentTimeNS - s_lastTimeNS) / 1e9; // ns -> s
        s_frameRate = (s_deltaTimeSec > 0.0) ? 1.0 / s_deltaTimeSec : 0.0;

        Log::Info("Time: {}ns | Last-Time: {}ns | Delta: {}s | FPS: {}",
            s_lastTimeNS, s_lastTimeNS, s_deltaTimeSec, s_frameRate);
        s_lastTimeNS = s_currentTimeNS;
    }

}