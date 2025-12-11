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

        void EndFrame() {
            if (IsContextNull("GetNode: Could not end frame"))
                return;
            return ctx->EndFrame();
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
    };

    static inline UICTXWrapper g_currentUIContext;
    static inline bool g_isCurrentUIContextDestroyed = false;
    
    namespace Internal {
        FrameNode* InternalBeginFrame(uintptr_t key) {
            return g_currentUIContext.BeginFrame(key); // element with key or new element
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
    }

    void RenderContext(UIContext* context) {
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
            stream << "Begin Node: " << FormatUtils::toString(node->GetType()) << std::endl;

            for (const auto& n : children) {
                BuildHierarchy(stream, n.get(), depth + 1);
            }

            for (int i = 0; i < depth; i++) {
                stream << "|    ";
            }

            stream << "End Node" << std::endl;
        }
        else {
            stream << "Node: " << FormatUtils::toString(node->GetType()) << std::endl;
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

    UIEvent EndFrame() {
        // if (g_UIContext.nodeStack.empty())
        //    return UIEvent{};

        // FrameNode* node = static_cast<FrameNode*>(g_UIContext.nodeStack.back());
        // g_UIContext.nodeStack.pop_back();

        //return node ? node->GetEvent() : UIEvent{};
        g_currentUIContext.EndFrame();
        return UIEvent{};
    }

    FrameScope::FrameScope() {
        // BeginFrame();
        // m_node = g_UIContext.nodeStack.back();
    }

    FrameScope::~FrameScope() {
        if (!m_manualEndDone)
            SafeEnd();
    }

    UIEvent FrameScope::EndGetEvent() {
        UIEvent evt = SafeEnd();

        if (m_nodeWasClosed)
            m_manualEndDone = true;
        else
            Log::Warn("SDLCore::UI::FrameScope::EndGetEvent: Ignored because this scope is not the active UI frame (node is not top of stack)");

        return evt;
    }

    UIEvent FrameScope::SafeEnd() {
        // if (g_UIContext.nodeStack.empty())
        //     return {};
        // 
        // if (g_UIContext.nodeStack.back() != m_node) {
        //     return {};
        // }
        // 
        // m_nodeWasClosed = true;
        return EndFrame();
    }

}