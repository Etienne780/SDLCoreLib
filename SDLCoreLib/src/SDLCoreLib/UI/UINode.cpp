#include <unordered_map>

#include "SDLCoreInput.h"
#include "UI/Types/UIStyleState.h"
#include "UI/Types/UIContext.h"
#include "UI/Nodes/FrameNode.h"
#include "UI/UINode.h"

namespace SDLCore::UI {

	UINode::UINode(int childPos, uintptr_t id, const std::string& typeName)
		: m_childPos(childPos), m_id(id), m_typeName(typeName) {
		m_typeID = GetUITypeID(m_typeName);

		Log::Debug("CreadedNode: id={} name={}", id, m_typeName);
	}

	UINode::~UINode() {
		Log::Debug("DestroyedNode: id={} name={}", m_id, m_typeName);
	}

	void UINode::ClearStyles() {
		m_appliedStyles.clear();
	}

	void UINode::ReserveStyles(size_t count) {
		m_appliedStyles.reserve(count);
	}

	void UINode::AddStyle(const UIStyle& style) {
		m_appliedStyles.push_back(style);
	}

	void UINode::ApplyStyle(UIContext* ctx) {
		if (!ctx)
			return;

		m_finalStyle = CreateStyle();
		
		// allways uses normal state as a base
		UIStyleState styleState = m_finalStyle.GetStyleState(UIState::NORMAL);

		// if state pressed apply hover
		if (m_state == UIState::PRESSED) {
			const UIStyleState& st = m_finalStyle.GetStyleState(UIState::HOVER);
			st.Merge(styleState);
		}

		if (m_state != UIState::NORMAL) {
			const UIStyleState& st = m_finalStyle.GetStyleState(m_state);
			st.Merge(styleState);
		}
		
		int layoutDir = 0;
		int alignHorizontal = 0;
		int alignVertical = 0;

		m_layoutDir = UILayoutDirection::ROW;
		m_horizontalAligment = UIAlignment::START;
		m_verticalAligment = UIAlignment::START;

		styleState.TryGetValue<int>(Properties::layoutDirection, layoutDir);
		styleState.TryGetValue<int>(Properties::alignHorizontal, alignHorizontal);
		styleState.TryGetValue<int>(Properties::alignVertical, alignVertical);

		m_layoutDir = static_cast<UILayoutDirection>(layoutDir);
		m_horizontalAligment = static_cast<UIAlignment>(alignHorizontal);
		m_verticalAligment = static_cast<UIAlignment>(alignVertical);

		float width = 0.0f;
		float height = 0.0f;

		styleState.TryGetValue<float>(Properties::width, width);
		styleState.TryGetValue<float>(Properties::height, height);

		int sizeUnitW = 0;// 0 = PX
		int sizeUnitH = 0;
		styleState.TryGetValue<int>(Properties::sizeUnitW, sizeUnitW);
		styleState.TryGetValue<int>(Properties::sizeUnitH, sizeUnitH);

		m_padding.Set(0);
		m_margin.Set(0);
		styleState.TryGetValue<Vector4>(Properties::padding, m_padding);
		styleState.TryGetValue<Vector4>(Properties::margin, m_margin);

		m_isHitTestEnabled = true;
		styleState.TryGetValue<bool>(Properties::hitTestEnabled, m_isHitTestEnabled);

		ApplyStyleCalled(ctx, styleState);

		m_size.Set(0);
		m_size = CalculateSize(ctx,
			static_cast<UISizeUnit>(sizeUnitW),
			static_cast<UISizeUnit>(sizeUnitH),
			width, height);
	}

	void UINode::ResetState() {
		m_state = UIState::NORMAL;
	}

	bool UINode::ContainsChildAtPos(uint16_t pos, uintptr_t id, UINode*& outNode) {
		if (pos >= m_children.size())
			return false;
		outNode = m_children[pos].get();
		return (outNode && outNode->GetID() == id);
	}

	uintptr_t UINode::GetID() const {
		return m_id;
	}

	const std::string& UINode::GeTypeName() const {
		return m_typeName;
	}

	uint32_t UINode::GetTypeID() const {
		return m_typeID;
	}

	UIEvent UINode::GetEvent() const {
		return m_eventState;
	}

	UIEvent* UINode::GetEventPtr() {
		return &m_eventState;
	}

	std::string UINode::GetName() const {
		return m_typeName;
	}

	UINode* UINode::GetParent() {
		return m_parent;
	}

	const std::vector<std::shared_ptr<UINode>>& UINode::GetChildren() const {
		return m_children;
	}

	bool UINode::GetChildHasEvent() const {
		return m_childHasEvent;
	}

	bool UINode::IsActive() const {
		return m_isActive;
	}

	void UINode::SetChildHasEvent(bool value) {
		m_childHasEvent = value;
	}

	Vector2 UINode::GetPosition() const {
		return m_position;
	}

	Vector2 UINode::GetSize() const {
		return m_size;
	}

	Vector4 UINode::GetPadding() const {
		return m_padding;
	}

	Vector4 UINode::GetMargin() const {
		return m_margin;
	}

	UILayoutDirection UINode::GetLayoutDirection() const {
		return m_layoutDir;
	}

	UIAlignment UINode::GetHorizontalAlignment() const {
		return m_horizontalAligment;
	}

	UIAlignment UINode::GetVerticalAlignment() const {
		return m_verticalAligment;
	}

	void UINode::RemoveChildrenFromIndex(uint16_t pos) {
		if (pos >= m_children.size())
			return;
		m_children.erase(m_children.begin() + pos, m_children.end());
	}

	UIStyle UINode::CreateStyle() {
		UIStyle outStyle;
		
		for (auto& style : m_appliedStyles) {
			style.Merge(outStyle);
		}

		return outStyle;
	}

	void UINode::SetNodeActive() {
		m_isActive = true;
	}

	bool UINode::IsPointInNode(const Vector2& point) {
		return point.x > m_position.x && point.x <= m_position.x + m_size.x && 
			point.y > m_position.y && point.y <= m_position.y + m_size.y;
	}

	Vector2 UINode::CalculateSize(UIContext* ctx, UISizeUnit unitW, UISizeUnit unitH, float w, float h) {
		if (!ctx)
			return Vector2(0.0f, 0.0f);

		auto resolveBaseSize = [&](bool horizontal) -> float {
			if (m_parent) {
				const auto& pPadding = m_parent->m_padding;
				return horizontal ? m_parent->m_size.x - pPadding.y - pPadding.w : m_parent->m_size.y - pPadding.x - pPadding.z;
			}
			return horizontal ? ctx->GetWindowSize().x : ctx->GetWindowSize().y;
		};

		auto calc = [&](bool horizontal, UISizeUnit unit, float value) -> float {
			switch (unit) {
			case UISizeUnit::PX:
				return value;

			case UISizeUnit::PERCENTAGE:
				return resolveBaseSize(horizontal) * value / 100.0f;

			case UISizeUnit::PERCENTAGE_W:
				return resolveBaseSize(true) * value / 100.0f;

			case UISizeUnit::PERCENTAGE_H:
				return resolveBaseSize(false) * value / 100.0f;

			default:
				return 0.0f;
			}
		};

		return Vector2(
			calc(true, unitW, w),
			calc(false, unitH, h)
		);
	}

	uint32_t UINode::GetUITypeID(const std::string& name) {
		auto it = Internal::nameToID.find(name);
		if (it == Internal::nameToID.end()) {
			uint32_t newID = m_typeIDCounter;
			Internal::nameToID[name] = newID;
			m_typeIDCounter++;
			return newID;
		}
		return it->second;
	}

	float UINode::GetAccumulatedChildSize(bool horizontal, int upToIndex) const {
		if (!m_parent)
			return 0.0f;

		const auto& childs = m_parent->GetChildren();
		float size = 0.0f;

		for (int i = 0; i < upToIndex && i < static_cast<int>(childs.size()); ++i) {
			const auto& cMargin = childs[i]->m_margin;
			size += horizontal ? childs[i]->m_size.x + cMargin.y + cMargin.w : childs[i]->m_size.y + cMargin.x + cMargin.z;
		}

		return size;
	}

	float UINode::GetTotalChildrenSize(bool horizontal) const {
		if (!m_parent)
			return 0.0f;

		const auto& childs = m_parent->GetChildren();
		float size = 0.0f;

		for (const auto& c : childs) {
			const auto& cMargin = c->m_margin;
			size += horizontal ? c->m_size.x + cMargin.y + cMargin.w : c->m_size.y + cMargin.x + cMargin.z;
		}

		return size;
	}

	float UINode::AlignOffset(bool isHor, UIAlignment align, float freeSpace) {
		if (!m_parent)
			return 0.0f;

		const auto& childs = m_parent->GetChildren();

		float offset = 0.0f;

		// Compute offset only along main axis
		switch (align) {
		case UIAlignment::START:
			if (m_childPos > 0 && m_childPos < static_cast<int>(childs.size())) {
				offset = GetAccumulatedChildSize(isHor, m_childPos);
			}
			break;
		case UIAlignment::CENTER: {
			const float totalSize = GetTotalChildrenSize(isHor);
			const float start = (freeSpace - totalSize) * 0.5f;
			offset = start + GetAccumulatedChildSize(isHor, m_childPos);
			break;
		}
		case UIAlignment::END: {
			const float totalSize = GetTotalChildrenSize(isHor);
			const float start = freeSpace - totalSize;
			offset = start + GetAccumulatedChildSize(isHor, m_childPos);
			break;
		}
		default:
			offset = 0.0f;
			break;
		}
		return offset;
	}

	static bool IsRow(UILayoutDirection d) {
		return d == UILayoutDirection::ROW || d == UILayoutDirection::ROW_REVERSE;
	}

	static bool IsReverse(UILayoutDirection d) {
		return d == UILayoutDirection::ROW_REVERSE || d == UILayoutDirection::COLUMN_REVERSE;
	}

	void UINode::CalculateLayout(const UIContext* uiContext) {
		if (!uiContext || !m_parent)
			return;

		const Vector2 parentSize = m_parent->m_size;
		const UILayoutDirection dir = m_parent->GetLayoutDirection();

		const bool isRow = IsRow(dir);
		const bool isReverse = IsReverse(dir);

		const auto& pPad = m_parent->m_padding;
		const float contentWidth =
			parentSize.x - pPad.y - pPad.w;
		const float contentHeight =
			parentSize.y - pPad.x - pPad.z;

		const float freeX =
			contentWidth - m_size.x;
		const float freeY =
			contentHeight - m_size.y;

		const float contentStartX = m_parent->m_position.x + pPad.y;
		const float contentStartY = m_parent->m_position.y + pPad.x;

		if (isRow) {
			float x = AlignOffset(true, m_parent->GetHorizontalAlignment(), contentWidth);

			if (isReverse)
				x = contentWidth - x - m_size.x;

			m_position.x = contentStartX + x + m_margin.y;
		}
		else {
			float y = AlignOffset(false, m_parent->GetVerticalAlignment(), contentHeight);

			if (isReverse)
				y = contentHeight - y - m_size.y;

			m_position.y = contentStartY + y + m_margin.x;
		}

		if (isRow) {
			switch (m_parent->GetVerticalAlignment()) {
			case UIAlignment::START:
				m_position.y = contentStartY + m_margin.x;
				break;

			case UIAlignment::CENTER:
				m_position.y =
					contentStartY + (contentHeight - m_size.y) * 0.5f;
				break;

			case UIAlignment::END:
				m_position.y =
					contentStartY + contentHeight - m_size.y - m_margin.z;
				break;
			}
		}
		else {
			switch (m_parent->GetHorizontalAlignment()) {
			case UIAlignment::START:
				m_position.x = contentStartX + m_margin.y;
				break;

			case UIAlignment::CENTER:
				m_position.x =
					contentStartX + (contentWidth - m_size.x) * 0.5f;
				break;

			case UIAlignment::END:
				m_position.x =
					contentStartX + contentWidth - m_size.x - m_margin.w;
				break;
			}
		}
	}

	void UINode::ProcessEventInternal(UIContext* ctx, UIEvent* event) {
		if (!ctx || !event)
			return;

		event->Reset();

		if (!m_isHitTestEnabled) {
			m_state = UIState::NORMAL;
			ProcessEvent(event);
			return;
		}

		const Vector2 mousePos = Input::GetMousePosition();
		const Vector2 mouseDelta = Input::GetMouseDelta();

		const bool mouseDown = Input::MousePressed(MouseButton::LEFT);
		const bool mouseJustDown = Input::MouseJustPressed(MouseButton::LEFT);
		const bool mouseJustUp = Input::MouseJustReleased(MouseButton::LEFT);

		const bool isHovered = IsPointInNode(mousePos);

		const bool pressCaptured = ctx->GetActiveCapturedPressNode() == m_id;
		const bool dragCaptured = ctx->GetActiveCapturedDragNode() == m_id;

		// Hover state
		if (isHovered)
			event->SetIsHovered(true);

		// Press start
		if (mouseJustDown && isHovered) {
			ctx->CapturePressNode(m_id);
			m_state = UIState::PRESSED;
			event->SetIsPressed(true);
			event->SetIsClicked(true);
		}
		
		// Drag start
		if (pressCaptured && mouseDown && mouseDelta != Vector2::zero && !dragCaptured) {
			ctx->CaptureDragNode(m_id);
		}

		// Drag active
		if (dragCaptured && mouseDown) {
			event->SetIsDragging(true);
		}

		// Drag release
		if (dragCaptured && mouseJustUp) {
			ctx->ReleaseDragNode(m_id);
		}

		// Press state, press ends if mouse out of node
		if (pressCaptured && isHovered) {
			m_state = UIState::PRESSED;
			event->SetIsPressed(true);
		}
		else {
			if (pressCaptured && !isHovered) {
				ctx->ReleasePressNode(m_id);
			}
			m_state = isHovered ? UIState::HOVER : UIState::NORMAL;
		}

		ProcessEvent(event);
	}

}