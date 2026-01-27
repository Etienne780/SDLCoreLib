#include <CoreLib/Log.h>
#include <SDL3/SDL.h>
#include "SDLCoreTime.h"

namespace SDLCore {

    static uint64_t s_frameCount = 0;
    static uint64_t s_lastTimeNS = 0;
    static uint64_t s_currentTimeNS = 0;
    static double s_deltaTimeSec = 0.0;
    static double s_frameRate = 0.0;

    uint64_t Time::GetFrameCount() {
        return s_frameCount;
    }

    uint64_t Time::GetTimeMS() {
        return SDL_GetTicks();
    }

    uint64_t Time::GetTimeNS() {
        return SDL_GetTicksNS();
    }

    float Time::GetTimeSec() {
        return static_cast<float>(GetTimeMS()) / 1000.0f;
    }

    float Time::GetDeltaMSTime() {
        return static_cast<float>(s_deltaTimeSec * 1000.0);
    }

    float Time::GetDeltaTimeSec() {
        return static_cast<float>(s_deltaTimeSec);
    }

    float Time::GetFrameRate() {
        return static_cast<float>(s_frameRate);
    }

    double Time::GetTimeSecDouble() {
        return static_cast<double>(GetTimeMS()) / 1000.0;
    }
    
    double Time::GetDeltaTimeMSDouble() {
        return s_deltaTimeSec * 1000.0;
    }

    double Time::GetDeltaTimeSecDouble() {
        return s_deltaTimeSec;
    }

    double Time::GetFrameRateDouble() {
        return s_frameRate;
    }

    void Time::Update() {
        s_frameCount++;
        s_currentTimeNS = GetTimeNS();
        s_deltaTimeSec = static_cast<double>(s_currentTimeNS - s_lastTimeNS) / SDL_NS_PER_SECOND; // ns -> s
        s_frameRate = (s_deltaTimeSec > 0.0) ? 1.0 / s_deltaTimeSec : 0.0;
        s_lastTimeNS = s_currentTimeNS;
    }

}