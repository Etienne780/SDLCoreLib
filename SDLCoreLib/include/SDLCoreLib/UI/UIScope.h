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

    void SetContextWindow(UIContext* ctx, WindowID id);
    UIContext* GetCurrentContext();
    std::string GetContextStringHierarchy();
    std::string GetContextStringHierarchy(UIContext* context);

    namespace Internal {
        FrameNode* InternalBeginFrame(uintptr_t key);
        TextNode* InternalAddText(uintptr_t key);
    }

    template<typename... Styles>
    void BeginFrame(UIKey&& key, const Styles&... styles) {
        FrameNode* node = Internal::InternalBeginFrame(key.id);
        if (node) {
            (node->AddStyle(styles), ...);
            GetCurrentContext();
        }
    }

    /*
    * @brief if the root nodes ends than the UI wil be rendererd. SDLCore::Render::Present() needs to be called ot see
    */
    UIEvent EndFrame();

    template<typename... Styles>
    UIEvent Text(UIKey&& key, const std::string& text, const Styles&... styles) {
        TextNode* node = Internal::InternalAddText(key.id);
        if (node) {
            node->m_text = text;
            (node->AddStyle(styles), ...);
            node->ApplyStyle(GetCurrentContext());
        }
        return UIEvent{};
    }

}