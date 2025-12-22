#include <string>

#include "UI/Types/UIContext.h"
#include "SDLCoreRenderer.h"
#include "UI/Nodes/FrameNode.h"

namespace SDLCore::UI {

	static std::string frameNodeName = "Frame";
	FrameNode::FrameNode(int childPos, uintptr_t key)
		: UINode(childPos, key, frameNodeName) {
	}

	void FrameNode::ApplyStyleCalled(UIContext* ctx, const UIStyleState& styleState) {
		if (!ctx)
			return;

		m_backgroundColor.Set(0);
		styleState.TryGetValue<Vector4>(Properties::backgroundColor, m_backgroundColor);

		m_borderColor.Set(0);
		styleState.TryGetValue<Vector4>(Properties::borderColor, m_borderColor);

		m_borderWidth = 0;
		styleState.TryGetValue<float>(Properties::borderWidth, m_borderWidth);

		m_innerBorder = false;
		styleState.TryGetValue<bool>(Properties::borderInset, m_innerBorder);
	}

	void FrameNode::RenderNode(UIContext* ctx) const {
		namespace RE = SDLCore::Render;

		RE::SetColor(m_backgroundColor);
		RE::FillRect(this->GetPosition(), this->GetSize());

		if (m_borderWidth > 0) {
			RE::SetColor(m_borderColor);
			RE::SetInnerStroke(m_innerBorder);
			RE::SetStrokeWidth(m_borderWidth);
			RE::Rect(this->GetPosition(), this->GetSize());
		}
	}

	uint32_t FrameNode::GetType() {
		return Internal::nameToID[frameNodeName];
	}

}