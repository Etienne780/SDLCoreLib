#pragma once
#include "Types/Types.h"

namespace SDLCore {
    /*
    |Audio - Type	                    | Duration  | Recommendation	        | Reason
    |------------------------------------------------------------------------------------------------------------
    |SFX (Sound Effects)                | < 2 s     | predecode = true	        | Lowest latency, minimal RAM usage
    |Medium-length Effects	            | 2–10 s    | predecode / stream	    | Consider RAM vs. number of simultaneous instances
    |Background Music / Long Clips      | >10 s     | stream(Decoder)	        | Saves RAM, efficient for large files
    */

    class SoundManager;

    class SoundClip {
        friend class SoundManager;
    public:
        SoundClip(const SystemFilePath& filePath);
        ~SoundClip();

    private:

    };

}