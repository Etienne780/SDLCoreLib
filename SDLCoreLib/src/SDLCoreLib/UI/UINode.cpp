#include "UI/Types/UIStyleState.h"
#include "UI/Types/UIContext.h"
#include "UI/UINode.h"

namespace SDLCore::UI {

	#pragma region UINode

	UINode::UINode(uintptr_t id)
		: UINode(id, GenerateUIType()){
	}

	UINode::UINode(uintptr_t id, UINodeType type) 
		: m_id(id), m_type(type) {
		Log::Debug("CreadedNode: id={} type={}", id, m_type);
	}

	UINode::~UINode() {
		Log::Debug("DestroyedNode: id={} type={}", m_id, m_type);
	}

	void UINode::AddStyle(const UIStyle& style) {
		m_appliedStyles.push_back(style);
	}

	void UINode::ApplyStyle(UIContext* ctx) {
		m_finalStyle = CreateStyle();

		const UIStyleState& styleState = m_finalStyle.GetStyleState(m_state);

		int layoutDir = 0;
		int alignHorizontal = 0;
		int alignVertical = 0;

		styleState.TryGetValue<int>(Properties::layoutDirection, layoutDir);
		styleState.TryGetValue<int>(Properties::alignHorizontal, alignHorizontal);
		styleState.TryGetValue<int>(Properties::alignVertical, alignVertical);

		m_layoutDir = static_cast<UILayoutDirection>(layoutDir);
		m_horizontalAligment = static_cast<UIAlignment>(alignHorizontal);
		m_verticalAligment = static_cast<UIAlignment>(alignVertical);

		ApplyStyleCalled(ctx, styleState);
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

	UIEvent UINode::GetEvent() const {
		return m_eventState;
	}
	
	UIEvent* UINode::GetEventPtr() {
		return &m_eventState;
	}

	UINodeType UINode::GetType() const {
		return m_type;
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
	
	void UINode::SetChildHasEvent(bool value) {
		m_childHasEvent = value;
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

	UINodeType UINode::GenerateUIType() {
		static UINodeType type = UINodeTypeRegistry::RegisterType();
		return type;
	}

	#pragma endregion

	#pragma region FrameNode

	FrameNode::FrameNode(uintptr_t key)
		: UINode(key, UINodeType(0)) {
		// hard codes Frame node to ui type 0
	}

	void FrameNode::ApplyStyleCalled(UIContext* ctx, const UIStyleState& styleState) {
		if (!ctx)
			return;

		float width = 0.0f;
		float height = 0.0f;

		styleState.TryGetValue<float>(Properties::width, width);
		styleState.TryGetValue<float>(Properties::height, height);

		int sizeUnitW = 0;// 0 = PX
		int sizeUnitH = 0;
		styleState.TryGetValue<int>(Properties::sizeUnitW, sizeUnitW);
		styleState.TryGetValue<int>(Properties::sizeUnitH, sizeUnitH);

		m_size = CalculateSize(ctx,
			static_cast<UISizeUnit>(sizeUnitW), 
			static_cast<UISizeUnit>(sizeUnitH), 
			width, height);

		m_padding.Set(0);
		m_margin.Set(0);
		styleState.TryGetValue<Vector4>(Properties::padding, m_padding);
		styleState.TryGetValue<Vector4>(Properties::margin, m_margin);
	}

	Vector2 FrameNode::CalculateSize(UIContext* ctx, UISizeUnit unitW, UISizeUnit unitH, float w, float h) {
		if (!ctx)
			return Vector2(0.0f, 0.0f);

		const float winScale = ctx->GetWindowScale();

		auto resolveBaseSize = [&](bool horizontal) -> float {
			if (m_parent) {
				return horizontal ? m_parent->m_size.x : m_parent->m_size.y;
			}
			return horizontal ? ctx->GetWindowSize().x : ctx->GetWindowSize().y;
		};

		auto calc = [&](bool horizontal, UISizeUnit unit, float value) -> float {
			switch (unit) {
			case UISizeUnit::PX:
				return value * winScale;

			case UISizeUnit::PERCENTAGE:
				return (resolveBaseSize(horizontal) * value / 100.0f) * winScale;

			case UISizeUnit::PERCENTAGE_W:
				return (resolveBaseSize(true) * value / 100.0f) * winScale;

			case UISizeUnit::PERCENTAGE_H:
				return (resolveBaseSize(false) * value / 100.0f) * winScale;

			default:
				return 0.0f;
			}
		};

		return Vector2(
			calc(true, unitW, w),
			calc(false, unitH, h)
		);
	}

	float FrameNode::AlignOffset(UIAlignment align, float freeSpace) {
		switch (align) {
		case UIAlignment::START:  return 0.0f;
		case UIAlignment::CENTER: return freeSpace * 0.5f;
		case UIAlignment::END:    return freeSpace;
		default:                 return 0.0f;
		}
	}

	void FrameNode::CalculateLayout(const UIContext* uiContext) {
		if (!uiContext)
			return;

		// Root node
		if (!m_parent) {
			m_position = Vector2(0.0f, 0.0f);
			return;
		}

		const Vector2 parentSize = m_parent->m_size;

		const float freeX = parentSize.x - m_size.x;
		const float freeY = parentSize.y - m_size.y;

		m_position.x = m_parent->m_position.x + AlignOffset(m_horizontalAligment, freeX);
		m_position.y = m_parent->m_position.y + AlignOffset(m_verticalAligment, freeY);
	}

	#pragma endregion

	#pragma region TextNode

	TextNode::TextNode(uintptr_t key)
		: UINode(key, UINodeType(1)) {
		// hard codes Frame node to ui type 1
	}

	void TextNode::ApplyStyleCalled(UIContext* ctx, const UIStyleState& styleState) {
		if (!ctx)
			return;
	}

	#pragma endregion

}