#pragma once
#include "UI/UINode.h"

namespace SDLCore::UI {

    class TextNode : public UINode {
    public:
        TextNode(int childPos, uintptr_t key);

        void RenderNode(UIContext* ctx) const override;

        static uint32_t GetType();

        std::string m_text;
        float m_textSize = 0;
        Vector4 m_textColor;

    private:
        void ApplyStyleCalled(UIContext* context, const UIStyleState& styleState) override;
        Vector2 CalculateSize(UIContext* context, UISizeUnit unitW, UISizeUnit unitH, float w, float h) override;
    };

}