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
		bool IsImportantDiff(UIPropertyID id, bool value) const;

		// @return true if the incoming value(s) differ from the currently stored value(s)
		template<typename... Args>
		bool IsDifferent(UIPropertyID id, Args&&... args) const {
			constexpr size_t argCount = sizeof...(Args);

			// Composite property
			if (const auto* refs = TryGetCompositeProperyValues(id)) {
				if (refs->size() != argCount) {
#ifndef NDEBUG
					Log::Error(
						"Composite property '{}' expects {} values, got {}",
						id, refs->size(), argCount);
#endif
					return true;
				}

				size_t index = 0;
				bool different = false;

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
			if constexpr (argCount == 1) {
				const PropertyValue* propValue = TryGetPropValue(id);
				if (!propValue) {
#ifndef NDEBUG
					Log::Error(
						"Property '{}' not found",
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
			return false;
		}

		template<typename... Args>
		bool SetValue(UIPropertyID id, Args&&... args) {
			static_assert(
				(IsPropertyValueArg<std::decay_t<Args>>::value && ...),
				"Unsupported value type for SetValue"
				);

			constexpr size_t argCount = sizeof...(Args);

			if constexpr (argCount == 1) {
				if (PropertyValue* propValue = TryGetPropValue(id)) {
					return SetSingleValue(id, std::forward<Args>(args)...);
				}
			}

			// Composite property handling
			const std::vector<UIPropertyID>* refs = TryGetCompositeProperyValues(id);
			if (!refs) {
#ifndef NDEBUG
				Log::Error("Property '{}' not found", id);
#endif
				return false;
			}

			if (refs->size() != argCount) {
#ifndef NDEBUG
				Log::Error(
					"Composite property '{}' expects {} values, got {}",
					id, refs->size(), argCount
				);
#endif
				return false;
			}

			bool result = true;
			size_t index = 0;
			((result &= SetSingleValue((*refs)[index++], std::forward<Args>(args))), ...);

			// resets last prop set to composite id instead of the single id
			m_lastPropSet = id;
			return result;
		}

		// sets the last prop that was set to value
		bool SetImportant(bool value);
		bool SetImportant(UIPropertyID id, bool value);

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