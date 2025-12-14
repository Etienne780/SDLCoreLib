#pragma once
#include <CoreLib/Math/Vector2.h>

#include "UI/Types/UITypes.h"

namespace SDLCore::UI {

	class UIStyleState {
	public:
		UIStyleState();
		void SetValue(UIPropertyID id, PropertyValue value, bool important = false);

	private:
		std::unordered_map<UIPropertyID, PropertyValue> m_properties;
	};
	
}