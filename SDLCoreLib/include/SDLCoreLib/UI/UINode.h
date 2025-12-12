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
            child->m_parent = this;
            m_children.push_back(std::move(ptr));
            return child;
        }

        // copys style
        void AddStyle(const UIStyle& style);

        bool ContainsChildAtPos(uint16_t position, uintptr_t id, UINode*& outNode);

        uintptr_t GetID() const;
        UIEvent GetEvent() const;
        UINodeType GetType() const;
        const std::vector<std::shared_ptr<UINode>>& GetChildren() const;

    protected:
        void RemoveChildrenFromIndex(uint16_t position);

        /**
        * @brief Merge all appliedStyles into a final UIStyle object.
        * @return Resolved UIStyle
        */
        UIStyle CreateStyle();

        uintptr_t m_id = 0;
        UINodeType m_type;
        UINode* m_parent = nullptr;

        std::vector<std::shared_ptr<UINode>> m_children;
        std::vector<UIStyle> m_appliedStyles;
        UIStyle m_finalStyle;
        UIEvent m_eventState;

        Vector2 m_position;
    private: 
        UINodeType GenerateUIType();
    };

    class FrameNode : public UINode {
    public:
        FrameNode(uintptr_t key);

        /*
        * @brief Inits the Frame
        */
        void Init(const UIContext* uiContext);

        Vector2 size;
        Vector4 padding;
        Vector4 margin;

        Vector4 backgroundColor;
        Vector4 borderColor;

    private:
        void CalculateLayout(const UIContext* uiContext);
    };

    class TextNode : public UINode {
    public:
        TextNode(uintptr_t key);

        std::string text;
        float textSize = 0;
        Vector4 textColor;
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