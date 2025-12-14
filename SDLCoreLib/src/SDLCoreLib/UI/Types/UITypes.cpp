#include "UI/Types/UITypes.h"

namespace SDLCore::UI {

	#pragma region PropertyValue

	PropertyValue::PropertyValue() 
		: m_value(0), m_valueType(Type::INT) {
	}

	PropertyValue::PropertyValue(int i)
		: m_value(i), m_valueType(Type::INT) {
	}

	PropertyValue::PropertyValue(float f)
		: m_value(f), m_valueType(Type::FLOAT) {
	}

	PropertyValue::PropertyValue(double d) 
		: m_value(d), m_valueType(Type::DOUBLE) {
	}

	PropertyValue::PropertyValue(const Vector2& vec)
		: m_value(vec), m_valueType(Type::VECTOR2) {
	}

	PropertyValue::PropertyValue(const Vector4& vec)
		: m_value(vec), m_valueType(Type::VECTOR4) {
	}

	PropertyValue::PropertyValue(const Texture& tex)
		: m_value(tex), m_valueType(Type::TEXTURE) {
	}

	PropertyValue::PropertyValue(std::shared_ptr<Font> font)
		: m_value(font), m_valueType(Type::FONT) {
	}

	PropertyValue::PropertyValue(UIColorID id)
		: m_value(id), m_valueType(Type::COLOR_ID) {
	}

	PropertyValue::PropertyValue(UIFontID id)
		: m_value(id), m_valueType(Type::FONT_ID) {
	}

	PropertyValue::PropertyValue(UITextureID id)
		: m_value(id), m_valueType(Type::TEXTURE_ID) {
	}

	PropertyValue::PropertyValue(UINumberID id)
		: m_value(id), m_valueType(Type::NUMBER_ID) {
	}

	PropertyValue::PropertyValue(PropertyValue::Type type, const ValueVariant& value)
		: m_value(value), m_valueType(type) {
	}


	void PropertyValue::ApplyWithPriority(const PropertyValue& other) {
		if (!other.GetIsSet())
			return;

		if (m_isImportant && !other.m_isImportant)
			return;

		*this = other;
	}

	bool PropertyValue::IsSameType(Type other) const {
		return GetTypeClass(m_valueType) == GetTypeClass(other);
	}

	PropertyValue::Type PropertyValue::GetType() const {
		return m_valueType;
	}

	bool PropertyValue::GetIsSet() const {
		return m_isSet;
	}

	bool PropertyValue::GetIsImportant() const {
		return m_isImportant;
	}

	PropertyValue::ValueVariant PropertyValue::GetVariant() const {
		return m_value;
	}

	PropertyValue& PropertyValue::SetIsSet(bool value) {
		m_isSet = value;
		return *this;
	}

	PropertyValue::PropertyTypeClass PropertyValue::GetTypeClass(PropertyValue::Type t) {
		switch (t) {
		case Type::INT:
		case Type::FLOAT:
		case Type::DOUBLE:
		case Type::NUMBER_ID:
			return PropertyTypeClass::Numeric;

		case Type::VECTOR2:
			return PropertyTypeClass::Vector2;

		case Type::VECTOR4:
			return PropertyTypeClass::Vector4;

		case Type::TEXTURE:
		case Type::TEXTURE_ID:
			return PropertyTypeClass::Texture;

		case Type::FONT:
		case Type::FONT_ID:
			return PropertyTypeClass::Font;

		default:
			return PropertyTypeClass::Unknown;
		}
	}

	PropertyValue& PropertyValue::SetIsImportant(bool value) {
		m_isImportant = value;
		return *this;
	}

	PropertyValue& PropertyValue::SetValue(int i) {
		m_valueType = Type::INT;
		m_value = i;
		SetIsSet(true);
		return *this;
	}

	PropertyValue& PropertyValue::SetValue(float f) {
		m_valueType = Type::FLOAT;
		m_value = f;
		SetIsSet(true);
		return *this;
	}

	PropertyValue& PropertyValue::SetValue(double d) {
		m_valueType = Type::DOUBLE;
		m_value = d;
		SetIsSet(true);
		return *this;
	}

	PropertyValue& PropertyValue::SetValue(const Vector2& vec) {
		m_valueType = Type::VECTOR2;
		m_value = vec;
		SetIsSet(true);
		return *this;
	}

	PropertyValue& PropertyValue::SetValue(const Vector4& vec) {
		m_valueType = Type::VECTOR4;
		m_value = vec;
		SetIsSet(true);
		return *this;
	}

	PropertyValue& PropertyValue::SetValue(const Texture& tex) {
		m_valueType = Type::TEXTURE;
		m_value = tex;
		SetIsSet(true);
		return *this;
	}

	PropertyValue& PropertyValue::SetValue(std::shared_ptr<Font> font) {
		m_valueType = Type::FONT;
		m_value = font;
		SetIsSet(true);
		return *this;
	}

	PropertyValue& PropertyValue::SetValue(UIColorID id) {
		m_valueType = Type::COLOR_ID;
		m_value = id;
		SetIsSet(true);
		return *this;
	}

	PropertyValue& PropertyValue::SetValue(UIFontID id) {
		m_valueType = Type::FONT_ID;
		m_value = id;
		SetIsSet(true);
		return *this;
	}

	PropertyValue& PropertyValue::SetValue(UITextureID id) {
		m_valueType = Type::TEXTURE_ID;
		m_value = id;
		SetIsSet(true);
		return *this;
	}

	PropertyValue& PropertyValue::SetValue(UINumberID id) {
		m_valueType = Type::NUMBER_ID;
		m_value = id;
		SetIsSet(true);
		return *this;
	}

	PropertyValue& PropertyValue::SetValue(Type type, const ValueVariant& value) {
		m_valueType = type;
		m_value = value;
		SetIsSet(true);
		return *this;
	}

	std::string PropertyValue::GetReadableTypeName(const std::type_info& type) {
		if (type == typeid(int)) return "int";
		if (type == typeid(float)) return "float";
		if (type == typeid(Vector2)) return "Vector2";
		if (type == typeid(Vector4)) return "Vector4";
		if (type == typeid(Texture)) return "Texture";
		if (type == typeid(std::shared_ptr<Font>)) return "Font";
		if (type == typeid(UIColorID)) return "UIColorID";
		if (type == typeid(UIFontID)) return "UIFontID";
		if (type == typeid(UITextureID)) return "UITextureID";
		if (type == typeid(UINumberID)) return "UINumberID";
		return "UNKOWN";
	}

	#pragma endregion

}