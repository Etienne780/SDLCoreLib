#include <string>

#include "SDLCoreRenderer.h"
#include "UI/Nodes/FrameNode.h"

namespace SDLCore::UI {

	static std::string frameNodeName = "Frame";
	FrameNode::FrameNode(int childPos, uintptr_t key)
		: UINode(childPos, key, frameNodeName) {
		// hard codes Frame node to ui type 0
	}

	void FrameNode::ApplyStyleCalled(UIContext* ctx, const UIStyleState& styleState) {
		if (!ctx)
			return;

	}

	void FrameNode::RenderNode() const {
		namespace RE = SDLCore::Render;

		RE::SetColor(255, 0, 0, 100);
		RE::FillRect(root->GetPosition(), root->GetSize());
	}

	uint32_t FrameNode::GetType() {
		return Internal::nameToID[frameNodeName];
	}

}