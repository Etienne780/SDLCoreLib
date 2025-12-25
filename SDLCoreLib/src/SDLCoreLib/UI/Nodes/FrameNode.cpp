#include <string>

#include "UI/Types/UIContext.h"
#include "SDLCoreRenderer.h"
#include "UI/Nodes/FrameNode.h"

#include "Profiler.h"

namespace SDLCore::UI {

	static std::string frameNodeName = "Frame";
	FrameNode::FrameNode(int childPos, uintptr_t key)
		: UINode(childPos, key, frameNodeName) {
	}

	void FrameNode::RenderNode(UIContext* ctx) const {
		namespace RE = SDLCore::Render;
		SDLCore::Debug::ProfilerScope das("Render(Frame)");

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

		m_useTexture = styleState.TryGetValue<std::shared_ptr<Texture>>(Properties::backgroundTexture, m_texture);
	}


}