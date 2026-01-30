#include <unordered_map>

#include "SDLCoreInput.h"
#include "SDLCoreTime.h"
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


	void UINode::UpdateFinalStyle(UIContext* ctx) {
		m_finalStyle = CreateStyle();
		OnStyleStateChanged();
		ApplyStyle(ctx);
	}

	UINode& UINode::SetImportant(bool value) {
		if (m_overrideState.IsImportantDiff(m_lastOverrideID, value)) {
			if (m_overrideState.SetImportant(value)) {
				SetNodeStyleDirty();
			}
		}
		return *this;
	}

	UINode& UINode::ClearOverride(UIPropertyID id) {
		m_overrideState.ResetValue(id);
		return *this;
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

	bool UINode::GetStyleChanged() const {
		return m_styleDirty;
	}

	bool UINode::IsActive() const {
		return m_isActive;
	}

	bool UINode::HasPointerEvents() const {
		return m_hasPointerEvents;
	}

	bool UINode::HasHitTestTransparent() const {
		return m_hasHitTestTransparent;
	}

	bool UINode::IsStatePropagationEnabled() const {
		return m_propagateStateToChildren;
	}

	bool UINode::IsDisabled() const {
		return m_isDisabled;
	}

	bool UINode::IsOverflowHidden() const {
		return IsHorizontalOverflowHidden() ||
			IsVerticalOverflowHidden();
	}

	bool UINode::IsHorizontalOverflowHidden() const {
		return m_isHorizontalOverflowHidden;
	}

	bool UINode::IsVerticalOverflowHidden() const {
		return m_isVerticalOverflowHidden;
	}

	bool UINode::IsFlow() const {
		return m_positionType == UIPositionType::FLOW;
	}

	bool UINode::IsRelative() const {
		return m_positionType == UIPositionType::RELATIVE;
	}

	bool UINode::IsAbsolute() const {
		return m_positionType == UIPositionType::ABSOLUTE;
	}

	void UINode::SetChildHasEvent(bool value) {
		m_childHasEvent = value;
	}

	Vector2 UINode::GetPosition() const {
		return m_position;
	}

	Vector2 UINode::GetVisiblePosition() const {
		Vector4 bM = GetBorderLayoutMargin();
		Vector2 p = GetPosition();
		return Vector2(p.x - bM.y, p.y - bM.x);
	}

	Vector2 UINode::GetSize() const {
		return m_size;
	}

	Vector2 UINode::GetVisibleSize() const {
		Vector4 bM = GetBorderLayoutMargin();
		Vector2 s = GetSize();
		return Vector2(s.x + bM.y + bM.w, 
			s.y + bM.x + bM.z);
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

	UIPositionType UINode::GetPositionType() const {
		return m_positionType;
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

	Vector4 UINode::GetClippingRect() const {
		return m_clippingMask;
	}

	UIState UINode::GetResolvedState() const {
		return m_resolvedState;
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

	void UINode::SetNodeStyleDirty() {
		m_styleDirty = true;
	}

	void UINode::SetNodeActive() {
		m_isActive = true;
	}

	void UINode::SetState(UIState state) {
		m_state = state;
	}

	void UINode::SetResolvedState(UIState state) {
		m_resolvedState = state;
	}

	bool UINode::IsMouseInNode() const {
		return IsPointInNode(Input::GetMousePosition());
	}

	bool UINode::IsMouseInClipRect(const Vector2& point) const {
		return IsPointInClipRect(Input::GetMousePosition());
	}

	bool UINode::IsPointInNode(const Vector2& point) const {
		return point.x > m_position.x && point.x <= m_position.x + m_size.x && 
			point.y > m_position.y && point.y <= m_position.y + m_size.y;
	}

	bool UINode::IsPointInClipRect(const Vector2& point) const {
		return point.x > m_clippingMask.x && point.x <= m_clippingMask.x + m_clippingMask.z &&
			point.y > m_clippingMask.y && point.y <= m_clippingMask.y + m_clippingMask.w;
	}

	Vector2 UINode::CalculateSize(UIContext* ctx, UISizeUnit unitW, UISizeUnit unitH, float w, float h) {
		if (!ctx)
			return Vector2(0.0f, 0.0f);

		auto resolveBaseSize = [&](bool horizontal) -> float {
			const UINode* baseNode = m_parent;

			if (m_positionType == UIPositionType::ABSOLUTE) {
				baseNode = ctx->GetLastRelativeNode();
				if (!baseNode)
					baseNode = m_parent;
			}

			if (baseNode) {
				const auto& pPadding = baseNode->m_padding + baseNode->GetBorderLayoutPadding();
				return horizontal ? baseNode->m_size.x - pPadding.y - pPadding.w
					: baseNode->m_size.y - pPadding.x - pPadding.z;
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

	void UINode::ApplyStyle(UIContext* ctx) {
		if (!ctx)
			return;

		m_styleDirty = false;
		m_lastState = m_state;

		int layoutDir = 0;
		int alignHorizontal = 0;
		int alignVertical = 0;

		GetResolvedValue<int>(Properties::layoutDirection, layoutDir, 0);
		GetResolvedValue<int>(Properties::alignHorizontal, alignHorizontal, 0);
		GetResolvedValue<int>(Properties::alignVertical, alignVertical, 0);

		m_layoutDir = static_cast<UILayoutDirection>(layoutDir);
		m_horizontalAligment = static_cast<UIAlignment>(alignHorizontal);
		m_verticalAligment = static_cast<UIAlignment>(alignVertical);

		float width = 0.0f;
		float height = 0.0f;

		GetResolvedValue<float>(Properties::width, width, 0.0f);
		GetResolvedValue<float>(Properties::height, height, 0.0f);

		int sizeUnitW = 0; // PX
		int sizeUnitH = 0;

		GetResolvedValue<int>(Properties::widthUnit, sizeUnitW, 0);
		GetResolvedValue<int>(Properties::heightUnit, sizeUnitH, 0);

		GetResolvedValue<Vector4>(Properties::padding, m_padding, Vector4(0.0f));
		GetResolvedValue<Vector4>(Properties::margin, m_margin, Vector4(0.0f));

		GetResolvedValue<float>(Properties::borderWidth, m_borderWidth, 0.0f);

		int transitionTimeUnit = 1;
		GetResolvedValue<int>(Properties::durationUnit, transitionTimeUnit, 1);

		float transitionDuration = 0.0f;
		GetResolvedValue<float>(Properties::duration, transitionDuration, 0.0f);

		SetTransitionTime(
			transitionDuration,
			static_cast<UITimeUnit>(transitionTimeUnit)
		);

		int transitionEasing = 0;
		GetResolvedValue<int>(Properties::durationEasing, transitionEasing, 0);
		m_transitionEasing = static_cast<UIEasing>(transitionEasing);

		GetResolvedValue<bool>(Properties::borderInset, m_innerBorder, false);
		GetResolvedValue<bool>(Properties::pointerEvents, m_hasPointerEvents, true);
		GetResolvedValue<bool>(Properties::hitTestTransparent, m_hasHitTestTransparent, false);
		GetResolvedValue<bool>(Properties::propagateStateToChildren, m_propagateStateToChildren, false);
		GetResolvedValue<bool>(Properties::disabled, m_isDisabled, false);
		GetResolvedValue<bool>(Properties::borderAffectsLayout, m_borderAffectsLayout, true);
		GetResolvedValue<bool>(Properties::hideOverflowX, m_isHorizontalOverflowHidden, true);
		GetResolvedValue<bool>(Properties::hideOverflowY, m_isVerticalOverflowHidden, true);

		int positionType = 0;
		GetResolvedValue<int>(Properties::positionType, positionType, 0);
		m_positionType = static_cast<UIPositionType>(positionType);

		GetResolvedValue<float>(Properties::top, m_absolutePositionOffset.x, 0.0f);
		GetResolvedValue<float>(Properties::left, m_absolutePositionOffset.y, 0.0f);
		GetResolvedValue<float>(Properties::bottom, m_absolutePositionOffset.z, 0.0f);
		GetResolvedValue<float>(Properties::right, m_absolutePositionOffset.w, 0.0f);

		int absolutePosUnitTop = 0;
		int absolutePosUnitLeft = 0;
		int absolutePosUnitBottom = 0;
		int absolutePosUnitRight = 0;

		GetResolvedValue<int>(Properties::topUnit, absolutePosUnitTop, 0);
		GetResolvedValue<int>(Properties::leftUnit, absolutePosUnitLeft, 0);
		GetResolvedValue<int>(Properties::bottomUnit, absolutePosUnitBottom, 0);
		GetResolvedValue<int>(Properties::rightUnit, absolutePosUnitRight, 0);
		m_absolutePositionUnits[0] = static_cast<UISizeUnit>(absolutePosUnitTop);
		m_absolutePositionUnits[1] = static_cast<UISizeUnit>(absolutePosUnitLeft);
		m_absolutePositionUnits[2] = static_cast<UISizeUnit>(absolutePosUnitBottom);
		m_absolutePositionUnits[3] = static_cast<UISizeUnit>(absolutePosUnitRight);

		ApplyStyleCalled(ctx, m_renderedStyleState);

		m_size = CalculateSize(
			ctx,
			static_cast<UISizeUnit>(sizeUnitW),
			static_cast<UISizeUnit>(sizeUnitH),
			width,
			height
		);
	}

	UIStyleState UINode::ComputeTargetStyleState() {
		UIStyleState state = m_finalStyle.GetStyleState(UIState::NORMAL);

		if (m_state == UIState::PRESSED)
			state.Merge(m_finalStyle.GetStyleState(UIState::HOVER));

		if (m_state != UIState::NORMAL)
			state.Merge(m_finalStyle.GetStyleState(m_state));

		return state;
	}

	void UINode::OnStyleStateChanged() {
		UIStyleState target = ComputeTargetStyleState();

		if (m_transitionDuration <= 0.0f) {
			m_renderedStyleState = target;
			m_transitionActive = false;
			return;
		}

		m_transitionFrom = m_renderedStyleState;
		m_transitionTo = target;

		// interrupt old transition
		if (m_transitionActive) {
			m_currentTransitionEnd = m_currentTransition;
			m_currentTransition = 0.0f;
		}
		else {
			// start new transition
			m_currentTransition = 0.0f;
			m_currentTransitionEnd = m_transitionDuration;
		}

		m_transitionActive = true;
	}
	
	void UINode::Update(UIContext* ctx, float dt) {
		UpdateStyle(ctx, dt);
		CalculateLayout(ctx);
	}

	void UINode::UpdateStyle(UIContext* ctx, float dt) {
		if (HasStateChanged()) {
			OnStyleStateChanged();
			ApplyStyle(ctx);
		}
		
		if (!m_transitionActive)	
			return;

		m_currentTransition += dt;
		float time = std::clamp(m_currentTransition / m_currentTransitionEnd, 0.0f, 1.0f);
		if (time >= 1.0f) {
			m_renderedStyleState = m_transitionTo;
			m_transitionActive = false;
		}
		else {
			m_renderedStyleState = UIStyleState::Interpolate(
				m_transitionFrom, 
				m_transitionTo, 
				time, 
				m_transitionEasing);
		}

		ApplyStyle(ctx);
	}

	bool UINode::HasStateChanged() const {
		return m_state != m_lastState;
	}

	void UINode::SetAppliedStyleHash(uint64_t newHash) {
		m_appliedStyleHash = newHash;
	}

	void UINode::SetAppliedStyleNode(uint64_t node) {
		m_appliedStyleNode = node;
	}

	void UINode::SetTransitionTime(float time, UITimeUnit unit) {
		switch (unit) {
		case SDLCore::UI::UITimeUnit::SECONDS:
			m_transitionDuration = time * 1000.0f;
			break;
		case SDLCore::UI::UITimeUnit::MILLISECONDS:
		default:
			m_transitionDuration = time;
			break;
		}
	}

	void UINode::SetClippingRect(const Vector4& clipRect) {
		m_clippingMask = clipRect;
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

	float UINode::ResolveAbsoluteValue(
		const UINode* reference,
		bool horizontal,
		UISizeUnit unit,
		float value
	) {
		float refW = reference->GetSize().x;
		float refH = reference->GetSize().y;

		switch (unit) {
		case UISizeUnit::PX:
			return value;

		case UISizeUnit::PERCENTAGE:
			return ((horizontal ? refW : refH) * value) / 100.0f;

		case UISizeUnit::PERCENTAGE_W:
			return (refW * value) / 100.0f;

		case UISizeUnit::PERCENTAGE_H:
			return (refH * value) / 100.0f;

		default:
			return 0.0f;
		}
	}

	bool UINode::IsRow(UILayoutDirection d) {
		return d == UILayoutDirection::ROW || d == UILayoutDirection::ROW_REVERSE;
	}

	bool UINode::IsReverse(UILayoutDirection d) {
		return d == UILayoutDirection::ROW_REVERSE || d == UILayoutDirection::COLUMN_REVERSE;
	}

	void UINode::CalculateLayout(const UIContext* uiContext) {
		if (!uiContext || !m_parent)
			return;

		switch (m_positionType) {
		case SDLCore::UI::ABSOLUTE:
			CalculateLayoutAbsolute(uiContext);
			break;
		case SDLCore::UI::FLOW:
		case SDLCore::UI::RELATIVE:
		default:
			CalculateLayoutFlow(uiContext);
			break;
		}
	}

	void UINode::CalculateLayoutAbsolute(const UIContext* ctx) {
		const UINode* relNode = ctx->GetLastRelativeNode();
		if (!relNode)
			return;

		const Vector2 basePos = relNode->GetPosition();
		const Vector2 baseSize = relNode->GetSize();

		const float top = ResolveAbsoluteValue(
			relNode, false,
			m_absolutePositionUnits[0],
			m_absolutePositionOffset.x
		);

		const float left = ResolveAbsoluteValue(
			relNode, true,
			m_absolutePositionUnits[1],
			m_absolutePositionOffset.y
		);

		const float bottom = ResolveAbsoluteValue(
			relNode, false,
			m_absolutePositionUnits[2],
			m_absolutePositionOffset.z
		);

		const float right = ResolveAbsoluteValue(
			relNode, true,
			m_absolutePositionUnits[3],
			m_absolutePositionOffset.w
		);

		// CSS-like behavior:
		// position = left - right, top - bottom
		m_position.x = basePos.x + left - right;
		m_position.y = basePos.y + top - bottom;
	}

	void UINode::CalculateLayoutFlow(const UIContext* uiContext) {
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

		if (m_isDisabled) {
			SetState(UIState::DISABLED);
			event->SetIsDisabled(true);
			return;
		}

		if (!m_hasPointerEvents) {
			SetState(UIState::NORMAL);
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

		// if is outside of the clipping rect skip events
		if (!IsPointInClipRect(mousePos)) {
			SetState(UIState::NORMAL);
			ProcessEvent(event);
			return;
		}

		// Hover state
		if (isHovered)
			event->SetIsHovered(true);

		// Press start
		if (mouseJustDown && isHovered) {
			ctx->CapturePressNode(m_id);
			SetState(UIState::PRESSED);
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
			SetState(UIState::PRESSED);
			event->SetIsPressed(true);
		}
		else {
			if (pressCaptured && !isHovered) {
				ctx->ReleasePressNode(m_id);
			}
			SetState(isHovered ? UIState::HOVER : UIState::NORMAL);
		}

		ProcessEvent(event);
	}

}