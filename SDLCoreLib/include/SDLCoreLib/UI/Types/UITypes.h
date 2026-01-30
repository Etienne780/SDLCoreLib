#pragma once
#include <variant>

#include <CoreLib/Math/Vector2.h>
#include <CoreLib/Math/Vector4.h>
#include "Types/Texture.h"
#include "Types/Font/Font.h"
#include "Types/Types.h"

namespace SDLCore::UI {

	enum class UIState : uint16_t {
		NORMAL = 0,
		HOVER,
		PRESSED,
		DISABLED,
		FOCUSED
	};

	enum UIAlignment : uint16_t {
		START = 0,
		CENTER,
		END
	};

	enum UILayoutDirection : uint16_t {
		ROW = 0,
		COLUMN,
		ROW_REVERSE,
		COLUMN_REVERSE
	};
	using UILayoutDir = UILayoutDirection;

	enum UIPositionType : uint16_t {
		FLOW = 0,
		RELATIVE,
		ABSOLUTE
	};

	/*
	* @brief Unit describing how a scalar layout value is resolved.
	*/
	enum UISizeUnit : uint16_t {
		PX = 0,            // Absolute pixels
		PERCENTAGE,        // Percentage of the local reference size
		PERCENTAGE_W,      // Percentage of reference width
		PERCENTAGE_H       // Percentage of reference height
	};

	enum class UITimeUnit : uint16_t {
		MILLISECONDS = 0,
		SECONDS
	};

	enum class UIEasing : uint16_t {
		// Linear
		Linear = 0,        // constant speed

		// Quadratic
		EaseInQuad,         // accelerates
		EaseOutQuad,        // decelerates
		EaseInOutQuad,      // accelerates then decelerates

		// Cubic
		EaseInCubic,        // strong acceleration
		EaseOutCubic,       // strong deceleration
		EaseInOutCubic,     // combination of acceleration and deceleration

		// Quartic
		EaseInQuart,        // steep acceleration
		EaseOutQuart,       // steep deceleration
		EaseInOutQuart,     // combination of steep acceleration and deceleration

		// Quintic
		EaseInQuint,        // very steep acceleration
		EaseOutQuint,       // very steep deceleration
		EaseInOutQuint,     // combination of very steep acceleration and deceleration

		// Sinusoidal
		EaseInSine,         // smooth acceleration
		EaseOutSine,        // smooth deceleration
		EaseInOutSine       // smooth acceleration and deceleration
	};


	struct UIColorTag {};
	struct UIFontTag {};
	struct UITextureTag {};
	struct UINumberTag {};
	struct UINodeTag {};
	struct UIPropertyTag {};
	struct UIStyleTag {};

	/**
	* @brief Identifier for a color (used by the UIStyles).
	*        Internally stored as an uint32_t
	*/
	using UIColorID = SDLCoreID<UIColorTag>;

	/**
	* @brief Identifier for a font (used by the UIStyles).
	*        Internally stored as an uint32_t
	*/
	using UIFontID = SDLCoreID<UIFontTag>;

	/**
	* @brief Identifier for a texture (used by the UIStyles).
	*        Internally stored as an uint32_t
	*/
	using UITextureID = SDLCoreID<UITextureTag>;

	/**
	* @brief Identifier for a texture (used by the UIStyles).
	*        Internally stored as an uint32_t
	*/
	using UINumberID = SDLCoreID<UINumberTag>;

	/**
	* @brief Identifier for a node (used by the UIContext).
	*        Internally stored as an uint32_t
	*/
	using UINodeID = SDLCoreID<UINodeTag>;

	/**
	* @brief Identifier for an property (used by the UIStyles).
	*        Internally stored as an uint32_t
	*/
	using UIPropertyID = SDLCoreID<UIPropertyTag>;

	/**
	* @brief Identifier for a style (used by the UIContext).
	*        Internally stored as an uint32_t
	*/
	using UIStyleID = SDLCoreID<UIStyleTag>;

	struct UIKey {
		uintptr_t id;

		UIKey(const std::string& str)
			: id(std::hash<std::string>{}(str)) {
		}

		UIKey(const char* str)
			: id(std::hash<std::string>{}(str)) {
		}

		UIKey(char c)
			: id(std::hash<std::string>{}(std::string(1, c))) {
		}

		UIKey(const UIKey&) = delete;
		UIKey(UIKey&&) = default;
	};

}

template<>
static inline std::string FormatUtils::toString<SDLCore::UI::UIState>(SDLCore::UI::UIState state) {
	switch (state) {
	case SDLCore::UI::UIState::NORMAL:		return "Normal";
	case SDLCore::UI::UIState::HOVER:		return "Hover";
	case SDLCore::UI::UIState::PRESSED:		return "Active";
	case SDLCore::UI::UIState::DISABLED:	return "Disabled";
	default:								return "UNKOWN";
	}
}

template<>
static inline std::string FormatUtils::toString<SDLCore::UI::UIAlignment>(SDLCore::UI::UIAlignment alignment) {
	switch (alignment) {
	case SDLCore::UI::UIAlignment::START:	return "Start";
	case SDLCore::UI::UIAlignment::CENTER:	return "Center";
	case SDLCore::UI::UIAlignment::END:		return "End";
	default:								return "UNKOWN";
	}
}

template<>
static inline std::string FormatUtils::toString<SDLCore::UI::UILayoutDirection>(SDLCore::UI::UILayoutDirection dir) {
	switch (dir) {
	case SDLCore::UI::UILayoutDirection::ROW:			return "Row";
	case SDLCore::UI::UILayoutDirection::COLUMN:		return "Column";
	case SDLCore::UI::UILayoutDirection::ROW_REVERSE:	return "Row_reverse";
	case SDLCore::UI::UILayoutDirection::COLUMN_REVERSE:return "Column_reverse";
	default:											return "UNKOWN";
	}
}

template<>
static inline std::string FormatUtils::toString<SDLCore::UI::UIPositionType>(SDLCore::UI::UIPositionType type) {
	switch (type) {
	case SDLCore::UI::UIPositionType::FLOW:		return "Flow";
	case SDLCore::UI::UIPositionType::RELATIVE:	return "Relative";
	case SDLCore::UI::UIPositionType::ABSOLUTE:	return "Absolute";
	default:									return "UNKOWN";
	}
}

template<>
static inline std::string FormatUtils::toString<SDLCore::UI::UISizeUnit>(SDLCore::UI::UISizeUnit unit) {
	switch (unit) {
	case SDLCore::UI::UISizeUnit::PX:			return "px";
	case SDLCore::UI::UISizeUnit::PERCENTAGE:	return "%";
	case SDLCore::UI::UISizeUnit::PERCENTAGE_W: return "%w";
	case SDLCore::UI::UISizeUnit::PERCENTAGE_H: return "%h";
	default:									return "UNKOWN";
	}
}

template<>
static inline std::string FormatUtils::toString<SDLCore::UI::UITimeUnit>(SDLCore::UI::UITimeUnit unit) {
	switch (unit) {
	case SDLCore::UI::UITimeUnit::MILLISECONDS: return "Milliseconds";
	case SDLCore::UI::UITimeUnit::SECONDS:		return "Seconds";
	default:									return "UNKOWN";
	}
}

template<>
static inline std::string FormatUtils::toString<SDLCore::UI::UIEasing>(SDLCore::UI::UIEasing easing) {
	switch (easing) {
		// Linear
	case SDLCore::UI::UIEasing::Linear:       return "Linear";

		// Quadratic
	case SDLCore::UI::UIEasing::EaseInQuad:   return "EaseInQuad";
	case SDLCore::UI::UIEasing::EaseOutQuad:  return "EaseOutQuad";
	case SDLCore::UI::UIEasing::EaseInOutQuad:return "EaseInOutQuad";

		// Cubic
	case SDLCore::UI::UIEasing::EaseInCubic:   return "EaseInCubic";
	case SDLCore::UI::UIEasing::EaseOutCubic:  return "EaseOutCubic";
	case SDLCore::UI::UIEasing::EaseInOutCubic:return "EaseInOutCubic";

		// Quartic
	case SDLCore::UI::UIEasing::EaseInQuart:   return "EaseInQuart";
	case SDLCore::UI::UIEasing::EaseOutQuart:  return "EaseOutQuart";
	case SDLCore::UI::UIEasing::EaseInOutQuart:return "EaseInOutQuart";

		// Quintic
	case SDLCore::UI::UIEasing::EaseInQuint:   return "EaseInQuint";
	case SDLCore::UI::UIEasing::EaseOutQuint:  return "EaseOutQuint";
	case SDLCore::UI::UIEasing::EaseInOutQuint:return "EaseInOutQuint";

		// Sinus
	case SDLCore::UI::UIEasing::EaseInSine:    return "EaseInSine";
	case SDLCore::UI::UIEasing::EaseOutSine:   return "EaseOutSine";
	case SDLCore::UI::UIEasing::EaseInOutSine: return "EaseInOutSine";

	default: return "UNKNOWN";
	}
}

template<>
static inline std::string FormatUtils::toString<SDLCore::SDLCoreID<SDLCore::UI::UIColorTag>>(SDLCore::UI::UIColorID id) {
	return id.ToString();
}

template<>
static inline std::string FormatUtils::toString<SDLCore::SDLCoreID<SDLCore::UI::UIFontTag>>(SDLCore::UI::UIFontID id) {
	return id.ToString();
}

template<>
static inline std::string FormatUtils::toString<SDLCore::SDLCoreID<SDLCore::UI::UITextureTag>>(SDLCore::UI::UITextureID id) {
	return id.ToString();
}

template<>
static inline std::string FormatUtils::toString<SDLCore::SDLCoreID<SDLCore::UI::UINumberTag>>(SDLCore::UI::UINumberID id) {
	return id.ToString();
}

template<>
static inline std::string FormatUtils::toString<SDLCore::SDLCoreID<SDLCore::UI::UINodeTag>>(SDLCore::UI::UINodeID id) {
	return id.ToString();
}

template<>
static inline std::string FormatUtils::toString<SDLCore::SDLCoreID<SDLCore::UI::UIPropertyTag>>(SDLCore::UI::UIPropertyID id) {
	return id.ToString();
}

template<>
static inline std::string FormatUtils::toString<SDLCore::SDLCoreID<SDLCore::UI::UIStyleTag>>(SDLCore::UI::UIStyleID id) {
	return id.ToString();
}