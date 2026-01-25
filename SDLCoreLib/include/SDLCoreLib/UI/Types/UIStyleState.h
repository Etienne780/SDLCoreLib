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

		/**
		* @brief Retrieves the value of a property if it exists and matches the requested type.
		*
		* If the property does not exist or the stored type does not match @p T,
		* @p outValue is assigned the provided fallback value.
		*
		* @tparam T Expected value type.
		* @param id Property identifier.
		* @param outValue Reference receiving the resolved value or fallback.
		* @param fallback Value assigned when the property is missing or incompatible.
		*
		* @return True if a value of type @p T was successfully retrieved, false otherwise.
		*/
		template<typename T>
		bool TryGetValue(UIPropertyID id, T& outValue, const T& fallback) const {
			bool result = false;

			const PropertyValue* propVal = TryGetPropValue(id);
			if (propVal) {
				result = propVal->TryGetValue<T>(outValue);
			}

			if(!result)
				outValue = fallback;

			return result;
		}

		/**
		* @brief Retrieves the value of a property only if it is explicitly set.
		*
		* This function succeeds only if:
		* 
		* - the property exists,
		* 
		* - the property is marked as set,
		* 
		* - and the stored value matches the requested type.
		*
		* @tparam T Expected value type.
		* @param id Property identifier.
		* @param outValue Reference receiving the stored value.
		*
		* @return True if the property is set and successfully read, false otherwise.
		*/
		template<typename T>
		bool TryGetValueIfSet(UIPropertyID id, T& outValue) const {
			const PropertyValue* propVal = TryGetPropValue(id);
			if (!propVal)
				return false;

			if (!propVal->GetIsSet())
				return false;

			return propVal->TryGetValue<T>(outValue);
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