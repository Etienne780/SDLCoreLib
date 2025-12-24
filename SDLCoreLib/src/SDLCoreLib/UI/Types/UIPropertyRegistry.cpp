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
            "Width unit (PX, percentage, etc.)",
            PropertyValue(static_cast<int>(UISizeUnit::PX))
        );

        heightUnit = RegisterProperty(
            "height_unit",
            "Height unit (PX, percentage, etc.)",
            PropertyValue(static_cast<int>(UISizeUnit::PX))
        );

        width = RegisterProperty(
            "width",
            "Element width (interpreted using size_unit_w)",
            PropertyValue(0.0f)
        );

        height = RegisterProperty(
            "height",
            "Element height (interpreted using size_unit_h)",
            PropertyValue(0.0f)
        );

        padding = RegisterProperty(
            "padding",
            "Inner spacing (Top, Left, Bottom, Right)",
            PropertyValue(Vector4(0))
        );

        margin = RegisterProperty(
            "margin",
            "Outer spacing (Top, Left, Bottom, Right)",
            PropertyValue(Vector4(0))
        );

        layoutDirection = RegisterProperty(
            "layout_direction",
            "Child layout flow direction (row / column)",
            PropertyValue(UILayoutDirection::ROW)
        );

        alignHorizontal = RegisterProperty(
            "align_horizontal",
            "Horizontal alignment of child elements",
            PropertyValue(UIAlignment::START)
        );

        alignVertical = RegisterProperty(
            "align_vertical",
            "Vertical alignment of child elements",
            PropertyValue(UIAlignment::START)
        );

        backgroundTexture = RegisterProperty(
            "background_texture",
            "Background texture identifier",
            PropertyValue(UITextureID{})
        );

        backgroundColor = RegisterProperty(
            "background_color",
            "Background color (RGBA)",
            PropertyValue(Vector4(255))
        );

        borderColor = RegisterProperty(
            "border_color",
            "Border color (RGBA)",
            PropertyValue(Vector4(255))
        );

        borderWidth = RegisterProperty(
            "border_width",
            "Border width in pixels",
            PropertyValue(0.0f)
        );

        borderInset = RegisterProperty(
            "border_inset",
            "Render border inside the element bounds",
            PropertyValue(false)
        );


        // ============ Other ============

        hitTestEnabled = RegisterProperty(
            "hit_test_enabled",
            "Enables or disables event hit-testing for this element. "
            "If false, the element is visible but does not receive interaction events; events pass through. Commonly used for text.",
            PropertyValue(true)
        );

        notInteractible = RegisterProperty(
            "not_interactible",
            "Disables all interaction for this element. "
            "If true, the element will not respond to any events (clicks, hover, etc.) "
            "and behaves as disabled.",
            PropertyValue(false)
        );

        // ============ Text ============

        font = RegisterProperty(
            "font",
            "Font identifier used for text rendering",
            PropertyValue(UIFontID{ 0 })
        );

        fontSize = RegisterProperty(
            "font_size",
            "Font size in pixels",
            PropertyValue(24.0f)
        );

        textColor = RegisterProperty(
            "text_color",
            "Text color (RGBA)",
            PropertyValue(Vector4(255))
        );

        m_basePropertiesRegistered = true;
    }

}