#include "UI/UIStyle.h"

namespace SDLCore::UI {

	UIStyle::UIStyle(const std::string& name) 
		: m_name(name) {
	}

	UIStyle::UIStyle(std::string&& name) 
		: m_name(name) {
	}

	std::string UIStyle::ToString() const {
		return m_name;
	}

	void UIStyle::Merge(UIStyle& outStyle) const {
		for (auto& [state, style] : this->m_uiStates) {
			UIStyleState* outStyleState = outStyle.GetState(state);

			outStyleState->alignmentHor.ApplyWithPriority(style.alignmentHor);
			outStyleState->alignmentVer.ApplyWithPriority(style.alignmentVer);
			outStyleState->backgroundColor.ApplyWithPriority(style.backgroundColor);
			outStyleState->backgroundTexture.ApplyWithPriority(style.backgroundTexture);
			outStyleState->borderColor.ApplyWithPriority(style.borderColor);
			outStyleState->borderThickness.ApplyWithPriority(style.borderThickness);
			outStyleState->height.ApplyWithPriority(style.height);
			outStyleState->layoutDirection.ApplyWithPriority(style.layoutDirection);
			outStyleState->margin.ApplyWithPriority(style.margin);
			outStyleState->padding.ApplyWithPriority(style.padding);
			outStyleState->sizeUnitH.ApplyWithPriority(style.sizeUnitH);
			outStyleState->sizeUnitW.ApplyWithPriority(style.sizeUnitW);
			outStyleState->width.ApplyWithPriority(style.width);
		}
	}

	std::string UIStyle::GetName() const {
		return m_name;
	}

	UIStyleState UIStyle::GetStyleState(UIState state) {
		UIStyleState* styleState = GetState(state);
		return *styleState;
	}

	UIStyle& UIStyle::SetActiveState(UIState state) {
		m_currentState = state;
		return *this;
	}

	UIStyle& UIStyle::SetSizeUnit(UISizeUnit unitW, UISizeUnit unitH, bool important) {
		UIStyleState* state = GetState(m_currentState);
		state->sizeUnitW.SetValue(static_cast<int>(unitW));
		state->sizeUnitW.SetIsImportant(important);

		state->sizeUnitH.SetValue(static_cast<int>(unitH));
		state->sizeUnitH.SetIsImportant(important);

		return *this;
	}

	UIStyle& UIStyle::SetSizeUnitW(UISizeUnit unit, bool important) {
		UIStyleState* state = GetState(m_currentState);
		state->sizeUnitW.SetValue(static_cast<int>(unit));
		state->sizeUnitW.SetIsImportant(important);

		return *this;
	}

	UIStyle& UIStyle::SetSizeUnitH(UISizeUnit unit, bool important) {
		UIStyleState* state = GetState(m_currentState);
		state->sizeUnitH.SetValue(static_cast<int>(unit));
		state->sizeUnitH.SetIsImportant(important);

		return *this;
	}

	UIStyle& UIStyle::SetSize(float w, float h, bool important) {
		UIStyleState* state = GetState(m_currentState);
		state->width.SetValue(w);
		state->width.SetIsImportant(important);

		state->height.SetValue(h);
		state->height.SetIsImportant(important);

		return *this;
	}

	UIStyle& UIStyle::SetSize(Vector2 size, bool important) {
		UIStyleState* state = GetState(m_currentState);
		state->width.SetValue(size.x);
		state->width.SetIsImportant(important);

		state->height.SetValue(size.y);
		state->height.SetIsImportant(important);

		return *this;
	}

	UIStyle& UIStyle::SetWidth(float width, bool important) {
		UIStyleState* state = GetState(m_currentState);
		state->width.SetValue(width);
		state->width.SetIsImportant(important);

		return *this;
	}

	UIStyle& UIStyle::SetHeight(float height, bool important) {
		UIStyleState* state = GetState(m_currentState);
		state->width.SetValue(height);
		state->width.SetIsImportant(important);

		return *this;
	}

	UIStyleState* UIStyle::GetState(UIState state) {
		auto it = m_uiStates.find(state);
		if (it == m_uiStates.end()) {
			m_uiStates[state] = UIStyleState();
			return &m_uiStates[state];
		}
		return &it->second;
	}

}