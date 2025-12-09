#include "UI/UINode.h"

namespace SDLCore::UI {

	#pragma region UINode

	void UINode::AddChild(const std::shared_ptr<UINode>& child) {
		child->parent = this;
		children.push_back(child);
	}

	UIEvent UINode::GetEvent() const {
		return this->eventState;
	}

	UINode::UINode(NodeType t) 
		: type(t) {
	}

	UIStyle UINode::CreateStyle() {
		UIStyle outStyle{ "style" };
		
		for (auto& style : appliedStyles) {
			style.Merge(outStyle);
		}

		return outStyle;
	}

	#pragma endregion

	#pragma region FrameNode

	FrameNode::FrameNode() 
		: UINode(NodeType::Frame) {
	}

	void FrameNode::Init(const UIContext* uiContext) {
		CalculateLayout(uiContext);
	}

	void FrameNode::CalculateLayout(const UIContext* uiContext) {
		
	}

	#pragma endregion

	#pragma region TextNode

	TextNode::TextNode() 
		: UINode(NodeType::Frame) {
	}

	#pragma endregion

}