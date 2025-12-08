#include "UI/Types/UIEvent.h"

namespace SDLCore::UI {

	bool UIEvent::IsHover() const {
		return m_isHovered;
	}

	bool UIEvent::IsActive() const {
		return m_isActive;
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

	void UIEvent::SetIsActive(bool value) {
		m_isActive = value;
	}

	void UIEvent::SetIsClicked(bool value) {
		m_isActive = value;
	}

	void UIEvent::SetIsDragging(bool value) {
		m_isDragging = value;
	}

	void UIEvent::SetScrollDir(float dir) {
		m_scrollDir = dir;
	}

}