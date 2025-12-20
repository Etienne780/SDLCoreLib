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

	struct UIColorTag {};
	struct UIFontTag {};
	struct UITextureTag {};
	struct UINumberTag {};
	struct UINodeTag {};
	struct UIPropertyTag {};

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
	* @brief Identifier for an property (used by the UIStyles).
	*        Internally stored as an uint32_t
	*/
	using UIPropertyID = SDLCoreID<UIPropertyTag>;

	/**
	* @brief Identifier for a node (used by the UIContext).
	*        Internally stored as an uint32_t
	*/
	using UINodeID = SDLCoreID<UINodeTag>;

	class PropertyValue {
	public:
		using ValueVariant = std::variant<int, float, double, bool, Vector2, Vector4, Texture, std::shared_ptr<Font>, UIColorID, UIFontID, UITextureID, UINumberID>;
		enum class Type {
			INT, FLOAT, DOUBLE, BOOL, VECTOR2, VECTOR4, TEXTURE, FONT, COLOR_ID, FONT_ID, TEXTURE_ID, NUMBER_ID
		};

		PropertyValue();
		PropertyValue(int i);
		PropertyValue(float f);
		PropertyValue(double d);
		PropertyValue(bool b);
		PropertyValue(const Vector2& vec);
		PropertyValue(const Vector4& vec);
		PropertyValue(const Texture& tex);
		PropertyValue(std::shared_ptr<Font> font);
		PropertyValue(UIColorID id);
		PropertyValue(UIFontID id);
		PropertyValue(UITextureID id);
		PropertyValue(UINumberID id);
		PropertyValue(Type type, const ValueVariant& value);

		void ApplyWithPriority(const PropertyValue& other);

		bool IsSameType(Type type) const;

		Type GetType() const;
		bool GetIsSet() const;
		bool GetIsImportant() const;
		ValueVariant GetVariant() const;

		PropertyValue& SetIsImportant(bool value);

		PropertyValue& SetValue(int i);
		PropertyValue& SetValue(float f);
		PropertyValue& SetValue(double d);
		PropertyValue& SetValue(const Vector2& vec);
		PropertyValue& SetValue(const Vector4& vec);
		PropertyValue& SetValue(const Texture& tex);
		PropertyValue& SetValue(std::shared_ptr<Font> font);
		PropertyValue& SetValue(UIColorID id);
		PropertyValue& SetValue(UIFontID id);
		PropertyValue& SetValue(UITextureID id);
		PropertyValue& SetValue(UINumberID id);
		PropertyValue& SetValue(Type type, const ValueVariant& value);

		/**
		* @brief Attempts to retrieve the stored value as a specific type.
		* @tparam T Expected value type.
		* @param out Output reference to receive the value.
		* @return True if successful, false if type mismatch.
		*/
		template<typename T>
		bool TryGetValue(T& out) const {
			if (std::holds_alternative<T>(m_value)) {
				out = std::get<T>(m_value);
				return true;
			}

			Log::Warn("SDLCore::UI::PropertyValue: Could not get value wrong data type, expected type '{}', got '{}'!",
				GetReadableTypeName(typeid(T)), m_valueType);
			return false;
		}

	private:
		ValueVariant m_value;
		Type m_valueType = Type::INT;
		bool m_isSet = false;
		bool m_isImportant = false;

		enum class PropertyTypeClass {
			Numeric,
			BOOL,
			Vector2,
			Vector4,
			Texture,
			Font,
			Unknown
		};

		PropertyValue& SetIsSet(bool value);

		static PropertyTypeClass GetTypeClass(PropertyValue::Type t);

		/**
		* @brief Helper function that returns a readable type name for type_info.
		* @param type Type information.
		* @return std::string representing the readable type name.
		*/
		static std::string GetReadableTypeName(const std::type_info& type);
	};

	struct UIKey {
		uintptr_t id;

		template<size_t N>
		explicit UIKey(const char(&str)[N])
			: id(reinterpret_cast<uintptr_t>(str)) {
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
static inline std::string FormatUtils::toString<SDLCore::SDLCoreID<SDLCore::UI::UIPropertyTag>>(SDLCore::UI::UIPropertyID id) {
	return id.ToString();
}

template<>
static inline std::string FormatUtils::toString<SDLCore::SDLCoreID<SDLCore::UI::UINodeTag>>(SDLCore::UI::UINodeID id) {
	return id.ToString();
}

template<>
static inline std::string FormatUtils::toString<SDLCore::UI::PropertyValue::Type>(SDLCore::UI::PropertyValue::Type type) {
	switch (type) {
	case SDLCore::UI::PropertyValue::Type::INT:			return "int";
	case SDLCore::UI::PropertyValue::Type::FLOAT:		return "float";
	case SDLCore::UI::PropertyValue::Type::DOUBLE:		return "double";
	case SDLCore::UI::PropertyValue::Type::BOOL:		return "bool";
	case SDLCore::UI::PropertyValue::Type::VECTOR2:		return "Vector2";
	case SDLCore::UI::PropertyValue::Type::VECTOR4:		return "Vector4";
	case SDLCore::UI::PropertyValue::Type::TEXTURE:		return "Texture";
	case SDLCore::UI::PropertyValue::Type::FONT:		return "Font";
	case SDLCore::UI::PropertyValue::Type::COLOR_ID:	return "UIColorID";
	case SDLCore::UI::PropertyValue::Type::FONT_ID:		return "UIFontID";
	case SDLCore::UI::PropertyValue::Type::TEXTURE_ID:	return "UITextureID";
	case SDLCore::UI::PropertyValue::Type::NUMBER_ID:	return "UINumberID";
	default:											return "UNKNOWN";
	}
}