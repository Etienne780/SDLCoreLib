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
        
        backgroundTexture = RegisterProperty(
            "background_texture",
            "Background texture identifier. Default: UITextureID(SDLCORE_INVALID_ID)",
            PropertyValue(UITextureID{})
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

        // ============ Other ============

        hitTestEnabled = RegisterProperty(
            "hit_test_enabled",
            "Enables or disables event hit-testing for this element. "
            "If false, the element is visible but does not receive interaction events; events pass through. Commonly used for text. Default: true",
            PropertyValue(true)
        );

        notInteractible = RegisterProperty(
            "not_interactible",
            "Disables all interaction for this element. "
            "If true, the element will not respond to any events (clicks, hover, etc.). Default: false"
            "and behaves as disabled.",
            PropertyValue(false)
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