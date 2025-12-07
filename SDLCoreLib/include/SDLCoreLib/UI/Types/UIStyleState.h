#pragma once
#include <CoreLib/Math/Vector2.h>

#include "UI/Types/UITypes.h"

namespace SDLCore::UI {

	struct UIStyleState {

		PropertyValue sizeUnitW{ static_cast<int>(UISizeUnit::PX) };
		PropertyValue sizeUnitH{ static_cast<int>(UISizeUnit::PX) };
		PropertyValue width{ 0.0f };
		PropertyValue height{ 0.0f };

		PropertyValue padding{ Vector4(0) };
		PropertyValue margin{ Vector4(0) };

		PropertyValue layoutDirection{ static_cast<int>(UILayoutDirection::ROW) };
		PropertyValue alignmentHor{ static_cast<int>(UIAlignment::START) };
		PropertyValue alignmentVer{ static_cast<int>(UIAlignment::START) };

		// not set by default
		PropertyValue backgroundTexture{ 0 };

		PropertyValue backgroundColor{ Vector4(255) };
		PropertyValue borderColor{ Vector4(255) };
		PropertyValue borderThickness{ 0.0f };

		// uses the default font
		// PropertyValue font{ UIFontID(0) };
		// PropertyValue fontSize{ 16.0f };
		// PropertyValue textColor{ Vector4(255) };

	};
	
}