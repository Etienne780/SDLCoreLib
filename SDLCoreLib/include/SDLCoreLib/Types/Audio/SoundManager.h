#pragma once
#include <vector>
#include <SDL3_mixer/SDL_mixer.h>

#include "IDManager.h"
#include "AudioPlaybackDevice.h"

namespace SDLCore {
    class Appliaction;

    /*
    * plays sounds mixer and Manages tags. static/but not static like application
    * events wenn sound ends
    * manages tag and master Volumn
    */
    class SoundManager {
        friend class Appliaction;
    public:
        /*
        * @brief sets a new audio playback(headphones) device
        * @param deviceID id of the device. 0 is default systme device
        */
        bool SetAudioDevice(AudioPlaybackDeviceID deviceID);

    private:
        SoundManager() = default;
        ~SoundManager();
        SoundManager(SoundManager&& s) = delete;
        SoundManager operator=(SoundManager & s) = delete;

        static inline MIX_Mixer* m_mixer = nullptr;
        static inline std::vector<AudioPlaybackDevice> m_devices;
        static inline IDManager m_deviceIDManager{ 1 };

        static bool Init();
        static void Quit();
        static void Cleanup();

        /*
        * @return true on success. Call SDLCore::GetError() for more information
        */
        static bool SetDevices();
    };

    /*
    * loads audio. 
    * checks audio length and chooses what decode mode chold be choosen. can also be set manul
    * 
    * length, shouldLoop, Volumn
    */
    // class SoundClip {
    // friend class SoundManager;
    // };

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