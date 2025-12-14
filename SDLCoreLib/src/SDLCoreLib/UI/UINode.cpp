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

	void UINode::ApplyStyle() {
		m_finalStyle = CreateStyle();



		ApplyStyleCalled();
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

	void FrameNode::ApplyStyleCalled() {
		
	}

	void FrameNode::CalculateLayout(const UIContext* uiContext) {

	}

	#pragma endregion

	#pragma region TextNode

	TextNode::TextNode(uintptr_t key)
		: UINode(key, UINodeType(1)) {
		// hard codes Frame node to ui type 1
	}

	void TextNode::ApplyStyleCalled() {
	
	}

	#pragma endregion

}