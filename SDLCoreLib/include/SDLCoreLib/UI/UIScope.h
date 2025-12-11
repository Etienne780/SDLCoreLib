#pragma once
#include <memory>
#include <vector>
#include <string>

#include <CoreLib/Log.h>

#include "UI/Types/UIContext.h" 
#include "UI/UINode.h"

namespace SDLCore::UI {

    UIContext* CreateContext();
    UIContext* CreateContext(WindowID id);
    void DestroyContext(UIContext* context);
    void BindContext(UIContext* context);
    /*
    * @brief SDLCore::Renderer::Presents needs to called to see the UI
    */
    void RenderContext(UIContext* context);

    void SetContextWindow(UIContext* ctx, WindowID id);
    UIContext* GetCurrentContext();

    namespace {
        /*
        * @brief Begins frame internal. Creates frame if necessary
        */
        FrameNode* InternalBeginFrame(uintptr_t key);
    }

    /**
    * @brief Begin a new FrameNode and push it onto the UI stack.
    */
    template<typename... Styles>
    void BeginFrame(UIKey&& key, const Styles&... styles) {
        // auto node = std::make_shared<FrameNode>();

        // Copy styles into node
        // 

        // if (!g_UIContext.nodeStack.empty()) {
        //     g_UIContext.nodeStack.back()->AddChild(node);
        // }
        // else {
        //     g_UIContext.rootNode = node;
        // }
        // 
        // node->Init(&g_UIContext);
        // g_UIContext.nodeStack.push_back(node.get());
        FrameNode* frame = InternalBeginFrame(key);
        if (frame) {
            (frame->m_appliedStyles.push_back(styles), ...);
        }
    }

    /**
    * @brief End the current FrameNode and pop from UI stack.
    */
    UIEvent EndFrame();

    /**
    * @brief Create a TextNode and add it to the stack top.
    */
    template<typename... Styles>
    void Text(const std::string& text, const Styles&... styles) {
        auto node = std::make_shared<TextNode>();
        node->text = text;

        // Copy styles safely
        (node->m_appliedStyles.push_back(styles), ...);

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
        FrameScope();

        template<typename... Styles>
        FrameScope(UIKey&& key, const Styles&... styles) {
           // BeginFrame(styles...);
           // m_node = g_UIContext.nodeStack.back();
        }

        ~FrameScope();

        UIEvent EndGetEvent();

    private:
        UINode* m_node = nullptr;
        bool m_manualEndDone = false;
        bool m_nodeWasClosed = false;

        UIEvent SafeEnd();
    };

}