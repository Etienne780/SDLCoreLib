#include <string>

#include "UI/Types/UIContext.h"
#include "SDLCoreRenderer.h"
#include "UI/Nodes/FrameNode.h"

namespace SDLCore::UI {

	static std::string frameNodeName = "Frame";
	FrameNode::FrameNode(int childPos, uintptr_t key)
		: UINode(childPos, key, frameNodeName) {
	}

	void FrameNode::RenderNode(UIContext* ctx) const {
		namespace RE = SDLCore::Render;

		if (m_backgroundColor.w > 0) {
			if (m_useTexture && m_texture) {
				const Vector2& size = this->GetSize();
				if (size != Vector2::zero) {
					m_texture->SetColorTint(m_backgroundColor);
					m_texture->Render(this->GetPosition(), size);
				}
			}
			else {
				RE::SetColor(m_backgroundColor);
				RE::FillRect(this->GetPosition(), this->GetSize());
			}
		}

		if (m_borderWidth > 0 && m_borderColor.w > 0) {
			RE::SetColor(m_borderColor);
			RE::SetInnerStroke(m_innerBorder);
			RE::SetStrokeWidth(m_borderWidth);
			RE::Rect(this->GetPosition(), this->GetSize());
		}
	}

	uint32_t FrameNode::GetType() {
		return Internal::nameToID[frameNodeName];
	}

	void FrameNode::ApplyStyleCalled(UIContext* ctx, const UIStyleState& styleState) {
		if (!ctx)
			return;

		styleState.TryGetValue<Vector4>(Properties::backgroundColor, m_backgroundColor, Vector4(0.0f));
		styleState.TryGetValue<Vector4>(Properties::borderColor, m_borderColor, Vector4(0.0f));
		m_useTexture = styleState.TryGetValue<std::shared_ptr<Texture>>(Properties::backgroundTexture, m_texture, nullptr);
	}


}