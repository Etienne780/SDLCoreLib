#include "UI/UINode.h"

namespace SDLCore::UI {

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

	FrameNode::FrameNode() 
		: UINode(NodeType::Frame) {
	}

	TextNode::TextNode() 
		: UINode(NodeType::Frame) {
	}

}