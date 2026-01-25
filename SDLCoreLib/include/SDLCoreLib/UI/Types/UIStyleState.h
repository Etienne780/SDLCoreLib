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
		bool IsImportant(UIPropertyID id) const;

		// @return true if the incoming value(s) differ from the currently stored value(s)
		template<typename... Args>
		bool IsDifferent(UIPropertyID id, Args&&... args) const {
			// Composite property 
			if (const auto* refs = TryGetCompositeProperyValues(id)) {
				if (refs->size() != sizeof...(Args)) {
#ifndef NDEBUG
					Log::Error(
						"SDLCore::UI::UIStyleState::IsDifferent: Composite property '{}' expects {} values, got {}",
						id, refs->size(), sizeof...(Args));
#endif
					return true;
				}

				size_t index = 0;
				bool different = false;

				// Fold expression over args
				(([&] {
					const UIPropertyID subID = (*refs)[index++];
					const PropertyValue* propValue = TryGetPropValue(subID);
					if (!propValue) {
						different = true;
						return;
					}

					PropertyValue incoming(std::forward<Args>(args));

					if (!propValue->IsSameType(incoming.GetType()) ||
						propValue->GetVariant() != incoming.GetVariant() ||
						propValue->GetIsImportant() != incoming.GetIsImportant()) {
						different = true;
					}
				}()), ...);

				return different;
			}

			// Single property 
			static_assert(sizeof...(Args) == 1,
				"Non-composite properties accept exactly one value");

			const PropertyValue* propValue = TryGetPropValue(id);
			if (!propValue) {
#ifndef NDEBUG
				Log::Error(
					"SDLCore::UI::UIStyleState::IsDifferent: Could not check diff, property '{}' not found",
					id);
#endif
				return false;
			}

			PropertyValue incoming(std::forward<Args>(args)...);

			if (!propValue->IsSameType(incoming.GetType()))
				return true;

			return propValue->GetVariant() != incoming.GetVariant()
				|| propValue->GetIsImportant() != incoming.GetIsImportant();
		}

		template<typename... Args>
		bool SetValue(UIPropertyID id, Args&&... args) {
			static_assert(
				(IsPropertyValueArg<std::decay_t<Args>>::value && ...),
				"Unsupported value type for SetValue"
				);

			constexpr size_t argCount = sizeof...(Args);

			PropertyValue* propValue = TryGetPropValue(id);
			if (propValue) {
				// Normal property: exactly one value expected
				static_assert(argCount == 1,
					"Non-composite properties accept exactly one PropertyValue");

				return SetSingleValue(id, std::forward<Args>(args)...);
			}

			// Composite property handling
			const std::vector<UIPropertyID>* refs = TryGetCompositeProperyValues(id);
			if (!refs) {
#ifndef NDEBUG
				Log::Error(
					"SDLCore::UI::UIStyleState: Property '{}' not found",
					id
				);
#endif
				return false;
			}

			if (refs->size() != argCount) {
#ifndef NDEBUG
				Log::Error(
					"SDLCore::UI::UIStyleState: Composite property '{}' expects {} values, got {}",
					id, refs->size(), argCount
				);
#endif
				return false;
			}

			bool result = true;
			size_t index = 0;

			// Expand arguments and map them to sub-properties
			((result &= SetSingleValue((*refs)[index++], args)), ...);

			return result;
		}

		// sets the last prop that was set to value
		bool SetImportant(bool value);

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
		UIPropertyID m_lastPropSet;
		std::unordered_map<UIPropertyID, PropertyValue> m_properties;
		std::unordered_map<UIPropertyID, std::vector<UIPropertyID>> m_compositeRefs;/* < composite id to list of other props*/

		const PropertyValue* TryGetPropValue(UIPropertyID id) const;
		PropertyValue* TryGetPropValue(UIPropertyID id);

		const std::vector<UIPropertyID>* TryGetCompositeProperyValues(UIPropertyID id) const;

		bool SetSingleValue(UIPropertyID id, const PropertyValue& value);
	};
	
}