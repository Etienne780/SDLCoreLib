#pragma once
#include <filesystem>
#include <SDL3/SDL.h>
#include <CoreLib/Log.h>
#include <CoreLib/File.h>
#include <CoreLib/FormatUtils.h>
#include <CoreLib/CoreMath.h>
#include <CoreLib/IDManager.h>
#include <CoreLib/CoreID.h>

inline constexpr uint32_t SDLCORE_INVALID_ID = std::numeric_limits<uint32_t>::max();
inline constexpr int APPLICATION_FPS_UNCAPPED = 0;
inline constexpr int APPLICATION_FPS_VSYNC_ON = -1;
inline constexpr int APPLICATION_FPS_VSYNC_ADAPTIVE_ON = -2;

namespace SDLCore {

	typedef int SDLResult;
	typedef SDL_Rect Rect;/**< Rect with int components (SDL_Rect) */
	typedef SDL_FRect FRect;/**< Rect with float components (SDL_FRect) */
	typedef SDL_Color Color;/**< Color with uint8 components (SDL_Color) */
	typedef SDL_FColor FColor;/**< Color with float components (SDL_FColor) */
	typedef SDL_DisplayID DisplayID;/**< Uint32 id for an display (SDL_DisplayID) */

	Rect ToRect(const FRect& fRect);
	Rect ToRect(const Vector4& fRect);

	FRect ToFRect(const Rect& rect);
	FRect ToFRect(const Vector4& rect);

	using SDLCoreIDManager = IDManager<uint32_t, SDLCORE_INVALID_ID>;

	struct WindowTag {};
	struct WindowCallbackTag {};
	struct AudioPlaybackDeviceTag {};
	struct AudioClipTag {};
	struct AudioTrackTag {};
	struct TextureTag {};

	
	template<typename Tag>
	using SDLCoreID = CoreID<uint32_t, SDLCORE_INVALID_ID, Tag>;

	/**
	* @brief Identifier for a window.
	*        Internally stored as an uint32_t
	*/
	using WindowID = SDLCoreID<WindowTag>;

	/**
	* @brief Identifier for a window callback.
	*        Internally stored as an uint32_t
	*/
	using WindowCallbackID = SDLCoreID<WindowCallbackTag>;

	/**
	* @brief Identifier for an audio playback device.
	*        Internally stored as an uint32_t
	*/
	using AudioPlaybackDeviceID = SDLCoreID<AudioPlaybackDeviceTag>;

	/**
	* @brief Identifier for an audio clip.
	*        Internally stored as an uint32_t
	*/
	using SoundClipID = SDLCoreID<AudioClipTag>;

	/**
	* @brief Identifier for an audio track.
	*        Internally stored as an uint32_t
	*/
	using AudioTrackID = SDLCoreID<AudioTrackTag>;

	/**
	* @brief Identifier for a texture (used internally).
	*        Internally stored as an uint32_t
	*/
	using TextureID = SDLCoreID<TextureTag>;

	enum class TextureParams : int {
		NONE		= 0,
		ROTATION	= 1 << 0,
		CENTER		= 1 << 1,
		COLOR_TINT	= 1 << 2,
		FLIP		= 1 << 3,
		SCALE_MODE	= 1 << 4,
		TYPE		= 1 << 5
	};

	TextureParams operator|(TextureParams a, TextureParams b);
	TextureParams operator&(TextureParams a, TextureParams b);
	TextureParams& operator|=(TextureParams& a, TextureParams b);
	TextureParams& operator&=(TextureParams& a, TextureParams b);
	bool operator!(TextureParams a);

	enum class Align {
		NONE = 0,
		START,
		CENTER,
		END
	};

	enum class UnitType {
		NONE = 0,
		CHARACTERS,
		PIXELS
	};

	enum class Platform {
		UNKOWN = 0,
		WINDOWS,
		MAC_OS,
		LINUX,
		IOS,
		ANDROID
	};

}

template<>
static inline std::string FormatUtils::toString<SDLCore::SDLCoreID<SDLCore::WindowTag>>(SDLCore::WindowID id) {
	return id.ToString();
}

template<>
static inline std::string FormatUtils::toString<SDLCore::SDLCoreID<SDLCore::WindowCallbackTag>>(SDLCore::WindowCallbackID id) {
	return id.ToString();
}

template<>
static inline std::string FormatUtils::toString<SDLCore::SDLCoreID<SDLCore::AudioPlaybackDeviceTag>>(SDLCore::AudioPlaybackDeviceID id) {
	return id.ToString();
}

template<>
static inline std::string FormatUtils::toString<SDLCore::SDLCoreID<SDLCore::AudioClipTag>>(SDLCore::SoundClipID id) {
	return id.ToString();
}

template<>
static inline std::string FormatUtils::toString<SDLCore::SDLCoreID<SDLCore::AudioTrackTag>>(SDLCore::AudioTrackID id) {
	return id.ToString();
}

template<>
static inline std::string FormatUtils::toString<SDLCore::SDLCoreID<SDLCore::TextureTag>>(SDLCore::TextureID id) {
	return id.ToString();
}

template<>
static inline std::string FormatUtils::toString<SDLCore::TextureParams>(SDLCore::TextureParams param) {
	switch (param) {
	case SDLCore::TextureParams::NONE: return "NONE";
	case SDLCore::TextureParams::ROTATION: return "ROTATION";
	case SDLCore::TextureParams::CENTER:  return "CENTER";
	case SDLCore::TextureParams::COLOR_TINT: return "COLOR_TINT";
	case SDLCore::TextureParams::FLIP: return "FLIP";
	case SDLCore::TextureParams::TYPE: return "TYPE";
	default: return "UNKNWON";
	}
}

template<>
static inline std::string FormatUtils::toString<SDLCore::Align>(SDLCore::Align align) {
	switch (align) {
	case SDLCore::Align::NONE:	return "None";
	case SDLCore::Align::START:	return "Start";
	case SDLCore::Align::CENTER:return "Center";
	case SDLCore::Align::END:	return "End";
	default:					return "UNKOWN";
	}
}

template<>
static inline std::string FormatUtils::toString<SDLCore::UnitType>(SDLCore::UnitType type) {
	switch (type) {
	case SDLCore::UnitType::NONE:		return "none";
	case SDLCore::UnitType::CHARACTERS: return "Characters";
	case SDLCore::UnitType::PIXELS:		return "Pixels";
	default:						return "UNKOWN";
	}
}

template<>
static inline std::string FormatUtils::toString<SDLCore::Platform>(SDLCore::Platform platform) {
	switch (platform) {
	case SDLCore::Platform::UNKOWN:    return "Unknown";
	case SDLCore::Platform::WINDOWS:   return "Windows";
	case SDLCore::Platform::MAC_OS:    return "MacOS";
	case SDLCore::Platform::LINUX:     return "Linux";
	case SDLCore::Platform::IOS:       return "iOS";
	case SDLCore::Platform::ANDROID:   return "Android";
	default:                           return "UNKNOWN";
	}
}