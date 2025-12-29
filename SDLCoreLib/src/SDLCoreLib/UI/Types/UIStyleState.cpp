#include <cmath>

#include "UI/Types/UIPropertyRegistry.h"
#include "UI/Types/UIStyleState.h"

namespace SDLCore::UI {

	UIStyleState::UIStyleState() {
		const auto& allProps = UIPropertyRegistry::GetAllProperties();

		for (const auto& [id, prop] : allProps) {
			m_properties[id] = PropertyValue(prop.GetType(), prop.GetDefaultValue());
		}
	}

	UIStyleState UIStyleState::Interpolate(const UIStyleState& start, const UIStyleState& end, float time, UIEasing easing) {
		time = std::clamp(time, 0.0f, 1.0f);

		if (time == 0.0f)
			return start;
		if (time == 1.0f)
			return end;

		UIStyleState result;
		const auto& startMap = start.GetAllPropertiesMap();
		const auto& endMap = end.GetAllPropertiesMap();
		auto& outMap = result.GetAllPropertiesMap();

		for (auto& [id, prop] : outMap) {
			auto startIt = startMap.find(id);
			if (startIt == startMap.end())
				continue;

			auto endIt = endMap.find(id);
			if (endIt == endMap.end())
				continue;
			
			const PropertyValue& startProp = startIt->second;
			const PropertyValue& endProp = endIt->second;
			prop.Interpolate(startProp, endProp, time, easing);
		}

		return result;
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

	void UIStyleState::Merge(const UIStyleState& other) {
		using propMap = std::unordered_map<UIPropertyID, PropertyValue>;
		propMap& sourceProps = this->GetAllPropertiesMap();
		const propMap& otherProps = other.GetAllPropertiesMap();

		for (auto& [id, prop] : otherProps) {
			sourceProps[id].ApplyWithPriority(prop);
		}
	}

	std::unordered_map<UIPropertyID, PropertyValue>& UIStyleState::GetAllPropertiesMap() {
		return m_properties;
	}

	const std::unordered_map<UIPropertyID, PropertyValue>& UIStyleState::GetAllPropertiesMap() const {
		return m_properties;
	}

}