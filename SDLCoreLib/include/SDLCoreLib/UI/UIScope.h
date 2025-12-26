#pragma once
#include <memory>
#include <vector>
#include <string>

#include <CoreLib/Log.h>

#include "UI/Types/UIContext.h" 
#include "UI/UINode.h"
#include "UI/Nodes/FrameNode.h"
#include "UI/Nodes/TextNode.h"

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
        void InternalSetAppliedStyleParams(UINode* node, uint64_t newHash, uint64_t frame);

        inline constexpr uint64_t hashSeed = 1469598103934665603ull;
        inline constexpr uint64_t hashMul = 1315423911ull;

        // Slow path – dynamic styles
        template<typename It>
        uint64_t InternalGenerateStyleHash(It begin, It end) {
            uint64_t hash = hashSeed;
            for (; begin != end; begin++) {
                hash = hash * hashMul + begin->GetID().value;
            }
            return hash;
        }
    }

    // faster begin frame. because styles canot change
    template<typename... Styles>
    void BeginFrame(UIKey&& key, const Styles&... styles) {
        static_assert((std::is_same_v<Styles, UIStyle> && ...),
            "BeginFrame only accepts UIStyle parameters");

        FrameNode* node = Internal::InternalBeginFrame(key.id);
        if (!node)
            return;

        uint64_t newestStyleFrame = 0;
        ((newestStyleFrame = std::max(newestStyleFrame, styles.GetLastModified())), ...);

        if (!node->IsActive() || node->GetAppliedStyleNode() < newestStyleFrame) {
            node->ClearStyles();
            node->ReserveStyles(sizeof...(Styles));
            (node->AddStyle(styles), ...);
            node->ApplyStyle(GetCurrentContext());

            Internal::InternalSetAppliedStyleParams(node, 0, newestStyleFrame);
        }
        else {
            if (node->HasStateChanged()) {
                node->ApplyStyle(GetCurrentContext());
            }
        }
    }

    // slower because styles can dynamically change
    void BeginFrame(UIKey&& key, const std::vector<UIStyle>& styles);

    /*
    * @brief if the root nodes ends than the UI wil be rendererd. SDLCore::Render::Present() needs to be called ot see
    */
    UIEvent EndFrame();

    // faster begin frame. because styles canot change
    template<typename... Styles>
    UIEvent Text(UIKey&& key, const std::string& text, const Styles&... styles) {
        static_assert((std::is_same_v<Styles, UIStyle> && ...),
            "BeginFrame only accepts UIStyle parameters");

        TextNode* node = Internal::InternalAddText(key.id);
        if (!node)
            return UIEvent{};

        if(node->m_text != text)
            node->m_text = text;

        uint64_t newestStyleFrame = 0;
        ((newestStyleFrame = std::max(newestStyleFrame, styles.GetLastModified())), ...);

        if (!node->IsActive() || node->GetAppliedStyleNode() < newestStyleFrame) {
            node->ClearStyles();
            node->ReserveStyles(sizeof...(Styles));
            (node->AddStyle(styles), ...);
            node->ApplyStyle(GetCurrentContext());

            Internal::InternalSetAppliedStyleParams(node, 0, newestStyleFrame);
        }
        else {
            if (node->HasStateChanged()) {
                node->ApplyStyle(GetCurrentContext());
            }
        }
      
        return node->GetEvent();
    }

    // slower because styles can dynamically change
    UIEvent Text(UIKey&& key, const std::string& text, const std::vector<UIStyle>& styles);

}