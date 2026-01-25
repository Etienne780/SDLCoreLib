#include "UI/Types/UIProperty.h"

namespace SDLCore::UI {

	UIProperty::UIProperty(const std::string& name,
		const std::string& description,
		PropertyValue::Type type,
		PropertyValue::ValueVariant defaultVaue)
		: m_name(name), m_description(description), m_type(type), m_defaultVaue(defaultVaue) {
	}

	bool UIProperty::IsComposite() const {
		return m_isComposite;
	}

	const std::string& UIProperty::GetName() const {
		return m_name;
	}

	const std::string& UIProperty::GetDesc() const {
		return m_description;
	}

	PropertyValue::Type UIProperty::GetType() const {
		if (m_isComposite)
			Log::Warn("SDLCore::UI::UIProperty::GetType: Undefined behaviour, Trying to get type of Composite value {}", m_name);
		return m_type;
	}
	
	PropertyValue::ValueVariant UIProperty::GetDefaultValue() const {
		if (m_isComposite)
			Log::Warn("SDLCore::UI::UIProperty::GetDefaultValue: Undefined behaviour, Trying to get default value of Composite value {}", m_name);
		return m_defaultVaue;
	}

}