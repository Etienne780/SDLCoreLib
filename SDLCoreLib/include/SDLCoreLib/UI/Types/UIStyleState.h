#pragma once
#include <CoreLib/Math/Vector2.h>

#include "UI/Types/UITypes.h"
#include "UI/Types/PropertyValue.h"

namespace SDLCore::UI {

	class UIStyleState {
	public:
		UIStyleState();

		/*
		* @brief idk
		* @param start
		* @param end
		* @param time 0 = start; 1 = end is clamped to 0-1
		*/
		static UIStyleState Interpolate(const UIStyleState& start, const UIStyleState& end, float time, UIEasing easing = UIEasing::Linear);

		bool IsValueSet(UIPropertyID id) const;
		// @returns true if prop value different
		bool IsDifferent(UIPropertyID id, const PropertyValue& value, bool important) const;
		bool SetValue(UIPropertyID id, const PropertyValue& value, bool important = false);
		void ResetValue(UIPropertyID id);

		/*
		* @brief Merges other style on top of this
		*/
		void Merge(const UIStyleState& other);

		template<typename T>
		bool TryGetValue(UIPropertyID id, T& outValue, const T& fallback) const {
			bool result = false;

			auto it = m_properties.find(id);
			if (it != m_properties.end()) {
				result = it->second.TryGetValue<T>(outValue);
			}

			if(!result)
				outValue = fallback;

			return result;
		}

		std::unordered_map<UIPropertyID, PropertyValue>& GetAllPropertiesMap();
		const std::unordered_map<UIPropertyID, PropertyValue>& GetAllPropertiesMap() const;
		std::vector<PropertyValue> GetAllProperties();

	private:
		std::unordered_map<UIPropertyID, PropertyValue> m_properties;

		const PropertyValue* TryGetPropValue(UIPropertyID id) const;
		PropertyValue* TryGetPropValue(UIPropertyID id);
	};
	
}