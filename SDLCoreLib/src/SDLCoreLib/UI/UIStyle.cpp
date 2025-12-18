#include "UI/UIStyle.h"
#include "UI/Types/UIPropertyRegistry.h"

namespace SDLCore::UI {

	UIStyle::UIStyle() {
		UIPropertyRegistry::RegisterBaseProperties();
	}

	UIStyle::UIStyle(const std::string& name) 
		: m_name(name) {
		UIPropertyRegistry::RegisterBaseProperties();
	}

	UIStyle::UIStyle(std::string&& name) 
		: m_name(name) {
		UIPropertyRegistry::RegisterBaseProperties();
	}

	std::string UIStyle::ToString() const {
		return m_name;
	}
	
	void UIStyle::Merge(UIStyle& outStyle) const {
		for (auto& [state, styleState] : this->m_uiStates) {
			UIStyleState* outStyleState = outStyle.GetState(state);
			styleState.Merge(*outStyleState);
		}
	}

	std::string UIStyle::GetName() const {
		return m_name;
	}

	UIStyleState UIStyle::GetStyleState(UIState state) {
		UIStyleState* styleState = GetState(state);
		return *styleState;
	}

	const UIStyleState& UIStyle::GetStyleState(UIState state) const {
		UIStyleState* styleState = GetState(state);
		return *styleState;
	}

	UIStyle& UIStyle::SetActiveState(UIState state) {
		m_currentState = state;
		return *this;
	}

	UIStyle& UIStyle::SetValue(UIPropertyID attID, PropertyValue value, bool important) {
		UIStyleState* state = GetState(m_currentState);
		state->SetValue(attID, value, important);
		return *this;
	}

	UIStyleState* UIStyle::GetState(UIState state) const {
		auto it = m_uiStates.find(state);
		if (it == m_uiStates.end()) {
			m_uiStates[state] = UIStyleState();
			return &m_uiStates[state];
		}
		return &it->second;
	}

}