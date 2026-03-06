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
    size_t GetNodeCount();
    size_t GetNodeCount(UIContext* context);

    namespace Internal {
        FrameNode* InternalBeginFrame(uintptr_t key);
        TextNode* InternalAddText(uintptr_t key);
        void InternalSetAppliedStyleParams(UINode* node, uint64_t newHash, uint64_t frame);

        inline constexpr uint64_t hashSeed = 1469598103934665603ull;
        inline constexpr uint64_t hashMul = 1315423911ull;

        template<typename It>
        uint64_t InternalGenerateStyleHash(It begin, It end) {
            uint64_t hash = hashSeed;
            for (; begin != end; begin++) {
                hash = hash * hashMul + begin->GetID().value;
            }
            return hash;
        }

        template<typename... Styles>
        uint64_t InternalGenerateStyleHash(const Styles&... styles) {
            static_assert((std::is_same_v<Styles, UIStyle> && ...),
                "GenerateStylesHash only accepts UIStyle parameters");

            uint64_t hash = hashSeed;
            ((hash = hash * hashMul + styles.GetID().value), ...);
            return hash;
        }
    }

    template<typename... Styles>
    FrameNode* BeginFrame(UIKey&& key, const Styles&... styles) {
        static_assert((std::is_same_v<Styles, UIStyle> && ...),
            "BeginFrame only accepts UIStyle parameters");

        FrameNode* node = Internal::InternalBeginFrame(key.id);
        if (!node)
            return nullptr;

        uint64_t styleHash = Internal::InternalGenerateStyleHash(styles...);

        uint64_t newestStyleFrame = 0;
        ((newestStyleFrame = std::max(newestStyleFrame, styles.GetLastModified())), ...);

        if (!node->IsActive() || node->GetStyleChanged() || 
            node->GetAppliedStyleHash() != styleHash ||
            node->GetAppliedStyleNode() < newestStyleFrame) {
            node->ClearStyles();
            node->ReserveStyles(sizeof...(Styles));
            (node->AddStyle(styles), ...);
            node->UpdateFinalStyle(GetCurrentContext());

            Internal::InternalSetAppliedStyleParams(node, styleHash, newestStyleFrame);
        }

        return node;
    }

    FrameNode* BeginFrame(UIKey&& key, const std::vector<UIStyle>& styles);

    /*
    * @brief if the root nodes ends than the UI wil be rendererd. SDLCore::Render::Present() needs to be called ot see
    */
    UIEvent EndFrame();

    template<typename... Styles>
    TextNode* Text(UIKey&& key, const std::string& text, const Styles&... styles) {
        static_assert((std::is_same_v<Styles, UIStyle> && ...),
            "Text only accepts UIStyle parameters");

        TextNode* node = Internal::InternalAddText(key.id);
        if (!node)
            return nullptr;

        if(node->GetText() != text)
            node->SetText(text);

        uint64_t styleHash = Internal::InternalGenerateStyleHash(styles...);

        uint64_t newestStyleFrame = 0;
        ((newestStyleFrame = std::max(newestStyleFrame, styles.GetLastModified())), ...);

        if (!node->IsActive() || node->GetStyleChanged() || 
            node->GetAppliedStyleHash() != styleHash ||
            node->GetAppliedStyleNode() < newestStyleFrame) {
            node->ClearStyles();
            node->ReserveStyles(sizeof...(Styles));
            (node->AddStyle(styles), ...);
            node->UpdateFinalStyle(GetCurrentContext());

            Internal::InternalSetAppliedStyleParams(node, styleHash, newestStyleFrame);
        }
      
        return node;
    }

    TextNode* Text(UIKey&& key, const std::string& text, const std::vector<UIStyle>& styles);

}