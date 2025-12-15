#include <unordered_map>
#include "UI/Types/UIStyleState.h"
#include "UI/Types/UIContext.h"
#include "UI/UINode.h"

namespace SDLCore::UI {

	static std::unordered_map<std::string, uint32_t> nameToID;

	#pragma region UINode

	UINode::UINode(int childPos, uintptr_t id, const std::string& typeName)
		: m_childPos(childPos), m_id(id), m_typeName(typeName){
		m_typeID = GetUITypeID(m_typeName);

		Log::Debug("CreadedNode: id={} name={}", id, m_typeName);
	}

	UINode::~UINode() {
		Log::Debug("DestroyedNode: id={} name={}", m_id, m_typeName);
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

		ApplyStyleCalled(ctx, styleState);
		// calculates the position after scale
		CalculateLayout(ctx);
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

	uint32_t UINode::GetUITypeID(const std::string& name) {
		auto it = nameToID.find(name);
		if (it == nameToID.end()) {
			uint32_t newID = m_typeIDCounter;
			nameToID[name] = newID;
			m_typeIDCounter++;
			return newID;
		}
		return it->second;
	}

	Vector2 UINode::CalculateSize(UIContext* ctx, UISizeUnit unitW, UISizeUnit unitH, float w, float h) {
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

	float UINode::GetAccumulatedChildSize(bool horizontal, int upToIndex) const {
		if (!m_parent)
			return 0.0f;

		const auto& childs = m_parent->GetChildren();
		float size = 0.0f;

		for (int i = 0; i < upToIndex && i < static_cast<int>(childs.size()); ++i) {
			size += horizontal ? childs[i]->m_size.x : childs[i]->m_size.y;
		}

		return size;
	}

	float UINode::GetTotalChildrenSize(bool horizontal) const {
		if (!m_parent)
			return 0.0f;

		const auto& childs = m_parent->GetChildren();
		float size = 0.0f;

		for (const auto& c : childs) {
			size += horizontal ? c->m_size.x : c->m_size.y;
		}

		return size;
	}

	float UINode::AlignOffset(bool isHor, UIAlignment align, float freeSpace) {
		if (!m_parent)
			return 0.0f;

		const auto& childs = m_parent->GetChildren();

		switch (align) {

		case UIAlignment::START:
			// Flow: position after previous siblings
			if (m_childPos > 0 && m_childPos < static_cast<int>(childs.size())) {
				return GetAccumulatedChildSize(isHor, m_childPos);
			}
			return 0.0f;

		case UIAlignment::CENTER: {
			const float totalSize = GetTotalChildrenSize(isHor);
			const float start = (freeSpace - totalSize) * 0.5f;
			return start + GetAccumulatedChildSize(isHor, m_childPos);
		}

		case UIAlignment::END: {
			const float totalSize = GetTotalChildrenSize(isHor);
			const float start = freeSpace - totalSize;
			return start + GetAccumulatedChildSize(isHor, m_childPos);
		}

		default:
			return 0.0f;
		}
	}

	void UINode::CalculateLayout(const UIContext* uiContext) {
		if (!uiContext)
			return;

		m_position.Set(0);

		// Root node, always at 0, 0
		if (!m_parent)
			return;

		const Vector2 parentSize = m_parent->m_size;

		const float freeX = parentSize.x - m_size.x;
		const float freeY = parentSize.y - m_size.y;

		m_position.x = m_parent->m_position.x + AlignOffset(true, m_horizontalAligment, freeX);
		m_position.y = m_parent->m_position.y + AlignOffset(false, m_verticalAligment, freeY);
	}

	#pragma endregion

	#pragma region FrameNode

	static std::string frameNodeName = "Frame";
	FrameNode::FrameNode(int childPos, uintptr_t key)
		: UINode(childPos, key, frameNodeName) {
		// hard codes Frame node to ui type 0
	}

	void FrameNode::ApplyStyleCalled(UIContext* ctx, const UIStyleState& styleState) {
		if (!ctx)
			return;

	}

	uint32_t FrameNode::GetType() {
		return nameToID[frameNodeName];
	}

	#pragma endregion

	#pragma region TextNode

	static std::string textNodeName = "Text";
	TextNode::TextNode(int childPos, uintptr_t key)
		: UINode(childPos, key, textNodeName) {
		// hard codes Frame node to ui type 1
	}

	void TextNode::ApplyStyleCalled(UIContext* ctx, const UIStyleState& styleState) {
		if (!ctx)
			return;

	}

	uint32_t TextNode::GetType() {
		return nameToID[textNodeName];
	}

	#pragma endregion

}