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

		const Vector2& pos = this->GetRenderPosition();
		const Vector2& size = this->GetRenderSize();

		if (m_backgroundColor.w > 0) {
			if (m_useTexture && m_texture) {
				if (size != Vector2::zero) {
					m_texture->SetColorTint(m_backgroundColor);
					m_texture->Render(pos, size);
				}
			}
			else {
				RE::SetColor(m_backgroundColor);
				RE::FillRect(pos, size);
			}
		}

		if (m_borderWidth > 0 && m_borderColor.w > 0) {
			RE::SetColor(m_borderColor);
			RE::SetInnerStroke(m_innerBorder);
			RE::SetStrokeWidth(m_borderWidth);
			RE::Rect(pos, size);
		}
	}

	uint32_t FrameNode::GetType() {
		return Internal::nameToID[frameNodeName];
	}

	void FrameNode::ApplyStyleCalled(UIContext* ctx, const UIStyleState& styleState) {
		if (!ctx)
			return;

		GetResolvedValue<Vector4>(Properties::backgroundColor, m_backgroundColor, Vector4(0.0f));
		GetResolvedValue<Vector4>(Properties::borderColor, m_borderColor, Vector4(0.0f));
		m_useTexture = GetResolvedValue<std::shared_ptr<Texture>>(Properties::backgroundTexture, m_texture, nullptr);
	}


}