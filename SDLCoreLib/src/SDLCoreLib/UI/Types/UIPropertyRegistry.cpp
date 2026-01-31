#include <CoreLib/MatchSet.h>
#include "UI/Types/UIPropertyRegistry.h"

namespace SDLCore::UI {

	UIPropertyID UIPropertyRegistry::RegisterProperty(const std::string& name,
		const std::string& description,
		PropertyValue defaultValue)
	{
		UIPropertyID newID = UIPropertyID(m_idManager.GetNewUniqueIdentifier());
		m_registeredProperties[newID] = UIProperty(name, 
			(description.empty()) ? "-" : description, 
			defaultValue.GetType(),
			defaultValue.GetVariant());
		return newID;
	}

	UIProperty* UIPropertyRegistry::TryGetProperty(UIPropertyID id) {
		auto it = m_registeredProperties.find(id);
		return (it == m_registeredProperties.end()) ? nullptr : &it->second;
	}
	
	const std::unordered_map<UIPropertyID, UIProperty>& UIPropertyRegistry::GetAllProperties() {
		return m_registeredProperties;
	}

    void UIPropertyRegistry::RegisterBaseProperties() {
        if (m_basePropertiesRegistered)
            return;

        using namespace SDLCore::UI::Properties;

        widthUnit = RegisterProperty(
            "width_unit",
            "Width unit (PX, percentage, etc.). Default: UISizeUnit::PX",
            PropertyValue(static_cast<int>(UISizeUnit::PX))
        );

        heightUnit = RegisterProperty(
            "height_unit",
            "Height unit (PX, percentage, etc.). Default: UISizeUnit::PX",
            PropertyValue(static_cast<int>(UISizeUnit::PX))
        );

        sizeUnit = RegisterCompositeProperty(
            "sizeUnit",
            "Shorthand property for both horizontal and vertical size units. Setting this applies the value to both widthUnit and heightHeight.",
            widthUnit,
            heightUnit
        );

        width = RegisterProperty(
            "width",
            "Element width (interpreted using size_unit_w). Default: 0.0f",
            PropertyValue(0.0f)
        );

        height = RegisterProperty(
            "height",
            "Element height (interpreted using size_unit_h). Default: 0.0f",
            PropertyValue(0.0f)
        );

        size = RegisterCompositeProperty(
            "size",
            "Shorthand property for both horizontal and vertical sizes. Setting this applies the value to both width and height.",
            width,
            height
        );

        padding = RegisterProperty(
            "padding",
            "Inner spacing (Top, Left, Bottom, Right). Default: Vector4(0.0f, 0.0f, 0.0f, 0.0f)",
            PropertyValue(Vector4(0))
        );

        margin = RegisterProperty(
            "margin",
            "Outer spacing (Top, Left, Bottom, Right). Default: Vector4(0.0f, 0.0f, 0.0f, 0.0f)",
            PropertyValue(Vector4(0))
        );

        layoutDirection = RegisterProperty(
            "layout_direction",
            "Child layout flow direction (row / column). Default: UILayoutDirection::ROW",
            PropertyValue(UILayoutDirection::ROW)
        );

        alignHorizontal = RegisterProperty(
            "align_horizontal",
            "Horizontal alignment of child elements. Default: UIAlignment::START",
            PropertyValue(UIAlignment::START)
        );
        
        alignVertical = RegisterProperty(
            "align_vertical",
            "Vertical alignment of child elements. Default: UIAlignment::START",
            PropertyValue(UIAlignment::START)
        );

        align = RegisterCompositeProperty(
            "align",
            "Shorthand property for both horizontal and vertical alignemt. Setting this will apply the value to both alignHorizontal and alignVertical.",
            alignHorizontal,
            alignVertical
        );

        positionType = RegisterProperty(
            "positionType",
            "Defines how this element is positioned within its parent layout (int, UIPositionType). Default: UIPositionType::FLOW",
            PropertyValue(UIPositionType::FLOW)
        );

        top = RegisterProperty(
            "top",
            "Absolute offset from the top edge of the parent element (float). Only applicable when positionType == UIPositionType::ABSOLUTE.",
            PropertyValue(0.0f)
        );

        left = RegisterProperty(
            "left",
            "Absolute offset from the left edge of the parent element (float). Only applicable when positionType == UIPositionType::ABSOLUTE.",
            PropertyValue(0.0f)
        );

        bottom = RegisterProperty(
            "bottom",
            "Absolute offset from the bottom edge of the parent element (float). Only applicable when positionType == UIPositionType::ABSOLUTE.",
            PropertyValue(0.0f)
        );

        right = RegisterProperty(
            "right",
            "Absolute offset from the right edge of the parent element (float). Only applicable when positionType == UIPositionType::ABSOLUTE.",
            PropertyValue(0.0f)
        );

        topLeft = RegisterCompositeProperty(
            "top_left",
            "Shorthand property for absolute positioning using top and left offsets. Only applicable when positionType == UIPositionType::ABSOLUTE.",
            top,
            left
        );

        topRight = RegisterCompositeProperty(
            "top_right",
            "Shorthand property for absolute positioning using top and right offsets. Only applicable when positionType == UIPositionType::ABSOLUTE.",
            top,
            right
        );

        bottomLeft = RegisterCompositeProperty(
            "bottom_left",
            "Shorthand property for absolute positioning using bottom and left offsets. Only applicable when positionType == UIPositionType::ABSOLUTE.",
            bottom,
            left
        );

        bottomRight = RegisterCompositeProperty(
            "bottom_right",
            "Shorthand property for absolute positioning using bottom and right offsets. Only applicable when positionType == UIPositionType::ABSOLUTE.",
            bottom,
            right
        );

        topUnit = RegisterProperty(
            "top_unit",
            "Unit used to interpret the 'top' absolute offset value (UISizeUnit).",
            PropertyValue(UISizeUnit::PX)
        );

        leftUnit = RegisterProperty(
            "left_unit",
            "Unit used to interpret the 'left' absolute offset value (UISizeUnit).",
            PropertyValue(UISizeUnit::PX)
        );

        bottomUnit = RegisterProperty(
            "bottom_unit",
            "Unit used to interpret the 'bottom' absolute offset value (UISizeUnit).",
            PropertyValue(UISizeUnit::PX)
        );

        rightUnit = RegisterProperty(
            "right_unit",
            "Unit used to interpret the 'right' absolute offset value (UISizeUnit).",
            PropertyValue(UISizeUnit::PX)
        );

        topLeftUnit = RegisterCompositeProperty(
            "top_left_unit",
            "Shorthand unit property for both 'top' and 'left' absolute offsets.",
            topUnit,
            leftUnit
        );

        topRightUnit = RegisterCompositeProperty(
            "top_right_unit",
            "Shorthand unit property for both 'top' and 'right' absolute offsets.",
            topUnit,
            rightUnit
        );

        bottomLeftUnit = RegisterCompositeProperty(
            "bottom_left_unit",
            "Shorthand unit property for both 'bottom' and 'left' absolute offsets.",
            bottomUnit,
            leftUnit
        );

        bottomRightUnit = RegisterCompositeProperty(
            "bottom_right_unit",
            "Shorthand unit property for both 'bottom' and 'right' absolute offsets.",
            bottomUnit,
            rightUnit
        );
        
        backgroundTexture = RegisterProperty(
            "background_texture",
            "Background texture identifier. Default: UITextureID(SDLCORE_INVALID_ID)",
            PropertyValue(UITextureID{ SDLCORE_INVALID_ID })
        );

        backgroundColor = RegisterProperty(
            "background_color",
            "Background color (RGBA). Default: Vector4(0.0f, 0.0f, 0.0f, 0.0f)",
            PropertyValue(Vector4(0))
        );

        borderColor = RegisterProperty(
            "border_color",
            "Border color (RGBA). Default: Vector4(255.0f, 255.0f, 255.0f, 255.0f)",
            PropertyValue(Vector4(255))
        );

        borderWidth = RegisterProperty(
            "border_width",
            "Border width in pixels. Default: 0.0f",
            PropertyValue(0.0f)
        );

        borderInset = RegisterProperty(
            "border_inset",
            "Render border inside the element bounds. Default: false",
            PropertyValue(false)
        );

        borderAffectsLayout = RegisterProperty(
            "border_affects_layout",
            "Controls whether the border contributes to layout size and child positioning. Default: true",
            PropertyValue(true)
        );

        hideOverflowX = RegisterProperty(
            "hide_overflow_x",
            "Controls whether content overflowing the element horizontally is hidden (bool). Default: false.",
            PropertyValue(false)
        );

        hideOverflowY = RegisterProperty(
            "hide_overflow_y",
            "Controls whether content overflowing the element vertically is hidden (bool). Default: false.",
            PropertyValue(false)
        );

        hideOverflow = RegisterCompositeProperty(
            "hide_overflow",
            "Shorthand property for hidding both horizontal and vertical overflow. Setting this will apply the value to both hideOverflowX and hideOverflowY.",
            hideOverflowX,
            hideOverflowY
        );

        visualOffsetX = RegisterProperty(
            "visual_offset_x",
            "Visual horizontal offset applied after layout (float). Does not affect layout. Default: 0.0f.",
            PropertyValue(0.0f)
        );

        visualOffsetY = RegisterProperty(
            "visual_offset_y",
            "Visual vertical offset applied after layout (float). Does not affect layout. Default: 0.0f.",
            PropertyValue(0.0f)
        );

        visualOffset = RegisterCompositeProperty(
            "visual_offset",
            "Shorthand for visualOffsetX and visualOffsetY.",
            visualOffsetX,
            visualOffsetY
        );

        visualOffsetXUnit = RegisterProperty(
            "visual_offset_x_unit",
            "Unit for visualOffsetX (UISizeUnit). Default: UISizeUnit::PX",
            PropertyValue(UISizeUnit::PX)
        );

        visualOffsetYUnit = RegisterProperty(
            "visual_offset_y_unit",
            "Unit for visualOffsetY (UISizeUnit). Default: UISizeUnit::PX",
            PropertyValue(UISizeUnit::PX)
        );

        visualOffsetUnit = RegisterCompositeProperty(
            "visual_offset_unit",
            "Shorthand for visualOffsetXUnit and visualOffsetYUnit.",
            visualOffsetXUnit,
            visualOffsetYUnit
        );

        visualScaleX = RegisterProperty(
            "visual_scale_x",
            "Visual scale factor on the X axis. Does not affect layout (float). Default: 0.0f",
            PropertyValue(1.0f)
        );

        visualScaleY = RegisterProperty(
            "visual_scale_y",
            "Visual scale factor on the Y axis. Does not affect layout (float). Default: 0.0f",
            PropertyValue(1.0f)
        );

        visualScale = RegisterCompositeProperty(
            "visual_scale",
            "Shorthand for visualScaleX and visualScaleY.",
            visualScaleX,
            visualScaleY
        );

        visualScaleXUnit = RegisterProperty(
            "visual_scale_x_unit",
            "Unit for visualScaleX (UISizeUnit). Default: UISizeUnit::PX",
            PropertyValue(UISizeUnit::PX)
        );

        visualScaleYUnit = RegisterProperty(
            "visual_scale_y_unit",
            "Unit for visualScaleY (UISizeUnit). Default: UISizeUnit::PX",
            PropertyValue(UISizeUnit::PX)
        );

        visualScaleUnit = RegisterCompositeProperty(
            "visual_scale_unit",
            "Shorthand for visualScaleXUnit and visualScaleYUnit.",
            visualScaleXUnit,
            visualScaleYUnit
        );

        // ============ Other ============

        pointerEvents = RegisterProperty(
            "pointer_events",
            "Controls whether this element receives pointer interaction events. "
            "If false, the element does not receive hover, click, or any other pointer events, "
            "but it is still rendered and participates in layout.",
            PropertyValue(true)
        );

        hitTestTransparent = RegisterProperty(
            "hit_test_transparent",
            "Allows pointer events to pass through this element to elements behind it. "
            "If true, this element does not block hit-testing for underlying elements, "
            "even though it may still receive its own pointer events.",
            PropertyValue(false)
        );

        propagateStateToChildren = RegisterProperty(
            "propagate_state_to_children",
            "Forces all child elements to inherit this element's interaction and visual state "
            "(e.g. hover, active, disabled). "
            "Useful for compound widgets such as buttons with icons and labels.",
            PropertyValue(false)
        );

        disabled = RegisterProperty(
            "disabled",
            "Marks this element as disabled. "
            "A disabled element does not react to interaction states such as hover, active, or focus. "
            "Visual appearance may reflect the disabled state. "
            "Pointer event handling is controlled separately via pointer_events and hit_test_transparent.",
            PropertyValue(false)
        );
        
        duration = RegisterProperty(
            "duration",
            "Transitions number, colors other values get instantly set",
            PropertyValue(0.0f)
        );

        durationUnit = RegisterProperty(
            "duration_unit",
            "Unit for transition duration",
            PropertyValue(static_cast<int>(UITimeUnit::SECONDS))
        );

        durationEasing = RegisterProperty(
            "duration_easing",
            "Easing function applied to transitions (linear, ease-in, ease-out, etc.)",
            PropertyValue(static_cast<int>(UIEasing::Linear))
        );
        
        // ============ Text ============

        font = RegisterProperty(
            "font",
            "Font identifier used for text rendering. Default: UIFontID{ 0 } (Default font)",
            PropertyValue(UIFontID{ 0 })
        );

        textSize = RegisterProperty(
            "text_size",
            "Text size in pixels. Default: 24.0f",
            PropertyValue(24.0f)
        );

        textColor = RegisterProperty(
            "text_color",
            "Text color (RGBA). Default: Vector4(255.0f, 255.0f, 255.0f, 255.0f)",
            PropertyValue(Vector4(255))
        );

        m_basePropertiesRegistered = true;
    }

}