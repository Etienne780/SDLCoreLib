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

    class UINode {
    public:
        UINode(NodeType t);
        virtual ~UINode() = default;

        /**
        * @brief Add a child node and assign parent pointer.
        */
        void AddChild(const std::shared_ptr<UINode>& child);

        /**
        * @brief Return event state of this node.
        */
        UIEvent GetEvent() const;

    protected:

        /**
        * @brief Merge all appliedStyles into a final UIStyle object.
        * @return Resolved UIStyle
        */
        UIStyle CreateStyle();

        NodeType type;
        UINode* parent = nullptr;

        std::vector<std::shared_ptr<UINode>> children;
        std::vector<UIStyle> appliedStyles;

        UIEvent eventState;

        Vector2 position;
    };

    class FrameNode : public UINode {
    public:
        FrameNode();

        Vector2 size;
        Vector4 padding;
        Vector4 margin;

        Vector4 backgroundColor;
        Vector4 borderColor;
    };

    class TextNode : public UINode {
    public:
        TextNode();

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