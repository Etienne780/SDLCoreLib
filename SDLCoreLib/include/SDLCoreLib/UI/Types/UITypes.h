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

	enum UISizeUnit : uint16_t {
		PX = 0,
		PERCENTAGE,
		PERCENTAGE_W,
		PERCENTAGE_H
	};

	enum class UITimeUnit : uint16_t {
		MILLISECONDS = 0,
		SECONDS
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