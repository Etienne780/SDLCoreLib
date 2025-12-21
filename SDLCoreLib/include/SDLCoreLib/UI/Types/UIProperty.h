#pragma once
#include <string>
#include "UI/Types/UITypes.h"
#include "UI/Types/PropertyValue.h"

namespace SDLCore::UI {

	class UIProperty {
	public:
		UIProperty() = default;
		UIProperty(const std::string& name,
			const std::string& description,
			PropertyValue::Type type,
			PropertyValue::ValueVariant defaultVaue);

		const std::string& GetName() const;
		const std::string& GetDesc() const;
		PropertyValue::Type GetType() const;
		PropertyValue::ValueVariant GetDefaultValue() const;

	private:
		std::string m_name = "UNKOWN";
		std::string m_description = "-";
		PropertyValue::Type m_type = PropertyValue::Type::INT;
		PropertyValue::ValueVariant m_defaultVaue;
	};

}