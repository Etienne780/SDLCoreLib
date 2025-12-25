#include <string>

#include "UI/Types/UIContext.h"
#include "SDLCoreRenderer.h"
#include "UI/Nodes/TextNode.h"

namespace SDLCore::UI {

	static std::string textNodeName = "Text";
	TextNode::TextNode(int childPos, uintptr_t key)
		: UINode(childPos, key, textNodeName) {
	}

	void TextNode::RenderNode(UIContext* ctx) const {
		namespace RE = SDLCore::Render;
		if(m_textSize > 0) {
			RE::SetFontSize(m_textSize);
			RE::SetColor(m_textColor);
			RE::CachText(true);
			RE::Text(m_text, this->GetPosition());
		}
	}

	uint32_t TextNode::GetType() {
		return Internal::nameToID[textNodeName];
	}

	void TextNode::ApplyStyleCalled(UIContext* ctx, const UIStyleState& styleState) {
		if (!ctx)
			return;

		styleState.TryGetValue(Properties::textSize, m_textSize);
		styleState.TryGetValue(Properties::textColor, m_textColor);
	}

	Vector2 TextNode::CalculateSize(UIContext* context, UISizeUnit unitW, UISizeUnit unitH, float w, float h) {
		namespace RE = SDLCore::Render;
		if (m_textSize <= 0) 
			return Vector2(0);

		RE::SetFontSize(m_textSize);
		return Vector2(
			RE::GetTextBlockWidth(m_text),
			RE::GetTextBlockHeight(m_text)
		);
	}

}