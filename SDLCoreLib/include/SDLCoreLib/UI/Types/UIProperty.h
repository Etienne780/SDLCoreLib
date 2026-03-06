#pragma once
#include <string>
#include "UI/Types/UITypes.h"
#include "UI/Types/PropertyValue.h"

namespace SDLCore::UI {

    /**
    * @brief Describes a UI style property and its static metadata.
    *
    * UIProperty represents the definition of a property (name, type, default value),
    * not the value itself. It contains no runtime or layout logic and is shared
    * across all style states and UI nodes.
    */
    class UIProperty {
    public:
        /**
        * @brief Constructs an empty, invalid property definition.
        *
        * Intended only for internal use or placeholder initialization.
        */
        UIProperty() = default;

        /**
        * @brief Constructs a property definition.
        *
        * @param name property name (e.g. "width", "overflow").
        * @param description Short description of the property's purpose.
        * @param type Expected value type of the property.
        * @param defaultVaue Default value used when the property is not explicitly set.
        */
        UIProperty(const std::string& name,
            const std::string& description,
            PropertyValue::Type type,
            PropertyValue::ValueVariant defaultVaue);

        template<typename... SubProps>
        UIProperty(const std::string& name,
            const std::string& description,
            SubProps... subProperties)
            : m_name(name),
            m_description(description),
            m_isComposite(true)
        {
            static_assert((std::is_same_v<SubProps, UIPropertyID> && ...),
                "Composite properties must reference UIPropertyID values");

            (m_compositeProperties.push_back(subProperties), ...);
        }

        bool IsComposite() const;

        /**
        * @brief Returns the name of the property.
        */
        const std::string& GetName() const;

        /**
        * @brief Returns the description of the property.
        */
        const std::string& GetDesc() const;

        /**
        * @brief Returns the expected value type of the property.
        */
        PropertyValue::Type GetType() const;

        /**
        * @brief Returns the default value of the property.
        *
        * This value is used when no explicit value is provided by a style or override.
        */
        PropertyValue::ValueVariant GetDefaultValue() const;

        const std::vector<UIPropertyID>& GetCompositePropertyIDs() const;

    private:
        std::string m_name = "UNKOWN";
        std::string m_description = "-";
        PropertyValue::Type m_type = PropertyValue::Type::INT;
        PropertyValue::ValueVariant m_defaultVaue;

        /**
        * @brief Indicates whether this property is a composite (shorthand) property.
        *
        * Composite properties do not have direct layout or rendering logic.
        * Instead, they expand into one or more concrete properties when applied
        * (e.g. "overflow" setting "overflowX" and "overflowY").
        *
        * The value of a composite property is not stored or queried at runtime;
        * it is only used during style application or parsing.
        */
        bool m_isComposite = false;

        /**
        * @brief List of concrete properties affected by this composite property.
        *
        * This list defines which individual properties are set when the composite
        * property is applied. Each entry represents a concrete property that receives
        * a derived value from the composite property.
        *
        * This vector is only valid and used when @ref m_isComposite is true.
        */
        std::vector<UIPropertyID> m_compositeProperties;
    };

}