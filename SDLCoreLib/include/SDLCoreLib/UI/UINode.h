#pragma once
#include <vector>
#include <string>

#include <CoreLib/Math/Vector2.h>
#include <CoreLib/Math/Vector4.h>
#include <CoreLib/FormatUtils.h>

#include "UI/Types/UINodeTypeRegistry.h"
#include "UI/UIStyle.h"
#include "UI/Types/UIEvent.h"

namespace SDLCore::UI {

    class UIContext;
    class FrameNode;

    class UINode {
        friend class UIContext;
    public:
        UINode(uintptr_t id);
        UINode(uintptr_t id, UINodeType type);
        virtual ~UINode();

        template<typename T, typename... Args>
        T* AddChild(Args&&... args) {
            static_assert(std::is_base_of<UINode, T>::value, "T must derive from UINode");

            auto ptr = std::make_unique<T>(std::forward<Args>(args)...);
            T* child = ptr.get();
            child->m_parent = reinterpret_cast<FrameNode*>(this);
            m_children.push_back(std::move(ptr));
            return child;
        }

        // copys style
        void AddStyle(const UIStyle& style);

        /*
        * @brief Create and applys the style from all of the added styles
        */
        void ApplyStyle(UIContext* context);

        /*
        * @brief checks if a child at a given position has the id, if true outNode is this child
        */
        bool ContainsChildAtPos(uint16_t position, uintptr_t id, UINode*& outNode);

        uintptr_t GetID() const;
        UIEvent GetEvent() const;
        UIEvent* GetEventPtr();
        UINodeType GetType() const;
        UINode* GetParent();
        const std::vector<std::shared_ptr<UINode>>& GetChildren() const;
        bool GetChildHasEvent() const;

        /*
        * @brief used internaly to find out what elements have events
        * 
        * should not be set manuly
        */
        void SetChildHasEvent(bool value);

    protected:
        void RemoveChildrenFromIndex(uint16_t position);

        /**
        * @brief Merge all appliedStyles into the final UIStyle object.
        */
        UIStyle CreateStyle();

        virtual void ApplyStyleCalled(UIContext* context, const UIStyleState& styleState) = 0;

        uintptr_t m_id = 0;
        UINodeType m_type;
        FrameNode* m_parent = nullptr;

        std::vector<std::shared_ptr<UINode>> m_children;
        std::vector<UIStyle> m_appliedStyles;
        UIState m_state = UIState::NORMAL;
        UIStyle m_finalStyle;
        UIEvent m_eventState;
        bool m_childHasEvent = false;

        Vector2 m_position;
        UILayoutDirection m_layoutDir = UILayoutDirection::ROW;
        UIAlignment m_horizontalAligment = UIAlignment::START;
        UIAlignment m_verticalAligment = UIAlignment::START;
    private: 
        UINodeType GenerateUIType();
    };

    class FrameNode : public UINode {
    public:
        FrameNode(uintptr_t key);

        void ApplyStyleCalled(UIContext* context, const UIStyleState& styleState) override;

        Vector2 m_size;
        Vector4 m_padding;
        Vector4 m_margin;

        Vector4 m_backgroundColor;
        Vector4 m_borderColor;

    private:
        static float AlignOffset(UIAlignment align, float freeSpace);
        Vector2 CalculateSize(UIContext* context, UISizeUnit unitW, UISizeUnit unitH, float w, float h);
        void CalculateLayout(const UIContext* uiContext);
    };

    class TextNode : public UINode {
    public:
        TextNode(uintptr_t key);

        void ApplyStyleCalled(UIContext* context, const UIStyleState& styleState) override;

        std::string m_text;
        float m_textSize = 0;
        Vector4 m_textColor;
    };

}

template<>
static inline std::string FormatUtils::toString<SDLCore::UI::UINodeType>(SDLCore::UI::UINodeType type) {
    switch (type.value) {
    case 0:     return "Frame";
    case 1:     return "Text"; 
    default: {
        if (type.value < SDLCore::UI::UINodeTypeRegistry::GetRegisterTypeCount())
            return "Custom";
        else
            return "UNKWON";
    }
    }
}