#pragma once
#include <unordered_map>
#include "IDManager.h"
#include "UI/Types/UIProperty.h"

namespace SDLCore::UI {

	class  UIPropertyRegistry {
	public:
		UIPropertyRegistry() = delete;
        static void RegisterBaseProperties();
		static UIPropertyID RegisterProperty(const std::string& name, 
			const std::string& description, 
			PropertyValue defaultValue);

		static UIProperty* TryGetProperty(UIPropertyID id);
		static const std::unordered_map<UIPropertyID, UIProperty>& GetAllProperties();

	private:
		static inline bool m_basePropertiesRegistered = false;
		static inline IDManager m_idManager;
		static inline std::unordered_map<UIPropertyID, UIProperty> m_registeredProperties;
	};

}

namespace SDLCore::UI::Properties {
    // size / layout
    inline UIPropertyID sizeUnitW;    // width size unit (PX, %, etc.)
    inline UIPropertyID sizeUnitH;    // height size unit (PX, %, etc.)

    inline UIPropertyID width;        // width of an element
    inline UIPropertyID height;       // height of an element

    inline UIPropertyID padding;      // inner spacing
    inline UIPropertyID margin;       // outer spacing

    inline UIPropertyID layoutDirection;  // layout flow direction (row / column)
    inline UIPropertyID alignHorizontal;  // horizontal alignment (start / center / end)
    inline UIPropertyID alignVertical;    // vertical alignment (start / center / end)

    // visuals
    inline UIPropertyID backgroundTexture;  // background texture id
    inline UIPropertyID backgroundColor;    // background color
    inline UIPropertyID borderColor;        // border color
    inline UIPropertyID borderThickness;    // border thickness

    // text
    inline UIPropertyID font;         // font id
    inline UIPropertyID fontSize;     // font size
    inline UIPropertyID textColor;    // text color
}
