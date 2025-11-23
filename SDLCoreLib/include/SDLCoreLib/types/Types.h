#pragma once
#include <filesystem>
#include <SDL3/SDL.h>
#include <CoreLib/Log.h>
#include <CoreLib/File.h>
#include <CoreLib/FormatUtils.h>
#include <CoreLib/CoreMath.h>

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

	Rect ToRect(const FRect& fRect);
	Rect ToRect(const Vector4& fRect);

	FRect ToFRect(const Rect& rect);
	FRect ToFRect(const Vector4& rect);

	template<typename Tag>
	struct SDLCoreID {
		uint32_t value = SDLCORE_INVALID_ID;

		constexpr SDLCoreID() = default;
		explicit constexpr SDLCoreID(uint32_t v) : value(v) {}

		constexpr bool operator==(uint32_t other) const { return value == other; }
		constexpr bool operator!=(uint32_t other) const { return value != other; }
		constexpr bool operator<(uint32_t other) const { return value < other; }
		constexpr bool operator>(uint32_t other) const { return value > other; }
		constexpr bool operator<=(uint32_t other) const { return value <= other; }
		constexpr bool operator>=(uint32_t other) const { return value >= other; }

		constexpr bool operator==(const SDLCoreID& other) const { return value == other.value; }
		constexpr bool operator!=(const SDLCoreID& other) const { return value != other.value; }
		constexpr bool operator<(const SDLCoreID& other) const { return value < other.value; }
		constexpr bool operator>(const SDLCoreID& other) const { return value > other.value; }
		constexpr bool operator<=(const SDLCoreID& other) const { return value <= other.value; }
		constexpr bool operator>=(const SDLCoreID& other) const { return value >= other.value; }

		explicit constexpr operator bool() const { return value != 0; }
	};

	struct WindowTag {};
	struct WindowCallbackTag {};
	struct AudioPlaybackDeviceTag {};
	struct AudioClipTag {};

	/**
	 * @brief Identifier for a window.
	 *        Internally stored as an unsigned int.
	 */
	using WindowID = SDLCoreID<WindowTag>;

	/**
	 * @brief Identifier for a window callback.
	 *        Internally stored as an unsigned int.
	 */
	using WindowCallbackID = SDLCoreID<WindowCallbackTag>;

	/**
	 * @brief Identifier for an audio playback device.
	 *        Internally stored as an unsigned int.
	 */
	using AudioPlaybackDeviceID = SDLCoreID<AudioPlaybackDeviceTag>;

	/**
	 * @brief Identifier for an audio clip.
	 *        Internally stored as an unsigned int.
	 */
	using SoundClipID = SDLCoreID<AudioClipTag>;

	enum class TextureParams : int {
		NONE = 0,
		ROTATION = 1 << 0,
		CENTER = 1 << 1,
		COLOR_TINT = 1 << 2,
		FLIP = 1 << 3,
		TYPE = 1 << 4
	};

	TextureParams operator|(TextureParams a, TextureParams b);
	TextureParams operator&(TextureParams a, TextureParams b);
	TextureParams& operator|=(TextureParams& a, TextureParams b);
	TextureParams& operator&=(TextureParams& a, TextureParams b);
	bool operator!(TextureParams a);

}

namespace std {
	template<typename Tag>
	struct hash<SDLCore::SDLCoreID<Tag>> {
		size_t operator()(const SDLCore::SDLCoreID<Tag>& id) const noexcept {
			return std::hash<uint32_t>{}(id.value);
		}
	};
}

template<>
static inline std::string FormatUtils::toString<SDLCore::SDLCoreID<SDLCore::WindowTag>>(SDLCore::WindowID id) {
	return FormatUtils::toString(id.value);
}

template<>
static inline std::string FormatUtils::toString<SDLCore::SDLCoreID<SDLCore::WindowCallbackTag>>(SDLCore::WindowCallbackID id) {
	return FormatUtils::toString(id.value);
}

template<>
static inline std::string FormatUtils::toString<SDLCore::SDLCoreID<SDLCore::AudioPlaybackDeviceTag>>(SDLCore::AudioPlaybackDeviceID id) {
	return FormatUtils::toString(id.value);
}

template<>
static inline std::string FormatUtils::toString<SDLCore::SDLCoreID<SDLCore::AudioClipTag>>(SDLCore::SoundClipID id) {
	return FormatUtils::toString(id.value);
}

template<>
static inline std::string FormatUtils::toString<SDLCore::TextureParams>(SDLCore::TextureParams param) {
	switch (param)
	{
	case SDLCore::TextureParams::NONE: return "NONE";
	case SDLCore::TextureParams::ROTATION: return "ROTATION";
	case SDLCore::TextureParams::CENTER:  return "CENTER";
	case SDLCore::TextureParams::COLOR_TINT: return "COLOR_TINT";
	case SDLCore::TextureParams::FLIP: return "FLIP";
	case SDLCore::TextureParams::TYPE: return "TYPE";
	default: return "UNKNWON";
	}
}