#pragma once
#include <vector>
#include <SDL3_mixer/SDL_mixer.h>

#include "IDManager.h"
#include "SoundClip.h"
#include "AudioPlaybackDevice.h"

namespace SDLCore {
    class Application;

    namespace SoundTags {
        inline constexpr char* DEFAULT = "default";
    }

    inline constexpr bool SOUND_ON_SHOOT = true;

    /*
    * plays sounds mixer and Manages tags. static/but not static like application
    * events wenn sound ends
    * manages tag and master Volumn
    */
    class SoundManager {
    friend class SoundClip;
    friend class Application;
    public:
        ~SoundManager();

        // ============== Static ==============

        static MIX_Mixer* GetMixer();

        /*
        * @brief sets a new audio playback(headphones) device
        * @param deviceID id of the device. 0 is default systme device
        * @return true on success. Call SDLCore::GetError() for more information
        */
        static bool SetAudioDevice(AudioPlaybackDeviceID deviceID);

        /*
        * Creats (adds) the sound as an audio track to intern storage
        */
        static bool AddSound(const SoundClip& clip, const std::string& tag = SoundTags::DEFAULT);
        /*
        * Deletes the internal stored audio track (stops the sound)
        */
        static bool RemoveSound(const SoundClip& clip);
        static bool RemoveSound(const SoundClipID& id);

        /*
        * tags can be stored in the sound tag. if no tags are set the default tag will be used. start/restarts the sound
        * @return true on success. Call SDLCore::GetError() for more information
        */
        static bool PlaySound(const SoundClip& clip, bool onShoot = false, const std::string& tag = SoundTags::DEFAULT);
        static bool PlayTag(const std::string& tag);

        /*
        * pauses the sound
        */
        static bool PauseSound(const SoundClip& clip);
        static bool PauseAllSounds();
        static bool PauseTag(const std::string& tag);

        /*
        * resumes the paused sound
        */
        static bool ResumeSound(const SoundClip& clip);
        static bool ResumeAllSounds();
        static bool ResumeTag(const std::string& tag);

        /*
        * stops the sound
        */
        static bool StopSound(const SoundClip& clip, Sint64 fadeOutMS = 0);
        static bool StopAllSounds(Sint64 fadeOutMS = 0);
        static bool StopTag(const std::string& tag, Sint64 fadeOutMS = 0);

        static bool IsPlaying(const SoundClip& clip);

        static bool SetMasterVolume(float volume);
        static bool SetTagVolume(const std::string& tag, float volume);

        static bool GetInfo(std::string& outInfo);

    private:
        SoundManager() = default;
        SoundManager(SoundManager&& s) = delete;
        SoundManager operator=(SoundManager & s) = delete;

        IDManager m_trackIDManager;
        struct Audio {
            MIX_Audio* mixAudio = nullptr;
            AudioTrackID audioTrackID{ SDLCORE_INVALID_ID };
            bool fireAndForget = false;
            size_t refCount = 0;// if refCount == 0; object can be deleted

            void IncreaseRefCount() {
                refCount++;
            }

            void DecreaseRefCount() {
                refCount--;
                if (refCount < 0)
                    refCount = 0;
            }

            Audio() = default;
            Audio(MIX_Audio* audio)
                : mixAudio(audio) {}
        };

        struct AudioTrack {
            MIX_Track* track = nullptr;
            float durationMS = 0.0f;
            Sint64 frameCount = 0;
            int frequency = 0;

            float volume = 1.0f;
            Vector2 position;

            std::string tag;
            bool isDeleted = false;
            bool isPlaying = false;

            AudioTrack() = default;
            AudioTrack(MIX_Track* _track, const SoundClip& _clip, const std::string& _tag) 
                : track(_track), durationMS(_clip.GetDurationMS()), 
                frameCount(_clip.GetNumberOfFrames()), frequency(_clip.GetFrequency()), tag(_tag) {}
        };

        // ============== Static ==============

        static bool Init(SDL_AudioDeviceID audio = SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK);
        static void Quit();

        /**
        * @brief Checks if a current valid instance of this class exist
        * @return true if valid instance exists. Call SDLCore::GetError() for more information 
        */
        static bool InstanceExist();

        static bool AddSoundRef(SoundClipID id);
        static bool ReleaseSoundRef(SoundClipID id);

        /*
        * @brief applys params like volume and pitch of clip to its corresponding audiotrack
        */
        static bool ApplyClipParams(const SoundClip& clip);
        static bool ApplyClipParams(AudioTrack* audioTrack, const SoundClip& clip);

        /*
        * gets only called internaly from SoundClip class
        * takes ownership of the given audio
        */
        static bool CreateSound(SoundClipID id, MIX_Audio* audio);

        /*
        * gets only called internaly from SoundClip class
        * Marks audio as deleted and delets it when its done
        */
        static bool DeleteSound(SoundClipID id);

        // ============== Member ==============
        MIX_Mixer* m_mixer = nullptr;
        std::unordered_map<AudioTrackID, AudioTrack> m_audioTracks;// uses audio from m_audios to play sounds
        std::unordered_map<SoundClipID, Audio> m_audios;// audio gets added from clip
        std::vector<AudioPlaybackDevice> m_devices;
        IDManager m_deviceIDManager{ 1 };

        void Cleanup();

        /*
        * @brief Creates a new list of AudioPlaybackDevice and deletes the old one
        * @return true on success. Call SDLCore::GetError() for more information
        */
        bool CreateDevices();

        AudioTrack* GetAudioTrack(SoundClipID id);
        AudioTrack* GetAudioTrack(AudioTrackID id);
        Audio* GetAudio(SoundClipID id);

        bool TryGetMixer(MIX_Mixer*& mixer, const std::string& func);
        bool CreateAudioTrack(AudioTrack*& audioTrack, const SoundClip& clip, const std::string& tag);
        void MarkTrackAsDeleted(AudioTrack* audioTrack);
        void OnTrackStopped(MIX_Track* track);
    };

}