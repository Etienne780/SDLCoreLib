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

		m_lastState = m_state;
		m_finalStyle = CreateStyle();
		
		// allways uses normal state as a base
		UIStyleState styleState = m_finalStyle.GetStyleState(UIState::NORMAL);

		// if state pressed apply hover
		if (m_state == UIState::PRESSED) {
			const UIStyleState& st = m_finalStyle.GetStyleState(UIState::HOVER);
			styleState.Merge(st);
		}

		if (m_state != UIState::NORMAL) {
			const UIStyleState& st = m_finalStyle.GetStyleState(m_state);
			styleState.Merge(st);
		}
		
		int layoutDir = 0;
		int alignHorizontal = 0;
		int alignVertical = 0;

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
		styleState.TryGetValue<int>(Properties::widthUnit, sizeUnitW);
		styleState.TryGetValue<int>(Properties::heightUnit, sizeUnitH);

		m_padding.Set(0.0f);
		m_margin.Set(0.0f);
		styleState.TryGetValue<Vector4>(Properties::padding, m_padding);
		styleState.TryGetValue<Vector4>(Properties::margin, m_margin);

		m_borderWidth = 0.0f;
		styleState.TryGetValue<float>(Properties::borderWidth, m_borderWidth);

		m_innerBorder = false;
		styleState.TryGetValue<bool>(Properties::borderInset, m_innerBorder);

		m_isHitTestEnabled = true;
		styleState.TryGetValue<bool>(Properties::hitTestEnabled, m_isHitTestEnabled);

		m_notInteractible = false;
		styleState.TryGetValue<bool>(Properties::notInteractible, m_notInteractible);

		m_borderAffectsLayout = true;
		styleState.TryGetValue<bool>(Properties::borderAffectsLayout, m_borderAffectsLayout);

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

	UIState UINode::GetState() const {
		return m_state;
	}

	UIState UINode::GetLastState() const {
		return m_lastState;
	}

	bool UINode::GetChildHasEvent() const {
		return m_childHasEvent;
	}

	uint64_t UINode::GetAppliedStyleHash() const {
		return m_appliedStyleHash;
	}

	uint64_t UINode::GetAppliedStyleNode() const {
		return m_appliedStyleNode;
	}

	bool UINode::IsActive() const {
		return m_isActive;
	}

	bool UINode::HasStateChanged() const {
		return m_state != m_lastState;
	}

	bool UINode::HasHitTestEnabled() const {
		return m_isHitTestEnabled;
	}

	bool UINode::IsInteractible() const {
		return !m_notInteractible;
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

	Vector4 UINode::GetBorderLayoutPadding() const {
		if (!m_borderAffectsLayout || !m_innerBorder)
			return Vector4(0.0f);

		return Vector4(m_borderWidth);
	}

	Vector4 UINode::GetBorderLayoutMargin() const {
		if (!m_borderAffectsLayout || m_innerBorder)
			return Vector4(0.0f);

		return Vector4(m_borderWidth);
	}

	void UINode::RemoveChildrenFromIndex(uint16_t pos) {
		if (pos >= m_children.size())
			return;
		m_children.erase(m_children.begin() + pos, m_children.end());
	}

	UIStyle UINode::CreateStyle() {
		UIStyle outStyle;
		
		for (auto& style : m_appliedStyles) {
			outStyle.Merge(style);
		}

		return outStyle;
	}

	void UINode::SetNodeActive() {
		m_isActive = true;
	}

	bool UINode::IsMouseInNode() const {
		return IsPointInNode(Input::GetMousePosition());
	}

	bool UINode::IsPointInNode(const Vector2& point) const {
		return point.x > m_position.x && point.x <= m_position.x + m_size.x && 
			point.y > m_position.y && point.y <= m_position.y + m_size.y;
	}

	Vector2 UINode::CalculateSize(UIContext* ctx, UISizeUnit unitW, UISizeUnit unitH, float w, float h) {
		if (!ctx)
			return Vector2(0.0f, 0.0f);

		auto resolveBaseSize = [&](bool horizontal) -> float {
			if (m_parent) {
				const auto& pPadding = m_parent->m_padding + m_parent->GetBorderLayoutPadding();
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

	void UINode::SetAppliedStyleHash(uint64_t newHash) {
		m_appliedStyleHash = newHash;
	}

	void UINode::SetAppliedStyleNode(uint64_t node) {
		m_appliedStyleNode = node;
	}

	float UINode::GetAccumulatedChildSize(bool horizontal, int upToIndex) const {
		if (!m_parent)
			return 0.0f;

		const auto& childs = m_parent->GetChildren();
		float size = 0.0f;

		for (int i = 0; i < upToIndex && i < static_cast<int>(childs.size()); ++i) {
			const auto& cMargin = childs[i]->m_margin + childs[i]->GetBorderLayoutMargin();
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
			const auto& cMargin = c->m_margin + c->GetBorderLayoutMargin();
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

		const Vector4 margin = m_margin + this->GetBorderLayoutMargin();
		const auto& pPad = m_parent->m_padding + m_parent->GetBorderLayoutPadding();
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

			m_position.x = contentStartX + x + margin.y;
		}
		else {
			float y = AlignOffset(false, m_parent->GetVerticalAlignment(), contentHeight);

			if (isReverse)
				y = contentHeight - y - m_size.y;

			m_position.y = contentStartY + y + margin.x;
		}

		if (isRow) {
			switch (m_parent->GetVerticalAlignment()) {
			case UIAlignment::START:
				m_position.y = contentStartY + margin.x;
				break;

			case UIAlignment::CENTER:
				m_position.y =
					contentStartY + (contentHeight - m_size.y) * 0.5f;
				break;

			case UIAlignment::END:
				m_position.y =
					contentStartY + contentHeight - m_size.y - margin.z;
				break;
			}
		}
		else {
			switch (m_parent->GetHorizontalAlignment()) {
			case UIAlignment::START:
				m_position.x = contentStartX + margin.y;
				break;

			case UIAlignment::CENTER:
				m_position.x =
					contentStartX + (contentWidth - m_size.x) * 0.5f;
				break;

			case UIAlignment::END:
				m_position.x =
					contentStartX + contentWidth - m_size.x - margin.w;
				break;
			}
		}
	}

	void UINode::ProcessEventInternal(UIContext* ctx, UIEvent* event) {
		if (!ctx || !event)
			return;

		event->Reset();

		if (m_notInteractible) {
			m_state = UIState::NORMAL;
			return;
		}

		if (!m_isHitTestEnabled) {
			m_state = UIState::NORMAL;
			ProcessEvent(event);
			return;
		}

		const Vector2 mousePos = ctx->GetMousePos();
		const Vector2 mouseDelta = ctx->GetMouseDelta();

		const bool mouseDown = ctx->GetLeftMouseDown();
		const bool mouseJustDown = ctx->GetLeftMouseJustDown();
		const bool mouseJustUp = ctx->GetLeftMouseJustUp();

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
		if (pressCaptured && isHovered && mouseDown) {
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