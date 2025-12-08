#pragma once
#include <string>

#include "UI/Types/UITypes.h"
#include "UI/Types/UIStyleState.h"

namespace SDLCore::UI {

	class UIStyle {
	public:
		UIStyle(const std::string& name);
		UIStyle(std::string&& name);
		
		/*
		* not finished
		*/
		std::string ToString() const;

		/*
		* @brief Merges this style with on top of outStyle
		*/
		void Merge(UIStyle& outStyle) const;

		std::string GetName() const;
		UIStyleState GetStyleState(UIState state);

		/*
		* @brief Sets the state that will be used when setting properties
		*/
		UIStyle& SetActiveState(UIState state);

		UIStyle& SetSizeUnit(UISizeUnit unitW, UISizeUnit unitH, bool important = false);
		UIStyle& SetSizeUnitW(UISizeUnit unit, bool important = false);
		UIStyle& SetSizeUnitH(UISizeUnit unit, bool important = false);

		UIStyle& SetSize(float w, float h, bool important = false);
		UIStyle& SetSize(Vector2 size, bool important = false);
		UIStyle& SetWidth(float width, bool important = false);
		UIStyle& SetHeight(float height, bool important = false);

	private:
		std::string m_name;
		UIState m_currentState = UIState::NORMAL;
		std::unordered_map<UIState, UIStyleState> m_uiStates;

		/*
		* @brief Gets the state or creates a new one
		* @return allways returns a valid ptr
		*/
		UIStyleState* GetState(UIState state);
	};

}