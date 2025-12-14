#include "UI/Types/UIProperty.h"

namespace SDLCore::UI {

	UIProperty::UIProperty(const std::string& name,
		const std::string& description,
		PropertyValue::Type type,
		PropertyValue::ValueVariant defaultVaue)
		: m_name(name), m_description(description), m_type(type), m_defaultVaue(defaultVaue) {
	}

	const std::string& UIProperty::GetName() const {
		return m_name;
	}

	const std::string& UIProperty::GetDesc() const {
		return m_description;
	}

	PropertyValue::Type UIProperty::GetType() const {
		return m_type;
	}
	
	PropertyValue::ValueVariant UIProperty::GetDefaultValue() const {
		return m_defaultVaue;
	}

}