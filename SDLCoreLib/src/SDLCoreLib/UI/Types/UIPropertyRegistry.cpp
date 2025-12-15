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

        sizeUnitW = RegisterProperty("size_unit_w", "width size unit",
            PropertyValue(static_cast<int>(UISizeUnit::PX)));

        sizeUnitH = RegisterProperty("size_unit_h", "height size unit",
            PropertyValue(static_cast<int>(UISizeUnit::PX)));

        width = RegisterProperty("width", "width of an element", 
            PropertyValue(0.0f));

        height = RegisterProperty("height", "height of an element", 
            PropertyValue(0.0f));

        padding = RegisterProperty("padding", "inner spacing", 
            PropertyValue(Vector4(0)));

        margin = RegisterProperty("margin", "outer spacing", 
            PropertyValue(Vector4(0)));

        layoutDirection = RegisterProperty("layout_direction", "layout flow direction", 
            PropertyValue(UILayoutDirection::ROW));

        alignHorizontal = RegisterProperty("align_horizontal", "horizontal alignment",
            PropertyValue(UIAlignment::START));

        alignVertical = RegisterProperty("align_vertical", "vertical alignment",
            PropertyValue(UIAlignment::START));


        // not implemented
        backgroundTexture = RegisterProperty("background_texture", "background texture id",
            PropertyValue(UITextureID{}));

        backgroundColor = RegisterProperty("background_color", "background color",
            PropertyValue(Vector4(255)));

        borderColor = RegisterProperty("border_color", "border color",
            PropertyValue(Vector4(255)));

        borderThickness = RegisterProperty("border_thickness", "border thickness",
            PropertyValue(0.0f));

        borderThickness = RegisterProperty("border_thickness", "border thickness",
            PropertyValue(true));

        // ============ Text ============
        font = RegisterProperty("font", "font id", 
            PropertyValue(UIFontID{ 0 }));// default font

        fontSize = RegisterProperty("font_size", "font size", 
            PropertyValue(16.0f));

        textColor = RegisterProperty("text_color", "text color", 
            PropertyValue(Vector4(255)));

        m_basePropertiesRegistered = true;
    }

}