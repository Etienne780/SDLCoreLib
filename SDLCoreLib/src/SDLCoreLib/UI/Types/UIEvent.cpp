#include "UI/Types/UIEvent.h"

namespace SDLCore::UI {

	bool UIEvent::IsHover() const {
		return m_isHovered;
	}

	bool UIEvent::IsPressed() const {
		return m_isPressed;
	}

	bool UIEvent::IsClick() const {
		return m_isClicked;
	}
	
	bool UIEvent::IsDragging() const {
		return m_isDragging;
	}

	float UIEvent::GetScrollDir() const {
		return m_scrollDir;
	}

	bool UIEvent::TryGetScrollDir(float& outDir) const {
		outDir = m_scrollDir;
		return (m_scrollDir != 0);
	}

	void UIEvent::SetIsHovered(bool value) {
		m_isHovered = value;
	}

	void UIEvent::SetIsPressed(bool value) {
		m_isPressed = value;
	}

	void UIEvent::SetIsClicked(bool value) {
		m_isClicked = value;
	}

	void UIEvent::SetIsDragging(bool value) {
		m_isDragging = value;
	}

	void UIEvent::SetScrollDir(float dir) {
		m_scrollDir = dir;
	}

	void UIEvent::Reset() {
		m_isHovered = false;
		m_isPressed = false;
		m_isClicked = false;
		m_isDragging = false;

		 m_scrollDir = 0;
	}

}