#include <cmath>

#include "SDLCoreError.h"
#include "UI/Types/PropertyValue.h"

namespace SDLCore::UI {

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

	PropertyValue::PropertyValue(bool b)
		: m_value(b), m_valueType(Type::BOOL) {
	}

	PropertyValue::PropertyValue(const Vector2& vec)
		: m_value(vec), m_valueType(Type::VECTOR2) {
	}

	PropertyValue::PropertyValue(const Vector4& vec)
		: m_value(vec), m_valueType(Type::VECTOR4) {
	}

	PropertyValue::PropertyValue(std::shared_ptr<Texture>  tex)
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

	bool PropertyValue::Interpolate(const PropertyValue& start, const PropertyValue& end, float time, UIEasing easing) {
		if (start.GetIsImportant() || !end.GetIsSet())
			*this = start;

		time = std::clamp(time, 0.0f, 1.0f);
		time = ResolveEasing(time, easing);
		
		if (!start.IsSameType(end.GetType())) {
			SetErrorF(
				"PropertyValue::Interpolate: Type mismatch {} != {}",
				start.GetType(), end.GetType()
			);
			*this = (time < 1.0f) ? start : end;
			return false;
		}

		if (time == 0.0f) {
			*this = start;
			return true;
		}

		if (time == 1.0f) {
			*this = end;
			return true;
		}

		return InterpolateInternal(start, end, time);
	}

	void PropertyValue::Reset() {
		m_isImportant = false;
		m_isSet = false;
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

	PropertyValue::ValueVariant& PropertyValue::GetVariant() {
		return m_value;
	}
		
	const PropertyValue::ValueVariant& PropertyValue::GetVariant() const {
		return m_value;
	}

	PropertyValue& PropertyValue::SetIsSet(bool value) {
		m_isSet = value;
		return *this;
	}

	bool PropertyValue::InterpolateInternal(const PropertyValue& a, const PropertyValue& b, float t) {
		switch (a.m_valueType) {
		case Type::INT:    return LerpInternalNumber<int>(*this, a, b, t);
		case Type::FLOAT:  return LerpInternalNumber<float>(*this, a, b, t);
		case Type::DOUBLE: return LerpInternalNumber<double>(*this, a, b, t);

		case Type::VECTOR2:
			if (!std::holds_alternative<Vector2>(a.m_value) || !std::holds_alternative<Vector2>(b.m_value))
				return false;
			return LerpInternalVector<Vector2>(*this,
				std::get<Vector2>(a.m_value),
				std::get<Vector2>(b.m_value),
				t,
				Vector2::Lerp);

		case Type::VECTOR4:
		case Type::COLOR_ID:
			return LerpColorOrVector4(*this, a, b, t);

		case Type::NUMBER_ID:
			return LerpInternalNumber<double>(*this, a, b, t);

		case Type::BOOL:
		case Type::TEXTURE:
		case Type::TEXTURE_ID:
		case Type::FONT:
		case Type::FONT_ID:
			*this = (t < 1.0f) ? a : b;
			return true;

		default:
			return false;
		}
	}

	bool PropertyValue::LerpColorOrVector4(PropertyValue& out, const PropertyValue& a, const PropertyValue& b, float t) {
		Vector4 va, vb;
		if (!ResolveColorToVector4(a, va) || !ResolveColorToVector4(b, vb))
			return false;

		Vector4 result = Vector4::Lerp(va, vb, t);
		out.SetValue(result);
		return true;
	}

	bool PropertyValue::ResolveColorToVector4(const PropertyValue& a, Vector4& outValue) {
		switch (a.GetType())
		{
		case Type::COLOR_ID:
			return UIRegistry::TryResolve(std::get<UIColorID>(a.m_value), outValue);
		case Type::VECTOR4:
			outValue = std::get<Vector4>(a.m_value);
			return true;
		default:
			return false;
		}
	}

	float PropertyValue::ResolveEasing(float time, UIEasing easing) {
		switch (easing) {
		case UIEasing::Linear:
			return time;

		case UIEasing::EaseInQuad:
			return time * time;

		case UIEasing::EaseOutQuad:
			return 1.0f - (1.0f - time) * (1.0f - time);

		case UIEasing::EaseInOutQuad:
			if (time < 0.5f) return 2.0f * time * time;
			return 1.0f - static_cast<float>(std::pow(-2.0f * time + 2.0f, 2.0f)) / 2.0f;

		case UIEasing::EaseInCubic:
			return time * time * time;

		case UIEasing::EaseOutCubic:
			return 1.0f - static_cast<float>(std::pow(1.0f - time, 3.0f));

		case UIEasing::EaseInOutCubic:
			if (time < 0.5f) return 4.0f * time * time * time;
			return 1.0f - static_cast<float>(std::pow(-2.0f * time + 2.0f, 3.0f)) / 2.0f;

		case UIEasing::EaseInSine:
			return 1.0f - static_cast<float>(std::cos((time * CORE_PI) / 2.0f));

		case UIEasing::EaseOutSine:
			return  static_cast<float>(std::sin((time * CORE_PI) / 2.0f));

		case UIEasing::EaseInOutSine:
			return  static_cast<float>( - (std::cos(CORE_PI * time) - 1.0f) / 2.0f);

		default:
			return time;
		}
	}

	PropertyValue::PropertyTypeClass PropertyValue::GetTypeClass(PropertyValue::Type t) {
		switch (t) {
		case Type::INT:
		case Type::FLOAT:
		case Type::DOUBLE:
		case Type::NUMBER_ID:
		case Type::COLOR_ID:
		case Type::VECTOR2:
		case Type::VECTOR4:
			return PropertyTypeClass::Numeric;

		case Type::BOOL:
			return PropertyTypeClass::BOOL;

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

	PropertyValue& PropertyValue::SetValue(std::shared_ptr<Texture> tex) {
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

}