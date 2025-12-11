#include "UI/UINode.h"

namespace SDLCore::UI {

	#pragma region UINode

	UINode::UINode(uintptr_t id, NodeType t)
		: m_id(id), m_type(t) {
		Log::Debug("CreadedNode: id={} type={}", id, t);
	}

	UINode::~UINode() {
		Log::Debug("DestroyedNode: id={} type={}", m_id, m_type);
	}

	void UINode::AddChild(const std::shared_ptr<UINode>& child) {
		child->m_parent = this;
		m_children.push_back(child);
	}

	void UINode::AddStyle(const UIStyle& style) {
		m_appliedStyles.push_back(style);
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

	NodeType UINode::GetType() const {
		return m_type;
	}

	const std::vector<std::shared_ptr<UINode>>& UINode::GetChildren() const {
		return m_children;
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