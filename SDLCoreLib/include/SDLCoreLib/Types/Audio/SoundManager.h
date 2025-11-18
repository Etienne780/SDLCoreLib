#pragma once
#include <SDL3_mixer/SDL_mixer.h>

namespace SDLCore {
    /*
    |Audio - Type	                    | Duration  | Recommendation	        | Reason
    |------------------------------------------------------------------------------------------------------------
    |SFX (Sound Effects)                | < 2 s     | predecode = true	        | Lowest latency, minimal RAM usage
    |Medium-length Effects	            | 2–10 s    | predecode / stream	    | Consider RAM vs. number of simultaneous instances
    |Background Music / Long Clips      | >10 s     | stream(Decoder)	        | Saves RAM, efficient for large files
    */


    /*
    * plays sounds mixer and Manages tags. static/but not static like application
    * events wenn sound ends
    * manages tag and master Volumn
    */
    class SoundManager { 
    public:

    private:

    };

    /*
    * loads audio. 
    * checks audio length and chooses what decode mode chold be choosen. can also be set manul
    * 
    * length, shouldLoop, Volumn
    */
    class SoundClip {
    friend class SoundManager;
    };

    /*
    importnant funcs

    MIX_Mixer* mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    MIX_Track* track = MIX_CreateTrack(mixer);
    MIX_Audio* sample = MIX_LoadAudio(mixer, "C:/Users/Admin/Downloads/LunaraSounds/sample.mp3", true);

    int reDeCount = 0;
    int plaDeCount = 0;
    SDL_AudioDeviceID* reDe = SDL_GetAudioRecordingDevices(&reDeCount);
    SDL_AudioDeviceID* plaDe = SDL_GetAudioPlaybackDevices(&plaDeCount);


    MIX_SetMasterGain(mixer, 1.0f)

    MIX_SetTrackAudio(track, sample)
    MIX_SetTrackGain(track, 0.25f)
    SDL_PropertiesID propID = MIX_GetAudioProperties(sample);
    MIX_PlayTrack(track, propID)
    MIX_SetTagGain();
    MIX_TagTrack();

    MIX_PlayAudio(mixer, sample)

    MIX_DestroyTrack(track);
    MIX_DestroyAudio(sample);
    MIX_DestroyMixer(mixer);

    */
}