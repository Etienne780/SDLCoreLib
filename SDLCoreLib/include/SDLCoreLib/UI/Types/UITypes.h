#pragma once
#include <variant>

#include <CoreLib/Math/Vector2.h>
#include <CoreLib/Math/Vector4.h>
#include "Types/Texture.h"
#include "Types/Font/Font.h"
#include "Types/Types.h"

namespace SDLCore::UI {
	
	enum class UIState {
		NORMAL = 0,
		HOVER,
		ACTIVE,
		CLICK,
		DISABLED
	};

	enum class UIAlignment {
		START = 0,
		CENTER,
		END
	};

	enum class UILayoutDirection {
		ROW = 0,// is equal to using ROW_START
		COLUMN,// is equal to using COLUMN_START
		ROW_START,
		ROW_END,
		COLUMN_START, 
		COLUMN_END
	};
	using UILayoutDir = UILayoutDirection;

	enum class SizeUnit {
		PX = 0,
		PERCENTAGE,
		PERCENTAGE_W,
		PERCENTAGE_H
	};

	struct UIColorTag {};
	struct UIFontTag {};
	struct UITextureTag {};
	struct UINumberTag {};

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

	class PropertyValue {
	public:
		enum class Type {
			INT, FLOAT, DOUBLE, VECTOR2, VECTOR4, TEXTURE, FONT, COLOR_ID, FONT_ID, TEXTURE_ID, NUMBER_ID
		};

		PropertyValue();
		PropertyValue(int i);
		PropertyValue(float f);
		PropertyValue(double d);
		PropertyValue(const Vector2& vec);
		PropertyValue(const Vector4& vec);
		PropertyValue(const Texture& tex);
		PropertyValue(std::shared_ptr<Font> font);
		PropertyValue(UIColorID id);
		PropertyValue(UIFontID id);
		PropertyValue(UITextureID id);
		PropertyValue(UINumberID id);

		Type GetType() const;
		bool GetIsSet() const;
		bool GetIsImportant() const;

		PropertyValue& SetIsSet(bool value);
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
		using ValueVariant = std::variant<int, float, double, Vector2, Vector4, Texture, std::shared_ptr<Font>, UIColorID, UIFontID, UITextureID, UINumberID>;
		ValueVariant m_value;
		Type m_valueType = Type::INT;
		bool m_isSet = false;
		bool m_isImportant = false;

		/**
		* @brief Helper function that returns a readable type name for type_info.
		* @param type Type information.
		* @return std::string representing the readable type name.
		*/
		static std::string GetReadableTypeName(const std::type_info& type);
	};

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
static inline std::string FormatUtils::toString<SDLCore::UI::PropertyValue::Type>(SDLCore::UI::PropertyValue::Type type) {
	switch (type) {
	case SDLCore::UI::PropertyValue::Type::INT: return "int";
	case SDLCore::UI::PropertyValue::Type::FLOAT: return "float";
	case SDLCore::UI::PropertyValue::Type::DOUBLE: return "double";
	case SDLCore::UI::PropertyValue::Type::VECTOR2: return "Vector2";
	case SDLCore::UI::PropertyValue::Type::VECTOR4: return "Vector4";
	case SDLCore::UI::PropertyValue::Type::TEXTURE: return "Texture";
	case SDLCore::UI::PropertyValue::Type::FONT: return "Font";
	case SDLCore::UI::PropertyValue::Type::COLOR_ID: return "UIColorID";
	case SDLCore::UI::PropertyValue::Type::FONT_ID: return "UIFontID";
	case SDLCore::UI::PropertyValue::Type::TEXTURE_ID: return "UITextureID";
	case SDLCore::UI::PropertyValue::Type::NUMBER_ID: return "UINumberID";
	default: return "UNKNOWN";
	}
}