#pragma once
#include <CoreLib/Math/Vector2.h>
#include <CoreLib/Math/Vector4.h>
#include <CoreLib/FormatUtils.h>

#include "UI/Types/UITypes.h"
#include "UI/UIRegistry.h"

template<typename T>
struct IsUITextureTarget : std::false_type {};

template<> struct IsUITextureTarget<std::shared_ptr<SDLCore::Texture>> : std::true_type {};

template<typename T>
inline constexpr bool IsUITextureTarget_v = IsUITextureTarget<T>::value;

template<typename T>
struct IsUINumberTarget : std::false_type {};

template<> struct IsUINumberTarget<int> : std::true_type {};
template<> struct IsUINumberTarget<float> : std::true_type {};
template<> struct IsUINumberTarget<double> : std::true_type {};
template<> struct IsUINumberTarget<Vector2> : std::true_type {};
template<> struct IsUINumberTarget<Vector4> : std::true_type {};

template<typename T>
inline constexpr bool IsUINumberTarget_v = IsUINumberTarget<T>::value;

namespace SDLCore::UI {
	
	class PropertyValue {
	public:
		using ValueVariant = std::variant<int, float, double, bool, Vector2, Vector4, std::shared_ptr<Texture>, std::shared_ptr<Font>, UIColorID, UIFontID, UITextureID, UINumberID>;
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
		PropertyValue(std::shared_ptr<Texture> tex);
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
		PropertyValue& SetValue(std::shared_ptr<Texture> tex);
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
			// Direct value
			if (std::holds_alternative<T>(m_value)) {
				out = std::get<T>(m_value);
				return true;
			}

			// UIColorID -> Vector4
			if constexpr (std::is_same_v<T, Vector4>) {
				if (std::holds_alternative<UIColorID>(m_value)) {
					return UIRegistry::TryResolve(
						std::get<UIColorID>(m_value), out);
				}
			}

			// UINumberID -> numeric (int/float/double only)
			if constexpr (IsUINumberTarget_v<T>) {
				if (std::holds_alternative<UINumberID>(m_value)) {
					return UIRegistry::TryResolve(
						std::get<UINumberID>(m_value), out);
				}
			}

			if constexpr (IsUITextureTarget_v<T>) {
				if (std::holds_alternative<UITextureID>(m_value)) {
					return UIRegistry::TryResolve(
						std::get<UITextureID>(m_value), out);
				}
			}

			Log::Warn(
				"SDLCore::UI::PropertyValue: Type mismatch, requested '{}', stored '{}'",
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