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
		
		bool SetValue(UIPropertyID id, PropertyValue value, bool important = false);

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
	};
	
}