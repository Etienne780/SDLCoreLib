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
#ifndef NDEBUG
		if (m_isComposite)
			Log::Warn("SDLCore::UI::UIProperty::GetType: Undefined behaviour, trying to get type of composite property '{}'!", m_name);
#endif
		return m_type;
	}
	
	PropertyValue::ValueVariant UIProperty::GetDefaultValue() const {
#ifndef NDEBUG
		if (m_isComposite)
			Log::Warn("SDLCore::UI::UIProperty::GetDefaultValue: Undefined behaviour, trying to get default value of composite property '{}'!", m_name);
#endif
		return m_defaultVaue;
	}

	const std::vector<UIPropertyID>& UIProperty::GetCompositePropertys() const {
#ifndef NDEBUG
		if(!m_isComposite)
			Log::Warn("SDLCore::UI::UIProperty::GetCompositePropertys: Undefined behaviour, trying to get composite values of property '{}'!", m_name);
#endif
		return m_compositeProperties;
	}

}