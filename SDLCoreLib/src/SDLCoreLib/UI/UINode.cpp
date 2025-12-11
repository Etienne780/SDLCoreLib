#include "UI/UINode.h"

namespace SDLCore::UI {

	#pragma region UINode

	UINode::UINode(uintptr_t id, NodeType t)
		: m_id(id), m_type(t) {
	}

	void UINode::AddChild(const std::shared_ptr<UINode>& child) {
		child->m_parent = this;
		m_children.push_back(child);
	}

	bool UINode::ContainsChildAtPos(uint16_t pos, uintptr_t id) {
		if (pos >= m_children.size())
			return false;
		return (m_children[pos]->GetID() == id);
	}

	UIEvent UINode::GetEvent() const {
		return m_eventState;
	}

	uintptr_t UINode::GetID() const {
		return m_id;
	}

	UIStyle UINode::CreateStyle() {
		UIStyle outStyle;
		
		for (auto& style : m_appliedStyles) {
			style.Merge(outStyle);
		}

		return outStyle;
	}

	#pragma endregion

	#pragma region FrameNode

	FrameNode::FrameNode(uintptr_t key)
		: UINode(key, NodeType::Frame) {
	}

	void FrameNode::Init(const UIContext* uiContext) {
		m_finalStyle = CreateStyle();
		CalculateLayout(uiContext);
	}

	void FrameNode::CalculateLayout(const UIContext* uiContext) {

	}

	#pragma endregion

	#pragma region TextNode

	TextNode::TextNode(uintptr_t key)
		: UINode(key, NodeType::Frame) {
	}

	#pragma endregion

}