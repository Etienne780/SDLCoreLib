#pragma once
#include <vector>
#include <string>

#include <CoreLib/Math/Vector2.h>
#include <CoreLib/Math/Vector4.h>
#include <CoreLib/FormatUtils.h>

#include "UI/UIStyle.h"
#include "UI/Types/UIEvent.h"

namespace SDLCore::UI {

    enum class NodeType {
        Frame,
        Text
    };

    class UIContext;

    class UINode {
        friend class UIContext;
    public:
        UINode(uintptr_t id, NodeType t);
        virtual ~UINode();

        template<typename T, typename... Args>
        T* AddChild(Args&&... args) {
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
        NodeType GetType() const;
        const std::vector<std::shared_ptr<UINode>>& GetChildren() const;

    protected:
        void RemoveChildrenFromIndex(uint16_t position);

        /**
        * @brief Merge all appliedStyles into a final UIStyle object.
        * @return Resolved UIStyle
        */
        UIStyle CreateStyle();

        uintptr_t m_id = 0;
        NodeType m_type;
        UINode* m_parent = nullptr;

        std::vector<std::shared_ptr<UINode>> m_children;
        std::vector<UIStyle> m_appliedStyles;
        UIStyle m_finalStyle;
        UIEvent m_eventState;

        Vector2 m_position;
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
static inline std::string FormatUtils::toString<SDLCore::UI::NodeType>(SDLCore::UI::NodeType type) {
    switch (type) {
    case SDLCore::UI::NodeType::Frame: return "Frame";
    case SDLCore::UI::NodeType::Text:  return "Text";
    default:                           return "UNKOWN";
    }
}