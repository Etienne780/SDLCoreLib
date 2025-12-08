#pragma once

namespace SDLCore::UI {

	class UINode;
	class UIEvent {
	friend class UINode;
	public:
		UIEvent() = default;
		~UIEvent() = default;

		bool IsHover() const;
		bool IsActive() const;
		bool IsClick() const;
		bool IsDragging() const;

		float GetScrollDir() const;
		bool TryGetScrollDir(float& outDir) const;

	private:
		bool m_isHovered = false;
		bool m_isActive = false;
		bool m_isClicked = false;
		bool m_isDragging = false;

		float m_scrollDir = 0;

		void SetIsHovered(bool value);
		void SetIsActive(bool value);
		void SetIsClicked(bool value);
		void SetIsDragging(bool value);
		void SetScrollDir(float dir);
	};

}