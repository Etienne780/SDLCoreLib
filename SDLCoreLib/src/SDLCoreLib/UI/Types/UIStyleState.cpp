#include <cmath>

#include "UI/Types/UIPropertyRegistry.h"
#include "UI/Types/UIStyleState.h"

namespace SDLCore::UI {

	UIStyleState::UIStyleState() {
		const auto& allProps = UIPropertyRegistry::GetAllProperties();

		for (const auto& [id, prop] : allProps) {
			if (prop.IsComposite()) {
				m_compositeRefs[id] = prop.GetCompositePropertyIDs();
			}
			else {
				m_properties[id] = PropertyValue(prop.GetType(), prop.GetDefaultValue());
			}
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

	bool UIStyleState::IsValueSet(UIPropertyID id) const {
		const PropertyValue* propValue = TryGetPropValue(id);
		if (!propValue) {
#ifndef NDEBUG
			Log::Error("SDLCore::UI::UIStyleState::IsValueSet: Could not check if value is set, property '{}' not found", id);
#endif
			return false;
		}
		return propValue->GetIsSet();
	}

	bool UIStyleState::IsImportant(UIPropertyID id) const {
		const PropertyValue* propValue = TryGetPropValue(id);
		if (!propValue) {
#ifndef NDEBUG
			Log::Error("SDLCore::UI::UIStyleState::IsImportant: Could not check if is important, property '{}' not found", id);
#endif
			return false;
		}
		return propValue->GetIsImportant();
	}

	bool UIStyleState::SetImportant(bool value) {
		if (m_lastPropSet.IsInvalid()) {
			Log::Error("SDLCore::UI::UIStyleState::SetImportant: Could not set important, no property was set before this call!");
			return false;
		}

		PropertyValue* propValue = TryGetPropValue(m_lastPropSet);
		if (!propValue) {
#ifndef NDEBUG
			Log::Error("SDLCore::UI::UIStyleState::SetImportant: Could not set value, property '{}' not found", m_lastPropSet);
#endif
			return false;
		}

		propValue->SetIsImportant(value);
		return true;
	}

	void UIStyleState::ResetValue(UIPropertyID id) {
		PropertyValue* propValue = TryGetPropValue(id);
		if (!propValue) {
#ifndef NDEBUG
			Log::Error("SDLCore::UI::UIStyleState::ResetValue: Could not set value, property '{}' not found", id);
#endif
			return;
		}

		UIProperty* prop =  UIPropertyRegistry::TryGetProperty(id);
		if (!prop)
			return;


		propValue->SetValue(prop->GetType(), prop->GetDefaultValue());
		propValue->Reset();
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

	const PropertyValue* UIStyleState::TryGetPropValue(UIPropertyID id) const {
		auto it = m_properties.find(id);
		if (it == m_properties.end()) {
			return nullptr;
		}
		return &(it->second);
	}

	PropertyValue* UIStyleState::TryGetPropValue(UIPropertyID id) {
		auto it = m_properties.find(id);
		if (it == m_properties.end()) {
			return nullptr;
		}
		return &(it->second);
	}

	const std::vector<UIPropertyID>* UIStyleState::TryGetCompositeProperyValues(UIPropertyID id) const {
		auto it = m_compositeRefs.find(id);
		if (it == m_compositeRefs.end()) {
			return nullptr;
		}
		return &(it->second);
	}

	bool UIStyleState::SetSingleValue(UIPropertyID id, const PropertyValue& value) {
		PropertyValue* propValue = TryGetPropValue(id);
		if (!propValue) {
#ifndef NDEBUG
			Log::Error(
				"SDLCore::UI::UIStyleState: Could not set value, property '{}' not found",
				id
			);
#endif
			return false;
		}

		// Check for compatible type
		if (!propValue->IsSameType(value.GetType())) {
#ifndef NDEBUG
			Log::Error(
				"SDLCore::UI::UIStyleState: Type mismatch for property '{}'",
				id
			);
#endif
			return false;
		}

		propValue->SetValue(value.GetType(), value.GetVariant());
		m_lastPropSet = id;
		return true;
	}

}