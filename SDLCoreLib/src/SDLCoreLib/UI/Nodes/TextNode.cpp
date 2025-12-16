#include <string>

#include "UI/Types/UIContext.h"
#include "SDLCoreRenderer.h"
#include "UI/Nodes/TextNode.h"

namespace SDLCore::UI {

	static std::string textNodeName = "Text";
	TextNode::TextNode(int childPos, uintptr_t key)
		: UINode(childPos, key, textNodeName) {
		// hard codes Frame node to ui type 1
	}

	void TextNode::ApplyStyleCalled(UIContext* ctx, const UIStyleState& styleState) {
		if (!ctx)
			return;

	}

	void TextNode::RenderNode(UIContext* ctx) const {
		namespace RE = SDLCore::Render;
		RE::SetFontSize(24);
		RE::SetColor(255);
		RE::Text(m_text, this->GetPosition());
	}

	uint32_t TextNode::GetType() {
		return Internal::nameToID[textNodeName];
	}

}