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

    // ================= Size / Layout =================

    /*
    * @brief Width size unit (int, UISizeUnit)
    *
    * Usage: style.SetValue<int>(static_cast<int>(UISizeUnit::PX))
    */
    inline UIPropertyID sizeUnitW;

    /*
    * @brief Height size unit (int, UISizeUnit)
    *
    * Usage: style.SetValue<int>(static_cast<int>(UISizeUnit::PX))
    */
    inline UIPropertyID sizeUnitH;

    /*
    * @brief Width of an element (float)
    *
    * Usage: style.SetValue<float>(100.0f)
    */
    inline UIPropertyID width;

    /*
    * @brief Height of an element (float)
    *
    * Usage: style.SetValue<float>(50.0f)
    */
    inline UIPropertyID height;

    /*
    * @brief Inner spacing (Vector4)
    *
    * Usage: style.SetValue<Vector4>(Vector4(Top, Left, Bottom, Right))
    */
    inline UIPropertyID padding;

    /*
    * @brief Outer spacing (Vector4)
    *
    * Usage: style.SetValue<Vector4>(Vector4(Top, Left, Bottom, Right))
    */
    inline UIPropertyID margin;

    /*
    * @brief Layout flow direction for children (int, UILayoutDirection)
    *
    * Usage: style.SetValue<int>(static_cast<int>(UILayoutDirection::ROW))
    */
    inline UIPropertyID layoutDirection;

    /*
    * @brief Horizontal alignment of children (int, UIAlignment)
    *
    * Usage: style.SetValue<int>(static_cast<int>(UIAlignment::CENTER))
    */
    inline UIPropertyID alignHorizontal;

    /*
    * @brief Vertical alignment of children (int, UIAlignment)
    *
    * Usage: style.SetValue<int>(static_cast<int>(UIAlignment::START))
    */
    inline UIPropertyID alignVertical;


    // ================= Visuals =================

    /*
    * @brief Background texture ID (UITextureID)
    *
    * Usage: style.SetValue<UITextureID>(UITextureID{0})
    */
    inline UIPropertyID backgroundTexture;

    /*
    * @brief Background color (Vector4)
    *
    * Usage: style.SetValue<Vector4>(Vector4(255))
    */
    inline UIPropertyID backgroundColor;

    /*
    * @brief Border color (Vector4)
    *
    * Usage: style.SetValue<Vector4>(Vector4(255))
    */
    inline UIPropertyID borderColor;

    /*
    * @brief Border thickness (float)
    *
    * Usage: style.SetValue<float>(2.0f)
    */
    inline UIPropertyID borderThickness;


    // ================= Text =================

    /*
    * @brief Font ID (UIFontID)
    *
    * Usage: style.SetValue<UIFontID>(UIFontID{0})
    */
    inline UIPropertyID font;

    /*
    * @brief Font size (float)
    *
    * Usage: style.SetValue<float>(16.0f)
    */
    inline UIPropertyID fontSize;

    /*
    * @brief Text color (Vector4)
    *
    * Usage: style.SetValue<Vector4>(Vector4(255))
    */
    inline UIPropertyID textColor;

}