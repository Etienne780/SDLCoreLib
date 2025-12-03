#include <memory>
#include <CoreLib/Log.h>

#include "Application.h"
#include "SDLCoreError.h"
#include "Types/Audio/SoundClip.h"
#include "Types/Audio/SoundManager.h"

namespace SDLCore {
	
	static std::unique_ptr<SoundManager> s_soundManager = nullptr;

	SoundManager::~SoundManager() {
		Cleanup();
	}

	#pragma region Static

	bool SoundManager::Init(SDL_AudioDeviceID audio) {
		// Quit current SoundManager if exist
		Quit();

		s_soundManager = std::unique_ptr<SoundManager>(new SoundManager());
		s_soundManager->m_mixer = MIX_CreateMixerDevice(audio, nullptr);
		if (!s_soundManager->m_mixer) {
			SetError("SDLCore::SoundManager::Init: Faild to create mixer! " + std::string(SDL_GetError()));
			return false;
		}

		if (!SetMasterVolume(1.0f))
			return false;

		if (!s_soundManager->CreateDevices())
			return false;

		return true;
	}

	void SoundManager::Quit() {
		if (s_soundManager) {
			s_soundManager->Cleanup();
			s_soundManager = nullptr;
		}
	}

	bool SoundManager::InstanceExist() {
		if (!s_soundManager) {
			SetError("SDLCore::SoundManager::InstanceExist: No valid instance of SoundManager exists!");
			return false;
		}

		return true;
	}

	bool SoundManager::AddSoundRef(SoundClipID id) {
		if (!InstanceExist())
			return false;

		if (id == SDLCORE_INVALID_ID) {
			SetError("SDLCore::SoundManager::AddSoundRef: id was invalid!");
			return false;
		}

		Audio* audio = s_soundManager->GetAudio(id);
		if (!audio)
			return true;

		audio->IncreaseRefCount();
		return true;
	}

	bool SoundManager::ReleaseSoundRef(SoundClipID id) {
		if (!InstanceExist())
			return false;

		if (id == SDLCORE_INVALID_ID) {
			SetError("SDLCore::SoundManager::ReleaseSoundRef: id was invalid!");
			return false;
		}

		Audio* audio = s_soundManager->GetAudio(id);
		if (!audio)
			return true;

		audio->DecreaseRefCount();
		return true;
	}

	bool SoundManager::ApplyClipParams(const SoundClip& clip) {
		if (!InstanceExist())
			return false;

		AudioTrack* audioTrack = s_soundManager->GetAudioTrack(clip.GetID(), clip.GetSubID());
		return ApplyClipParams(audioTrack, clip);
	}

	bool SoundManager::ApplyClipParams(AudioTrack* audioTrack, const SoundClip& clip) {
		if (!InstanceExist())
			return false;

		if (!audioTrack) {
			SetErrorF("SDLCore::SoundManager::ApplyClipParams: Could not apply clip '{}' params, AudioTrack is nullptr!", clip.GetID());
			return false;
		}

		MIX_Track* track = audioTrack->track;
		if (!track) {
			SetErrorF("SDLCore::SoundManager::ApplyClipParams: AudioTrack '{}' has invalid MIX_Track (nullptr)!", clip.GetID());
			return false;
		}

		float newVolume = clip.GetVolume();
		Vector2 pos = clip.GetPosition();
		MIX_Point3D mixPoint{ pos.x, 0.0f, pos.y };

		bool result = true;

		if (audioTrack->volume != newVolume) {
			audioTrack->volume = newVolume;
			if (!MIX_SetTrackGain(track, newVolume)) {
				SetErrorF("SDLCore::SoundManager::ApplyClipParams: Failed to set volume for clip '{}': {}", clip.GetID(), SDL_GetError());
				result = false;
			}
		}

		if (audioTrack->position != pos) {
			audioTrack->position = pos;
			if (!MIX_SetTrack3DPosition(track, &mixPoint)) {
				AddErrorF("SDLCore::SoundManager::ApplyClipParams: Failed to set position for clip '{}': {}", clip.GetID(), SDL_GetError());
				result = false;
			}
		}

		return result;
	}

	bool SoundManager::CreateSound(SoundClipID id, MIX_Audio* audio) {
		if (!InstanceExist())
			return false;

		if (!audio) {
			SetError("SDLCore::SoundManager::CreateSound: Could not create audio, audio was nullptr");
			return false;
		}

		auto& audios = s_soundManager->m_audios;
		auto it = audios.find(id);

		if (it != audios.end()) {
			// delets old element
			auto& a = it->second;
			AudioTrack* track = s_soundManager->GetAudioTrack(a.audioTrackID);
			s_soundManager->MarkTrackAsDeleted(track, a.audioTrackID);

			MIX_DestroyAudio(a.mixAudio);
		}

		audios[id] = Audio(audio);
		audios[id].IncreaseRefCount();
		return true;
	}

	bool SoundManager::DeleteSound(const SoundClip& clip) {
		if (!InstanceExist())
			return false;

		auto& audios = s_soundManager->m_audios;
		auto it = audios.find(clip.GetID());
		if (it == audios.end())
			return true;

		auto& audio = it->second;
		if (clip.IsSubSound()) {
			auto subIt = audio.subSounds.find(clip.GetSubID());
			if (subIt != audio.subSounds.end()) {
				Audio& subAu = subIt->second;
				subAu.DecreaseRefCount();

				if (subAu.refCount <= 0) {
					AudioTrack* track = s_soundManager->GetAudioTrack(subAu.audioTrackID);
					s_soundManager->MarkTrackAsDeleted(track, subAu.audioTrackID);
					audio.subSounds.erase(subIt);
				}
			}
		}

		audio.DecreaseRefCount();
		// delete if there are no refs to this audio
		if (audio.refCount <= 0) {
			AudioTrack* track = s_soundManager->GetAudioTrack(audio.audioTrackID);
			s_soundManager->MarkTrackAsDeleted(track, audio.audioTrackID);

			MIX_DestroyAudio(audio.mixAudio);
			audios.erase(it);
		}
		return true;
	}

	MIX_Mixer* SoundManager::GetMixer() {
		if (!InstanceExist())
			return nullptr;
		if (!s_soundManager->m_mixer)
			SetError("SDLCore::SoundManager::GetMixer: Mixer of sound manager is nullptr!");
		return s_soundManager->m_mixer;
	}

	bool SoundManager::SetAudioDevice(AudioPlaybackDeviceID deviceID) {
		if (!InstanceExist())
			return false;

		Log::Warn("SDLCore::SoundManager::SetAudioDevice: Set audio device cancels all sounds currently playing");

		bool result = true;
		SDL_AudioDeviceID targetSDLID = SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK;
		auto& devices = s_soundManager->m_devices;

		s_soundManager->Cleanup();

		if (deviceID != 0) {
			auto it = std::find_if(devices.begin(), devices.end(),
				[deviceID](const AudioPlaybackDevice& dev) { return dev.GetID() == deviceID; });

			if (it != devices.end()) {
				targetSDLID = it->GetSDLID();
			}
			else {
				SetErrorF("SDLCore::SoundManager::SetAudioDevice: Device with id '{}' not found! Using default device", deviceID);
				result = false;
			}
		}

		s_soundManager->m_mixer = MIX_CreateMixerDevice(targetSDLID, nullptr);

		if (!s_soundManager->m_mixer) {
			const std::string err = "SDLCore::SoundManager::SetAudioDevice: " + std::string(SDL_GetError());

			if (result)
				SetError(err);
			else
				AddError(err);

			result = false;
		}
		return result;
	}

	bool SoundManager::RemoveSound(const SoundClip& clip) {
		if (!InstanceExist())
			return false;

		return RemoveSound(clip.GetID());
	}

	bool SoundManager::RemoveSound(const SoundClipID& id) {
		if (!InstanceExist())
			return false;

		Audio* audio = s_soundManager->GetAudio(id);
		if (!audio)
			return false;

		AudioTrack* track = s_soundManager->GetAudioTrack(audio->audioTrackID);
		if (!track)
			return true;
		// deletes the track immediately
		track->isDeleted = true;
		s_soundManager->OnTrackStopped(audio->audioTrackID);
		return true;
	}

	bool SoundManager::PlaySound(const SoundClip& clip, bool onShoot, const std::string& tag) {
		if (!InstanceExist())
			return false;

		AudioTrack* outAudioTrack = nullptr;
		if (!onShoot) {
			outAudioTrack = s_soundManager->GetAudioTrack(clip.GetID(), clip.GetSubID());

			// create audio track if it was not found
			if (!outAudioTrack) {
				if (!s_soundManager->CreateAudioTrack(outAudioTrack, clip, tag)) {
					return false;
				}
			}
		}
		else {
			// onShoot == true
			if (!s_soundManager->CreateAudioTrack(outAudioTrack, clip, tag)) {
				return false;
			}

			if (!outAudioTrack) {
				SetErrorF("SDLCore::SoundManager::PlaySound: Could not create AudioTrack for on shoot audio '{}'!", clip.GetID());
				return false;
			}

			outAudioTrack->isDeleted = true;
		}

		// dosent have a null check because CreateAudioTrack would return false if track could be created and it would also not be stored in the map
		MIX_Track* track = outAudioTrack->track;

		if (!MIX_PlayTrack(track, 0)) {
			SetError("SDLCore::SoundManager::PlaySound: Could not play sound!\n" + std::string(SDL_GetError()));
			return false;
		}

		outAudioTrack->isPlaying = true;
		return true;
	}

	bool SoundManager::PlayTag(const std::string& tag) {
		if (!InstanceExist())
			return false;

		MIX_Mixer* mixer = nullptr;
		if (!s_soundManager->TryGetMixer(mixer, "PlayTag"))
			return false;

		if (!MIX_PlayTag(mixer, tag.c_str(), 0)) {
			SetErrorF("SDLCore::SoundManager::PlayTag: Faild to play tag '{}'!\n{}", tag, SDL_GetError());
			return false;
		}

		return true;
	}

	bool SoundManager::PauseSound(const SoundClip& clip) {
		if (!InstanceExist())
			return false;

		AudioTrack* audioTrack = s_soundManager->GetAudioTrack(clip.GetID(), clip.GetSubID());
		if (!audioTrack) {
			SetErrorF("SDLCore::SoundManager::PauseSound: Could not pause Sound '{}', audio track of sound was not found!", clip.GetID());
			return false;
		}
		MIX_Track* track = audioTrack->track;

		if (!MIX_PauseTrack(track)) {
			SetErrorF("SDLCore::SoundManager::PauseSound: Could not pause sound '{}'!\n{}", clip.GetID(), SDL_GetError());
			return false;
		}

		return true;
	}

	bool SoundManager::PauseAllSounds() {
		if (!InstanceExist())
			return false;

		MIX_Mixer* mixer = nullptr;
		if (!s_soundManager->TryGetMixer(mixer, "PauseAllSounds"))
			return false;

		if (!MIX_PauseAllTracks(mixer)) {
			SetError("SDLCore::SoundManager::PauseAllSounds: Could not Pause all sounds!\n" + std::string(SDL_GetError()));
			return false;
		}
		return true;
	}

	bool SoundManager::PauseTag(const std::string& tag) {
		if (!InstanceExist())
			return false;

		MIX_Mixer* mixer = nullptr;
		if (!s_soundManager->TryGetMixer(mixer, "PauseTag"))
			return false;

#ifndef NDEBUG
		if (tag.empty()) {
			Log::Warn("SDLCore::SoundManager::PauseTag: The given tag is empty!");
		}
#endif
		
		if(!MIX_PauseTag(mixer, tag.c_str())) {
			return false;
		}
		return true;
	}

	bool SoundManager::ResumeSound(const SoundClip& clip) {
		if (!InstanceExist())
			return false;

		AudioTrack* audioTrack = s_soundManager->GetAudioTrack(clip.GetID(), clip.GetSubID());
		if (!audioTrack) {
			SetErrorF("SDLCore::SoundManager::ResumeSound: Could not resume Sound '{}', audio track of sound was not found!", clip.GetID());
			return false;
		}
		MIX_Track* track = audioTrack->track;

		if (!MIX_ResumeTrack(track)) {
			SetErrorF("SDLCore::SoundManager::ResumeSound: Could not resume sound '{}'!\n{}", clip.GetID(), SDL_GetError());
			return false;
		}
		return true;
	}

	bool SoundManager::ResumeAllSounds() {
		if (!InstanceExist())
			return false;

		MIX_Mixer* mixer = nullptr;
		if (!s_soundManager->TryGetMixer(mixer, "ResumeAllSounds"))
			return false;

		if (!MIX_ResumeAllTracks(mixer)) {
			SetError("SDLCore::SoundManager::ResumeAllSounds: Could not Resume all sounds!\n" + std::string(SDL_GetError()));
			return false;
		}
		return true;
	}

	bool SoundManager::ResumeTag(const std::string& tag) {
		if (!InstanceExist())
			return false;

		MIX_Mixer* mixer = nullptr;
		if (!s_soundManager->TryGetMixer(mixer, "ResumeTag"))
			return false;

#ifndef NDEBUG
		if (tag.empty()) {
			Log::Warn("SDLCore::SoundManager::PauseTag: The given tag is empty!");
		}
#endif

		if (!MIX_ResumeTag(mixer, tag.c_str())) {
			SetErrorF("SDLCore::SoundManager::ResumeTag: Could not resume tag '{}'!\n{}", tag, SDL_GetError());
			return false;
		}
		return true;
	}


	bool SoundManager::StopSound(const SoundClip& clip, Sint64 fadeOutMS) {
		if (!InstanceExist())
			return false;

		AudioTrack* audioTrack = s_soundManager->GetAudioTrack(clip.GetID(), clip.GetSubID());
		if (!audioTrack) {
			SetErrorF("SDLCore::SoundManager::StopSound: Could not stop Sound '{}', audio track of sound was not found!", clip.GetID());
			return false;
		}
		MIX_Track* track = audioTrack->track;

		Sint64 fadeOutFrames = MIX_TrackMSToFrames(track, fadeOutMS);
		if (!MIX_StopTrack(track, fadeOutFrames)) {
			return false;
		}
		return true;
	}

	bool SoundManager::StopAllSounds(Sint64 fadeOutMS) {
		if (!InstanceExist())
			return false;

		MIX_Mixer* mixer = nullptr;
		if (!s_soundManager->TryGetMixer(mixer, "StopAllSounds"))
			return false;


		if (fadeOutMS == 0) {
			if (!MIX_StopAllTracks(mixer, 0)) {
				SetError("SDLCore::SoundManager::StopAllSounds: Could stop all sounds!\n" + std::string(SDL_GetError()));
				return false;
			}
		}
		else {
			bool failed = false;
			for (auto& [id, audioTrack] : s_soundManager->m_audioTracks) {
				if (!audioTrack.track)
					continue;

				Sint64 fadeOutFrames = MIX_TrackMSToFrames(audioTrack.track, fadeOutMS);
				if (!MIX_StopTrack(audioTrack.track, fadeOutFrames)) {
					if (!failed) {
						SetError("SDLCore::SoundManager::StopTag: Could not stop audio:");
					}

					AddErrorF("\n-	id '{}', tag '{}'!", id, audioTrack.tag);
					failed = true;
					continue;
				}
			}
			// add sdl error if one track failed
			if (failed) {
				AddError("\n" + std::string(SDL_GetError()));
			}
		}
		return true;
	}

	bool SoundManager::StopTag(const std::string& tag, Sint64 fadeOutMS) {
		if (!InstanceExist())
			return false;

		MIX_Mixer* mixer = nullptr;
		if (!s_soundManager->TryGetMixer(mixer, "StopTag"))
			return false;
		
		// if no fade out use intern sdl func
		if (fadeOutMS == 0) {
			if (!MIX_StopTag(mixer, tag.c_str(), 0)) {
				SetErrorF("SDLCore::SoundManager::StopTag: Could not stop tag '{}'!\n", tag, SDL_GetError());
				return false;
			}
		}
		else {
			bool failed = false;
			for (auto& [id, audioTrack] : s_soundManager->m_audioTracks) {
				if (audioTrack.tag != tag || !audioTrack.track)
					continue;

				Sint64 fadeOutFrames = MIX_TrackMSToFrames(audioTrack.track, fadeOutMS);
				if (!MIX_StopTrack(audioTrack.track, fadeOutFrames)) {
					if (!failed) {
						SetError("SDLCore::SoundManager::StopTag: Could not stop audio:");
					}

					AddErrorF("\n-	id '{}', tag '{}'!", id, tag);
					failed = true;
					continue;
				}
			}
			// add sdl error if one track failed
			if (failed) {
				AddError("\n" + std::string(SDL_GetError()));
			}
		}
		return true;
	}

	bool SoundManager::IsPlaying(const SoundClip& clip) {
		if (!InstanceExist())
			return false;

		AudioTrack* audioTrack = s_soundManager->GetAudioTrack(clip.GetID(), clip.GetSubID());
		if (!audioTrack)
			return false;

		return audioTrack->isPlaying;
	}

	bool SoundManager::SetMasterVolume(float volume) {
		if (!InstanceExist())
			return false;

		MIX_Mixer* mixer = nullptr;
		if (!s_soundManager->TryGetMixer(mixer, "SetMasterVolume"))
			return false;

		if (!MIX_SetMasterGain(mixer, volume)) {
			SetErrorF("SDLCore::SoundManager::SetMasterVolume: Could not set master volume '{}'!\n{}", volume, SDL_GetError());
			return false;
		}
		return true;
	}

	bool SoundManager::SetTagVolume(const std::string& tag, float volume) {
		if (!InstanceExist())
			return false;

		MIX_Mixer* mixer = nullptr;
		if (!s_soundManager->TryGetMixer(mixer, "SetTagVolume"))
			return false;

		if (!MIX_SetTagGain(mixer, tag.c_str(), volume)) {
			SetErrorF("SDLCore::SoundManager::SetTagVolume: Could not set tag '{}' volume '{}'!\n{}", 
				tag, volume, SDL_GetError());
			return false;
		}
		return true;
	}

	bool SoundManager::GetInfo(std::string& outInfo) {
		if (!InstanceExist())
			return false;

		std::stringstream ss;
		ss << "=== SoundManager Info ===\n";

		ss << "Mixer: " << (s_soundManager->m_mixer ? "Valid" : "Null") << "\n";
		ss << "Loaded Audios: " << s_soundManager->m_audios.size() << "\n";
		for (const auto& [id, audio] : s_soundManager->m_audios) {
			ss << "  Audio ID: " << id.value
				<< ", MIX_Audio: " << audio.mixAudio
				<< ", TrackID: " << audio.audioTrackID.value
				<< "\n";
		}

		ss << "Active Audio Tracks: " << s_soundManager->m_audioTracks.size() << "\n";
		for (const auto& [id, track] : s_soundManager->m_audioTracks) {
			ss << "  Track ID: " << id.value
				<< ", MIX_Track: " << track.track
				<< ", Tag: " << track.tag
				<< ", DurationMS: " << track.durationMS
				<< ", FrameCount: " << track.frameCount
				<< ", Frequency: " << track.frequency
				<< ", IsDeleted: " << (track.isDeleted ? "Yes" : "No")
				<< "\n";
		}

		ss << "Audio Devices: " << s_soundManager->m_devices.size() << "\n";
		for (const auto& dev : s_soundManager->m_devices) {
			ss << "  Device ID: " << dev.GetID().value
				<< ", SDL DeviceID: " << dev.GetSDLID()
				<< "\n";
		}

		outInfo = ss.str();
		return true;
	}

	#pragma endregion

	#pragma region Member

	void SoundManager::Cleanup() {
		if (!Application::IsQuit()) {
			for (auto& [id, audioTrack] : m_audioTracks) {
				OnTrackStopped(id);
			}
			MIX_DestroyMixer(m_mixer);
		}
		m_audioTracks.clear();
	}

	bool SoundManager::CreateDevices() {
		int count = 0;
		SDL_AudioDeviceID* pDevice = SDL_GetAudioPlaybackDevices(&count);
		
		if (!pDevice) {
			SetError("SDLCore::SoundManager: " + std::string(SDL_GetError()));
			SDL_free(pDevice);
			return false;
		}
		
		m_deviceIDManager.Reset();
		m_devices.clear();
		m_devices.reserve(count);

		for (int i = 0; i < count; i++) {
			m_devices.emplace_back(
				AudioPlaybackDeviceID(m_deviceIDManager.GetNewUniqueIdentifier()),
				pDevice[i]
			);
		}

		SDL_free(pDevice);
		return true;
	}

	SoundManager::AudioTrack* SoundManager::GetAudioTrack(SoundClipID id, SoundClipID subID) {
		Audio* audio = GetAudio(id);
		if (!audio) {
			SetErrorF("SDLCore::SoundManager::GetAudioTrack(SoundClipID): Could not get track, audio ID '{}', audio was nullptr!", id);
			return nullptr;
		}
		AudioTrackID trackID = audio->audioTrackID;

		// if a sub sound is set
		if (subID != SDLCORE_INVALID_ID) {
			auto& subSounds = audio->subSounds;
			auto it = subSounds.find(subID);
			// if AudioTrack of sub sound not found return null
			if (it == subSounds.end())
				return nullptr;
			// set track id to sub sound track id
			trackID = it->second.audioTrackID;
		}

		return GetAudioTrack(trackID);
	}

	SoundManager::AudioTrack* SoundManager::GetAudioTrack(AudioTrackID id) {
		if (id == SDLCORE_INVALID_ID)
			return nullptr;

		auto it = m_audioTracks.find(id);
		if (it == m_audioTracks.end())
			return nullptr;

		return &it->second;
	}

	SoundManager::Audio* SoundManager::GetAudio(SoundClipID id) {
		if (id == SDLCORE_INVALID_ID)
			return nullptr;

		auto it = m_audios.find(id);
		if (it == m_audios.end())
			return nullptr;

		return &it->second;
	}

	bool SoundManager::TryGetMixer(MIX_Mixer*& mixer, const std::string& func) {
		if (!m_mixer) {
			mixer = nullptr;
			SetErrorF("SDLCore::SoundManager::{}: Mixer of the current sound manager is nullptr!", func);
			return false;
		}
		mixer = m_mixer;
		return true;
	}

	bool SoundManager::CreateAudioTrack(AudioTrack*& audioTrack, const SoundClip& clip, const std::string& tag) {
		if (!m_mixer) {
			SetError("SDLCore::SoundManager::CreateAudioTrack: Mixer is nullptr!");
			return false;
		}

		MIX_Track* track = MIX_CreateTrack(m_mixer);
		if (!track) {
			SetErrorF("SDLCore::SoundManager::CreateAudioTrack: Could not create track for sound '{}'!\n{}", clip.GetID(), SDL_GetError());
			audioTrack = nullptr;
			return false;
		}

		Audio* audio = GetAudio(clip.GetID());
		if (!audio) {
			SetErrorF("SDLCore::SoundManager::CreateAudioTrack: The audio of the given sound clip '{}' was nullptr!", clip.GetID());
			MIX_DestroyTrack(track);
			audioTrack = nullptr;
			return false;
		}

		if (!audio->mixAudio) {
			SetErrorF("SDLCore::SoundManager::CreateAudioTrack: The mix audio (SDL_mixer) of the given sound clip '{}' was nullptr!", clip.GetID());
			MIX_DestroyTrack(track);
			audioTrack = nullptr;
			return false;
		}

		if (!MIX_SetTrackAudio(track, audio->mixAudio)) {
			SetErrorF("SDLCore::SoundManager::CreateAudioTrack: Could not set the given clip '{}' to audio track!\n{}", clip.GetID(), SDL_GetError());
			MIX_DestroyTrack(track);
			audioTrack = nullptr;
			return false;
		}

		const char* c_tag = tag.empty() ? SoundTags::DEFAULT : tag.c_str();
		if (tag.empty()) {
			Log::Warn("SDLCore::SoundManager::CreateAudioTrack: The given tag for sound '{}' was empty, using default tag!", clip.GetID());
		}

		if (!MIX_TagTrack(track, c_tag)) {
			SetErrorF("SDLCore::SoundManager::CreateAudioTrack: Could not set tag for audio '{}'!\n{}", clip.GetID(), SDL_GetError());
			MIX_DestroyTrack(track);
			audioTrack = nullptr;
			return false;
		}

		AudioTrackID newID = AudioTrackID(m_trackIDManager.GetNewUniqueIdentifier());

		if (clip.IsSubSound()) {
			auto& subSound = audio->subSounds;
			auto it = subSound.find(clip.GetSubID());
			if (it != subSound.end()) {
				Audio& a = it->second;
				AudioTrack* t = GetAudioTrack(a.audioTrackID);
				MarkTrackAsDeleted(t, a.audioTrackID);
			}
			Audio subAudio;
			subAudio.audioTrackID = newID;
			subAudio.IncreaseRefCount();
			subSound[clip.GetSubID()] = subAudio;
		}
		else {
			// mark old track with this audio as deleted
			AudioTrack* t = GetAudioTrack(audio->audioTrackID);
			MarkTrackAsDeleted(t, audio->audioTrackID);
			// set the id of the new track
			audio->audioTrackID = newID;
		}

		AudioTrack at{ track, clip, tag };
		m_audioTracks.emplace(newID, at);
		audioTrack = &m_audioTracks.at(newID);

		struct TrackCallbackData {
			SoundManager* manager;
			AudioTrackID  trackID;
		};

		auto* cbData = new TrackCallbackData{ this, newID };
		MIX_SetTrackStoppedCallback(track,
			[](void* u, MIX_Track* t) {
				auto* data = static_cast<TrackCallbackData*>(u);

				if (data->manager && SoundManager::InstanceExist()) {
					data->manager->OnTrackStopped(data->trackID);
				}

				// cleanup
				delete data;
			},
			cbData
		);

		ApplyClipParams(audioTrack, clip);
		return true;
	}

	void SoundManager::MarkTrackAsDeleted(AudioTrack* audioTrack, AudioTrackID id) {
		if (!audioTrack)
			return;

		audioTrack->isDeleted = true;
		MIX_Track* track = audioTrack->track;
		if (track) {
			if (!MIX_TrackPlaying(track) && !MIX_TrackPaused(track)) {
				OnTrackStopped(id);
			}
		}
	}

	void SoundManager::OnTrackStopped(AudioTrackID id) {
		auto it = m_audioTracks.find(id);
		if (it == m_audioTracks.end())
			return;

		it->second.isPlaying = false;
		if (it->second.isDeleted) {
			m_trackIDManager.FreeUniqueIdentifier(it->first.value);
			MIX_DestroyTrack(it->second.track);
			m_audioTracks.erase(it);
		}
	}

	#pragma endregion
}