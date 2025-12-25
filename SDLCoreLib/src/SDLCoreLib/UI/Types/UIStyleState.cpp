#include "UI/Types/UIPropertyRegistry.h"
#include "UI/Types/UIStyleState.h"

namespace SDLCore::UI {

	UIStyleState::UIStyleState() {
		const auto& allProps = UIPropertyRegistry::GetAllProperties();

		for (const auto& [id, prop] : allProps) {
			m_properties[id] = PropertyValue(prop.GetType(), prop.GetDefaultValue());
		}
	}

	bool UIStyleState::SetValue(UIPropertyID id, PropertyValue value, bool important) {
		auto it = m_properties.find(id);
		if (it == m_properties.end()) {
#ifndef NDEBUG
			Log::Error("SDLCore::UI::UIStyleState: Could not set value, property '{}' not found", id);
#endif
			return false;
		}
		auto& prop = it->second;

		// checks if there the same type or similer like float and numberID
		if (!prop.IsSameType(value.GetType())) {
#ifndef NDEBUG
			Log::Error("SDLCore::UI::UIStyleState: Could not set value, value needs to be of a similer type as the property, '{}' != '{}'", 
				prop.GetType(), value.GetType());
#endif
			return false;
		}

		prop.SetValue(value.GetType(), value.GetVariant());
		prop.SetIsImportant(important);
		return true;
	}

	void UIStyleState::Merge(UIStyleState& outStyleState) const {
		using propMap = std::unordered_map<UIPropertyID, PropertyValue>;
		const propMap& sourceProps = this->GetAllProperties();
		propMap& targetProps = outStyleState.GetAllProperties();

		for (auto& [id, prop] : sourceProps) {
			targetProps[id].ApplyWithPriority(prop);
		}
	}

	std::unordered_map<UIPropertyID, PropertyValue>& UIStyleState::GetAllProperties() {
		return m_properties;
	}

	const std::unordered_map<UIPropertyID, PropertyValue>& UIStyleState::GetAllProperties() const {
		return m_properties;
	}

}