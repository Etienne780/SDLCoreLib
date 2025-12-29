#include <CoreLib/Log.h>
#include "UI/UIScope.h"

namespace SDLCore::UI {

    class UICTXWrapper {
    public:
        UIContext* ctx = nullptr;

        UICTXWrapper() = default;
        UICTXWrapper(UIContext* c) 
            : ctx(c) {
        }

        bool IsContextNull(const char* action) const {
            if (!ctx) {
                Log::Error("SDLCore::UI::UICTXWrapper::{}, UIContext is null!", action);
                return true;
            }
            return false;
        }

        FrameNode* BeginFrame(uintptr_t key) {
            if (IsContextNull("BeginFrame: Could not begin frame"))
                return nullptr;
            return ctx->BeginFrame(key);
        }

        UIEvent EndFrame() {
            if (IsContextNull("EndFrame: Could not end frame"))
                return UIEvent{};
            return ctx->EndFrame();
        }

        template<typename T, typename ...Args>
        T* AddNode(uintptr_t id, Args&&... args) {
            if (IsContextNull("AddNode: Could not add node"))
                return nullptr;
            return ctx->AddNode<T>(id, std::forward<Args>(args)...);
        }

        UINode* GetRootNode() {
            if (IsContextNull("GetRootNode: Could not get root node"))
                return nullptr;
            return ctx->GetRootNode();
        }

        void SetWindowParams(WindowID id) {
            if (IsContextNull("SetWindowParams: Could not set window params"))
                return;
            ctx->SetWindowParams(id);
        }

        void SetAppliedStyleParams(UINode* node, uint64_t newHash, uint64_t frame) {
            if (!node)
                return;

            node->SetAppliedStyleHash(newHash);
            node->SetAppliedStyleNode(frame);
        }
    };

    static inline UICTXWrapper g_currentUIContext;
    static inline bool g_isCurrentUIContextDestroyed = false;
    
    namespace Internal {
        FrameNode* InternalBeginFrame(uintptr_t key) {
            return g_currentUIContext.BeginFrame(key); // node with key or new node
        }

        TextNode* InternalAddText(uintptr_t key) {
            return g_currentUIContext.AddNode<TextNode>(key);// node with key or new node
        }

        void InternalSetAppliedStyleParams(UINode* node, uint64_t newHash, uint64_t frame) {
            g_currentUIContext.SetAppliedStyleParams(node, newHash, frame);
        }
    }

    UIContext* CreateContext() {
        return UIContext::CreateContext();
    }

    UIContext* CreateContext(WindowID id) {
        UIContext* ctx = UIContext::CreateContext();
        SetContextWindow(ctx, id);
        return ctx;
    }

    void DestroyContext(UIContext* context) {
        if (!context)
            return;

        if (context == g_currentUIContext.ctx) {
            // Defer deletion until next BindContext
            g_isCurrentUIContextDestroyed = true;
            return;
        }

        delete context;
    }

    void BindContext(UIContext* context) {
        if (!context)
            return;

        // If the previous context was marked for deletion, destroy it now
        if (g_isCurrentUIContextDestroyed && g_currentUIContext.ctx) {
            delete g_currentUIContext.ctx;
            g_currentUIContext.ctx = nullptr;
            g_isCurrentUIContextDestroyed = false;
        }

        g_currentUIContext.ctx = context;
        // update window params
        WindowID id = context->GetWindowID();
        if(!id.IsInvalid())
            g_currentUIContext.SetWindowParams(id);
    }

    void SetContextWindow(UIContext* ctx, WindowID id) {
        if (!ctx) {
            Log::Error("SDLCore::UI::SetContextWindow: Cannot assign window '{}': context is null", id);
            return;
        }

        UICTXWrapper temp(ctx);
        temp.SetWindowParams(id);
    }

    UIContext* GetCurrentContext() {
        return g_currentUIContext.ctx;
    }

    std::string GetContextStringHierarchy() {
        return GetContextStringHierarchy(g_currentUIContext.ctx);
    }

    static void BuildHierarchy(std::ostringstream& stream, const UINode* node, int depth = 0) {
        for (int i = 0; i < depth; i++) {
            stream << "|    ";
        }

        const auto& children = node->GetChildren();
        if (children.size() > 0) {
            stream << "Begin Node: " << FormatUtils::toString(node->GetName()) << std::endl;

            for (const auto& n : children) {
                BuildHierarchy(stream, n.get(), depth + 1);
            }

            for (int i = 0; i < depth; i++) {
                stream << "|    ";
            }

            stream << "End Node" << std::endl;
        }
        else {
            stream << "Node: " << FormatUtils::toString(node->GetName()) << std::endl;
        }
    }

    std::string GetContextStringHierarchy(UIContext* ctx) {
        std::ostringstream stream;
        if (!ctx) {
            stream << "Invalid UIContext, " << "UIContext is null" << std::endl;
            return stream.str();
        }

        UICTXWrapper wrapper{ ctx };
        UINode* root = wrapper.GetRootNode();
        if (!root) {
            stream << "Invalid UIContext, root node is null" << std::endl;
            return stream.str();
        }

        BuildHierarchy(stream, root);
        return stream.str();
    }

    void BeginFrame(UIKey&& key, const std::vector<UIStyle>& styles) {
        FrameNode* node = Internal::InternalBeginFrame(key.id);
        if (!node)
            return;

        const uint64_t newHash =
            Internal::InternalGenerateStyleHash(styles.begin(), styles.end());

        uint64_t newestStyleFrame = 0;
        for (const auto& style : styles)
            newestStyleFrame = std::max(newestStyleFrame, style.GetLastModified());
        
        if (!node->IsActive() || node->GetAppliedStyleHash() != newHash ||
            node->GetAppliedStyleNode() < newestStyleFrame) 
        {
            node->ClearStyles();
            node->ReserveStyles(styles.size());
            for (const auto& style : styles)
                node->AddStyle(style);
            node->UpdateFinalStyle(g_currentUIContext.ctx);
            
            g_currentUIContext.SetAppliedStyleParams(node, newHash, newestStyleFrame);
        }
    }

    UIEvent EndFrame() {
        return g_currentUIContext.EndFrame();
    }

    UIEvent Text(UIKey&& key, const std::string& text, const std::vector<UIStyle>& styles) {
        TextNode* node = Internal::InternalAddText(key.id);
        if (!node)
            return UIEvent{};

        if (node->m_text != text)
            node->m_text = text;

        const uint64_t newHash =
            Internal::InternalGenerateStyleHash(styles.begin(), styles.end());

        uint64_t newestStyleFrame = 0;
        for (const auto& style : styles)
            newestStyleFrame = std::max(newestStyleFrame, style.GetLastModified());

        if (!node->IsActive() || node->GetAppliedStyleHash() != newHash ||
            node->GetAppliedStyleNode() < newestStyleFrame)
        {
            node->ClearStyles();
            node->ReserveStyles(styles.size());
            for (const auto& style : styles)
                node->AddStyle(style);
            node->UpdateFinalStyle(g_currentUIContext.ctx);

            Internal::InternalSetAppliedStyleParams(node, newHash, newestStyleFrame);
        }
        return node->GetEvent();
    }

}