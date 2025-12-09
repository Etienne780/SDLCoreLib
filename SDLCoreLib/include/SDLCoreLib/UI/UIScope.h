#pragma once
#include <memory>
#include <vector>
#include <string>

#include <CoreLib/Log.h>
#include "UI/UINode.h"

namespace SDLCore::UI {

    static inline UIContext g_UIContext;

    /**
    * @brief Begin a new FrameNode and push it onto the UI stack.
    */
    template<typename... Styles>
    void BeginFrame(const Styles&... styles) {
        auto node = std::make_shared<FrameNode>();

        // Copy styles into node
        (node->appliedStyles.push_back(styles), ...);

        if (!g_UIContext.nodeStack.empty()) {
            g_UIContext.nodeStack.back()->AddChild(node);
        }
        else {
            g_UIContext.rootNode = node;
        }
        
        node->Init(&g_IUContext);
        g_UIContext.nodeStack.push_back(node.get());
    }

    /**
     * @brief End the current FrameNode and pop from UI stack.
     */
    inline UIEvent EndFrame() {
        if (g_UIContext.nodeStack.empty())
            return UIEvent{};

        FrameNode* node = static_cast<FrameNode*>(g_UIContext.nodeStack.back());
        g_UIContext.nodeStack.pop_back();

        return node ? node->GetEvent() : UIEvent{};
    }

    /**
    * @brief Create a TextNode and add it to the stack top.
    */
    template<typename... Styles>
    void Text(const std::string& text, const Styles&... styles) {
        auto node = std::make_shared<TextNode>();
        node->text = text;

        // Copy styles safely
        (node->appliedStyles.push_back(styles), ...);

        if (!g_UIContext.nodeStack.empty()) {
            g_UIContext.nodeStack.back()->AddChild(node);
        }
        else {
            Log::Error("SDLCore::UI::Text: Text cannot be added as root element");
        }
    }

    /**
    * @brief RAII scope for FrameNode begin/end.
    */
    struct FrameScope
    {
        FrameScope() {
            BeginFrame();
            m_node = g_UIContext.nodeStack.back();
        }

        template<typename... Styles>
        FrameScope(const Styles&... styles) {
            BeginFrame(styles...);
            m_node = g_UIContext.nodeStack.back();
        }

        ~FrameScope() {
            if (!m_manualEndDone)
                SafeEnd();
        }

        UIEvent EndGetEvent() {
            UIEvent evt = SafeEnd();

            if (m_nodeWasClosed)
                m_manualEndDone = true;
            else
                Log::Warn("SDLCore::UI::FrameScope::EndGetEvent: Ignored because this scope is not the active UI frame (node is not top of stack)");

            return evt;
        }

    private:
        UINode* m_node = nullptr;
        bool m_manualEndDone = false;
        bool m_nodeWasClosed = false;

        UIEvent SafeEnd() {
            if (g_UIContext.nodeStack.empty())
                return {};

            if (g_UIContext.nodeStack.back() != m_node) {
                return {};
            }

            m_nodeWasClosed = true;
            return EndFrame();
        }
    };

}