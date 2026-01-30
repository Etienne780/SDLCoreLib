#pragma once
#include <unordered_map>
#include "UI/Types/UIProperty.h"

namespace SDLCore::UI {

	class  UIPropertyRegistry {
	public:
		UIPropertyRegistry() = delete;
        static void RegisterBaseProperties();
		static UIPropertyID RegisterProperty(const std::string& name, 
			const std::string& description, 
			PropertyValue defaultValue);

		template<typename... SubProps>
		static UIPropertyID RegisterCompositeProperty(
			const std::string& name,
			const std::string& description,
			SubProps... subProperties)
		{
			static_assert((std::is_same_v<SubProps, UIPropertyID> && ...),
				"Composite properties must be constructed from UIPropertyID values");

			UIPropertyID newID = UIPropertyID(m_idManager.GetNewUniqueIdentifier());
			m_registeredProperties.emplace(
				newID,
				UIProperty(
					name,
					description.empty() ? "-" : description,
					subProperties...
				)
			);

			return newID;
		}

		static UIProperty* TryGetProperty(UIPropertyID id);
		static const std::unordered_map<UIPropertyID, UIProperty>& GetAllProperties();

	private:
		static inline bool m_basePropertiesRegistered = false;
		static inline SDLCoreIDManager m_idManager;
		static inline std::unordered_map<UIPropertyID, UIProperty> m_registeredProperties;
	};

}

namespace SDLCore::UI::Properties {

	#pragma region ================= Size / Layout =================

	/*
	* @brief Width size unit (int, UISizeUnit)
	*
	* Defines how the width value is interpreted (pixels, percentage, etc.).
	*
	* Default: UISizeUnit::PX
	*
	* Usage: style.SetValue<int>(UISizeUnit::PX)
	*/
	inline UIPropertyID widthUnit;

	/*
	* @brief Height size unit (int, UISizeUnit)
	*
	* Defines how the height value is interpreted (pixels, percentage, etc.).
	*
	* Default: UISizeUnit::PX
	*
	* Usage: style.SetValue<int>(UISizeUnit::PX)
	*/
	inline UIPropertyID heightUnit;

	/*
	* @brief Shorthand property for both horizontal and vertical size units (composite)
	*
	* Setting this will apply the value to both widthUnit and heightUnit.
	*
	* Default: UISizeUnit::PX, UISizeUnit::PX
	*
	* Composite layout: widthUnit, heightHeight
	*
	* Usage: style.SetValue(UISizeUnit::PX, UISizeUnit::PX)
	*/
	inline UIPropertyID sizeUnit;

	/*
	* @brief Element width (float)
	*
	* Interpreted according to widthUnit.
	*
	* Default: 0.0f
	*
	* Usage: style.SetValue<float>(100.0f)
	*/
	inline UIPropertyID width;

	/*
	* @brief Shorthand property for both horizontal and vertical sizes (composite)
	*
	* Setting this will apply the value to both width and height.
	*
	* Default: 0.0f, 0.0f
	*
	* Composite layout: width, height
	*
	* Usage: style.SetValue(10.0f, 20.0f)
	*/
	inline UIPropertyID size;

	/*
	* @brief Element height (float)
	*
	* Interpreted according to heightUnit.
	*
	* Default: 0.0f
	*
	* Usage: style.SetValue<float>(50.0f)
	*/
	inline UIPropertyID height;

	/*
	* @brief Inner spacing (Vector4)
	*
	* Order: Top, Left, Bottom, Right.
	*
	* Default: Vector4(0, 0, 0, 0)
	*
	* Usage: style.SetValue<Vector4>(Vector4(top, left, bottom, right))
	*/
	inline UIPropertyID padding;

	/*
	* @brief Outer spacing (Vector4)
	*
	* Order: Top, Left, Bottom, Right.
	*
	* Default: Vector4(0, 0, 0, 0)
	*
	* Usage: style.SetValue<Vector4>(Vector4(top, left, bottom, right))
	*/
	inline UIPropertyID margin;

	/*
	* @brief Layout flow direction for child elements (int, UILayoutDirection)
	*
	* Controls how children are arranged inside this element.
	*
	* Default: UILayoutDirection::ROW
	*
	* Usage: style.SetValue<int>(UILayoutDirection::ROW)
	*/
	inline UIPropertyID layoutDirection;

	/*
	* @brief Horizontal alignment of children (int, UIAlignment)
	*
	* Affects child placement on the X axis inside this element.
	*
	* Default: UIAlignment::START
	*
	* Usage: style.SetValue<int>(UIAlignment::CENTER)
	*/
	inline UIPropertyID alignHorizontal;

	/*
	* @brief Vertical alignment of children (int, UIAlignment)
	*
	* Affects child placement on the Y axis inside this element.
	*
	* Default: UIAlignment::START
	*
	* Usage: style.SetValue<int>(UIAlignment::START)
	*/
	inline UIPropertyID alignVertical;

	/*
	* @brief Shorthand property for both horizontal and vertical alignemt (composite)
	*
	* Setting this will apply the value to both alignHorizontal and alignVertical.
	*
	* Default: UIAlignment::START, UIAlignment::START
	*
	* Composite layout: alignHorizontal, alignVertical
	*
	* Usage: style.SetValue(UIAlignment::START, UIAlignment::START)
	*/
	inline UIPropertyID align;

	/*
	* @brief (UIPositionType)
	*
	* Defines how this element is positioned within its parent layout (int, UIPositionType).
	*
	* FLOW:
	* 
	*   - Participates in normal layout flow
	* 
	*   - Position is fully determined by the parent layout
	*
	* RELATIVE:
	* 
	*   - Participates in layout flow
	* 
	*   - Applies a local offset relative to its flow position
	*
	* ABSOLUTE:
	* 
	*   - Removed from layout flow
	* 
	*   - Positioned relative to the nearest positioned ancestor
	*
	* Default: UIPositionType::FLOW
	*
	* Usage: style.SetValue<UIPositionType>(UIPositionType::RELATIVE);
	*/
	inline UIPropertyID positionType;

	/*
	* @brief Absolute offset from the top edge of the parent (float)
	*
	* Defines the vertical distance to the parent's top edge.
	* Only evaluated when positionType == UIPositionType::ABSOLUTE.
	*
	* Default: 0.0f
	*
	* Usage: style.SetValue(Prop::top, 20.0f)
	*/
	inline UIPropertyID top;

	/*
	* @brief Absolute offset from the left edge of the parent (float)
	*
	* Defines the horizontal distance to the parent's left edge.
	* Only evaluated when positionType == UIPositionType::ABSOLUTE.
	*
	* Default: 0.0f
	*
	* Usage: style.SetValue(Prop::left, 20.0f)
	*/
	inline UIPropertyID left;

	/*
	* @brief Absolute offset from the bottom edge of the parent (float)
	*
	* Defines the vertical distance to the parent's bottom edge.
	* Only evaluated when positionType == UIPositionType::ABSOLUTE.
	*
	* Default: 0.0f
	*
	* Usage: style.SetValue(Prop::bottom, 20.0f)
	*/
	inline UIPropertyID bottom;

	/*
	* @brief Absolute offset from the right edge of the parent (float)
	*
	* Defines the horizontal distance to the parent's right edge.
	* Only evaluated when positionType == UIPositionType::ABSOLUTE.
	*
	* Default: 0.0f
	*
	* Usage: style.SetValue(Prop::right, 20.0f)
	*/
	inline UIPropertyID right;

	/*
	* @brief Shorthand property for top and left absolute offsets (composite)
	*
	* Applies values to both top and left.
	* Only evaluated when positionType == UIPositionType::ABSOLUTE.
	*
	* Composite layout: top, left
	*
	* Default: 0.0f, 0.0f
	*
	* Usage: style.SetValue(Prop::topLeft, 20.0f, 20.0f)
	*/
	inline UIPropertyID topLeft;

	/*
	* @brief Shorthand property for top and right absolute offsets (composite)
	*
	* Applies values to both top and right.
	* Only evaluated when positionType == UIPositionType::ABSOLUTE.
	*
	* Composite layout: top, right
	*
	* Default: 0.0f, 0.0f
	*
	* Usage: style.SetValue(Prop::topRight, 20.0f, 20.0f)
	*/
	inline UIPropertyID topRight;

	/*
	* @brief Shorthand property for bottom and left absolute offsets (composite)
	*
	* Applies values to both bottom and left.
	* Only evaluated when positionType == UIPositionType::ABSOLUTE.
	*
	* Composite layout: bottom, left
	*
	* Default: 0.0f, 0.0f
	*
	* Usage: style.SetValue(Prop::bottomLeft, 20.0f, 20.0f)
	*/
	inline UIPropertyID bottomLeft;

	/*
	* @brief Shorthand property for bottom and right absolute offsets (composite)
	*
	* Applies values to both bottom and right.
	* Only evaluated when positionType == UIPositionType::ABSOLUTE.
	*
	* Composite layout: bottom, right
	*
	* Default: 0.0f, 0.0f
	*
	* Usage: style.SetValue(Prop::bottomRight, 20.0f, 20.0f)
	*/
	inline UIPropertyID bottomRight;

	/*
	* @brief Unit for the absolute 'top' offset (UISizeUnit)
	*
	* Defines how the 'top' offset value is interpreted
	* (pixels, percentage, etc.).
	*
	* Only evaluated when positionType == UIPositionType::ABSOLUTE.
	*
	* Default: UISizeUnit::PX
	*
	* Usage: style.SetValue(Prop::topUnit, UISizeUnit::PERCENTAGE)
	*/
	inline UIPropertyID topUnit;

	/*
	* @brief Unit for the absolute 'left' offset (UISizeUnit)
	*
	* Defines how the 'left' offset value is interpreted.
	*
	* Only evaluated when positionType == UIPositionType::ABSOLUTE.
	*
	* Default: UISizeUnit::PX
	*/
	inline UIPropertyID leftUnit;

	/*
	* @brief Unit for the absolute 'bottom' offset (UISizeUnit)
	*
	* Defines how the 'bottom' offset value is interpreted.
	*
	* Only evaluated when positionType == UIPositionType::ABSOLUTE.
	*
	* Default: UISizeUnit::PX
	*/
	inline UIPropertyID bottomUnit;

	/*
	* @brief Unit for the absolute 'right' offset (UISizeUnit)
	*
	* Defines how the 'right' offset value is interpreted.
	*
	* Only evaluated when positionType == UIPositionType::ABSOLUTE.
	*
	* Default: UISizeUnit::PX
	*/
	inline UIPropertyID rightUnit;

	/*
	* @brief Shorthand unit property for 'top' and 'left' offsets (composite)
	*
	* Setting this will apply the unit to both topUnit and leftUnit.
	*
	* Default: UISizeUnit::PX, UISizeUnit::PX
	*
	* Composite layout: topUnit, leftUnit
	*
	* Usage: style.SetValue(Prop::topLeftUnit, UISizeUnit::PERCENTAGE)
	*/
	inline UIPropertyID topLeftUnit;

	/*
	* @brief Shorthand unit property for 'top' and 'right' offsets (composite)
	*
	* Setting this will apply the unit to both topUnit and rightUnit.
	*
	* Default: UISizeUnit::PX, UISizeUnit::PX
	*
	* Composite layout: topUnit, rightUnit
	*/
	inline UIPropertyID topRightUnit;

	/*
	* @brief Shorthand unit property for 'bottom' and 'left' offsets (composite)
	*
	* Setting this will apply the unit to both bottomUnit and leftUnit.
	*
	* Default: UISizeUnit::PX, UISizeUnit::PX
	*
	* Composite layout: bottomUnit, leftUnit
	*/
	inline UIPropertyID bottomLeftUnit;

	/*
	* @brief Shorthand unit property for 'bottom' and 'right' offsets (composite)
	*
	* Setting this will apply the unit to both bottomUnit and rightUnit.
	*
	* Default: UISizeUnit::PX, UISizeUnit::PX
	*
	* Composite layout: bottomUnit, rightUnit
	*/
	inline UIPropertyID bottomRightUnit;


	#pragma endregion

	#pragma region ================= Visuals =======================

	/*
	* @brief Background texture ID (UITextureID)
	*
	* References a texture used as element background.
	*
	* Default: UITextureID{ SDLCORE_INVALID_ID } (no texture)
	*
	* Usage: style.SetValue<UITextureID>(UITextureID{0})
	*/
	inline UIPropertyID backgroundTexture;

	/*
	* @brief Background color (Vector4)
	*
	* RGBA color used when no background texture is present.
	*
	* Default: Vector4(255, 255, 255, 255)
	*
	* Usage: style.SetValue<Vector4>(Vector4(r, g, b, a))
	*/
	inline UIPropertyID backgroundColor;

	/*
	* @brief Border color (Vector4)
	*
	* RGBA color of the element border.
	*
	* Default: Vector4(255, 255, 255, 255)
	*
	* Usage: style.SetValue<Vector4>(Vector4(r, g, b, a))
	*/
	inline UIPropertyID borderColor;

	/*
	* @brief Border width (float)
	*
	* Width of the element border in pixels.
	*
	* Default: 0.0f
	*
	* Usage: style.SetValue<float>(2.0f)
	*/
	inline UIPropertyID borderWidth;

	/*
	* @brief Border inset mode (bool)
	*
	* Controls whether the border is rendered inside the element bounds.
	*
	* Default: true
	*
	* Usage: style.SetValue<bool>(true)
	*/
	inline UIPropertyID borderInset;

	/*
	* @brief Controls whether the border affects layout calculations (bool)
	*
	* If true, the border contributes to element size and child layout.
	* If false, the border is rendered visually only.
	*
	* Default: true
	*
	* Usage: style.SetValue<bool>(false)
	*/
	inline UIPropertyID borderAffectsLayout;

	/*
	* @brief Hide horizontal overflow (bool)
	*
	* Controls whether content overflowing the element horizontally is hidden.
	*
	* Default: false
	*
	* Usage: style.SetValue<bool>(true)
	*/
	inline UIPropertyID hideOverflowX;

	/*
	* @brief Hide vertical overflow (bool)
	*
	* Controls whether content overflowing the element vertically is hidden.
	*
	* Default: false
	*
	* Usage: style.SetValue<bool>(true)
	*/
	inline UIPropertyID hideOverflowY;

	/*
	* @brief Shorthand property for hidding both horizontal and vertical overflow (composite)
	*
	* Setting this will apply the value to both hideOverflowX and hideOverflowY.
	*
	* Default: false, false
	*
	* Composite layout: hideOverflowX, hideOverflowY
	*
	* Usage: style.SetValue(false, true)
	*/
	inline UIPropertyID hideOverflow;

	#pragma endregion

	#pragma region ================= Interaction ===================

	/*
	* @brief Controls whether the element receives pointer interaction events (bool)
	*
	* If false, the element does not receive hover, click, or any other pointer events,
	* but it may still be rendered and participate in layout.
	*
	* Default: true
	*
	* Usage: style.SetValue<bool>(true)
	*/
	inline UIPropertyID pointerEvents;

	/*
	* @brief Allows pointer events to pass through to elements below (bool)
	*
	* If true, this element does not block hit-testing for elements behind it,
	* even though it may still receive its own pointer events.
	*
	* Default: false
	*
	* Usage: style.SetValue<bool>(true)
	*/
	inline UIPropertyID hitTestTransparent;

	/*
	* @brief Propagates this element's state to all child elements (bool)
	*
	* If true, all children are forced to use the same interaction and visual state
	* as this element (e.g. hover, active, disabled).
	*
	* Default: false
	*
	* Usage: style.SetValue<bool>(true)
	*/
	inline UIPropertyID propagateStateToChildren;

	/*
	* @brief Marks the element as disabled (bool)
	*
	* A disabled element does not react to interaction states such as hover,
	* active or focus. Visual appearance may reflect the disabled state.
	*
	* Pointer event handling is controlled separately via pointerEvents
	* and hitTestTransparent.
	*
	* Default: false
	*
	* Usage: style.SetValue<bool>(true)
	*/
	inline UIPropertyID disabled;

	/*
	* @brief Transition duration (float). 
	* 
	* Transitions number, colors other values get instantly set
	* 
	* Default: 0.0f
	* 
	* Usage: style.SetValue<float>(0.0f)
	*/
	inline UIPropertyID duration;

	/*
	* @brief Unit for the transition duration.
	*
	* Determines whether 'duration'is interpreted in milliseconds or seconds.
	* 
	* Default: UITimeUnit::SECONDS
	*
	* Usage: style.SetValue<int>(UITimeUnit::SECONDS)
	*/
	inline UIPropertyID durationUnit;

	/*
	* @brief Easing function for transitions.
	*
	* Determines how values interpolate over time (linear, quadratic, cubic, sinus, etc.).
	*
	* Default: UIEasing::Linear
	*
	* Usage: style.SetValue<int>(UIEasing::EaseInOutSine)
	*/
	inline UIPropertyID durationEasing;

	#pragma endregion

	#pragma region ================= Text ==========================

	/*
	* @brief Font identifier (UIFontID)
	*
	* Defines which font is used for text rendering.
	*
	* Default: UIFontID{0}
	*
	* Usage: style.SetValue<UIFontID>(UIFontID{0})
	*/
	inline UIPropertyID font;

	/*
	* @brief Text size (float)
	*
	* Text size in pixels.
	*
	* Default: 16.0f
	*
	* Usage: style.SetValue<float>(16.0f)
	*/
	inline UIPropertyID textSize;

	/*
	* @brief Text color (Vector4)
	*
	* RGBA color of rendered text.
	*
	* Default: Vector4(255, 255, 255, 255)
	*
	* Usage: style.SetValue<Vector4>(Vector4(r, g, b, a))
	*/
	inline UIPropertyID textColor;

	#pragma endregion

}