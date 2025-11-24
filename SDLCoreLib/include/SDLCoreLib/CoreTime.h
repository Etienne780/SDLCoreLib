#pragma once
#include <cstdint>

namespace SDLCore {

    class Application;

    class Time {
        friend class Application;

    public:
        /*
        * @brief Gets the current number of frames since application instance creation
        * @return Current frame count
        */
        static uint64_t GetFrameCount();

        /**
        * @brief Gets the current time in milliseconds since application instance creation
        * @return Current time in milliseconds
        */
        static uint64_t GetTime();

        /**
        * @brief Gets the current time in Nanoseconds since application instance creation
        * @return Current time in Nanoseconds
        */
        static uint64_t GetTimeNS();

        /**
        * @brief Gets the current time in seconds since application instance creation
        * @return Current time in seconds
        */
        static float GetTimeSec();

        /**
        * @brief Gets the delta time since last frame in milliseconds
        * @return Delta time in milliseconds
        */
        static float GetDeltaTime();

        /**
        * @brief Gets the delta time since last frame in seconds
        * @return Delta time in seconds
        */
        static float GetDeltaTimeSec();

        /**
        * @brief Gets the current frame rate
        * @return Frames per second
        */
        static float GetFrameRate();

        /**
        * @brief Gets the current time in seconds since application instance creation
        * @return Current time in seconds
        */
        static double GetTimeSecDouble();

        /**
        * @brief Gets the delta time since last frame in milliseconds
        * @return Delta time in milliseconds
        */
        static double GetDeltaTimeDouble();

        /**
        * @brief Gets the delta time since last frame in seconds
        * @return Delta time in seconds
        */
        static double GetDeltaTimeSecDouble();

        /**
        * @brief Gets the current frame rate
        * @return Frames per second
        */
        static double GetFrameRateDouble();

    private:
        Time() = delete;

        /**
        * @brief Updates internal timing info, called by Application each frame
        */
        static void Update();
    };

}