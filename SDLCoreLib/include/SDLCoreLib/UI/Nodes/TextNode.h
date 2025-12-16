#pragma once
#include "UI/UINode.h"

namespace SDLCore::UI {

    class TextNode : public UINode {
    public:
        TextNode(int childPos, uintptr_t key);

        void ApplyStyleCalled(UIContext* context, const UIStyleState& styleState) override;
        void RenderNode(UIContext* ctx) const override;

        static uint32_t GetType();

        std::string m_text;
        float m_textSize = 0;
        Vector4 m_textColor;
    };

}