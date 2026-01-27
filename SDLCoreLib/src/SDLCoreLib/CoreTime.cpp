#include <CoreLib/Log.h>
#include <SDL3/SDL.h>
#include "SDLCoreTime.h"

namespace SDLCore {

    static uint64_t s_frameCount = 0;
    static uint64_t s_lastTimeNS = 0;
    static uint64_t s_currentTimeNS = 0;
    static double s_deltaTimeSec = 0.0;
    static double s_frameRateHz = 0.0;

    uint64_t Time::GetFrameCount() {
        return s_frameCount;
    }

    uint64_t Time::GetTimeMS() {
        return SDL_GetTicks();
    }

    uint64_t Time::GetTimeNS() {
        return SDL_GetTicksNS();
    }

    float Time::GetTimeSecF() {
        return static_cast<float>(GetTimeNS()) / SDL_NS_PER_SECOND;
    }

    float Time::GetDeltaTimeMSF() {
        return static_cast<float>(s_deltaTimeSec * SDL_MS_PER_SECOND);
    }

    float Time::GetDeltaTimeSecF() {
        return static_cast<float>(s_deltaTimeSec);
    }

    float Time::GetFrameRateHzF() {
        return static_cast<float>(s_frameRateHz);
    }

    double Time::GetTimeSecD() {
        return static_cast<double>(GetTimeMS()) / SDL_MS_PER_SECOND;
    }
    
    double Time::GetDeltaTimeMSD() {
        return s_deltaTimeSec * SDL_MS_PER_SECOND;
    }

    double Time::GetDeltaTimeSecD() {
        return s_deltaTimeSec;
    }

    double Time::GetFrameRateHzD() {
        return s_frameRateHz;
    }

    void Time::Update() {
        s_currentTimeNS = GetTimeNS();

        if (s_lastTimeNS == 0) {
            s_lastTimeNS = s_currentTimeNS;
            s_deltaTimeSec = 0.0;
            s_frameRateHz = 0.0;
            return;
        }

        s_frameCount++;
        s_deltaTimeSec = static_cast<double>(s_currentTimeNS - s_lastTimeNS) / SDL_NS_PER_SECOND; // ns -> s
        s_frameRateHz = (s_deltaTimeSec > 0.0) ? 1.0 / s_deltaTimeSec : 0.0;
        s_lastTimeNS = s_currentTimeNS;
    }

}