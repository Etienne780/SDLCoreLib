#pragma once
#include <CoreLib/Math/Vector2.h>

#include "UI/Types/UITypes.h"

namespace SDLCore::UI {

	class UIStyleState {
	public:
		UIStyleState();
		void SetValue(UIPropertyID id, PropertyValue value, bool important = false);

		/*
		* @brief Merges this style on top of outStyle
		*/
		void Merge(UIStyleState& outStyleState) const;

		template<typename T>
		bool TryGetValue(UIPropertyID id, T& outValue) const {
			auto it = m_properties.find(id);
			if (it == m_properties.end())
				return false;

			auto& prop = it->second;
			return prop.TryGetValue<T>(outValue);
		}

		std::unordered_map<UIPropertyID, PropertyValue>& GetAllProperties();
		const std::unordered_map<UIPropertyID, PropertyValue>& GetAllProperties() const;

	private:
		std::unordered_map<UIPropertyID, PropertyValue> m_properties;
	};
	
}