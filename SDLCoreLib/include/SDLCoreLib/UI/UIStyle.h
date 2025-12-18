#pragma once
#include <string>

#include "UI/Types/UIPropertyRegistry.h"
#include "UI/Types/UITypes.h"
#include "UI/Types/UIStyleState.h"

namespace SDLCore::UI {

	class UIStyle {
	public:
		UIStyle();
		UIStyle(const std::string& name);
		UIStyle(std::string&& name);
		
		/*
		* not finished
		*/
		std::string ToString() const;

		/*
		* @brief Merges this style on top of outStyle
		*/
		void Merge(UIStyle& outStyle) const;

		std::string GetName() const;
		UIStyleState GetStyleState(UIState state);
		const UIStyleState& GetStyleState(UIState state) const;

		/*
		* @brief Sets the state that will be used when setting properties
		*/
		UIStyle& SetActiveState(UIState state);

		// all properties registered by the UIPropertyRegistry should be in the SDLCore::UI::Properties namespace
		UIStyle& SetValue(UIPropertyID attID, PropertyValue value, bool important = false);

	private:
		std::string m_name = "UNKOWN";
		UIState m_currentState = UIState::NORMAL;
		mutable std::unordered_map<UIState, UIStyleState> m_uiStates;

		/*
		* @brief Gets the state or creates a new one
		* @return allways returns a valid ptr
		*/
		UIStyleState* GetState(UIState state) const;
	};

}