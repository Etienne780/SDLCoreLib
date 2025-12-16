#pragma once
#include "UI/UINode.h"

namespace SDLCore::UI {

    class FrameNode : public UINode {
    public:
        FrameNode(int childPos, uintptr_t key);

        void ApplyStyleCalled(UIContext* context, const UIStyleState& styleState) override;
        void RenderNode(UIContext* ctx) const override;

        static uint32_t GetType();

        Vector4 m_backgroundColor;
        Vector4 m_borderColor;

    private:
    };

}
